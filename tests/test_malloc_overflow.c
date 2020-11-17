#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
/*this requests a byteload in excess of the 4096 chunk size requirement 
and thus will catch error related to the case of the pool
being exausted

It test this for allocations that should belong in pools size 1024 and allocations that should belong in of size 32 */
int main(int argc, char *argv[])
{
    void *p1 ;

    p1=malloc(1016);
    p1=malloc(1016);
    p1=malloc(1016);
    p1=malloc(1016);
    p1=malloc(1016);
    size_t *allocSize = (void*)p1-8;
    if (((*allocSize)^1)!=1024)
        {
            fprintf(stderr,"\n tried to access %p expected 1024 bytes instead found %ln \n",p1,allocSize);
          return 1;
      }
    void *p2;
    for(int i=0;i<(32/4096);i++)
        {
            p2=malloc(5);
            size_t *allocSize = (void*)p2-8;
            if  (((*allocSize)^1)!=32)
                {fprintf(stderr,"\n tried to access %p expected 32 bytes instead found %ln \n",p1,allocSize);
                    return 1;}
                
        }
    
    return 0; 
}
