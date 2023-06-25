#pragma once

#include <stdlib.h>

typedef struct
{
    int64_t count;
    const char *data;
} StringView;

static StringView sv_cStringAsStringView(const char *cString)
{
    return (StringView) {strlen(cString), cString};
}

static StringView sv_trimStart(StringView sv)
{
    int64_t i = 0;
    while (i < sv.count && isspace(sv.data[i])) ++i;

    return (StringView) {sv.count - i, sv.data + i};
}

static StringView sv_trimEnd(StringView sv)
{
    int64_t i = 0;
    while (i < sv.count && isspace(sv.data[sv.count - 1 - i])) ++i;

    return (StringView) {sv.count - i, sv.data};
}

static StringView sv_trim(StringView sv) { return sv_trimStart(sv_trimEnd(sv)); }

static StringView sv_trimByDelimiter(StringView *sv, char delimiter)
{
    int64_t i = 0;
    while (i < sv->count && sv->data[i] != delimiter) ++i;

    StringView result = {i, sv->data};

    if (i < sv->count)
    {
        sv->count -= i + 1;
        sv->data += i + 1;
    } else
    {
        sv->count -= i;
        sv->data += i;
    }

    return result;
}

static int sv_equals(StringView a, StringView b)
{
    return a.count != b.count ? 0 : memcmp(a.data, b.data, a.count) == 0;
}

static int sv_toInt(StringView sv)
{
    int result = 0;
    for (int64_t i = 0; i < sv.count && isdigit(sv.data[i]); ++i) result = result * 10 + sv.data[i] - '0';

    return result;
}

static StringView sv_readFile(const char *filePath)
{
    FILE *file = fopen(filePath, "r");
    if (file == NULL)
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not open file \"%s\" (%s)\n", filePath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (fseek(file, 0, SEEK_END) < 0)
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not set EOF for file \"%s\" (%s)\n", filePath,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    long fileSize = ftell(file);
    if (fileSize < 0)
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not determine length of file \"%s\" (%s)\n", filePath,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    char *fileContents = malloc(fileSize);
    if (fileContents == NULL)
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not allocate memory for file \"%s\" (%s)\n", filePath,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (fseek(file, 0, SEEK_SET) < 0)
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not read file \"%s\" (%s)\n", filePath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    int64_t readBytes = (int64_t) fread(fileContents, 1, fileSize, file);
    if (ferror(file))
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not get contents for file \"%s\" (%s)\n", filePath,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    fileContents[fileSize] = '\0';
    fclose(file);

    return (StringView) {readBytes, fileContents};
}
