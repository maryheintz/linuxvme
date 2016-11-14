  unsigned long swap32(unsigned long wd)
    { unsigned long b1,b2,b3,b4,b5,b6,b7,b8;
      unsigned long reorder;
      b1 = (wd>>28) & 0xf;
      b2 = (wd>>24) & 0xf;
      b3 = (wd>>16) & 0xf;
      b4 = (wd>>20) & 0xf;
      b5 = (wd>>12) & 0xf;
      b6 = (wd>>8) & 0xf;
      b7 = (wd>>4) & 0xf;
      b8 = wd & 0xf;
      reorder = b7<<28 | b8<<24 | b5<<20 | b6<<16 
             | b4<<12 | b3<<8 | b1<<4 | b2;
      return(reorder);
    } 
	  
