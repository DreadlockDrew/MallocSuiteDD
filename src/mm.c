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

static struct memPool  **free_table;//Aves free_table Global Static Variable reference
static unsigned int malloc_called = 0;//0 if false non zero if true

typedef struct memPool //TODO make this static?
{   unsigned long avail;// 8 byte should come first
    struct memPool *next; // 8 byte pointer should come first

}block;

void *malloc(size_t size)
{
    if(size>4088)//Throws this to the bulk allocator if its to big to deal with.
        {return bulk_alloc(size);}

    //CREATION OF THE FREE TABLE
    if(malloc_called==0)//checks if this is the first time malloc has been called
        {free_table=sbrk(CHUNK_SIZE);malloc_called=1;//should make freetable stridable by 8.
            //as opposed to free_table=*((void**)sbrk(CHUNK_SIZE))
            for(int loc = 5; loc<13;loc++)
                {
                    free_table[loc]=NULL;//SETS all free_table pools to NULL
                }
        }
    //CREATION OF THE FREE TABLE

    //ALLOCATION OF A NEW POOL
    /*size_t bytesNeeded = ( log(size)/ log(2) );
    
    if(size % 2 !=0) bytesNeeded = bytesNeeded << 1;*/
    
    size_t poolNum = 1<< block_index(size);
    //returns the literal index you want in the array not a raw log
     unsigned long pool_size =1<<poolNum;
     int pools = CHUNK_SIZE/pool_size;

     if(free_table[poolNum]==NULL)
         { /*
             free_table[poolNum]=sbrk(CHUNK_SIZE);
             void *stridingForkLift = free_table[poolNum];//ByteWise Strider
           */
             void *stridingForklift=sbrk(CHUNK_SIZE);

             struct memPool* lastHead=stridingForklift;
             

             
             for(int i=0;i<pools;i++)
                 {//free_table[poolNum]LITTERARLY ALWAYS STARTS AS NULL USE THIS
                      
                      lastHead=stridingForklift;
                      *lastHead=(struct memPool){pool_size,free_table[poolNum]};
                      free_table[poolNum]=lastHead;
                      stridingForklift=stridingForklift+ pool_size;
                 //this is fine because I take for granted that free_table[poolnum] points to null this could cause an infinite LOOP tho

                 }
             }


     struct memPool* NodeToGive=free_table[poolNum];
     unsigned long  mask=0x01;
     NodeToGive->avail=NodeToGive->avail | mask;//MARKS IT AS USED
     return NodeToGive;
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
