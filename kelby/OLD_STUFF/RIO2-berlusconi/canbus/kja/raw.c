reset_can()
  {
     unsigned char *adr;
   int k;

   k = 0xdffe6001; /* CONTROL REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x01;  /*RESET HIGH*/
   
   k = 0xdffe600d; /* BTR0 */
   adr = (unsigned char*) k;
   *adr = 0x00;  

   k = 0xdffe600f; /* BTR1 */
   adr = (unsigned char*) k;
   *adr = 0x14;  /*BTR 1*/

   k = 0xdffe6011;  /* OCR */
   adr = (unsigned char*) k;
   *adr = 0xda;
 
   k = 0xdffe6001;  /* COMMAND REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x0e;  /* Reset Request low.  Enable interrupts */
}

c_stat_read(addr, bp)
uchar addr;
Buffer* bp;
{
   r_write(IP + 0x15, (addr >> 3));
   r_write(IP + 0x17, ((addr << 5) + 0x18));
   r_write(IP + 0x03, 0x01);

   read_buf(IP + 0x29, bp);   
}

on_off(addr)
uchar addr;
{
   Buffer rbuf;
   c_stat_read(addr, &rbuf);
   
   r_write(IP + 0x15, (0x01 << 4) + (addr >> 3));
   r_write(IP + 0x17, (addr << 5) + 0x01);
   if(rbuf.data[2] & 0x01) r_write(IP + 0x19, 0x41);
   else r_write(IP + 0x19, 0x43);
   r_write(IP + 0x03, 0x01);
}

