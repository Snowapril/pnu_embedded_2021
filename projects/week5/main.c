
void delay() {
   int i;
   for (i = 0; i < 10000000; i++);
}

int main(void)
{
  *(volatile unsigned int *)0x40021018 |= 0x30; // port c, d clock enable
  // port d register address = 0x4001 1400
  // button port d configuration
  *(volatile unsigned int *)0x40011404 &= ~0xff000;
  *(volatile unsigned int *)0x40011404 |= 0x88000;
  // relay port c configuration
  *(volatile unsigned int *)0x40011004 &= ~0xf;
  *(volatile unsigned int *)0x40011004 |= 0x3;
  
  while (1) {
    
    
    if (!(*(volatile unsigned int *)0x40011408 & 0x1000)) // s2
    {
      *(volatile unsigned int *)0x40011410 |= 0x1C0000;
      *(volatile unsigned int *)0x40011410 |= 0x80;
      delay();
      *(volatile unsigned int *)0x40011410 |= 0x800000;
    }
    
    if (!(*(volatile unsigned int *)0x40011408 & 0x800)) // s1
    {
      *(volatile unsigned int *)0x40011010 |= 0x100;
      delay();
      *(volatile unsigned int *)0x40011010 |= 0x1000000;
    }
  }
  return 0;
}