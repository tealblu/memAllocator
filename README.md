# memAllocator


DESIGN:
This memory allocation uses a Big Bag of Pages design. An array of pointers is used to store a free list, which contains pre-allocated blocks of memory that the program can give up when requested. For requests that are larger than the pre-allocated blocks in the free list, the program rounds up the allocation size to the nearest power of 2 and allocates a new page of memory. The program keeps track of the number of pages allocated and the number of pages in the free list. When the number of pages allocated is greater than the number of pages in the free list, the program will allocate a new page of memory and add it to the free list. When the number of pages allocated is less than the number of pages in the free list, the program will free the last page of memory in the free list and remove it from the free list.

KNOWN PROBLEMS:
None (which I know is a problem in and of itself)

SOURCES:
https://curtsinger.cs.grinnell.edu/teaching/2017S/CSC213/labs/04.malloc.html
https://legacy.cs.indiana.edu/~dyb/pubs/bibop.pdf