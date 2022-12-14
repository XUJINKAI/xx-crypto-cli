#include "cc/format/hex.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "gmssl/hex.h"
#include "gmssl/pbkdf2.h"
#include "gmssl/sm2.h"
#include "gmssl/sm4.h"
#include "utils/console.h"
#include <ctype.h> // isxdigit

static cmdp_action_t __process(cmdp_process_param_st *params);

struct chat_session_st
{
    XIO *in_stream;
    XIO *out_stream;
    uint8_t key[16];
    uint8_t iv[16];
};

static struct
{
    bool init;
    bool join;
    bool restore;
    bool psk;
} arg_chat;

cmdp_command_st sc_chat = {
    .name = "chat",
    .desc = "Interactive encryption chat helper",
    .doc  = "Usage: chat [OPTIONS]\n"
           "Initiate: chat --init\n"
           "Join:     chat --join <KEY>\n"
           "Restore:  chat --restore <KEY>\n"
           "PSK:      chat --psk <PSK>\n"
           "Attention! Cipher text will permanently lost if KEY lost.\n"
           "\n",
    .options =
        (cmdp_option_st[]){
            {0, "init", "Initiate a chat", CMDP_TYPE_BOOL, &arg_chat.init},
            {0, "join", "Join a chat", CMDP_TYPE_BOOL, &arg_chat.join},
            {0, "restore", "Restore a chat", CMDP_TYPE_BOOL, &arg_chat.restore},
            {0, "psk", "Use a Pre-Shared Key to start chat", CMDP_TYPE_BOOL, &arg_chat.psk},
            {0},
        },
    .fn_process = __process,
};

static bool is_cipher_string(const char *str)
{
    if (strlen(str) % 32 != 0)
    {
        return false;
    }
    for (uint32_t i = 0; i < strlen(str); i++)
    {
        if (!isxdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

static void chat_session_start(struct chat_session_st *session)
{
    XIO_printf(session->out_stream, "\n"
                                    "Chat session started.\n");

    char *key_hex = cc_hex_encode(session->key, 16, 0);
    char *iv_hex  = cc_hex_encode(session->iv, 16, 0);
    LOG_SECRET("Use following command to restore session:\n"
               "chat --restore %s%s\n",
               key_hex, iv_hex);
    free(key_hex);
    free(iv_hex);

    while (1)
    {
        char *line      = console_readline(TC_GREEN, "\nCHAT> ");
        size_t lineSize = strlen(line);
        if (is_cipher_string(line))
        {
            uint8_t *cipher_bytes   = calloc(1, lineSize / 2);
            size_t cipher_bytes_len = 0;
            hex_to_bytes(line, lineSize, cipher_bytes, &cipher_bytes_len);
            uint8_t *plain = malloc(lineSize);
            size_t plain_len;
            SM4_CBC_CTX dec_ctx;
            sm4_cbc_decrypt_init(&dec_ctx, session->key, session->iv);
            sm4_cbc_decrypt_update(&dec_ctx, cipher_bytes, cipher_bytes_len, plain, &plain_len);
            XIO_write(session->out_stream, plain, plain_len);
            sm4_cbc_decrypt_finish(&dec_ctx, plain, &plain_len);
            XIO_write(session->out_stream, plain, plain_len);
            XIO_printf(session->out_stream, "\n");
            free(plain);
            free(cipher_bytes);
        }
        else
        {
            uint8_t cipher_block[1024];
            size_t cipher_block_len;
            SM4_CBC_CTX enc_ctx;
            sm4_cbc_encrypt_init(&enc_ctx, session->key, session->iv);
            size_t readin;
            for (size_t i = 0; i < lineSize; i += readin)
            {
                readin = lineSize - i > 1024 ? 1024 : lineSize - i;
                sm4_cbc_encrypt_update(&enc_ctx, (uint8_t *)line + i, readin, cipher_block, &cipher_block_len);
                char *cipher_hex = cc_hex_encode(cipher_block, cipher_block_len, 0);
                XIO_printf(session->out_stream, "%s", cipher_hex);
                free(cipher_hex);
            }
            sm4_cbc_encrypt_finish(&enc_ctx, cipher_block, &cipher_block_len);
            char *final_hex = cc_hex_encode(cipher_block, cipher_block_len, 0);
            XIO_printf(session->out_stream, "%s\n", final_hex);
            free(final_hex);
        }
        free(line);
    }
}

static void SM2_point_mul_bytes(SM2_POINT Point, uint8_t private_key_bytes[32], uint8_t result_bytes[64])
{
    LOG_DBG_HEX("x: ", Point.x, 32);
    LOG_DBG_HEX("y: ", Point.y, 32);
    LOG_DBG_HEX("k: ", private_key_bytes, 32);
    SM2_BN sk, pt_x, pt_y;
    sm2_bn_from_bytes(sk, private_key_bytes);
    sm2_bn_from_bytes(pt_x, Point.x);
    sm2_bn_from_bytes(pt_y, Point.y);

    SM2_JACOBIAN_POINT _P, *P = &_P;
    SM2_JACOBIAN_POINT _R, *R = &_R;
    sm2_jacobian_point_set_xy(P, pt_x, pt_y);
    sm2_jacobian_point_mul(R, sk, P);

    SM2_BN result_x, result_y;
    sm2_jacobian_point_get_xy(R, result_x, result_y);

    sm2_bn_to_bytes(result_x, result_bytes);
    sm2_bn_to_bytes(result_y, result_bytes + 32);
    LOG_DBG_HEX("R: ", result_bytes, 64);
}

static void chat_kdf(void *pass, size_t pass_len, uint8_t result_key[32], uint8_t result_iv[32])
{
    uint8_t secret[32] = {0};
    pbkdf2_hmac_sm3_genkey(pass, pass_len, NULL, 0, 10001, 32, secret);
    memcpy(result_key, secret, 16);
    memcpy(result_iv, secret + 16, 16);
}

static RESULT sm2_point_from_string(SM2_POINT *P, const char *str)
{
    xbytes *mem = cc_hex_decode(str, strlen(str), 0);
    if (mem == NULL)
    {
        return RET_FAIL;
    }
    int r = sm2_point_from_octets(P, xbytes_buffer(mem), xbytes_length(mem));
    xbytes_free(mem);
    return r == 1 ? RET_OK : RET_FAIL;
}

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    if (params->argc == 0 && params->opts == 0)
    {
        return CMDP_ACT_SHOW_HELP;
    }
    if (arg_chat.init + arg_chat.join + arg_chat.restore + arg_chat.psk != 1)
    {
        LOG_ERROR("Only one of --init, --join, --restore, --psk should be specified");
        return CMDP_ACT_ERROR | CMDP_ACT_SHOW_HELP;
    }

    struct chat_session_st session = {
        .in_stream  = XIO_new_fp(stdin, 0),
        .out_stream = XIO_new_fp(stdout, 0),
    };

    if (arg_chat.init)
    {
        SM2_KEY key;
        sm2_key_generate(&key);
        uint8_t compress_pk[33];
        sm2_point_to_compressed_octets(&key.public_key, compress_pk);

        LOG_DBG_HEX("sk: ", key.private_key, 32);
        LOG_DBG_HEX("pk: ", &key.public_key, 64);
        LOG_C(TC_YELLOW, "Send following command to other side:");
        LOG_C_HEX(TC_CYAN, "xx chat --join ", compress_pk, 33);
        RESULT other_ret = RET_FAIL;
        SM2_POINT other_pk;
        do
        {
            char *resp_hex = console_readline(0, "Input other side's response: ");
            other_ret      = sm2_point_from_string(&other_pk, resp_hex);
            free(resp_hex);
            if (other_ret != RET_OK)
            {
                LOG_ERROR("Invalid response");
            }
        } while (other_ret != RET_OK);
        uint8_t result_point[64];
        SM2_point_mul_bytes(other_pk, key.private_key, result_point);
        chat_kdf(result_point, 64, session.key, session.iv);
    }
    else if (arg_chat.join)
    {
        if (params->argc != 1)
        {
            LOG_ERROR("Invalid argument.");
            return CMDP_ACT_ERROR;
        }
        SM2_POINT other_pk;
        if (sm2_point_from_string(&other_pk, params->argv[0]) != RET_OK)
        {
            LOG_ERROR("Invalid argument.");
            return CMDP_ACT_ERROR;
        }

        SM2_KEY key;
        sm2_key_generate(&key);
        LOG_DBG_HEX("sk: ", key.private_key, 32);
        LOG_DBG_HEX("pk: ", &key.public_key, 64);

        uint8_t result_point[64];
        SM2_point_mul_bytes(other_pk, key.private_key, result_point);

        chat_kdf(result_point, 64, session.key, session.iv);

        uint8_t compress_pk[33];
        sm2_point_to_compressed_octets(&key.public_key, compress_pk);
        LOG_C(TC_YELLOW, "Send following text to other side:");
        LOG_C_HEX(TC_CYAN, "", compress_pk, 33);
    }
    else if (arg_chat.restore)
    {
        if (params->argc != 1)
        {
            return CMDP_ACT_ERROR | CMDP_ACT_SHOW_HELP;
        }

        if (strlen(params->argv[0]) != 64)
        {
            LOG_ERROR("Invalid KEY length");
            return CMDP_ACT_ERROR | CMDP_ACT_SHOW_HELP;
        }
        size_t outlen;
        hex_to_bytes(params->argv[0], 32, session.key, &outlen);
        hex_to_bytes(params->argv[0] + 32, 32, session.iv, &outlen);
    }
    else if (arg_chat.psk)
    {
        if (params->argc != 1)
        {
            LOG_ERROR("--psk requires an argument");
            return CMDP_ACT_ERROR | CMDP_ACT_SHOW_HELP;
        }
        chat_kdf(params->argv[0], strlen(params->argv[0]), session.key, session.iv);
    }
    else
    {
        LOG_ERROR("not implemented");
        return CMDP_ACT_ERROR | CMDP_ACT_SHOW_HELP;
    }

    chat_session_start(&session);
    XIO_close(session.in_stream);
    XIO_close(session.out_stream);

    return CMDP_ACT_OK;
}
