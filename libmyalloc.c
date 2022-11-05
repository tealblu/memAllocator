// headers

#include "libmyalloc.h"

// global variables

// functions

/**
 * @brief Allocates memory.
 * 
 * @param byteNum Number of bytes to allocate.
 * @return void* Pointer to the allocated memory.
 */
void* malloc(size_t byteNum) {
    // simple allocator using mmap
    void* ptr = mmap(NULL, byteNum, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        return NULL;
    }

    return ptr;
}

/**
 * @brief Frees memory.
 * 
 * @param ptr Pointer to the memory to free.
 */
void free(void* ptr) {
    // free the memory
    munmap(ptr, BLOCKSIZE);
}

/**
 * @brief Reallocates memory.
 * 
 * @param ptr Pointer to the memory to reallocate.
 * @param byteNum Number of bytes to allocate.
 * @return void* Pointer to the reallocated memory.
 */
void* realloc(void* ptr, size_t byteNum) {
    free(ptr);

    return malloc(byteNum);
}

/**
 * @brief Allocates memory and initializes it to zero.
 * 
 * @param byteNum Number of bytes to allocate.
 * @return void* Pointer to the allocated memory.
 */
void* calloc(size_t byteNum) {
    void* ptr = malloc(byteNum);

    if (ptr == NULL) {
        return NULL;
    }

    memset(ptr, 0, byteNum);

    return ptr;
}