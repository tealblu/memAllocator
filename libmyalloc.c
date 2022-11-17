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

    // if no page exists, create one
    if(freeLists[index] == NULL) {
        // make a page of the appropriate size
        page* newPage = mmap(NULL, PAGESIZE + sizeof(page), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        newPage->size = (int) pow(2, index + 1);
        newPage->next = NULL;
        newPage->data = (block*) ((void*) newPage + sizeof(page));
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
            block* newBlock = (block*) ((void*) newPage + sizeof(page) + (i * (int) pow(2, index + 1)));
            newBlock->size = (int) pow(2, index + 1);
            newBlock->data = newPage->data + sizeof(page) + (i * newBlock->size);
            newBlock->next = NULL;

            // check if block is the first block in the page
            if(i == 0) {
                // if so, set page's data to point to block
                newPage->data = newBlock;
            } else {
                // if not, set previous block's next to point to block
                block* prevBlock = (block*) ((void*) newPage + sizeof(page) + ((i - 1) * (int) pow(2, index + 1)));
                prevBlock->next = newBlock;
            }
        }

        // remove first block from free list
        block* result = newPage->data;
        newPage->data = newPage->data->next;

        // return pointer to allocated memory
        return result->data;
    }

    // if a page exists and has a free block, use it
    else if(freeLists[index] != NULL) {
        // get last page in free list
        page* currPage = freeLists[index];
        while(currPage->next != NULL) {
            currPage = currPage->next;
        }

        // get first free block in page
        block* result = currPage->data;

        // ensure block is not only block in page
        if(result->next != NULL) {
            // remove block from free list
            currPage->data = result->next;
        } else {
            // set page's data to NULL
            currPage->data = NULL;
        }

        // return pointer to allocated memory
        return result->data;
    }

    // if a page exists but has no free blocks, create a new page
    else if (freeLists[index] != NULL && freeLists[index]->data == NULL) {
        // make a page of the appropriate size
        page* newPage = mmap(NULL, PAGESIZE + sizeof(page), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        newPage->size = PAGESIZE + sizeof(page);
        newPage->next = NULL;
        if(newPage == MAP_FAILED) {
            return NULL;
        }

        // find last page in free list
        page* currPage = freeLists[index];
        while(currPage->next != NULL) {
            currPage = currPage->next;
        }

        // add page to free list
        currPage->next = newPage;

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

        // remove first block from free list
        block* result = newPage->data;
        newPage->data = newPage->data->next;

        // return pointer to allocated memory
        return result->data;
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
    // check if pointer is NULL
    if(ptr == NULL) {
        return;
    }

    // figure out if ptr is in a page
    int inPage = 0; // <- used later
    for(int i = 0; i < 10; i++) {
        // if free list is empty, skip
        if(freeLists[i] != NULL) {
            // get the page ptr is in
            page* currPage = freeLists[i];
            while(currPage != NULL) {
                // check if ptr is in page
                if((intptr_t*) ptr >= (intptr_t*) currPage && (intptr_t*) ptr < (intptr_t*) currPage + PAGESIZE) {
                    // if so, set inPage to 1
                    inPage = 1;
                    break;
                } else {
                    // check if next page exists
                    if(currPage->next != NULL) {
                        // if so, set currPage to next page
                        currPage = currPage->next;
                    } else {
                        // if not, break
                        break;
                    }
                }
            }

            // if ptr is in currPage:
            if(inPage == 1) {
                // iteratively find the block ptr is in
                block* currBlock = currPage->data;
                currBlock->next = NULL;
                while(currBlock != NULL) {
                    // check if ptr is in block
                    if((intptr_t*) ptr >= (intptr_t*) currBlock && (intptr_t*) ptr < (intptr_t*) currBlock + currBlock->size) {
                        // if so, add block to free list
                        currBlock->next = currPage->data;
                        currPage->data = currBlock;
                        break;
                    } else {
                        // check if next block exists
                        if(currBlock->next != NULL) {
                            // if so, set currBlock to next block
                            currBlock = currBlock->next;
                        } else {
                            // if not, break
                            break;
                        }
                    }
                }
                
                // check if all blocks are free
                int allFree = 1; // <- used later
                block* currBlock2 = currPage->data;
                while(currBlock2 != NULL) {
                    // check if block is free
                    if(currBlock2->data != NULL) {
                        // if not, set allFree to 0
                        allFree = 0;
                        break;
                    } else {
                        // check if next block exists
                        if(currBlock2->next != NULL) {
                            // if so, set currBlock2 to next block
                            currBlock2 = currBlock2->next;
                        } else {
                            // if not, break
                            break;
                        }
                    }
                }

                // if all blocks are free:
                if(allFree == 1) {
                    // if page is the only page in the free list:
                    if(freeLists[i]->next == NULL) {
                        // free page
                        munmap(freeLists[i], PAGESIZE + sizeof(page));
                        freeLists[i] = NULL;
                    } else {
                        // if page is not the only page in the free list:
                        // find page before currPage
                        page* prevPage = freeLists[i];
                        while(prevPage->next != currPage) {
                            prevPage = prevPage->next;
                        }

                        // remove page from free list
                        prevPage->next = currPage->next;

                        // free page
                        munmap(currPage, PAGESIZE + sizeof(page));
                    }
                }

                // break out of loop
                break;
            }
        }
    }

    // if ptr is not in a page:
    if(inPage == 0) {
        // i don't think this works:
        // figure out size of allocated memory using pointer arithmetic
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