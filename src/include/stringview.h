#ifndef QUARK_VM_STRINGVIEW_H_
#define QUARK_VM_STRINGVIEW_H_

#include <stdlib.h>

typedef struct
{
	size_t count;
	const char* data;
} StringView;

StringView sv_cstrAsStringView(const char* cstr);
StringView sv_trimStart(StringView sv);
StringView sv_trimEnd(StringView sv);
StringView sv_trim(StringView sv);
StringView sv_trimByDelimeter(StringView* sv, char delimeter);
int sv_equals(StringView a, StringView b);
int sv_toInt(StringView sv);
StringView sv_readFile(const char* filePath);

#endif //! QUARK_VM_COMPILER_H_
#ifdef QUARK_VM_STRINGVIEW_IMPLEMENTATION

StringView sv_cstrAsStringView(const char* cstr)
{
	return (StringView)
	{
		.count = strlen(cstr),
			.data = cstr,
	};
}

StringView sv_trimStart(StringView sv)
{
	size_t i = 0;
	while (i < sv.count && isspace(sv.data[i])) i += 1;

	return (StringView)
	{
		.count = sv.count - i,
			.data = sv.data + i,
	};
}

StringView sv_trimEnd(StringView sv)
{
	size_t i = 0;
	while (i < sv.count && isspace(sv.data[sv.count - 1 - i])) i += 1;

	return (StringView)
	{
		.count = sv.count - i,
			.data = sv.data,
	};
}

StringView sv_trim(StringView sv)
{
	return sv_trimStart(sv_trimEnd(sv));
}

StringView sv_trimByDelimeter(StringView* sv, char delimeter)
{
	size_t i = 0;
	while (i < sv->count && sv->data[i] != delimeter) i += 1;

	StringView result =
	{
		.count = i,
			.data = sv->data,
	};

	if (i < sv->count)
	{
		sv->count -= i + 1;
		sv->data += i + 1;
	}
	else
	{
		sv->count -= i;
		sv->data += i;
	}

	return result;
}

int sv_equals(StringView a, StringView b)
{
	if (a.count != b.count) return 0;
	else return memcmp(a.data, b.data, a.count) == 0;
}

int sv_toInt(StringView sv)
{
	int result = 0;

	for (size_t i = 0; i < sv.count && isdigit(sv.data[i]); ++i)
		result = result * 10 + sv.data[i] - '0';

	return result;
}

StringView sv_readFile(const char* filePath)
{
	FILE* file = fopen(filePath, "r");
	if (file == NULL)
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not open file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fseek(file, 0, SEEK_END) < 0)
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not read file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	long fileSize = ftell(file);
	if (fileSize < 0)
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not read file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	char* fileContent = malloc(fileSize + 1);
	if (fileContent == NULL)
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not allocate memory for file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fseek(file, 0, SEEK_SET) < 0)
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not read file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	size_t readBytes = fread(fileContent, 1, fileSize, file);
	if (ferror(file))
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not read file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	fileContent[fileSize] = '\0';

	fclose(file);
	return (StringView)
	{
		.count = readBytes,
			.data = fileContent,
	};
}

#endif //! QUARK_VM_STRINGVIEW_IMPLEMENTATION