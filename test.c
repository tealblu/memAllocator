// A simple test file for the library


#include "libmyalloc.h"

int main(void) {
    test_malloc_1();
}

void test_malloc_1() {
    // test malloc
    void *result = malloc(10);

    if(result == NULL) {
        printf("malloc failed\n");
    } else {
        printf("malloc succeeded\n");
    }

}