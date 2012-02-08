
#include "dev/sci3.h"

int putchar(int c)
{
  sci3_writeb((char)c);
  return c;
}

void pf_putc(void* p,char c)
{
  sci3_writeb(c);
}
