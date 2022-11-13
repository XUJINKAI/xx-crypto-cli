#include <stdio.h>

int xx_main(int argc, char *argv[], FILE *in_stream, FILE *out_stream, FILE *log_stream);

int main(int argc, char *argv[])
{
    return xx_main(argc, argv, stdin, stdout, stderr);
}
