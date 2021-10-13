
int main(void)
{
  *(volatile unsigned int *)0x40021018 |= 0x30; // port c, d clock enable
  // led configuration
  *(volatile unsigned int *)0x40011400 &= ~0xf00fff00; 
  *(volatile unsigned int *)0x40011400 |= 0x30033300;
  // joystick configuration
  *(volatile unsigned int *)0x40011000 &= ~0xffff00;
  *(volatile unsigned int *)0x40011000 |= 0x888800;
  while (1) {
    // port c 0x40011000
    // down 2
    if (!(*(volatile unsigned int *)0x40011008 & 0x4))
    {
      *(volatile unsigned int *)0x40011410 |= 0x940000; // 1001 0100
      *(volatile unsigned int *)0x40011410 |= 0x8;
    }
    else
    {
      *(volatile unsigned int *)0x40011410 |= 0x080000;
    }
    // up 5
    if (!(*(volatile unsigned int *)0x40011008 & 0x20))
    {
      *(volatile unsigned int *)0x40011410 |= 0x980000;
      *(volatile unsigned int *)0x40011410 |= 0x4;
    }
    else
    {
      *(volatile unsigned int *)0x40011410 |= 0x040000;
    }
    // left 3
    if (!(*(volatile unsigned int *)0x40011008 & 0x8))
    {
      *(volatile unsigned int *)0x40011410 |= 0x8C0000;
      *(volatile unsigned int *)0x40011410 |= 0x10;
    }
    else
    {
      *(volatile unsigned int *)0x40011410 |= 0x100000;
    }
    // right 4
    if (!(*(volatile unsigned int *)0x40011008 & 0x10))
    {
      *(volatile unsigned int *)0x40011410 |= 0x1C0000;
      *(volatile unsigned int *)0x40011410 |= 0x80;
    }
    else
    {
      *(volatile unsigned int *)0x40011410 |= 0x800000;
    }
  }
  return 0;
}