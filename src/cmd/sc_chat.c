#include "cc/hex.h"
#include "cmd/io_helper.h"
#include "cmd/opt_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "gmssl/hex.h"
#include "gmssl/pbkdf2.h"
#include "gmssl/sm4.h"
#include <ctype.h> // isxdigit

static void chat_before(cmdp_before_param_st *params);
static cmdp_action_t chat_process(cmdp_process_param_st *params);

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
    bool sm4_cbc;
} arg_chat;

cmdp_command_st sc_chat = {
    .name = "chat",
    .desc = "Interactive encryption chat helper",
    .doc  = "Usage: chat [OPTIONS]\n"
           "Initiate: chat --init\n"
           "Join:     chat --join <TEXT>\n"
           "Restore:  chat --restore [--sm4-cbc] <KEY>\n"
           "PSK:      chat --psk <PSK>\n"
           "Attention! Cipher text will permanently lost if KEY lost.\n"
           "\n",
    .options =
        (cmdp_option_st[]){
            {0, "init", "Initiate a chat", CMDP_TYPE_BOOL, &arg_chat.init},
            {0, "join", "Join a chat", CMDP_TYPE_BOOL, &arg_chat.join},
            {0, "restore", "Restore a chat", CMDP_TYPE_BOOL, &arg_chat.restore},
            {0, "psk", "Use a Pre-Shared Key to start chat", CMDP_TYPE_BOOL, &arg_chat.psk},
            {0, "sm4-cbc", "Use SM4-CBC mode", CMDP_TYPE_BOOL, &arg_chat.sm4_cbc},
            {0},
        },
    .fn_before  = chat_before,
    .fn_process = chat_process,
};

static bool is_cipher_string(const char *str)
{
    if (strlen(str) % 32 != 0)
    {
        return false;
    }
    for (int i = 0; i < strlen(str); i++)
    {
        if (!isxdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

static void chat_start_session(struct chat_session_st *session)
{
    char *key_hex = bytes_to_hex(session->key, 16);
    char *iv_hex  = bytes_to_hex(session->iv, 16);
    XIO_printf(session->out_stream,
               "Chat session started.\n"
               "Use following command to restore session:\n"
               "> chat --restore --sm4-cbc %s%s\n",
               key_hex, iv_hex);
    free(key_hex);
    free(iv_hex);

    while (1)
    {
        printf("> ");
        char line[10240] = {0};
        fgets(line, 10240, stdin);
        size_t lineSize = strlen(line);
        if (line[lineSize - 1] == '\n')
        {
            line[lineSize - 1] = '\0';
            lineSize--;
        }
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
                sm4_cbc_encrypt_update(&enc_ctx, line + i, readin, cipher_block, &cipher_block_len);
                char *cipher_hex = bytes_to_hex(cipher_block, cipher_block_len);
                XIO_printf(session->out_stream, "%s", cipher_hex);
                free(cipher_hex);
            }
            sm4_cbc_encrypt_finish(&enc_ctx, cipher_block, &cipher_block_len);
            char *final_hex = bytes_to_hex(cipher_block, cipher_block_len);
            XIO_printf(session->out_stream, "%s\n", final_hex);
            free(final_hex);
        }
    }
}


static void chat_before(cmdp_before_param_st *params)
{
    memset(&arg_chat, 0, sizeof(arg_chat));
}
static cmdp_action_t chat_process(cmdp_process_param_st *params)
{
    if (params->argc == 0 && params->opts == 0)
    {
        return CMDP_ACT_SHOW_HELP;
    }
    if (arg_chat.init + arg_chat.join + arg_chat.restore + arg_chat.psk != 1)
    {
        LOG0("Only one of --init, --join, --restore, --psk can be specified");
        return CMDP_ACT_FAIL | CMDP_ACT_SHOW_HELP;
    }

    struct chat_session_st session = {
        .in_stream  = XIO_new_from_FILE(stdin, false),
        .out_stream = XIO_new_from_FILE(stdout, false),
    };
    if (arg_chat.init)
    {
        LOG0("not implemented");
        return CMDP_ACT_OVER;
    }
    else if (arg_chat.join)
    {
        LOG0("not implemented");
        return CMDP_ACT_OVER;
    }
    else if (arg_chat.restore)
    {
        if (params->argc != 1)
        {
            return CMDP_ACT_FAIL | CMDP_ACT_SHOW_HELP;
        }

        if (strlen(params->argv[0]) != 64)
        {
            LOG0("Invalid KEY length");
            return CMDP_ACT_FAIL | CMDP_ACT_SHOW_HELP;
        }
        size_t outlen;
        hex_to_bytes(params->argv[0], 32, session.key, &outlen);
        hex_to_bytes(params->argv[0] + 32, 32, session.iv, &outlen);
    }
    else if (arg_chat.psk)
    {
        if (params->argc != 1)
        {
            LOG0("--psk requires an argument");
            return CMDP_ACT_FAIL | CMDP_ACT_SHOW_HELP;
        }
        uint8_t secret[32];
        pbkdf2_hmac_sm3_genkey(params->argv[0], strlen(params->argv[0]), NULL, 0, 10001, 32, secret);
        memcpy(session.key, secret, 16);
        memcpy(session.iv, secret + 16, 16);
    }
    else
    {
        LOG0("not implemented");
        return CMDP_ACT_FAIL | CMDP_ACT_SHOW_HELP;
    }

    chat_start_session(&session);
    XIO_close(session.in_stream);
    XIO_close(session.out_stream);

    return CMDP_ACT_OVER;
}
