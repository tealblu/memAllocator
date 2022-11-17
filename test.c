#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define NUMBUFS 10
int bufsizes[NUMBUFS] = {2,3,7,14,65,36,700,12,15,64};

void repeatedMallocTest();

int main()
{
    repeatedMallocTest();
}

void repeatedMallocTest() {
    // allocate memory
    void* ptrs[NUMBUFS];
    for(int i = 0; i < NUMBUFS; i++) {
        printf("Allocating %d bytes\n", bufsizes[i]);
        ptrs[i] = malloc(bufsizes[i]);
        assert(ptrs[i] != NULL);
    }

    // free memory
    for(int i = 0; i < NUMBUFS; i++) {
        printf("Freeing %d bytes\n", bufsizes[i]);
        free(ptrs[i]);
        printf("Freed %d bytes\n", bufsizes[i]);
    }
}

void test1() {
    uint8_t *bufs[NUMBUFS];

	void * firstbreak = sbrk(0);
	void * midbreak;

	free(NULL); //just for kicks

	for (int i=0; i < NUMBUFS; i++)
	{
		//allocate the next block
		bufs[i] = malloc(bufsizes[i]);
        puts("malloc");
		assert(bufs[i] != NULL); //should never return NULL

		//write some data into the buffer
		memset(bufs[i], i, bufsizes[i]);
	}

	midbreak = sbrk(0);
	assert(firstbreak == midbreak);

	for (int i=0; i < NUMBUFS; i++)
	{
		//check whether or not the memory is still intact
		for (int b=0; b < bufsizes[i]; b++)
		{
			assert (bufs[i][b] == i);
		}

		free(bufs[i]);
	}

	void * lastbreak = sbrk(0);

	//verify that the program break never moved up.
	assert (firstbreak == lastbreak);

	return 0;
}