main()
 { unsigned long vmebase,swadr=0x57;
   for(i=0;i<6;i++) 
     { k = fpsw(i);
       printf(" fpsw(%d) = %d\n",i,k);
     }
   for(i=0;i<10;i++)
     { k=sw1(i);
       printf(" sw1(%d) = %d\n",i,k);
     }
   for(i=0;i<10;i++)
     { k=sw2(i);
       printf(" sw2(%d) = %d\n",i,k);
     }
    vmebase = vme_a24_base() + swadr;
    adr = (unsigned long *)(vmebase+0x14);
    *adr = 0xaaaaaaaa;
    adr = (unsigned long *)(vmebase+0x10);
    k = *adr;
    printf(" test reg 0xaaaaaaaa written %x read back\n",k);    adr = (unsigned long *)(vmebase+0x14);
    *adr = 0x55555555;
    adr = (unsigned long *)(vmebase+0x10);
    k = *adr;
    printf(" test reg 0x55555555 written %x read back\n",k);
  }
fpsw(int i)
  { int ent=0,l;
    unsigned long kk, *adr;
    if(ent == 1) goto skip;
    ent = 1;
    vmebase = vme_a24_base() + swadr;
    adr = (unsigned long *)vmebase;
skip:
    kk = *adr;
    l = (kk>>i) & 1;
    return(l);
  }
  sw1(int i)
  { int ent=0,l,m;
    unsigned long kk, *adr;
    if(ent == 1) goto skip;
    ent = 1;
    vmebase = vme_a24_base() + swadr;
    adr = (unsigned long *)vmebase;
skip:
    kk = *adr;
    m = i+12;
    l = (kk>>m) & 1;
    return(l);
  }
  sw2(int i)
  { int ent=0,l,m;
    unsigned long kk, *adr;
    if(ent == 1) goto skip;
    ent = 1;
    vmebase = vme_a24_base() + swadr;
    adr = (unsigned long *)vmebase;
skip:
    kk = *adr;
    m = i+22;
    l = (kk>>m) & 1;
    return(l);
  }
  
