#include <unistd.h>
#include <stdio.h>
#include "bmalloc.h" 

bm_option bm_mode = BestFit ;
bm_header bm_list_head = { 0, 0, 0x0 } ;

/* returns the header address of the suspected sibling block of h */
void * sibling (void * h)
{

    /*
    if size is 4096 return NULL, no sibling*/

    /*
    start with 1
    even, then return [iter1 - 1]
    odd, then return [iter1 + 1]
    */


	// TODO
	/*
	check the next node if the size is the same
	1. get the size of the node
	2. get the size of the next node
	3. if they are the same size && continuous index, then they are siblings
		else not a sibling

	what if the memory is on the left?
    how to check if they are continuous
    1. traverse the list, stop when NULL or *h
    iteration == h

    when node is found:
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
	// TODO
	/*
	return when one of the of possible sizes is able to contain the argument size
	traversing from the smallest will be easier?

    iterate from smallest (2^4) to largest (2^12).

    if greater then 2^12, return error. (exception)
	*/
}

void * bmalloc (size_t s) 
{
	// TODO
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
	// TODO
	/*
	assign the opt given to the global mode, bm_mode
	which is currently set as BEST_FIT as default
	*/
    if (opt != BestFit || opt != FirstFit)
        // exception
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

	// TODO: print out the stat's.
    /*
    print out
	total amount of all given memory,
	total amount of memory given to the user,
	total amount of the available memory,
	total amount of the internal fragmentation
    */
}
