#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

/* The standard allocator interface from stdlib.h.  These are the
 * functions you must implement, more information on each function is
 * found below. They are declared here in case you want to use one
 * function in the implementation of another. */
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

/* When requesting memory from the OS using sbrk(), request it in
 * increments of CHUNK_SIZE. */
#define CHUNK_SIZE (1<<12)

/*
 * This function, defined in bulk.c, allocates a contiguous memory
 * region of at least size bytes.  It MAY NOT BE USED as the allocator
 * for pool-allocated regions.  Memory allocated using bulk_alloc()
 * must be freed by bulk_free().
 *
 * This function will return NULL on failure.
 */
extern void *bulk_alloc(size_t size);

/*
 * This function is also defined in bulk.c, and it frees an allocation
 * created with bulk_alloc().  Note that the pointer passed to this
 * function MUST have been returned by bulk_alloc(), and the size MUST
 * be the same as the size passed to bulk_alloc() when that memory was
 * allocated.  Any other usage is likely to fail, and may crash your
 * program.
 */
extern void bulk_free(void *ptr, size_t size);

/*
 * This function computes the log base 2 of the allocation block size
 * for a given allocation.  To find the allocation block size from the
 * result of this function, use 1 << block_size(x).
 *
 * Note that its results are NOT meaningful for any
 * size > 4088!
 *
 * You do NOT need to understand how this function works.  If you are
 * curious, see the gcc info page and search for __builtin_clz; it
 * basically counts the number of leading binary zeroes in the value
 * passed as its argument.
 */
static inline __attribute__((unused)) int block_index(size_t x) {
    if (x <= 8) {
        return 5;
    } else {
        return 32 - __builtin_clz((unsigned int)x + 7);
    }
}

/*
 * You must implement malloc().  Your implementation of malloc() must be
 * the multi-pool allocator described in the project handout.
 */

static void**free_table;//Aves free_table Global Static Variable reference
static unsigned int malloc_called = 0;//0 if false non zero if true
struct memPool //TODO make this static?
{
    struct memPool *next; // 8 byte pointer should come after prev
    struct memPool *prev; // 8 byte pointer should come after avail
    unsigned long avail;// 8 byte should come first
};
void *malloc(size_t size)
{
    size_t sizeWithMeta = size +8;
    if(size>4088)//Throws this to the bulk allocator if its to big to deal with.
        {return bulk_alloc(size);}

    //CREATION OF THE FREE TABLE
    if(malloc_called==0)//checks if this is the first time malloc has been called
        {free_table=*(void**)sbrk(CHUNK_SIZE);malloc_called=1;//should make freetable stridable by 8.
            for(int loc = 5; loc<13;loc++)
                {
                    free_table[loc]=NULL;//SETS all free_table pools to NULL
                }
        }
    //CREATION OF THE FREE TABLE

    //ALLOCATION OF A NEW POOL
     size_t poolNum = 1<<block_index(sizeWithMeta);
     unsigned long pool_size =pow(2,poolNum);
     int pools = CHUNK_SIZE/pool_size;
     if(free_table[poolNum]==NULL)//IF THIS POOL IS EMPTY 
        {
            free_table[poolNum]=sbrk(CHUNK_SIZE);
            for(int i=0;i<pools;i++)
                {//((struct memPool*)free_table[poolNum]+i*pool_size) should cause memPool[poolNum]to point to first freshly freed block
                    *((struct memPool*)free_table[poolNum]+i*pool_size)/*TODO will this stride 24??*/ = (struct memPool){NULL,NULL,size/*shouldnt need to be masked*/};//TODO ask a TA if this actually even would work
                    if(i!=0){*((struct memPool*)free_table[poolNum]+i*pool_size)->prev=*((struct memPool*)free_table[poolNum]+(i-1)*pool_size);}
                    if(i!=pools-1){*((struct memPool*)free_table[poolNum]+i*pool_size)->next=*((struct memPool*)free_table[poolNum]+(i+1)*pool_size);}
                }
            
        }
    
    //ALLOCATION OF A NEW POOL
     
   //code for checking if free *((struct memPool*)free_table[poolNum]+i*pool_size)->avail & 0==0
     //THIS WILL NOT WORK IF THE POOL HAS HAD ALL BLOCKS ALLOCATED
     struct memPool *nodeToGive =((struct memPool*)free_table[poolNum]);
     

         
     struct memPool *newHead =((struct memPool*)free_table[poolNum])->next;
     newHead->prev=NULL;
     
     nodeToGive->prev=NULL;nodeToGive->next=NULL;
     
         
     
     return nodeToGive;
}

/*
 * You must also implement calloc().  It should create allocations
 * compatible with those created by malloc().  In particular, any
 * allocations of a total size <= 4088 bytes must be pool allocated,
 * while larger allocations must use the bulk allocator.
 *
 * calloc() (see man 3 calloc) returns a cleared allocation large enough
 * to hold nmemb elements of size size.  It is cleared by setting every
 * byte of the allocation to 0.  You should use the function memset()
 * for this (see man 3 memset).
 */
void *calloc(size_t nmemb, size_t size) {
    void *ptr = bulk_alloc(nmemb * size);
    memset(ptr, 0, nmemb * size);
    return ptr;
}

/*
 * You must also implement realloc().  It should create allocations
 * compatible with those created by malloc(), honoring the pool
 * alocation and bulk allocation rules.  It must move data from the
 * previously-allocated block to the newly-allocated block if it cannot
 * resize the given block directly.  See man 3 realloc for more
 * information on what this means.
 *
 * It is not possible to implement realloc() using bulk_alloc() without
 * additional metadata, so the given code is NOT a working
 * implementation!
 */
void *realloc(void *ptr, size_t size) {
    fprintf(stderr, "Realloc is not implemented!\n");
    return NULL;
}

/*
 * You should implement a free() that can successfully free a region of
 * memory allocated by any of the above allocation routines, whether it
 * is a pool- or bulk-allocated region.
 *
 * The given implementation does nothing.
 */
void free(void *ptr) {
    return;
}
