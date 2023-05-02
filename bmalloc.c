#include <unistd.h>
#include <stdio.h>
#include "bmalloc.h"
/* include */
#include <sys/mman.h>

bm_option bm_mode = BestFit ;
bm_header bm_list_head = { 0, 0, 0x0 } ;

/* range of memory allocation size, power of 2 */
#define MAX_POWER 12;
#define MIN_POWER 4;
/* size of the header of the memory block, in bytes */
#define HEADER_SIZE 9;

/* global variable to store the starting address of the heap */
bm_header *heap_ptr = NULL:

/* returns the header address of the suspected sibling block of h */
void * sibling (void * h)
{
	int size_of_node = ((bm_header *) h)->size;
    int i;

	/* size of 4096 has no sibling */
	if (size_of_node == MAX_POWER)
		return NULL;

	for (itr = bm_list_head, i = 1; itr != NULL; itr = itr->next, i++) {
		if (itr == (bm_header *) h) {
			if (i % 2 == 0) { // if even index, check left side

			}
			else { // if odd index, check right side

			}
		}
	}

    /*
    start with 1
    even, then return [iter1 - 1]
    odd, then return [iter1 + 1]

    if (even node), sibling is the previous one

    itr_curr, itr_prev;
    itr_prev = NULL;
    itr_curr = header;
    for () {
        if (check if itr_curr == h) {
            return itr_prev
        }

        itr_prev = itr_curr;
        itr_curr = itr_Curr->next;
    }
        
    else if (odd node), sibling is the next one
        return iteration->next;
	*/
}

int fitting (size_t s) 
{
	/* if size is invalid, return -1 */
	if (s > (1 << MAX_POWER) || s < (1 << MIN_POWER))
		return -1;

	int fitting_size = MIN_POWER;
	while (fitting_size <= MAX_POWER) {
		if (s <= (1 << fitting_size) - HEADER_SIZE) // check if size of payload is able to fit the request
			break;

		fitting_size++;
	}

	return fitting_size;
}

void * bmalloc (size_t s) 
{
	/* if size is invalid, return NULL */
	if (s > (1 << MAX_POWER) || s < (1 << MIN_POWER))
		return NULL;

	int fitting_size = fitting(s);

	for (;;) {

	}

	void *address = mmap();
	if (address == MAP_FAILED)
		return NULL;
		
	/*
    if size > 2^12 exception
    
	1. get the fitting size of the node
	2. traverse the linked list to find fitting size
    
    Firstfit
        first block that can accomodate it (if its bigger, divide)
    Bestfit
        search for the smallest block, and if greater than fitting->divide

    if we find a block that "can" fit, save the address.
    update if another block is smaller && fitting

    if loop ends without finding a possible block,
    use mmap() with MAP_ANONYMOUS
	*/
}

void bfree (void * p) 
{
    /*
    1. iterate to find p in list
    2. delete the contents of p, switch used bit
    3. check if sibling is also unused
        if yes, merge -> brealloc
        cascade up, overwrite node with the next node

    4. check if merged size is 2^12
        if yes, unmap the page/free the page
    */
}

void * brealloc (void * p, size_t s) 
{
	// TODO

    /*
    used for merging
    */

	/*
	1. get the fitting size of the current node
	2. if equal, return p
	3. if smaller, check if better fitting exsits, else return p
	4. if greater, allocate a new block with requested size
		free the previous block
	*/
}

void bmconfig (bm_option opt) 
{
	/* assign the to bm_mode, which is currently set as Bestfit as default */
    if (opt != BestFit || opt != FirstFit)
        return;
    bm_mode = opt;
}


void 
bmprint () 
{
	bm_header_ptr itr ;
	int i ;

	printf("==================== bm_list ====================\n") ;
	for (itr = bm_list_head.next, i = 0 ; itr != 0x0 ; itr = itr->next, i++) {
		printf("%3d:%p:%1d %8d:", i, ((void *) itr) + sizeof(bm_header), (int) itr->used, (int) itr->size) ;

		int j ;
		char * s = ((char *) itr) + sizeof(bm_header) ;
		for (j = 0 ; j < (itr->size >= 8 ? 8 : itr->size) ; j++) 
			printf("%02x ", s[j]) ;
		printf("\n") ;
	}
	printf("=================================================\n") ;
	printf("\n");
	
	int all_given_memory = 0;
	int user_given_memory = 0;
	int available_memory = 0;
	int internal_fragmentation = 0;

	for (itr = bm_list_head.next, i = 0 ; itr != NULL ; itr = itr->next, i++) {
		/*
		print out
		total amount of all given memory,
		total amount of memory given to the user,
		total amount of the available memory,
		total amount of the internal fragmentation
		*/
	}

	printf("===================== stats =====================\n") ;

	printf("=================================================\n") ;
}
