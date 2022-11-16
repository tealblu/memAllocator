// A simple test file for the library


#include "libmyalloc.h"
#include <stdio.h>

int main(void) {
    test_malloc_1();
}

void test_malloc_1() {
    // test malloc
    void *result = malloc(512);

    if(result == NULL) {
        printf("malloc failed\n");
    } else {
        printf("malloc succeeded\n");
    }

    // test free
    free(result);

    if(result == NULL) {
        printf("free failed\n");
    } else {
        printf("free succeeded\n");
    }

    // test realloc
    result = realloc(result, 0);

    if(result == NULL) {
        printf("realloc failed\n");
    } else {
        printf("realloc succeeded\n");
    }

    // test calloc
    result = calloc(0);

    if(result == NULL) {
        printf("calloc failed\n");
    } else {
        printf("calloc succeeded\n");
    }
}