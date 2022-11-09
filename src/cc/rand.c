#include "rand.h"
#include "cross-platform/string.h"
#include "gmssl/rand.h"
#include "gmssl/sdf.h"

typedef int (*rand_func)(uint8_t *buf, size_t buflen);

RESULT cc_random_bytes(const char *source, XIO *out, size_t len)
{
    rand_func fn = NULL;
    if (source == NULL || strcmp(source, "soft") == 0)
    {
        fn = rand_bytes;
    }
    else if (strcmp(source, "rdrand") == 0)
    {
        fn = rdrand_bytes;
    }
    else if (strcmp(source, "rdseed") == 0)
    {
        fn = rdseed_bytes;
    }
    else if (strncasecmp(source, "sdf:", 4) == 0)
    {
        return cc_random_bytes_sdf(source + 4, out, len);
    }
    else
    {
        LOG_ERR("Unknown random source: %s", source);
        return RET_FAIL;
    }

    uint8_t buf[1024];
    while (len > 0)
    {
        size_t n = len > sizeof(buf) ? sizeof(buf) : len;
        if (fn(buf, n) < 0)
        {
            return RET_FAIL;
        }
        XIO_write(out, buf, n);
        len -= n;
    }
    return RET_OK;
}

RESULT cc_random_bytes_sdf(const char *libname, XIO *out, size_t len)
{
    if (sdf_load_library(libname, NULL) < 0)
    {
        return RET_FAIL;
    }

    SDF_DEVICE dev;
    if (sdf_open_device(&dev) < 0)
    {
        return RET_FAIL;
    }

    uint8_t buf[1024];
    while (len > 0)
    {
        size_t n = len > sizeof(buf) ? sizeof(buf) : len;
        if (sdf_rand_bytes(&dev, buf, n) < 0)
        {
            return RET_FAIL;
        }
        XIO_write(out, buf, n);
        len -= n;
    }

    sdf_close_device(&dev);
    sdf_unload_library();
    return RET_OK;
}

#include "random-check/stat_fncs.h"

RESULT cc_random_check(const uint8_t *rand, size_t len)
{
    RESULT ret        = RET_OK;
    int bslen         = len * 8;
    BitSequence *bseq = (BitSequence *)calloc(bslen, sizeof(BitSequence));
    BytesToBitSequence((unsigned char *)rand, len, bseq, bslen);

    int r;
#define CHECK(chTitle, title, expression)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        fprintf(stdout, "%s", chTitle);                                                                                \
        fflush(stdout);                                                                                                \
        r = (expression);                                                                                              \
        if (r == 0)                                                                                                    \
        {                                                                                                              \
            ret = RET_FAIL;                                                                                            \
        }                                                                                                              \
        fprintf(stdout, "\t%s\n", (r == 0) ? "Failed" : "Pass");                                                       \
    } while (0)

    CHECK("单比特频数检测", "Frequency", Frequency(bslen, bseq));
    CHECK("块内频数检测(m=10000)", "BlockFrequency", BlockFrequency(10000, bslen, bseq));
    CHECK("扑克检测(m=4)", "PokerDetect(4)", PokerDetect(4, bslen, bseq));
    CHECK("扑克检测(m=8)", "PokerDetect(8)", PokerDetect(8, bslen, bseq));
    CHECK("重叠子序列检测(m=3)", "Serial(3)", Serial(3, bslen, bseq));
    CHECK("重叠子序列检测(m=5)", "Serial(5)", Serial(5, bslen, bseq));
    CHECK("游程总数检测", "Runs", Runs(bslen, bseq));
    CHECK("游程分布检测", "RunsDistribution", RunsDistribution(bslen, bseq));
    CHECK("块内最大1游程检测", "LongestRunOfOnes", LongestRunOfOnes(bslen, bseq));
    CHECK("二元推导检测(k=3)", "BinaryDerivative(3)", BinaryDerivate(3, bslen, bseq, sizeof(bseq)));
    CHECK("二元推导检测(k=7)", "BinaryDerivative(7)", BinaryDerivate(7, bslen, bseq, sizeof(bseq)));
    CHECK("自相关检测(d=1)", "SelfCorrelation(1)", SelfCorrelation(1, bslen, bseq));
    CHECK("自相关检测(d=2)", "SelfCorrelation(2)", SelfCorrelation(2, bslen, bseq));
    CHECK("自相关检测(d=8)", "SelfCorrelation(8)", SelfCorrelation(8, bslen, bseq));
    CHECK("自相关检测(d=16)", "SelfCorrelation(16)", SelfCorrelation(16, bslen, bseq));
    CHECK("矩阵秩检测", "Rank", Rank(bslen, bseq));
    CHECK("累加和检测", "CumulativeSums", CumulativeSums(bslen, bseq));
    CHECK("近似熵检测(m=2)", "ApproximateEntropy(2)", ApproximateEntropy(2, bslen, bseq));
    CHECK("近似熵检测(m=5)", "ApproximateEntropy(5)", ApproximateEntropy(5, bslen, bseq));
    CHECK("线性复杂度检测(m=500)", "LinearComplexity(500)", LinearComplexity(500, bslen, bseq));
    CHECK("线性复杂度检测(m=1000)", "LinearComplexity(1000)", LinearComplexity(1000, bslen, bseq));
    CHECK("通用统计检测", "Universal", Universal(bslen, bseq));
    CHECK("离散傅里叶检测", "DiscreteFourierTransform", DiscreteFourierTransform(bslen, bseq));

    free(bseq);
    return ret;
}
