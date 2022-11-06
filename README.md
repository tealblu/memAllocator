# memAllocator


DESIGN:

KNOWN PROBLEMS:
- free(): Suspected culprit for 'Segfault (core dumped)'
- realloc(): Suspected 'Segfault (core dumped)' due to 'free()'
- calloc(): Suspected 'Segfault (core dumped)' due to 'free()'

SOURCES: DEV
https://github.com/uzonwike/213-bibop-allocator/blob/master/allocator.c