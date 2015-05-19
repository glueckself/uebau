#include <stdio.h>
#include <stdlib.h>

extern long callchecking();

static long heap[1000000];
register long *heapptr asm("%r15");

#define RET return

int call(void)
{
#include CALL
}

int main(int argc, char *argv[])
{
  long r;
  heapptr=heap;
  r=callchecking();
  if(r==0 || r==1)
    return !r;
  return r;
}

void raisesig()
{
  raise(11);
}
