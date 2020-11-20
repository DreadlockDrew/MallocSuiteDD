#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
/*this ensures you are given clean size accurate allocations from calloc without segmentation faults*/
int main(int argc, char *argv[])
{
    size_t *p1,*p2;
    size_t requestSize=8;
    p1=calloc(requestSize,7);
    p2=calloc(requestSize,700);


    size_t allocSize1 = (size_t)*(p1-1);
    size_t allocSize2 = (size_t)*(p2-1);

    allocSize1=allocSize1^1;
    allocSize2=allocSize2^1;
    //fprintf(stderr,"\n %ld %ld \n",allocSize1,allocSize2);
   
       if (((allocSize1))!=64)
        {fprintf(stderr,"\n tried to access a 64 bytes calloc request was given an allocation of %ld \n",allocSize1);}
       if(((allocSize2))!=5600)
        {fprintf(stderr,"\n tried to access a 5600 bytes calloc request was given an allocation of %ld \n",allocSize2); return 1;}

       size_t intervals;
       intervals = ((allocSize1-8)/sizeof(size_t));
    for(size_t i = 0; i<intervals;i++)
        {
            if((size_t)*(p1+i)!=0)//what is (size_t)*(pointer)even supposed to mean
                {fprintf(stderr,"\n while searching %ld intervals found a %ld size_t at interval %ld\n",intervals,(size_t)*(p1+i),i);return 1;}
        }
    
    intervals = ((allocSize2-8)/sizeof(size_t));
    for(size_t i = 0; i<intervals;i++)
        {
            if((size_t)*(p2+i)!=0)
                {fprintf(stderr,"\n while searching %ld intervals found a %ld size_t at interval %ld\n",intervals,(size_t)*(p2+i),i);return 1;}
                }
    
    



    
    return 0; 
}
