#define _GNU_SOURCE
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

// defines
#define PAGESIZE 4096

// functions

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