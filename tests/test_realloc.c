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


    size_t allocSize1 = ((size_t)*(p1-1))^1;
    size_t allocSize2 = ((size_t)*(p2-1))^1;

    allocSize1=allocSize1^1;
    allocSize2=allocSize2^1;
    size_t run;
    // fprintf(stderr,"\nJUMPED1 \n");
    for(run=0;run<(7);run++)
        {//fprintf(stderr,"%ld \n",run);
            *(p1+run) =run;
            //fprintf(stderr,"%ld \n",(size_t)*(p1+run));
        }
//fprintf(stderr,"JUMPED2 \n");
    p1 = realloc(p1,requestSize*700);

    //fprintf(stderr,"JUMPED3 \n");
    for (run=0;run<700;run++)
        {//fprintf(stderr,"%ld \n",(size_t)*(p1+run));
/*if ((size_t)*(p1+run)!=run)
           {fprintf(stderr," p1[%ld] resolved to %ld expected %ld \n",run,(size_t)*(p1+run),run);return 1;}*/
        }
    
    for (run=run;run<700;run++)
        {
            if ((size_t)*(p1+run)!=0)
                {fprintf(stderr," p1[%ld] resolved to %ld expected %ld \n",run,p1[run],run);return 1;}
        }

return 0;
}
