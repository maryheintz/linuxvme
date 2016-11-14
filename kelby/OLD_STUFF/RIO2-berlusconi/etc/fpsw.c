fpsw(int i)
{  int l,offst;
   unsigned long *adr;
   unsigned long kk;
   adr = (unsigned long *)0xde570000;
   kk = *adr;
   l = (kk>>i) & 1;
   return(l);
}
