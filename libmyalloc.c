// headers

#include "libmyalloc.h"

// global variables
static void* freeList = NULL; // pointer to the free list

// functions

/**
 * @brief Allocates memory. Uses Big Bag of Pages (BBOP) style allocation. Uses segregated free list. Free list segregated in powers of 2.
 * 
 * @param byteNum Number of bytes to allocate.
 * @return void* Pointer to the allocated memory.
 */
void* malloc(size_t byteNum) {
    //  check if byteNum is 0
    if (byteNum == 0) {
        return NULL;
    }

    //  check if byteNum is a power of 2
    if ((byteNum & (byteNum - 1)) != 0) {
        //  if not, round up to the next power of 2
        byteNum = pow(2, ceil(log2(byteNum)));
    }

    //  check if byteNum is greater than 2^32
    if (byteNum > 4294967296) {
        return NULL;
    }

    // allocate memory
    void* ptr = NULL;

    //  check if freeList is NULL
    if (freeList == NULL) {
        // if so, allocate a new page
        freeList = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        //  check if mmap failed
        if (freeList == MAP_FAILED) {
            return NULL;
        }

        // allocate blocks
        int blockNum = PAGESIZE / BLOCKSIZE;
        int i;
        for (i = 0; i < blockNum; i++) {
            //  check if this is the last block
            if (i == blockNum - 1) {
                // if so, set the next pointer to NULL
                *((void**)freeList + i) = NULL;
            } else {
                // if not, set the next pointer to the next block
                *((void**)freeList + i) = freeList + (i + 1) * BLOCKSIZE;
            }
        }

        //  set ptr to the first block
        ptr = freeList;

        //  set freeList to the second block
        freeList = *((void**)freeList);

        //  set the next pointer of the first block to NULL
        *((void**)ptr) = NULL;

        //  return ptr
        return ptr;
    } else {
        // if not, check if there is a free block of the correct size
        void* curr = freeList;
        void* prev = NULL;

        //  loop through the free list
        while (curr != NULL) {
            //  check if the current block is the correct size
            if (byteNum == *(size_t*)curr) {
                //  if so, remove the block from the free list
                if (prev == NULL) {
                    freeList = *(void**)curr;
                } else {
                    *(void**)prev = *(void**)curr;
                }

                //  return the block
                return (void*)((size_t)curr + sizeof(size_t));
            }

            //  check if the current block is larger than the correct size
            if (byteNum < *(size_t*)curr) {
                //  if so, split the block
                void* newBlock = (void*)((size_t)curr + byteNum + sizeof(size_t));
                *(size_t*)newBlock = *(size_t*)curr - byteNum - sizeof(size_t);
                *(void**)newBlock = *(void**)curr;

                //  remove the block from the free list
                if (prev == NULL) {
                    freeList = *(void**)curr;
                } else {
                    *(void**)prev = *(void**)curr;
                }

                //  return the block
                return (void*)((size_t)curr + sizeof(size_t));
            }

            //  move to the next block
            prev = curr;
            curr = *(void**)curr;
        }
    }
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