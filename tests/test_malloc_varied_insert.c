#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
/*this requests different types of by allocations and ensures that their allocation size is as expected to ensure good runs*/
int main(int argc, char *argv[])
{
    void *p1 ;
    size_t requestSize=1;
    p1=malloc(requestSize);
    size_t *allocSize = (void*)p1-8;

    
    if (((*allocSize)^1)!=32)
        {fprintf(stderr,"\n tried to access %p expected %ld bytes instead found %ln \n",p1,requestSize,allocSize);}

    requestSize=24;
    p1=malloc(requestSize);
    allocSize = (void*)p1-8;
    if (((*allocSize)^1)!=32)
    {fprintf(stderr,"\n tried to access %p expected %d bytes instead found %ln \n",p1,32,allocSize);}

    requestSize=32;
    p1=malloc(requestSize);
    allocSize = (void*)p1-8;
    if (((*allocSize)^1)!=64)
    {fprintf(stderr,"\n tried to access %p expected %d bytes instead found %ln \n",p1,64,allocSize);}

    requestSize=64;
    p1=malloc(requestSize);
    allocSize = (void*)p1-8;
    if (((*allocSize)^1)!=128)
    {fprintf(stderr,"\n tried to access %p expected %d bytes instead found %ln \n",p1,128,allocSize);}

    
    return 0; 
}
