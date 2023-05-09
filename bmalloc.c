#include <unistd.h>
#include <stdio.h>
#include "bmalloc.h"
/* include header for mmap() */
#include <sys/mman.h>
/* include header for exit() */
#include <stdlib.h>
/* include for memcpy() */
#include <string.h>

/* range of memory allocation size, power of 2 */
#define MAX_POWER 12
#define MIN_POWER 4
/* size of the header of the memory block, in bytes */
#define HEADER_SIZE sizeof(struct _bm_header)
/* block used, unused */
#define USED 1
#define UNUSED 0

bm_option bm_mode = BestFit ;
bm_header bm_list_head = { 0, 0, 0x0 } ;
int internal_fragmentation = 0;

// struct _mem_list {
//     size_t requested_size;
//     bm_header_ptr block_address;
// 	struct _mem_list * next ;
// } ;

// typedef struct _mem_list mem_list ;
// mem_list mem_list_head = { 0, NULL, NULL};



/* returns the header address of the suspected sibling block of h */
void * sibling (void * h)
{
    int size_of_node = ((bm_header *) h)->size;
    bm_header* itr;
    bm_header* prev;

    if (size_of_node == MAX_POWER) { // a separate page is not considered as a sibling
        return NULL;
    }

    /* the case when (bm_header *) h is the first node, prev == NULL */
    if (bm_list_head.next == (bm_header *) h) {
        if (size_of_node == ((bm_header *) h)->next->size) {
            return ((bm_header *) h)->next;
        } else {
            return NULL;
        }
    }

    int accumulate_size = 0;
    for (itr = bm_list_head.next, prev = NULL; itr != NULL; prev = itr, itr = itr->next) {
        accumulate_size += (1 << itr->size);

        if (itr == (bm_header *) h) {
            int pos = accumulate_size / (1 << itr->size);

            if (pos % 2 == 0) { // if even, on right
                if (prev != NULL && prev->size == itr->size) {
                    return (void *) prev;
                }

                break;
            } else { // if odd, on left
                if (itr->next != NULL && itr->next->size == itr->size) {
                    return (void *) itr->next;
                }

                break;
            }
        }
    }

    return NULL;
}

/* return the fitting size of a block */
int fitting (size_t s) 
{
	/* if size is invalid, return -1 */
	if (s > (1 << MAX_POWER))
		return -1;

    if (s == 0) {
        return MIN_POWER + 1;
    }

	int fitting_size = MIN_POWER;
	while (fitting_size <= MAX_POWER) {
		if (s <= ((1 << fitting_size) - HEADER_SIZE)) // check if size of payload is able to fit the request
			break;

		fitting_size++;
	}

	return fitting_size;
}

/* split the block to fit */
void *split_block (void *h, int s)
{
    bm_header *old_block = (bm_header *) h;

    /* split the block by twos until it reaches the minimum size */
    while (old_block->size > MIN_POWER) {
        if (old_block->size == s) { // if the current empty block is large enough for s, return it
            return old_block;
        }

        bm_header *new_block = (bm_header *) ((char *) h + (1 << (old_block->size - 1))); // 1byte * offset

        new_block->size = old_block->size - 1;
        new_block->used = 0; // mark the new block as unused
        new_block->next = old_block->next; // link the new block to the list
        
        old_block->next = new_block;
        old_block->size--;
    }

    return old_block;
}

/* initialize new page using mmap() */
void *heap_init()
{
    void *heap_block_ptr = NULL;
    heap_block_ptr = mmap(NULL, (size_t) (1 << MAX_POWER), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (heap_block_ptr == MAP_FAILED) {
        fprintf(stderr, "init_heap() : mmap()\n");
        exit(1);
    }

    bm_header *header_ptr = (bm_header *) heap_block_ptr; // assign the address of the new map to the header ptr
    header_ptr->size = MAX_POWER;
    header_ptr->next = NULL;

    return (void *) header_ptr;
}

/* allocate memory depending on size */
void * bmalloc (size_t s) 
{
    /* if size is invalid, return NULL */
    if (s > (1 << MAX_POWER) || s == 0)
        return NULL;

    int fitting_size = fitting(s);

    /* calculate internal fragmentation */
    // mem_list *mem_element = (mem_list *) malloc(sizeof(mem_list));
    // mem_element->requested_size = s;

    internal_fragmentation += s;

    /* the first bmalloc call should initialize the heap */
    if (bm_list_head.next == NULL) {

        bm_header *header_ptr = (bm_header *) heap_init();
        bm_list_head.next = header_ptr;

        bm_header *new_block_ptr = (bm_header *) split_block((void *) header_ptr, fitting_size);
        new_block_ptr->used = USED;

        return ((void *) new_block_ptr) + HEADER_SIZE;
    }

    bm_header *itr;
    bm_header *bestfit_node;
    int bestfit_size = MAX_POWER;
    int flag = 0;
    int i = 0;

    for (itr = bm_list_head.next; itr != NULL; itr = itr->next) {
        if (itr->used == UNUSED && itr->size >= fitting_size) { // is unused & is able to accomodate the requested block
            if (bm_mode == BestFit && itr->size <= bestfit_size) { // update to better fitting block
                flag = 1;
                /* update best fit node*/
                bestfit_node = itr;
                bestfit_size = itr->size;
            } else if (bm_mode == FirstFit) {
                flag = 1;
                break;
            }
        }
    }

    /* no fitting blocks were available
        similar process to when allocating a new page using mmap() */
    if (flag == 0) {
        bm_header *header_ptr = (bm_header *) heap_init();

        bm_header *new_block_ptr = (bm_header *) split_block((void *) header_ptr, fitting_size);
        new_block_ptr->used = USED;

        /* only difference, connect link from the other page to the created page */
        bm_header *itr;
        for (itr = bm_list_head.next, i = 0 ; itr->next != 0x0 ; itr = itr->next, i++); // iterate till end node
        itr->next = new_block_ptr; // link to new page

        return ((void *) new_block_ptr) + HEADER_SIZE;
    }

    bm_header *new_block_ptr;

    if (bm_mode == BestFit) {
        new_block_ptr = (bm_header *) split_block((void *) bestfit_node, fitting_size); // split the bestfit node, if needed
    } else if (bm_mode == FirstFit){
        new_block_ptr = (bm_header *) split_block((void *) itr, fitting_size); // split the firstfit node, if needed
    }

    new_block_ptr->used = USED;
    return ((void *) new_block_ptr) + HEADER_SIZE;
}

void clear_memory(void *ptr, size_t s)
{
    bzero(ptr, s);
}

/* free & merge unused memory */
void bfree (void *p)
{
    if (p == NULL) {
        return;
    }

    bm_header *header_ptr = (bm_header *) ((void *) p - HEADER_SIZE);

    /* iterate through list to see if the memory exist & is being used */
    bm_header *itr;
    int exists = 0;

	for (itr = bm_list_head.next; itr != NULL; itr = itr->next) {
        if (itr == header_ptr && itr->used == USED) {
            exists = 1; // memory exists & is being used
        }
    }

    if (exists == 0) {
        fprintf(stderr, "bfree() : invalid free of memory\n");
        exit(1);
    }

    clear_memory(p, header_ptr->size); // clear the memory

    /* calculate internal fragmentation */
    internal_fragmentation -= ;

    bm_header_ptr sibling_ptr = (bm_header_ptr) sibling(header_ptr);
    header_ptr->used = UNUSED; // free the memory

    while (sibling_ptr != NULL && sibling_ptr->used == UNUSED) {
        if (sibling_ptr->next == header_ptr) { // if sibling is before header
            // merge the two blocks toward sibling
            header_ptr->size--;
            sibling_ptr->size++;

            // update the linked list
            sibling_ptr->next = header_ptr->next;
            
            // need to unmap page
            if (sibling_ptr->size == MAX_POWER) {
                header_ptr = sibling_ptr;
                sibling_ptr = NULL;
                break;
            }
        } else if (header_ptr->next == sibling_ptr) { // if header is before sibling
            // merge the two blocks toward header
            header_ptr->size++;
            sibling_ptr->size--;

            // update the linked list
            header_ptr->next = sibling_ptr->next;

            // need to unmap page
            if (header_ptr->size == MAX_POWER) {
                sibling_ptr = NULL;
                break;
            }
        }

        header_ptr = (header_ptr->size > sibling_ptr->size) ? header_ptr : sibling_ptr;
        sibling_ptr = (bm_header_ptr) sibling((void*) header_ptr);
    }

    if (sibling_ptr == NULL && header_ptr->size == MAX_POWER) { // no sibling, size of block is the max size(4096)
        if (bm_list_head.next == header_ptr) {
            bm_list_head.next = header_ptr->next;
        } else {
            bm_header_ptr curr_ptr = bm_list_head.next;
            while (curr_ptr->next != header_ptr) {
                curr_ptr = curr_ptr->next;
            }
            curr_ptr->next = header_ptr->next;
        }
        
        clear_memory(header_ptr, (1 << MAX_POWER)); // clear the memory
        int result = munmap(header_ptr, (1 << MAX_POWER));
        
        if (result == -1) {
            fprintf(stderr, "bfree() : munmap()\n");
        }
    }
}

/* allocate to new size, merge unused */
void * brealloc (void * p, size_t s) 
{
    if (s > (1 << MAX_POWER)) {
        fprintf(stderr, "brealloc() : size requested to large\n");
        return NULL;
    }

    /* if ptr is NULL, brealloc() is identical to a call to bmalloc for s bytes */
    if (p == NULL) {
        return bmalloc(s);
    }

    /* if size is zero and ptr is not NULL, a new, minimum sized object is allocated and the original object is freed */
    if (s == 0 && p != NULL) {
        s = MIN_POWER;
    }

    bm_header *new_block = (bm_header *) bmalloc(s);
    size_t min = -1;

    if (((bm_header *) p)->size < s) {
        min = ((bm_header *) p)->size;
    } else {
        min = s;
    }

    /* copy contents amount: min(p->size, s) */
    new_block = (bm_header *) memcpy(new_block, p, min);

    /* free previous memory */
    bfree(p);

    return (void *) new_block;
}

/* modify allocation strategy */
void bmconfig (bm_option opt) 
{
	/* assign new option the to bm_mode */
    if (opt != BestFit && opt != FirstFit) {
        fprintf(stderr, "bmconfig() : bm_mode\n");
        exit(1);
    }

    bm_mode = opt;
}

/* print data of block linked list */
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
	
	int all_given_memory = 0;
	int user_given_memory = 0;
	int available_memory = 0;

	for (itr = bm_list_head.next, i = 0 ; itr != NULL ; itr = itr->next, i++) {
        if (itr->used == USED)
            user_given_memory += (1 << itr->size);
        else
            available_memory += (1 << itr->size);
        
        all_given_memory += (1 << itr->size);
	}

	printf("===================== stats =====================\n") ;
    printf("total given memory: \t\t%d\n",         all_given_memory);
    printf("total given memory to user: \t%d\n",   user_given_memory);
    printf("total available memory: \t%d\n",       available_memory);
    printf("total internal fragmentation: \t%d\n", internal_fragmentation);
	printf("=================================================\n") ;
    printf("\n");
}
