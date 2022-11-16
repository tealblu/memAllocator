// headers
#include "libmyalloc.h"

// global variables

// free lists, segregated by powers of 2 from 2 to 1024 (10 lists)
page* freeLists[10];

// functions
void __attribute__ ((constructor)) lib_start() {
    // initialize free lists
    for(int i = 0; i < 10; i++) {
        freeLists[i] = NULL;
    }
}

void __attribute__ ((destructor)) lib_kill() {
    // free all pages
    for(int i = 0; i < 10; i++) {
        page* currPage = freeLists[i];
        if (currPage != NULL) {
            munmap(currPage, PAGESIZE);
        }
    }
}

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
            return NULL;
        }

        // return pointer to allocated memory
        return result;
    }

    // if size of free list at index is 0:
    if(freeLists[index] == NULL) {
        // make a page of the appropriate size
        page* newPage = mmap(NULL, PAGESIZE + sizeof(page), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        newPage->size = (int) pow(2, index + 1);
        if(newPage == MAP_FAILED) {
            return NULL;
        }

        // add page to free list
        freeLists[index] = newPage;

        // divide remainder of page into blocks of appropriate size
        int numBlocks = (PAGESIZE - sizeof(page)) / (int) pow(2, index + 1);

        // for each block:
        for(int i = 0; i < numBlocks; i++) {
            // make a new block of memory in the page, offset by size of page struct and size of previous blocks
            block* newBlock = (block*) ((char*) newPage + sizeof(page) + (i * newPage->size));
            newBlock->size = (int) pow(2, index + 1);
            newBlock->data = newPage->data + sizeof(page) + (i * newBlock->size);
            newBlock->next = NULL;

            // check if block is the first block in the page
            if(i == 0) {
                // if so, set page's data to point to block
                newPage->data = newBlock;
            } else {
                // if not, set previous block's next to point to block
                block* prevBlock = newPage->data + sizeof(page) + ((i - 1) * newBlock->size);
                prevBlock->next = newBlock;
            }
        }

        // return pointer to allocated memory
        return newPage->data;
    }

    // if size of free list at index is not 0:
    else {
        // get first block from free list
        block* result = freeLists[index]->data;

        // remove block from free list
        freeLists[index]->data = result->next;
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
    // figure out if ptr is in a page
    int inPage = 0; // <- used later
    for(int i = 0; i < 10; i++) {
        if(freeLists[i] != NULL) {
            // if ptr is above the page start and below the page end:
            if((intptr_t) ptr >= (intptr_t) freeLists[i]->data && (intptr_t) ptr < (intptr_t) freeLists[i]->data + freeLists[i]->size) {
                inPage = 1;

                // get block size
                int blockSize = (int) pow(2, i + 1);

                // get block index from pointer arithmetic
                int blockIndex = ((intptr_t) ptr - (intptr_t) freeLists[i]->data) / blockSize;

                // get block
                block* freeBlock = freeLists[i]->data + (blockIndex * blockSize);

                // add block to front of free list
                freeBlock->next = freeLists[i]->data;
                freeLists[i]->data = freeBlock;

                // free page if all blocks are free
                int allFree = 1;
                for(int j = 0; j < freeLists[i]->size / blockSize; j++) {
                    block* checkBlock = freeLists[i]->data + (j * blockSize);
                    if(checkBlock->next != NULL) {
                        allFree = 0;
                        break;
                    }
                }

                // if all blocks are free:
                if(allFree == 1) {
                    // free page
                    munmap(freeLists[i]->data, PAGESIZE);

                    // remove page from free list
                    freeLists[i] = NULL;
                }

                // break out of loop
                break;
            }
        }
    }

    // if chunk is not in free list, size == round up to next power of 2
    if(inPage == 0) {
        // figure out size of allocated memory outside free lists
        int size = (int) pow(2, (int) log2((intptr_t) ptr) + 1);

        // round up to next power of 2
        int roundedSize = 1;
        while(roundedSize < size) {
            roundedSize *= 2;
        }

        // free chunk
        munmap(ptr, roundedSize);
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