#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

//This test ensures that pointers dereferenced by
//malloc actually return usuable pointers that do not segfault on access
int main(int argc, char *argv[])
{
    void *p1 ;

    p1=malloc(1016);//should request 4 blocks
    size_t *allocSize = (void*)p1-8;
    if (((*allocSize)^1)!=1024)
        {
            fprintf(stderr,"\n tried to access %p expected 1024 bytes instead found %ln \n",p1,allocSize);
          return 1;
      }
    
    return 0; 
}
