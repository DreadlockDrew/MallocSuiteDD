#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    void *p1 ;

    p1=malloc(7);//should request lowest order
    
  


    if (*(size_t*)p1!=33)
      {return 1;}
    
    return 0; 
}
