#include "cformat.h"

static void cstr_encode_buf(uint8_t *__buf, size_t __size, XIO *instream, XIO *outstream)
{
    size_t len = 0;
    while ((len = XIO_read(instream, __buf, __size)) > 0)
    {
        for (int i = 0; i < len; i++)
        {
            uint8_t *p = __buf + i;
            switch (*p)
            {
                case '\n':
                    XIO_write(outstream, "\\n", 2);
                    break;
                case '\r':
                    XIO_write(outstream, "\\r", 2);
                    break;
                case '\t':
                    XIO_write(outstream, "\\t", 2);
                    break;
                case '\\':
                    XIO_write(outstream, "\\\\", 2);
                    break;
                case '\"':
                    XIO_write(outstream, "\\\"", 2);
                    break;
                default:
                    XIO_write(outstream, p, 1);
                    break;
            }
        }
    }
}

void cstr_encode(XIO *instream, XIO *outstream, size_t bufsize)
{
    uint8_t buf[STACK_BUFFER_SIZE];
    if (bufsize > STACK_BUFFER_SIZE)
    {
        uint8_t *p = malloc(bufsize);
        if (!p)
        {
            LOG_ERROR("malloc failed");
            return;
        }
        cstr_encode_buf(p, bufsize, instream, outstream);
        free(p);
    }
    else
    {
        cstr_encode_buf(buf, STACK_BUFFER_SIZE, instream, outstream);
    }
}

static void cstr_decode_buf(uint8_t *__buf, size_t __size, XIO *instream, XIO *outstream)
{
    size_t len = 0;
    while ((len = XIO_read(instream, __buf, __size)) > 0)
    {
        for (int i = 0; i < len; i++)
        {
            uint8_t *p = __buf + i;
            if (*p == '\\')
            {
                i++;
                p++;
                switch (*p)
                {
                    case 'n':
                        XIO_write(outstream, "\n", 1);
                        break;
                    case 'r':
                        XIO_write(outstream, "\r", 1);
                        break;
                    case 't':
                        XIO_write(outstream, "\t", 1);
                        break;
                    case '\\':
                        XIO_write(outstream, "\\", 1);
                        break;
                    case '\"':
                        XIO_write(outstream, "\"", 1);
                        break;
                    default:
                        XIO_write(outstream, p, 1);
                        break;
                }
            }
            else
            {
                XIO_write(outstream, p, 1);
            }
        }
    }
}

void cstr_decode(XIO *instream, XIO *outstream, size_t bufsize)
{
    uint8_t buf[STACK_BUFFER_SIZE];
    if (bufsize > STACK_BUFFER_SIZE)
    {
        uint8_t *p = malloc(bufsize);
        if (!p)
        {
            LOG_ERROR("malloc failed");
            return;
        }
        cstr_decode_buf(p, bufsize, instream, outstream);
        free(p);
    }
    else
    {
        cstr_decode_buf(buf, STACK_BUFFER_SIZE, instream, outstream);
    }
}

static void cbytes_encode_buf(uint8_t *__buf, size_t __size, XIO *instream, XIO *outstream)
{
    u_char byte_buf[8] = ", 0x00\0";
    size_t read_len    = 0;
    while ((read_len = XIO_read(instream, __buf, __size)) > 0)
    {
        uint8_t *p = __buf;
        sprintf(byte_buf + 4, G_HEX_FMT, *p);
        XIO_write(outstream, byte_buf + 2, 4);

        for (int i = 1; i < read_len; i++)
        {
            p = __buf + i;
            sprintf((char *)byte_buf + 4, G_HEX_FMT, *p);
            XIO_write(outstream, byte_buf, 6);
        }
    }
}

void cbytes_encode(XIO *instream, XIO *outstream, size_t bufsize)
{
    uint8_t buf[STACK_BUFFER_SIZE];
    if (bufsize > STACK_BUFFER_SIZE)
    {
        uint8_t *p = malloc(bufsize);
        if (!p)
        {
            LOG_ERROR("malloc failed");
            return;
        }
        cbytes_encode_buf(p, bufsize, instream, outstream);
        free(p);
    }
    else
    {
        cbytes_encode_buf(buf, STACK_BUFFER_SIZE, instream, outstream);
    }
}

static void cbytes_decode_buf(uint8_t *__buf, size_t __size, XIO *instream, XIO *outstream)
{
    // TODO by AI
    size_t read_len = 0;
    while ((read_len = XIO_read(instream, __buf, __size)) > 0)
    {
        uint8_t *p = __buf;
        for (int i = 0; i < read_len; i++)
        {
            if (*p == '0' && *(p + 1) == 'x')
            {
                i += 2;
                p += 2;
                uint8_t byte = 0;
                for (int j = 0; j < 2; j++)
                {
                    if (*p >= '0' && *p <= '9')
                    {
                        byte = byte * 16 + (*p - '0');
                    }
                    else if (*p >= 'a' && *p <= 'f')
                    {
                        byte = byte * 16 + (*p - 'a' + 10);
                    }
                    else if (*p >= 'A' && *p <= 'F')
                    {
                        byte = byte * 16 + (*p - 'A' + 10);
                    }
                    else
                    {
                        LOG_ERROR("invalid hex char: %c", *p);
                        return;
                    }
                    i++;
                    p++;
                }
                XIO_write(outstream, &byte, 1);
            }
            else
            {
                p++;
            }
        }
    }
}
void cbytes_decode(XIO *instream, XIO *outstream, size_t bufsize)
{
    uint8_t buf[STACK_BUFFER_SIZE];
    if (bufsize > STACK_BUFFER_SIZE)
    {
        uint8_t *p = malloc(bufsize);
        if (!p)
        {
            LOG_ERROR("malloc failed");
            return;
        }
        cbytes_decode_buf(p, bufsize, instream, outstream);
        free(p);
    }
    else
    {
        cbytes_decode_buf(buf, STACK_BUFFER_SIZE, instream, outstream);
    }
}
