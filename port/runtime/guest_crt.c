#include <string.h>
#include <wchar.h>

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
