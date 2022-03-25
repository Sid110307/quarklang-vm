#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define VALUES_CAPACITY 256

#define EXPONENT_MASK (((1LL << 11LL) - 1LL) << 52LL)
#define FRACTION_MASK ((1LL << 52LL) - 1LL)
#define SIGN_MASK (1LL << 63LL)
#define TYPE_MASK (((1LL << 4LL) - 1LL) << 48LL)
#define VALUE_MASK ((1LL << 48LL) - 1LL)

#define TYPE(n) ((1LL << 3LL) + n)

static double makeInfinity(void)
{
	uint64_t y = EXPONENT_MASK;
	return *(double*)&y;
}

static double setType(double x, uint64_t type)
{
	uint64_t y = *(uint64_t*)&x;
	y = (y & (~TYPE_MASK)) | (((TYPE_MASK >> 48LL) & type) << 48LL);
	return *(double*)&y;
}

static uint64_t getType(double x)
{
	return ((*(uint64_t*)&x & TYPE_MASK) >> 48LL);
}

static double setValue(double x, uint64_t value)
{
	uint64_t y = *(uint64_t*)&x;
	y = (y & (~VALUE_MASK)) | (value & VALUE_MASK);
	return *(double*)&y;
}

static uint64_t getValue(double x)
{
	return (*(uint64_t*)&x & VALUE_MASK);
}

#define INTEGER_TYPE 1
#define POINTER_TYPE 2

static int isDouble(double x)
{
	return !isnan(x);
}

static int isInteger(double x)
{
	return isnan(x) && getType(x) == TYPE(INTEGER_TYPE);
}

static int isPointer(double x)
{
	return isnan(x) && getType(x) == TYPE(POINTER_TYPE);
}

static double asDouble(double x)
{
	return x;
}

static uint64_t asInteger(double x)
{
	return getValue(x);
}

static void* asPointer(double x)
{
	return (void*)getValue(x);
}

static double boxDouble(double x)
{
	return x;
}

static double boxInteger(uint64_t x)
{
	return setValue(setType(makeInfinity(), TYPE(INTEGER_TYPE)), x);
}

static double boxPointer(void* x)
{
	return setValue(setType(makeInfinity(), TYPE(POINTER_TYPE)), (uint64_t)x);
}

int main(void)
{
	double pi = 3.14159265359;

	assert(pi == asDouble(boxDouble(pi)));
	assert(12345678LL == asInteger(boxInteger(12345678LL)));
	assert(&pi == asPointer(boxPointer(&pi)));

	printf("\033[1;34m-----\033[0m \033[1;32mOK\033[0m \033[1;34m-----\033[0m\n");

	return EXIT_SUCCESS;
}
