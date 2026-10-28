/* More of the freestanding guest C runtime: the narrow printf family, number
 * parsing, case-insensitive compares, rand/time, and the libm calls the
 * recovered game makes that have no AArch64 instruction. */
#include "h2/host_abi.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

typedef struct {
    char* destination;
    size_t capacity;
    size_t count;
} h2_narrow_output;

static void narrow_emit(h2_narrow_output* out, char character) {
    if (out->count + 1 < out->capacity)
        out->destination[out->count] = character;
    ++out->count;
}

static void narrow_repeat(h2_narrow_output* out, char character, int amount) {
    while (amount-- > 0)
        narrow_emit(out, character);
}

static void narrow_field(h2_narrow_output* out, const char* text, size_t length,
                         int width, int left, char pad) {
    const int padding = width > (int)length ? width - (int)length : 0;
    size_t index = 0;
    if (!left && pad == '0' && length && (text[0] == '-' || text[0] == '+' || text[0] == ' ')) {
        narrow_emit(out, text[0]);
        index = 1;
    }
    if (!left)
        narrow_repeat(out, pad, padding);
    for (; index < length; ++index)
        narrow_emit(out, text[index]);
    if (left)
        narrow_repeat(out, ' ', padding);
}

static size_t format_unsigned(char* buffer, unsigned long long value, unsigned base, int upper,
                              int precision) {
    char reversed[32];
    size_t length = 0;
    const char* digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    do {
        reversed[length++] = digits[value % base];
        value /= base;
    } while (value);
    while ((int)length < precision && length < sizeof(reversed))
        reversed[length++] = '0';
    for (size_t i = 0; i < length; ++i)
        buffer[i] = reversed[length - 1 - i];
    return length;
}

/* %f with the given precision, rounded half away from zero. */
static size_t format_fixed(char* buffer, size_t capacity, double value, int precision, int sign) {
    size_t length = 0;
    if (value != value) {
        memcpy(buffer, "nan", 3);
        return 3;
    }
    if (value < 0 || (value == 0 && 1.0 / value < 0)) {
        buffer[length++] = '-';
        value = -value;
    } else if (sign) {
        buffer[length++] = (char)sign;
    }
    if (value > 1e18) {
        memcpy(buffer + length, "inf", 3);
        return length + 3;
    }
    if (precision > 9)
        precision = 9;
    unsigned long long scale = 1;
    for (int i = 0; i < precision; ++i)
        scale *= 10;
    unsigned long long whole = (unsigned long long)value;
    double fraction = value - (double)whole;
    unsigned long long digits = (unsigned long long)(fraction * (double)scale + 0.5);
    if (digits >= scale) {
        digits -= scale;
        ++whole;
    }
    length += format_unsigned(buffer + length, whole, 10, 0, 1);
    if (precision > 0 && length + 1 + (size_t)precision < capacity) {
        buffer[length++] = '.';
        length += format_unsigned(buffer + length, digits, 10, 0, precision);
    }
    return length;
}

int _vsnprintf(char* destination, size_t capacity, const char* format, va_list arguments) {
    h2_narrow_output out = {destination, capacity, 0};
    while (*format) {
        if (*format != '%') {
            narrow_emit(&out, *format++);
            continue;
        }
        ++format;
        int left = 0, sign = 0, alternate = 0;
        char pad = ' ';
        for (;; ++format) {
            if (*format == '-') left = 1;
            else if (*format == '0') pad = '0';
            else if (*format == '+') sign = '+';
            else if (*format == ' ') { if (!sign) sign = ' '; }
            else if (*format == '#') alternate = 1;
            else break;
        }
        int width = 0;
        if (*format == '*') {
            width = va_arg(arguments, int);
            if (width < 0) { left = 1; width = -width; }
            ++format;
        } else {
            while (*format >= '0' && *format <= '9')
                width = width * 10 + (*format++ - '0');
        }
        int precision = -1;
        if (*format == '.') {
            ++format;
            precision = 0;
            if (*format == '*') {
                precision = va_arg(arguments, int);
                ++format;
            } else {
                while (*format >= '0' && *format <= '9')
                    precision = precision * 10 + (*format++ - '0');
            }
        }
        int size = 0; /* 1 long, 2 long long, -1 short */
        for (;; ++format) {
            if (*format == 'l') ++size;
            else if (*format == 'h') size = -1;
            else if (*format == 'I' && format[1] == '6' && format[2] == '4') { size = 2; format += 2; }
            else break;
        }
        char buffer[64];
        size_t length = 0;
        const char conversion = *format ? *format++ : 0;
        switch (conversion) {
        case 'd':
        case 'i': {
            long long value = size >= 2 ? va_arg(arguments, long long) : va_arg(arguments, int);
            if (size < 0) value = (short)value;
            if (value < 0) buffer[length++] = '-';
            else if (sign) buffer[length++] = (char)sign;
            unsigned long long magnitude = value < 0 ? 0ull - (unsigned long long)value : (unsigned long long)value;
            length += format_unsigned(buffer + length, magnitude, 10, 0, precision < 0 ? 1 : precision);
            narrow_field(&out, buffer, length, width, left, precision < 0 ? pad : ' ');
            break;
        }
        case 'u':
        case 'x':
        case 'X':
        case 'o': {
            unsigned long long value = size >= 2 ? va_arg(arguments, unsigned long long)
                                                 : va_arg(arguments, unsigned int);
            if (size < 0) value = (unsigned short)value;
            const unsigned base = conversion == 'u' ? 10 : (conversion == 'o' ? 8 : 16);
            if (alternate && base == 16 && value) {
                buffer[length++] = '0';
                buffer[length++] = conversion;
            }
            length += format_unsigned(buffer + length, value, base, conversion == 'X',
                                      precision < 0 ? 1 : precision);
            narrow_field(&out, buffer, length, width, left, precision < 0 ? pad : ' ');
            break;
        }
        case 'p': {
            const unsigned int value = (unsigned int)(size_t)va_arg(arguments, void*);
            length = format_unsigned(buffer, value, 16, 1, 8);
            narrow_field(&out, buffer, length, width, left, ' ');
            break;
        }
        case 'c':
            buffer[0] = (char)va_arg(arguments, int);
            narrow_field(&out, buffer, 1, width, left, ' ');
            break;
        case 's': {
            const char* text = va_arg(arguments, const char*);
            if (!text) text = "(null)";
            size_t text_length = strlen(text);
            if (precision >= 0 && (size_t)precision < text_length) text_length = (size_t)precision;
            narrow_field(&out, text, text_length, width, left, ' ');
            break;
        }
        case 'S': {
            const wchar_t* text = va_arg(arguments, const wchar_t*);
            size_t count = 0;
            if (text)
                while (text[count] && (precision < 0 || (int)count < precision) && count < sizeof(buffer))
                    buffer[count] = (char)text[count], ++count;
            narrow_field(&out, buffer, count, width, left, ' ');
            break;
        }
        case 'f':
        case 'F':
        case 'g':
        case 'G':
        case 'e':
        case 'E': {
            const double value = va_arg(arguments, double);
            length = format_fixed(buffer, sizeof(buffer), value, precision < 0 ? 6 : precision, sign);
            narrow_field(&out, buffer, length, width, left, pad);
            break;
        }
        case '%':
            narrow_emit(&out, '%');
            break;
        case 0:
            break;
        default:
            narrow_emit(&out, '%');
            narrow_emit(&out, conversion);
            break;
        }
    }
    if (out.capacity)
        out.destination[out.count < out.capacity ? out.count : out.capacity - 1] = 0;
    /* MSVC _vsnprintf: -1 when the output did not fit. */
    return out.count < out.capacity ? (int)out.count : -1;
}

int vsprintf(char* destination, const char* format, va_list arguments) {
    return _vsnprintf(destination, 0x7fffffff, format, arguments);
}

int sprintf(char* destination, const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    const int result = _vsnprintf(destination, 0x7fffffff, format, arguments);
    va_end(arguments);
    return result;
}

int _snprintf(char* destination, size_t capacity, const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    const int result = _vsnprintf(destination, capacity, format, arguments);
    va_end(arguments);
    return result;
}

/* No console or C streams on the title: printing goes nowhere and files are
   opened through the Win32 calls instead. */
FILE* fopen(const char* path, const char* mode) {
    (void)path;
    (void)mode;
    return 0;
}
int fclose(FILE* stream) { (void)stream; return 0; }
char* fgets(char* buffer, int count, FILE* stream) { (void)buffer; (void)count; (void)stream; return 0; }
int fscanf(FILE* stream, const char* format, ...) { (void)stream; (void)format; return -1; }
int fputs(const char* text, FILE* stream) { (void)text; (void)stream; return 0; }
int puts(const char* text) { (void)text; return 0; }
int printf(const char* format, ...) { (void)format; return 0; }
int fprintf(FILE* stream, const char* format, ...) { (void)stream; (void)format; return 0; }

static int narrow_lower(int character) {
    return character >= 'A' && character <= 'Z' ? character + 32 : character;
}

int _stricmp(const char* first, const char* second) {
    for (;; ++first, ++second) {
        const int a = narrow_lower((unsigned char)*first);
        const int b = narrow_lower((unsigned char)*second);
        if (a != b || !a)
            return a - b;
    }
}

int _strnicmp(const char* first, const char* second, size_t size) {
    for (; size; --size, ++first, ++second) {
        const int a = narrow_lower((unsigned char)*first);
        const int b = narrow_lower((unsigned char)*second);
        if (a != b || !a)
            return a - b;
    }
    return 0;
}

char* strpbrk(const char* text, const char* accepted) {
    for (; *text; ++text)
        for (const char* candidate = accepted; *candidate; ++candidate)
            if (*text == *candidate)
                return (char*)text;
    return 0;
}

char* strcat(char* destination, const char* source) {
    strcpy(destination + strlen(destination), source);
    return destination;
}

char* strncat(char* destination, const char* source, size_t size) {
    char* end = destination + strlen(destination);
    while (size-- && *source)
        *end++ = *source++;
    *end = 0;
    return destination;
}

long strtol(const char* text, char** end, int base) {
    while (*text == ' ' || *text == '\t' || *text == '\n')
        ++text;
    int negative = 0;
    if (*text == '-' || *text == '+')
        negative = *text++ == '-';
    if ((base == 0 || base == 16) && text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
        text += 2;
        base = 16;
    } else if (base == 0) {
        base = text[0] == '0' ? 8 : 10;
    }
    unsigned long value = 0;
    for (;; ++text) {
        int digit;
        if (*text >= '0' && *text <= '9') digit = *text - '0';
        else if (*text >= 'a' && *text <= 'z') digit = *text - 'a' + 10;
        else if (*text >= 'A' && *text <= 'Z') digit = *text - 'A' + 10;
        else break;
        if (digit >= base)
            break;
        value = value * (unsigned long)base + (unsigned long)digit;
    }
    if (end)
        *end = (char*)text;
    return negative ? -(long)value : (long)value;
}

unsigned long strtoul(const char* text, char** end, int base) {
    return (unsigned long)strtol(text, end, base);
}

int atoi(const char* text) { return (int)strtol(text, 0, 10); }
long atol(const char* text) { return strtol(text, 0, 10); }
long labs(long value) { return value < 0 ? -value : value; }

wchar_t towlower(wchar_t character) {
    return character >= L'A' && character <= L'Z' ? (wchar_t)(character + 32) : character;
}

wchar_t towupper(wchar_t character) {
    return character >= L'a' && character <= L'z' ? (wchar_t)(character - 32) : character;
}

/* The MSVC CRT's linear congruential generator. */
static unsigned long guest_random_state = 1;

void srand(unsigned int seed) { guest_random_state = seed; }

int rand(void) {
    guest_random_state = guest_random_state * 214013ul + 2531011ul;
    return (int)((guest_random_state >> 16) & 0x7fff);
}

long time(long* result) {
    const long now = (long)((host_h2_system_time() - 116444736000000000ull) / 10000000ull);
    if (result)
        *result = now;
    return now;
}

double floor(double value) { return __builtin_floor(value); }
double ceil(double value) { return __builtin_ceil(value); }
float floorf(float value) { return __builtin_floorf(value); }
float ceilf(float value) { return __builtin_ceilf(value); }
double fmod(double numerator, double denominator);
float fmodf(float numerator, float denominator) { return (float)fmod(numerator, denominator); }
double atan(double value) { return host_h2_math_atan(value); }
double atan2(double y, double x) { return host_h2_math_atan2(y, x); }
double asin(double value) { return host_h2_math_asin(value); }
float cosf(float value) { return (float)host_h2_cos(value); }
