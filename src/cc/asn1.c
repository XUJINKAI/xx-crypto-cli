#include "asn1.h"
#include "gmssl/asn1.h"

#define NEXT_BYTE(value, ptr, len)                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        if (len == 0)                                                                                                  \
        {                                                                                                              \
            return RET_FAIL;                                                                                           \
        }                                                                                                              \
        len--;                                                                                                         \
        ptr++;                                                                                                         \
        value = *ptr;                                                                                                  \
    } while (0)

RESULT cc_asn1_parse_1(const uint8_t *__ptr_, size_t __len, ASN1_PARSE_INFO *outinfo)
{
    if (__len == 0)
    {
        return RET_FAIL;
    }

    uint8_t *__ptr = (uint8_t *)__ptr_;
    uint8_t *p     = __ptr;
    size_t l       = __len;
    uint8_t c      = *__ptr;
#define get_next_byte() NEXT_BYTE(c, p, l)

    uint32_t tagClass, tagConstructed, tagNumber;
    int64_t dataLength;
    uint8_t *lengthPtr, *dataPtr;

    tagClass       = c >> 6;
    tagConstructed = (c >> 5) & 1;
    tagNumber      = c & 0x1F;
    if (tagNumber == 0x1F)
    {
        tagNumber = 0;
        do
        {
            get_next_byte();
            tagNumber = (tagNumber << 7) | (c & 0x7F);
        } while (c & 0x80);
    }
    get_next_byte();
    lengthPtr = p;

    if (c & 0x80)
    {
        size_t len_len = c & 0x7F;
        get_next_byte();
        dataLength = 0;
        while (len_len--)
        {
            dataLength = (dataLength << 8) | c;
            get_next_byte();
        }
    }
    else
    {
        dataLength = c;
        get_next_byte();
    }
    dataPtr = p;

    if (outinfo)
    {
        *outinfo = (ASN1_PARSE_INFO){
            .tagClass       = tagClass,
            .tagConstructed = tagConstructed,
            .tagNumber      = tagNumber,
            .length         = dataLength,
            .tagPtr         = __ptr,
            .lengthPtr      = lengthPtr,
            .dataPtr        = dataPtr,
        };
    }
    return RET_OK;
}

static RESULT __asn1_parse_cb_do_nothing(ASN1_PARSE_INFO *info, int level, void *arg)
{
    (void)info;
    (void)level;
    (void)arg;
    return RET_OK;
}

static RESULT __asn1_parse_cb_r(uint8_t *__ptr, size_t __len, ASN1_PARSE_CALLBACK cb, void *arg, int level,
                                ASN1_PARSE_INFO *outinfo)
{
    if (__len == 0)
    {
        return RET_FAIL;
    }

    ASN1_PARSE_INFO info = {0};
    if (RET_OK != cc_asn1_parse_1(__ptr, __len, &info))
    {
        return RET_FAIL;
    }

    if (outinfo)
    {
        *outinfo = info;
    }

    if (cb)
    {
        if (cb(&info, level, arg) != RET_OK)
        {
            return RET_FAIL;
        }
    }
    if (info.tagConstructed)
    {
        int64_t len = info.length;
        while (len > 0)
        {
            ASN1_PARSE_INFO child_info = {0};
            if (__asn1_parse_cb_r(info.dataPtr, info.length, cb, arg, level + 1, &child_info) != RET_OK)
            {
                return RET_FAIL;
            }
            len -= child_info.dataPtr - child_info.tagPtr + child_info.length;
        }
    }
    return RET_OK;
}

RESULT cc_asn1_parse_cb(uint8_t *__ptr, size_t __len, ASN1_PARSE_CALLBACK cb, void *arg)
{
    if (__ptr == NULL || __len == 0)
    {
        LOG_DBG("asn1_parse_cb: invalid argument");
        return RET_FAIL;
    }
    if (cb == NULL)
    {
        cb = __asn1_parse_cb_do_nothing;
    }
    return __asn1_parse_cb_r(__ptr, __len, cb, arg, 0, NULL);
}

static RESULT __asn1_print_cb(ASN1_PARSE_INFO *info, int level, void *arg)
{
    XIO *__stream = (XIO *)arg;
    XIO_printf(__stream, "%*s", level * 2, "");
    switch (info->tagNumber)
    {
        case 0x01:
            XIO_printf(__stream, "BOOLEAN %s", info->dataPtr[0] ? "TRUE" : "FALSE");
            break;
        case 0x02:
            XIO_printf(__stream, "INTEGER ");
            for (int i = 0; i < info->length; i++)
            {
                XIO_printf(__stream, "%02X", info->dataPtr[i]);
            }
            XIO_putc(__stream, '\n');
            break;
        case 0x03:
            XIO_printf(__stream, "BIT_STRING ");
            for (int i = 0; i < info->length; i++)
            {
                XIO_printf(__stream, "%02X", info->dataPtr[i]);
            }
            break;
        case 0x04:
            XIO_printf(__stream, "OCTET_STRING ");
            for (int i = 0; i < info->length; i++)
            {
                XIO_printf(__stream, "%02X", info->dataPtr[i]);
            }
            break;
        case 0x05:
            XIO_printf(__stream, "NULL");
            break;
        case 0x06:
            XIO_printf(__stream, "OBJECT_IDENTIFIER ");
            for (int i = 0; i < info->length; i++)
            {
                XIO_printf(__stream, "%02X", info->dataPtr[i]);
            }
            break;
        case 0x07:
            XIO_printf(__stream, "ObjectDescriptor ");
            for (int i = 0; i < info->length; i++)
            {
                XIO_printf(__stream, "%02X", info->dataPtr[i]);
            }
            break;
        case 0x08:
            XIO_printf(__stream, "EXTERNAL ");
            for (int i = 0; i < info->length; i++)
            {
                XIO_printf(__stream, "%02X", info->dataPtr[i]);
            }
            break;
        case 0x09:
            XIO_printf(__stream, "REAL ");
            for (int i = 0; i < info->length; i++)
            {
                XIO_printf(__stream, "%02X", info->dataPtr[i]);
            }
            break;
        case 0x0A:
            XIO_printf(__stream, "ENUMERATED ");
            for (int i = 0; i < info->length; i++)
            {
                XIO_printf(__stream, "%02X", info->dataPtr[i]);
            }
            break;
        case 0x10:
            XIO_printf(__stream, "SEQUENCE\n");
            break;
    }
    return RET_OK;
}

RESULT cc_asn1_print(XIO *__stream, uint8_t *__ptr, size_t __len)
{
    return cc_asn1_parse_cb(__ptr, __len, __asn1_print_cb, __stream);
}

static RESULT __is_asn1_cb(ASN1_PARSE_INFO *info, int level, void *arg)
{
    return RET_OK;
}

RESULT cc_is_asn1(uint8_t *__ptr, size_t __len)
{
    return cc_asn1_parse_cb(__ptr, __len, __is_asn1_cb, NULL);
}
