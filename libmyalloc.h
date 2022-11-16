#define _GNU_SOURCE
#include <fcntl.h>
#include <stddef.h>
#include <sys/mman.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

// defines
#define PAGESIZE 4096

// structs
typedef struct block {
    void* data; // pointer to chunk of memory
    size_t size; // size of chunk of memory
    struct block* next; // pointer to next block in free list
} block;

typedef struct page {
    void* data; // pointer to first block in page
    size_t size; // size of chunk of memory
} page;

// functions
void __attribute__ ((constructor)) lib_start();
void __attribute__ ((destructor)) lib_kill();

/**
 * @brief Allocates memory.
 * 
 * @param byteNum Number of bytes to allocate.
 * @return void* Pointer to the allocated memory.
 */
void* malloc(size_t byteNum);

/**
 * @brief Frees memory.
 * 
 * @param ptr Pointer to the memory to free.
 */
void free(void* ptr);

/**
 * @brief Reallocates memory.
 * 
 * @param ptr Pointer to the memory to reallocate.
 * @param byteNum Number of bytes to allocate.
 * @return void* Pointer to the reallocated memory.
 */
void* realloc(void* ptr, size_t byteNum);

/**
 * @brief Allocates memory and initializes it to zero.
 * 
 * @param byteNum Number of bytes to allocate.
 * @return void* Pointer to the allocated memory.
 */
void* calloc(size_t byteNum);