#include <stdio.h>
#include "libmyalloc.h"

int main(void) {
    puts("entering test 1");
    test_malloc_1();
}

void test_malloc_1() {
    // test malloc
    puts("attempting to allocate 100 bytes");
    void *result = malloc(100);

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
    result = realloc(result, 512);

    if(result == NULL) {
        printf("realloc failed\n");
    } else {
        printf("realloc succeeded\n");
    }

    free(result);

    // test calloc
    result = calloc(512);

    if(result == NULL) {
        printf("calloc failed\n");
    } else {
        printf("calloc succeeded\n");
    }
}