#include <stdarg.h>
#include <string.h>
#include <wchar.h>

int h2_guest_abs(int value) {
    return value < 0 ? (int)(0U - (unsigned int)value) : value;
}

int h2_guest_iswalnum(wchar_t character) {
    return (character >= L'0' && character <= L'9') ||
           (character >= L'A' && character <= L'Z') ||
           (character >= L'a' && character <= L'z');
}

int h2_guest_iswspace(wchar_t character) {
    return character == L' ' || (character >= L'\t' && character <= L'\r');
}

float fabsf(float value) {
    union { float value; unsigned int bits; } converted = { value };
    converted.bits &= 0x7fffffffU;
    return converted.value;
}

double fabs(double value) {
    union { double value; unsigned long long bits; } converted = { value };
    converted.bits &= 0x7fffffffffffffffULL;
    return converted.value;
}

float sqrtf(float value) {
    return __builtin_sqrtf(value);
}

double sqrt(double value) {
    union { double value; unsigned long long bits; } converted = { value };
    const unsigned long long magnitude = converted.bits & 0x7fffffffffffffffULL;
    if (!magnitude || magnitude >= 0x7ff0000000000000ULL)
        return value;
    if (converted.bits >> 63) {
        converted.bits = 0x7ff8000000000000ULL;
        return converted.value;
    }

    converted.bits = (converted.bits >> 1) + 0x1ff8000000000000ULL;
    double estimate = converted.value;
    for (int iteration = 0; iteration < 7; ++iteration)
        estimate = 0.5 * (estimate + value / estimate);
    return estimate;
}

double tan(double value) {
    union { double value; unsigned long long bits; } converted = { value };
    const unsigned long long magnitude = converted.bits & 0x7fffffffffffffffULL;
    if (magnitude >= 0x7ff0000000000000ULL || fabs(value) > 1.0e18) {
        converted.bits = 0x7ff8000000000000ULL;
        return converted.value;
    }

    const double half_pi = 1.57079632679489661923;
    const double scaled = value / half_pi;
    long long quadrant = (long long)(scaled + (scaled < 0.0 ? -0.5 : 0.5));
    const double x = value - (double)quadrant * half_pi;
    const double square = x * x;
    const double polynomial = 1.0 + square * (1.0 / 3.0 + square * (2.0 / 15.0 +
        square * (17.0 / 315.0 + square * (62.0 / 2835.0 +
        square * (1382.0 / 155925.0 + square * (21844.0 / 6081075.0 +
        square * (929569.0 / 638512875.0 + square *
        (6404582.0 / 10854718875.0 + square *
        (443861162.0 / 1856156927625.0)))))))));
    const double tangent = x * polynomial;
    return (quadrant & 1) ? -1.0 / tangent : tangent;
}

struct h2_guest_filetime {
    unsigned long low;
    unsigned long high;
};

long CompareFileTime(const struct h2_guest_filetime* first,
                     const struct h2_guest_filetime* second) {
    if (first->high != second->high)
        return first->high < second->high ? -1 : 1;
    if (first->low != second->low)
        return first->low < second->low ? -1 : 1;
    return 0;
}

typedef int (*h2_guest_compare_fn)(const void*, const void*);

void h2_guest_qsort(void* base, size_t count, size_t size,
                    h2_guest_compare_fn compare) {
    unsigned char* bytes = (unsigned char*)base;
    if (!size) return;
    for (size_t index = 1; index < count; ++index) {
        size_t current = index;
        while (current && compare(bytes + (current - 1) * size,
                                  bytes + current * size) > 0) {
            unsigned char* left = bytes + (current - 1) * size;
            unsigned char* right = bytes + current * size;
            for (size_t byte = 0; byte < size; ++byte) {
                unsigned char temporary = left[byte];
                left[byte] = right[byte];
                right[byte] = temporary;
            }
            --current;
        }
    }
}

static void guest_wide_emit(wchar_t* destination, size_t capacity,
                            size_t* count, wchar_t character) {
    if (*count < capacity)
        destination[*count] = character;
    ++*count;
}

static void guest_wide_padding(wchar_t* destination, size_t capacity,
                               size_t* count, wchar_t character,
                               size_t amount) {
    while (amount--)
        guest_wide_emit(destination, capacity, count, character);
}

int h2_guest_vsnwprintf(wchar_t* destination, size_t capacity,
                        const wchar_t* format, va_list arguments) {
    size_t count = 0;
    while (*format) {
        if (*format != L'%') {
            guest_wide_emit(destination, capacity, &count, *format++);
            continue;
        }
        ++format;
        if (*format == L'%') {
            guest_wide_emit(destination, capacity, &count, *format++);
            continue;
        }

        int left = 0;
        int zero = 0;
        while (*format == L'-' || *format == L'0' || *format == L'+' ||
               *format == L' ' || *format == L'#') {
            if (*format == L'-') left = 1;
            if (*format == L'0') zero = 1;
            ++format;
        }
        int width = 0;
        if (*format == L'*') {
            width = va_arg(arguments, int);
            if (width < 0) { left = 1; width = -width; }
            ++format;
        } else {
            while (*format >= L'0' && *format <= L'9')
                width = width * 10 + (*format++ - L'0');
        }
        int precision = -1;
        if (*format == L'.') {
            ++format;
            precision = 0;
            if (*format == L'*') {
                precision = va_arg(arguments, int);
                ++format;
            } else {
                while (*format >= L'0' && *format <= L'9')
                    precision = precision * 10 + (*format++ - L'0');
            }
        }
        int length = 0;
        if (*format == L'h' || *format == L'l') {
            wchar_t marker = *format++;
            length = marker == L'l' ? 1 : -1;
            if (*format == marker) { ++format; length *= 2; }
        }

        wchar_t specifier = *format ? *format++ : 0;
        if (specifier == L'c' || specifier == L'C') {
            wchar_t character = (wchar_t)va_arg(arguments, int);
            if (!left && width > 1)
                guest_wide_padding(destination, capacity, &count, L' ', width - 1);
            guest_wide_emit(destination, capacity, &count, character);
            if (left && width > 1)
                guest_wide_padding(destination, capacity, &count, L' ', width - 1);
            continue;
        }
        if (specifier == L's' || specifier == L'S') {
            if (specifier == L'S') {
                const char* text = va_arg(arguments, const char*);
                size_t size = 0;
                if (!text) text = "(null)";
                while (text[size] && (precision < 0 || size < (size_t)precision)) ++size;
                if (!left && width > (int)size)
                    guest_wide_padding(destination, capacity, &count, L' ', width - size);
                for (size_t index = 0; index < size; ++index)
                    guest_wide_emit(destination, capacity, &count,
                                    (unsigned char)text[index]);
                if (left && width > (int)size)
                    guest_wide_padding(destination, capacity, &count, L' ', width - size);
            } else {
                const wchar_t* text = va_arg(arguments, const wchar_t*);
                static const wchar_t null_text[] = {
                    '(', 'n', 'u', 'l', 'l', ')', 0
                };
                size_t size = 0;
                if (!text) text = null_text;
                while (text[size] && (precision < 0 || size < (size_t)precision)) ++size;
                if (!left && width > (int)size)
                    guest_wide_padding(destination, capacity, &count, L' ', width - size);
                for (size_t index = 0; index < size; ++index)
                    guest_wide_emit(destination, capacity, &count, text[index]);
                if (left && width > (int)size)
                    guest_wide_padding(destination, capacity, &count, L' ', width - size);
            }
            continue;
        }

        unsigned base = 10;
        int negative = 0;
        unsigned long long value;
        if (specifier == L'd' || specifier == L'i') {
            long long signed_value = length == 2 ? va_arg(arguments, long long) :
                                     length == 1 ? va_arg(arguments, long) :
                                                   va_arg(arguments, int);
            negative = signed_value < 0;
            value = negative ? 0ULL - (unsigned long long)signed_value :
                               (unsigned long long)signed_value;
        } else if (specifier == L'u' || specifier == L'x' ||
                   specifier == L'X' || specifier == L'p') {
            base = (specifier == L'x' || specifier == L'X' ||
                    specifier == L'p') ? 16 : 10;
            if (specifier == L'p')
                value = (unsigned long)va_arg(arguments, void*);
            else if (length == 2)
                value = va_arg(arguments, unsigned long long);
            else if (length == 1)
                value = va_arg(arguments, unsigned long);
            else
                value = va_arg(arguments, unsigned int);
        } else {
            guest_wide_emit(destination, capacity, &count, L'%');
            if (specifier) guest_wide_emit(destination, capacity, &count, specifier);
            continue;
        }

        wchar_t digits[32];
        size_t digit_count = 0;
        do {
            unsigned digit = (unsigned)(value % base);
            digits[digit_count++] = (wchar_t)(digit < 10 ? L'0' + digit :
                (specifier == L'X' ? L'A' : L'a') + digit - 10);
            value /= base;
        } while (value);
        while (precision > (int)digit_count)
            digits[digit_count++] = L'0';
        size_t field_size = digit_count + (negative ? 1u : 0u);
        wchar_t padding = zero && precision < 0 && !left ? L'0' : L' ';
        if (!left && width > (int)field_size && padding == L' ')
            guest_wide_padding(destination, capacity, &count, padding, width - field_size);
        if (negative)
            guest_wide_emit(destination, capacity, &count, L'-');
        if (!left && width > (int)field_size && padding == L'0')
            guest_wide_padding(destination, capacity, &count, padding, width - field_size);
        while (digit_count)
            guest_wide_emit(destination, capacity, &count, digits[--digit_count]);
        if (left && width > (int)field_size)
            guest_wide_padding(destination, capacity, &count, L' ', width - field_size);
    }
    if (count < capacity) {
        destination[count] = 0;
        return (int)count;
    }
    return -1;
}

void* memchr(const void* data, int value, size_t size) {
    const unsigned char* bytes = (const unsigned char*)data;
    for (size_t index = 0; index < size; ++index)
        if (bytes[index] == (unsigned char)value)
            return (void*)(bytes + index);
    return 0;
}

int memcmp(const void* first, const void* second, size_t size) {
    const unsigned char* a = (const unsigned char*)first;
    const unsigned char* b = (const unsigned char*)second;
    for (size_t index = 0; index < size; ++index)
        if (a[index] != b[index])
            return (int)a[index] - (int)b[index];
    return 0;
}

void* memcpy(void* destination, const void* source, size_t size) {
    unsigned char* out = (unsigned char*)destination;
    const unsigned char* in = (const unsigned char*)source;
    for (size_t index = 0; index < size; ++index)
        out[index] = in[index];
    return destination;
}

void* memmove(void* destination, const void* source, size_t size) {
    unsigned char* out = (unsigned char*)destination;
    const unsigned char* in = (const unsigned char*)source;
    if (out < in) {
        for (size_t index = 0; index < size; ++index)
            out[index] = in[index];
    } else if (out > in) {
        while (size) {
            --size;
            out[size] = in[size];
        }
    }
    return destination;
}

void* memset(void* destination, int value, size_t size) {
    unsigned char* out = (unsigned char*)destination;
    for (size_t index = 0; index < size; ++index)
        out[index] = (unsigned char)value;
    return destination;
}

char* strchr(const char* text, int character) {
    do {
        if (*text == (char)character)
            return (char*)text;
    } while (*text++);
    return 0;
}

int strcmp(const char* first, const char* second) {
    while (*first && *first == *second) {
        ++first;
        ++second;
    }
    return (unsigned char)*first - (unsigned char)*second;
}

char* strcpy(char* destination, const char* source) {
    char* result = destination;
    while ((*destination++ = *source++)) {}
    return result;
}

size_t strlen(const char* text) {
    const char* end = text;
    while (*end)
        ++end;
    return (size_t)(end - text);
}

int strncmp(const char* first, const char* second, size_t size) {
    while (size && *first && *first == *second) {
        ++first;
        ++second;
        --size;
    }
    return size ? (unsigned char)*first - (unsigned char)*second : 0;
}

char* strncpy(char* destination, const char* source, size_t size) {
    char* result = destination;
    while (size && *source) {
        *destination++ = *source++;
        --size;
    }
    while (size--) *destination++ = 0;
    return result;
}

char* strrchr(const char* text, int character) {
    const char* result = 0;
    do {
        if (*text == (char)character)
            result = text;
    } while (*text++);
    return (char*)result;
}

size_t strspn(const char* text, const char* accepted) {
    size_t count = 0;
    while (text[count]) {
        const char* match = accepted;
        while (*match && *match != text[count]) ++match;
        if (!*match) break;
        ++count;
    }
    return count;
}

char* strstr(const char* text, const char* pattern) {
    if (!*pattern) return (char*)text;
    for (; *text; ++text) {
        const char* a = text;
        const char* b = pattern;
        while (*a && *b && *a == *b) { ++a; ++b; }
        if (!*b) return (char*)text;
    }
    return 0;
}

wchar_t* wcschr(const wchar_t* text, wchar_t character) {
    do {
        if (*text == character) return (wchar_t*)text;
    } while (*text++);
    return 0;
}

int wcscmp(const wchar_t* first, const wchar_t* second) {
    while (*first && *first == *second) { ++first; ++second; }
    return (int)*first - (int)*second;
}

size_t wcslen(const wchar_t* text) {
    const wchar_t* end = text;
    while (*end) ++end;
    return (size_t)(end - text);
}

int wcsncmp(const wchar_t* first, const wchar_t* second, size_t size) {
    while (size && *first && *first == *second) {
        ++first; ++second; --size;
    }
    return size ? (int)*first - (int)*second : 0;
}

wchar_t* wcsncpy(wchar_t* destination, const wchar_t* source, size_t size) {
    wchar_t* result = destination;
    while (size && *source) { *destination++ = *source++; --size; }
    while (size--) *destination++ = 0;
    return result;
}

static wchar_t guest_lower(wchar_t value) {
    return value >= L'A' && value <= L'Z' ? value + (L'a' - L'A') : value;
}

int _wcsicmp(const wchar_t* first, const wchar_t* second) {
    while (*first && guest_lower(*first) == guest_lower(*second)) {
        ++first; ++second;
    }
    return (int)guest_lower(*first) - (int)guest_lower(*second);
}
