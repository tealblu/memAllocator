// A simple test file for the library


#include "libmyalloc.h"

int main(int argc, char** argv) {
    test_malloc_1(argv[1]);
}

void test_malloc_1(size_t size) {
    void *result = malloc(size);

    if(result == NULL) {
        printf("malloc failed\n");
    } else {
        printf("malloc succeeded\n");
    }
}