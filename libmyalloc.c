// headers

#include "libmyalloc.h"

// global variables

// free lists, segregated by powers of 2 from 2 to 1024
block* freeLists[10];

// functions

/**
 * @brief Allocates memory. Uses Big Bag of Pages (BBOP) style allocation. Uses segregated free list. Free list segregated in powers of 2.
 * 
 * @param byteNum Number of bytes to allocate.
 * @return void* Pointer to the allocated memory.
 */
void* malloc(size_t byteNum) {
    // generate index of appropriate block list (log2(byteNum) - log2(2))
    int index = (int) log2(byteNum) - 1;

    // if index is out of bounds, return NULL
    if(index < 0) {
        return NULL;
    }

    // if index > 9, don't use free list
    if(index > 9) {
        // allocate memory
        void* result = mmap(NULL, byteNum, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(result == MAP_FAILED) {
            //return NULL;
        }

        // return pointer to allocated memory
        return result;
    }

    // if size of free list at index is 0:
    if(freeLists[index] == NULL) {
        // make a block of the appropriate size
        block* newBlock = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(newBlock == MAP_FAILED) {
            //return NULL;
        }

        // point block header at memory
        newBlock->data = (void*) newBlock + sizeof(block);

        // set up block
        newBlock->size = PAGESIZE - sizeof(block);
        newBlock->next = NULL;

        // add block to free list
        freeLists[index] = newBlock;

        // divide remainder of page into blocks of appropriate size
        int numBlocks = (PAGESIZE - sizeof(block)) / (int) pow(2, index + 1);

        // for each block:z
        for(int i = 0; i < numBlocks; i++) {
            // make a new chunk of memory of appropriate size
            block* newChunk = (void*) newBlock + (int) pow(2, index + 1);
            newChunk->data = (void*) newChunk + sizeof(block);

            // set size of chunk
            newChunk->size = (int) pow(2, index + 1) - sizeof(block);

            // set next pointer to NULL
            newChunk->next = NULL;

            // add chunk to front of list
            freeLists[index]->next = newChunk;
            freeLists[index] = newChunk;
        }

        // return pointer to allocated memory
        return freeLists[index]->data;
    }

    // if size of free list at index is not 0:
    else {
        // allocate block of memory from free list
        block* result = freeLists[index];

        // remove block from free list
        if(freeLists[index]->next != NULL) {
            freeLists[index] = freeLists[index]->next;
        }
        else {
            freeLists[index] = NULL;
        }

        result->next = NULL;

        // return pointer to allocated memory
        return result->data;
    }
}

/**
 * @brief Frees memory.
 * 
 * @param ptr Pointer to the memory to free.
 */
void free(void* ptr) {
    // find size of chunk, if it is in free list
    int size = 0;
    int index = 0;
    for(int i = 0; i < 10; i++) {
        if(freeLists[i] != NULL) {
            if(freeLists[i]->data == ptr) {
                size = freeLists[i]->size;
                index = i;
                break;
            }
        }
    }

    // if chunk is not in free list, size == round up to next power of 2
    if(size == 0) {
        size = (int) pow(2, ceil(log2((int) ptr - (int) ptr % PAGESIZE + PAGESIZE - (int) ptr)));
    }

    // generate index of appropriate free list (log2(byteNum) - log2(2))
    index = (int) log2(size) - 1;

    // if index is out of bounds, return NULL
    if(index < 0) {
        return;
    }

    // if index > 9, don't use free list
    if(index > 9) {
        // free memory
        munmap(ptr, size);
        return;
    }

    // round size down to nearest power of 2
    size = (int) pow(2, index + 1);

    // add the chunk to the appropriate free list
    block* newChunk = (void*) ptr - sizeof(block);
    newChunk->size = size - sizeof(block);
    newChunk->next = freeLists[index];
    freeLists[index] = newChunk;

    // if the free list is too big, free the page
    if(freeLists[index]->size > PAGESIZE) {
        // free page
        munmap(freeLists[index], PAGESIZE);

        // set free list to NULL
        freeLists[index] = NULL;
    }
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