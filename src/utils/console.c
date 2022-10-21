#include "console.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
https://stackoverflow.com/questions/735126/are-there-alternate-implementations-of-gnu-getline-interface/735472#735472
*/
#if _WIN32

#ifndef ssize_t
#define ssize_t long
#endif

ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
    char *bufptr = NULL;
    char *p      = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL)
    {
        return -1;
    }
    if (stream == NULL)
    {
        return -1;
    }
    if (n == NULL)
    {
        return -1;
    }
    bufptr = *lineptr;
    size   = *n;

    c = fgetc(stream);
    if (c == EOF)
    {
        return -1;
    }
    if (bufptr == NULL)
    {
        bufptr = malloc(128);
        if (bufptr == NULL)
        {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while (c != EOF)
    {
        if ((p - bufptr) > (size - 1))
        {
            size   = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL)
            {
                return -1;
            }
        }
        *p++ = c;
        if (c == '\n')
        {
            break;
        }
        c = fgetc(stream);
    }

    *p++     = '\0';
    *lineptr = bufptr;
    *n       = size;

    return p - bufptr - 1;
}
#endif


char *console_readline(const char *prompt)
{
    if (isatty(fileno(stdout)))
    {
        fprintf(stdout, "%s%s%s", TC_GREEN, prompt, TC_RESET);
    }
    else
    {
        fprintf(stdout, "%s", prompt);
    }
    char *line       = NULL;
    size_t __mem_len = 0;
    ssize_t len      = getline(&line, &__mem_len, stdin);
    if (len == -1)
    {
        free(line);
        return NULL;
    }
    line[len - 1] = 0;
    return line;
}

/*
https://stackoverflow.com/questions/13481058/commandlinetoargvw-equivalent-on-linux
 */

#define FALSE 0
#define TRUE  1

#define IS_BLANK(ch) (((ch) == ' ') || ((ch) == '\t'))
#define IS_QUOTE(ch) (((ch) == '"') || ((ch) == '\''))

int commandline_to_args(const char *cmd, int *retArgc, char ***retArgv)
{
    char ch;
    char *cp;
    char *cpOut;
    char *newStrings;
    int len;
    int quote;

    const char *argument;
    const char **newArgTable;
    int newArgTableSize;

    static char *strings;
    static int stringsLength;

    static const char **argTable;
    static int argCount;
    static int argTableSize;

    /*
     * Clear the returned values until we know them.
     */
    argCount = 0;
    *retArgc = 0;
    *retArgv = NULL;

    /*
     * Copy the command string into a buffer that we can modify,
     * reallocating it if necessary.
     */
    len = strlen(cmd) + 1;

    if (len > stringsLength)
    {
        newStrings = realloc(strings, len);

        if (newStrings == NULL)
        {
            LOG0("Cannot allocate string.");
            return FALSE;
        }

        strings       = newStrings;
        stringsLength = len;
    }

    memcpy(strings, cmd, len);
    cp = strings;

    /* pass start spaces */
    while (IS_BLANK(*cp))
        *cp++ = '\0';

    /*
     * Keep parsing the command string as long as there are any
     * arguments left.
     */

    while (*cp)
    {
        /*
         * Save the beginning of this argument.
         */
        argument = cp;
        cpOut    = cp;

        /*
         * Reset quoting and wildcarding for this argument.
         */
        quote = '\0';

        /*
         * Loop over the string collecting the next argument while
         * looking for quoted strings or quoted characters, and
         * remembering whether there are any wildcard characters
         * in the argument.
         */
        while (*cp)
        {
            ch = *cp++;

            /*
             * If we are not in a quote and we see a blank then
             * this argument is done.
             */
            if (IS_BLANK(ch) && (quote == '\0'))
                break;

            /*
             * If we see a backslash then accept the next
             * character no matter what it is.
             */
            if (ch == '\\')
            {
                ch = *cp++;

                /*
                 * Make sure there is a next character.
                 */
                if (ch == '\0')
                {
                    LOG0("Bad quoted character.");
                    return FALSE;
                }

                *cpOut++ = ch;

                continue;
            }

            /*
             * If we were in a quote and we saw the same quote
             * character again then the quote is done.
             */
            if (ch == quote)
            {
                quote = '\0';

                continue;
            }

            /*
             * If we weren't in a quote and we see either type
             * of quote character, then remember that we are
             * now inside of a quote.
             */
            if ((quote == '\0') && IS_QUOTE(ch))
            {
                quote = ch;

                continue;
            }

            /*
             * Store the character.
             */
            *cpOut++ = ch;
        }

        /*
         * Make sure that quoting is terminated properly.
         */
        if (quote)
        {
            LOG0("Unmatched quote character.");
            return FALSE;
        }

        /*
         * Null terminate the argument if it had shrunk, and then
         * skip over all blanks to the next argument, nulling them
         * out too.
         */
        if (cp != cpOut)
            *cpOut = '\0';

        while (IS_BLANK(*cp))
            *cp++ = '\0';

        /*
         * Now reallocate the argument table to hold the file name.
         */
        if (argCount + 1 >= argTableSize)
        {
            newArgTableSize = argCount + 2;

            newArgTable = (const char **)realloc(argTable, (sizeof(const char *) * newArgTableSize));

            if (newArgTable == NULL)
            {
                LOG0("No memory for arg list.");
                return FALSE;
            }

            argTable     = newArgTable;
            argTableSize = newArgTableSize;
        }

        *(argTable + argCount) = argument;

        /*
         * Add to the argument count.
         */
        argCount += 1;
    }

    if (argTable == NULL || argCount == 0)
    {
        *retArgc = 0;
        *retArgv = NULL;
        return TRUE;
    }

    /*
     * Null terminate the argument list and return it.
     */
    argTable[argCount] = NULL;

    *retArgc = argCount;
    *retArgv = (char **)argTable;

    return TRUE;
}
