#include <string.h>
#include <unistd.h>
#include <stdio.h>
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


static size_t malloc_called = 0;//0 if false non zero if true

typedef struct block //TODO make this static?
{   size_t avail;// 8 byte should come first
    struct block *next;// should come next

}block;

static block **free_table;//Aves free_table Global Static Variable reference

//TODO inquire over bulk malloc 3.4
void *malloc(size_t size)
{   if(size==0){return NULL;}

    int debugMode=0;
    if(debugMode==1){{fprintf(stderr,"\n DEBUG MODE --------- \n");}}

    if(size>4088)//Throws this to the bulk allocator if its to big to deal with.
        {   int sizeAndMeta= size+8;
            void  *NodeToGive= bulk_alloc(sizeAndMeta);
            *((size_t*)NodeToGive)=(size)^1;//sets size+allocation flag bit  TODOTODOTODO Is this correct form.
            return (void*)(NodeToGive+8);// TODO are we returning its size+8


        }// size + 8 via handout 3.4
    

    
    //CREATION OF THE FREE TABLE
    
    if(malloc_called==0)//checks if this is the first time malloc has been called
        {free_table=sbrk(CHUNK_SIZE);malloc_called=1;
            for(int loc = 5; loc<13;loc++)
                {
                    free_table[loc]=NULL;//SETS all free_table pools to NULL
                }
        }
    //CREATION OF THE FREE TABLE

    //ALLOCATION OF A NEW POOL
    size_t poolNum = block_index(size);
    
    size_t pool_size =(1<< block_index(size));
    
    int pools = CHUNK_SIZE/pool_size;
    
    if(free_table[poolNum]==NULL)
        {
            if(debugMode==1)
                {fprintf(stderr,"ALLOCATING A NEW POOL OF %ld. original size asked for was %ld, base power was 2^%ld \n",pool_size,size,poolNum);}      
            void *stridingForklift=sbrk(CHUNK_SIZE);
            struct block* lastHead=(block*)stridingForklift;
            
            if(debugMode==1){fprintf(stderr,"setting head at %p \n",lastHead);
                fprintf(stderr,"setting forklift at  %p \n",stridingForklift);}
            
                        for(int i=0;i<pools;i++)
                {
                    lastHead=(block*)stridingForklift;
                    lastHead->avail=pool_size;lastHead->next=free_table[poolNum];            
                    free_table[poolNum]=lastHead;
                    stridingForklift=stridingForklift+ pool_size;
                    if(debugMode==1)
                        {fprintf(stderr,"block at  %p with next %p \n",lastHead,lastHead->next);
                            fprintf(stderr,"but the free tables head was %p \n",free_table[poolNum]);}
                }
        }

    
     struct block* NodeToGive=free_table[poolNum];
     if(debugMode==1){fprintf(stderr,"returning block %p, which was of size %ld \n",NodeToGive,NodeToGive->avail);}
     free_table[poolNum]=NodeToGive->next;
     if (debugMode==1){fprintf(stderr,"\n as a result of this %p, is the new head \n",free_table[poolNum]);}
     size_t mask=0x01;
     NodeToGive->avail=NodeToGive->avail ^  mask;//MARKS IT AS USED
     if(debugMode==1){fprintf(stderr,"\n this blocks allocation status is %ld where a 1 resolves to it being allocated \n",NodeToGive->avail & 1 );}
     NodeToGive=(void*)NodeToGive+8;
     if(debugMode==1){fprintf(stderr,"\n the user received %p as the given pointer \n",NodeToGive);}
     
     
     if(debugMode==1){fprintf(stderr,"DEBUG MODE ENDED\n _______ \n");}
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
    if(nmemb==0||size==0)
        {return NULL;}

    size_t trueSize = nmemb * size;
    
    void *NodeToGive=malloc(trueSize);
    void *strider=NodeToGive;
    for(size_t i = 0; i<trueSize;i++)
        {
         *((char*)strider)='\0';
         strider=strider+1;
        }
    
/*
      void *ptr = bulk_alloc(nmemb * size);
      memset(ptr, 0, nmemb * size);
*/

    
    return NodeToGive;
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
        if(size==0)
        {return NULL;}
        
        struct block* NodeToTake=(void*)(ptr-8);
        NodeToTake->avail=NodeToTake-> avail ^ 1;
        fprintf(stderr,"had a pointer %ld where %ld are usable. \n",(size_t)(NodeToTake->avail),(size_t)(NodeToTake->avail-8));
        fprintf(stderr,"but you asked for %ld bytes  \n",size);
        if(size<=NodeToTake->avail-8)//TODO CAN WE USE SAME POINTER ON EQUAL AMOUNT AND SHOULD IT BE size<=avail-8
            {fprintf(stderr,"so im going to give you your pointer back sorry. \n");
            NodeToTake->avail=NodeToTake-> avail ^ 1;
            return ptr;}

    //EXECUTING CASE WERE WE ACTUALLY NEED A NEW ALLOCATION.
    void *newptr=malloc(size);


    for(unsigned int pos=0;pos<NodeToTake->avail;pos++)
        {
            *((char*)newptr+pos)=*((char*)ptr+pos);
        }
    struct block* temp = (void*)(newptr-8);
    size_t finalSize= temp->avail ^ 1;
    NodeToTake->avail=NodeToTake-> avail ^ 1;
    fprintf(stderr,"so ill free the pointer with %ld bytes and give you this one with %ld bytes instead \n",NodeToTake->avail-1,finalSize);
    free(ptr);
    return newptr;
}

/*
 * You should implement a free() that can successfully free a region of
 * memory allocated by any of the above allocation routines, whether it
 * is a pool- or bulk-allocated region.
 *
 * The given implementation does nothing.
 */
void free(void *ptr)//NOT REQUIRED TO HANDLE BULK FREEINGS
{  // fprintf(stderr,"\n Node %p received\n",&ptr);
    if(ptr==NULL){return;}

    struct block* NodeToTake=(void*)(ptr-8);//we go back to that metadata
    size_t freeOrUsed =NodeToTake->avail &1;
    
    if (freeOrUsed != 1)
        {fprintf(stderr,"pointer %p which has metadata at %p not designated as allocated. undefined behavior and \n",ptr,ptr-8);
         fprintf(stderr,"The status of this %ld size block is %ld",NodeToTake->avail ^1,NodeToTake->avail & 1);
        }
    NodeToTake->avail=NodeToTake-> avail ^ 1;

    if(NodeToTake->avail>4088)//Throw request to the bulk allocator and then duck
        {bulk_free(ptr, NodeToTake->avail);return;}
    
    size_t relevantIndex=block_index(NodeToTake->avail-8);
    //fprintf(stderr," index was %ld\n",relevantIndex);
    NodeToTake->next=free_table[relevantIndex];
    // fprintf(stderr,"\n head was pointing to %p so we set nodeToTake's->next value to %p \n",free_table[relevantIndex],NodeToTake->next);
    free_table[relevantIndex]=(void*)NodeToTake;
    //fprintf(stderr,"\n head of pool %d is now pointing to %p",1<< relevantIndex,free_table[relevantIndex]);
    return;
}
