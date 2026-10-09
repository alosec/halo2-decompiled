#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

struct h2_guest_filetime {
    unsigned long low;
    unsigned long high;
};

typedef int (*h2_guest_compare_fn)(const void*, const void*);

extern int h2_guest_abs(int value);
extern int h2_guest_iswalnum(wchar_t character);
extern int h2_guest_iswspace(wchar_t character);
extern float fabsf(float value);
extern double fabs(double value);
extern float sqrtf(float value);
extern double sqrt(double value);
extern double tan(double value);
extern long CompareFileTime(const struct h2_guest_filetime* first,
                            const struct h2_guest_filetime* second);
extern void h2_guest_qsort(void* base, size_t count, size_t size,
                           h2_guest_compare_fn compare);
extern int h2_guest_vsnwprintf(wchar_t* destination, size_t size,
                               const wchar_t* format, va_list arguments);

static int guest_swprintf(wchar_t* destination, size_t size,
                          const wchar_t* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    int result = h2_guest_vsnwprintf(destination, size, format, arguments);
    va_end(arguments);
    return result;
}

static int compare_ints(const void* first, const void* second) {
    const int a = *(const int*)first;
    const int b = *(const int*)second;
    return (a > b) - (a < b);
}

int main(void) {
    if (h2_guest_abs(-9) != 9 || h2_guest_abs(0) != 0 ||
        h2_guest_abs(17) != 17 || h2_guest_abs((int)0x80000000U) != (int)0x80000000U) {
        puts("guest integer absolute-value helper failed");
        return 1;
    }
    if (!h2_guest_iswalnum(L'A') || !h2_guest_iswalnum(L'z') ||
        !h2_guest_iswalnum(L'7') || h2_guest_iswalnum(L'_') ||
        !h2_guest_iswspace(L' ') || !h2_guest_iswspace(L'\n') ||
        h2_guest_iswspace(L'A')) {
        puts("guest wide character classification failed");
        return 1;
    }
    union { float value; unsigned int bits; } negative_zero_f = { -0.0f };
    union { double value; unsigned long long bits; } negative_zero = { -0.0 };
    negative_zero_f.value = fabsf(negative_zero_f.value);
    negative_zero.value = fabs(negative_zero.value);
    if (negative_zero_f.bits != 0 || negative_zero.bits != 0 ||
        fabsf(-3.5f) != 3.5f || fabs(-7.25) != 7.25) {
        puts("guest absolute-value helpers failed");
        return 1;
    }
    const float root_two_f = sqrtf(2.0f);
    const double root_two = sqrt(2.0);
    const double tangent_quarter_turn = tan(0.78539816339744830962);
    const double tangent_negative = tan(-0.25);
    if (sqrtf(0.0f) != 0.0f || sqrtf(81.0f) != 9.0f ||
        root_two_f < 1.4142134f || root_two_f > 1.4142137f ||
        sqrt(0.0) != 0.0 || sqrt(81.0) != 9.0 ||
        root_two < 1.414213562373094 || root_two > 1.414213562373096 ||
        tangent_quarter_turn < 0.999998 || tangent_quarter_turn > 1.000002 ||
        tangent_negative > -0.25533 || tangent_negative < -0.25535) {
        puts("guest square-root/tangent helpers failed");
        return 1;
    }

    int values[] = {7, -2, 7, 0, 3, -9};
    const int expected[] = {-9, -2, 0, 3, 7, 7};
    h2_guest_qsort(values, sizeof(values) / sizeof(values[0]),
                   sizeof(values[0]), compare_ints);
    for (size_t index = 0; index < sizeof(values) / sizeof(values[0]); ++index) {
        if (values[index] != expected[index]) {
            printf("guest qsort mismatch at %zu: %d != %d\n",
                   index, values[index], expected[index]);
            return 1;
        }
    }

    const struct h2_guest_filetime earlier = {0xffffffffUL, 3};
    const struct h2_guest_filetime later = {0, 4};
    const struct h2_guest_filetime same = earlier;
    if (CompareFileTime(&earlier, &later) != -1 ||
        CompareFileTime(&later, &earlier) != 1 ||
        CompareFileTime(&earlier, &same) != 0) {
        puts("guest file-time comparison failed");
        return 1;
    }
    wchar_t formatted[32];
    static const wchar_t expected_text[] = L"Halo 0042 BEEF ok";
    int formatted_size = guest_swprintf(formatted, 32, L"%s %04d %X %S",
                                        L"Halo", 42, 0xbeef, "ok");
    if (formatted_size != 17 || wcscmp(formatted, expected_text)) {
        puts("guest wide formatting failed");
        return 1;
    }
    wchar_t truncated[4] = {L'x', L'x', L'x', L'x'};
    if (guest_swprintf(truncated, 4, L"hello") != -1 ||
        truncated[0] != L'h' || truncated[3] != L'l') {
        puts("guest wide formatting truncation failed");
        return 1;
    }

    puts("guest CRT sort, file-time, and wide-format services passed");
    return 0;
}
