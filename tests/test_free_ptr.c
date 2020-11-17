#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#define SIZETESTED 1016

//This test repeteadly frees the head of the list after allocating then verifies it got the same memory address
//It was confirmed that this test suite only causes a singular sbrk() despite making 100 calls to malloc.
//confirming that indeed free() is working as expected.
int main(int argc, char *argv[])
{ void *ptr1, *ptr2;
    

    for(int i=0;i<100;i++)
        {
            ptr1=malloc(SIZETESTED);
            free(ptr1);
            ptr2=malloc(SIZETESTED);
            if(ptr1!=ptr2)//TODO Is this legal given my code should it be &ptr1 != &ptr2
                {fprintf(stderr,"\n pointer to %p was returned to the head and rerequested but got %p \n",&ptr1,&ptr2); return 1;}
            free(ptr2);
            //fprintf(stderr,"passing\n");
        }
    

    return 0;}
