#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>



void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

#define CHUNK_SIZE (1<<12)


extern void *bulk_alloc(size_t size);


extern void bulk_free(void *ptr, size_t size);

static inline __attribute__((unused)) int block_index(size_t x) {
    if (x <= 8) {
        return 5;
    } else {
        return 32 - __builtin_clz((unsigned int)x + 7);
    }
}


static size_t malloc_called = 0;//0 if false non zero if true

typedef struct block //TODO make this static?
{   size_t avail;// 8 byte
    struct block *next;

}block;

static block **free_table;//Aves free_table Global Static Variable reference

//TODO inquire over bulk malloc 3.4
void *malloc(size_t size)
{{fprintf(stderr,"\n MALLOC %ld \n",size);}
    if(size==0){return NULL;}

    int debugMode=0;
    if(debugMode==1){{fprintf(stderr,"\n DEBUG MODE --------- \n");}}

    if(size>4088)//Throws this to the bulk allocator if its to big to deal with.
        {   size_t sizeAndMeta= size+8;
            void  *NodeToGive= bulk_alloc(sizeAndMeta);
            *((size_t*)NodeToGive)=(size)^1;
            return (void*)(NodeToGive+8);


        }
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


void *calloc(size_t nmemb, size_t size) {
    fprintf(stderr,"\nCALLOC %ld\n",nmemb*size);
    if(nmemb==0||size==0)
        {return NULL;}

    size_t trueSize = nmemb * size;
    
    void *NodeToGive=malloc(trueSize);
    size_t strider=(size_t)NodeToGive;
    for(size_t i = 0; i<trueSize;i++)
        {
            *((char*)strider)='\0';
         // fprintf(stderr,"%ld ",*(size_t*)strider);
         strider=strider+1;
         
        }


    
    return NodeToGive;
}


void *realloc(void *ptr, size_t size) {
    fprintf(stderr,"\nREALLOC, %ld\n",size);
        if(size==0 && ptr!=NULL)
            {free(ptr);return NULL;;}
        if(size==0){return NULL;}
        if(ptr==NULL)
            {return malloc(size);}
            
        
        struct block* NodeToTake=(void*)(ptr-8);
        NodeToTake->avail=NodeToTake-> avail ^ 1;
        fprintf(stderr,"\n orig was was, %ld\n",NodeToTake->avail);

        if(size<=NodeToTake->avail-8)
            {
            NodeToTake->avail=NodeToTake-> avail ^ 1;
            return ptr;}

    //EXECUTING CASE WERE WE NEED A NEW ALLOCATION.
    void *newptr=malloc(size);

    //fprintf(stderr,"REALLOC1 \n");
    for(unsigned int pos=0;pos<(NodeToTake->avail-8);pos++)
        {
            
            *((char*)newptr+pos)=*((char*)(ptr+pos));
        }

    NodeToTake->avail=NodeToTake-> avail ^ 1;
    free(ptr);
    return newptr;
}


void free(void *ptr)
{ 
    fprintf(stderr,"\nFREE, \n");
    if(ptr==NULL){return;}

    struct block* NodeToTake=(void*)(ptr-8);//we go back to that metadata
    size_t freeOrUsed =NodeToTake-> avail & 1; 
    fprintf(stderr,"\n orig was, %ld\n",NodeToTake->avail);
    if (freeOrUsed != 1)
        {fprintf(stderr,"pointer %p which has metadata at %p not designated as allocated. undefined behavior and \n",ptr,ptr-8);
         fprintf(stderr,"The status of this %ld size block is %ld",NodeToTake->avail ^1,NodeToTake->avail & 1);
        }
   

    if(((NodeToTake->avail)^1)>4096)//Throw request to the bulk allocator and then duck
        {//NodeToTake->avail=NodeToTake-> avail ^ 1;
            NodeToTake->avail=NodeToTake-> avail ^ 1;
            fprintf(stderr,"calling bulk free for size %ld\n", (NodeToTake->avail+8));
            
            bulk_free(NodeToTake,NodeToTake->avail+8);
            return;
        }
     NodeToTake->avail=NodeToTake-> avail ^ 1;
    
    size_t relevantIndex=block_index(NodeToTake->avail-8);
    //fprintf(stderr," index was %ld\n",relevantIndex);
    NodeToTake->next=free_table[relevantIndex];
    // fprintf(stderr,"\n head was pointing to %p so we set nodeToTake's->next value to %p \n",free_table[relevantIndex],NodeToTake->next);
    free_table[relevantIndex]=(void*)NodeToTake;
    //fprintf(stderr,"\n head of pool %d is now pointing to %p",1<< relevantIndex,free_table[relevantIndex]);
    return;
}
