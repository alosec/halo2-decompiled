#include <stddef.h>
#include <stdio.h>

struct h2_guest_filetime {
    unsigned long low;
    unsigned long high;
};

typedef int (*h2_guest_compare_fn)(const void*, const void*);

extern long CompareFileTime(const struct h2_guest_filetime* first,
                            const struct h2_guest_filetime* second);
extern void h2_guest_qsort(void* base, size_t count, size_t size,
                           h2_guest_compare_fn compare);

static int compare_ints(const void* first, const void* second) {
    const int a = *(const int*)first;
    const int b = *(const int*)second;
    return (a > b) - (a < b);
}

int main(void) {
    int values[] = {7, -2, 7, 0, 3, -9};
    const int expected[] = {-9, -2, 0, 3, 7, 7};
    h2_guest_qsort(values, sizeof(values) / sizeof(values[0]),
                   sizeof(values[0]), compare_ints);
    for (size_t index = 0; index < sizeof(values) / sizeof(values[0]); ++index) {
        if (values[index] != expected[index]) {
            fprintf(stderr, "guest qsort mismatch at %zu: %d != %d\n",
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
        fputs("guest file-time comparison failed\n", stderr);
        return 1;
    }
    puts("guest CRT sort and file-time services passed");
    return 0;
}
