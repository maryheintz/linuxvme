#define GO_FB 101
#define GO_ISP 102
#define IDALLOC 107
#include <stdio.h>
#include <math.h>
int id,ttcadr;
main()
{ 
   int tube,i,k,board;
   int tadr[5] = {0x2001,0x2002,0x2003,0x2007,0x2004};  
   char serial[5][6] = {"PS1005","PS1004","PS1002",
                        "PS1001","PS1008"};
   char snum[7]; 
   unsigned int i1,i2,itst;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   long status;
strt:
   for(board=1;board<6;board++)
    {printf("\n\n  start board %d\n",board);
   ttcadr = tadr[board-1];
   id = (board<<6) | 1;
   printf(" board %d  id=%x  ttcadr=%x\n",board,id,ttcadr);
   snum[0]=serial[board-1][0];
   snum[1]=serial[board-1][1];
   snum[2]=serial[board-1][2];
   snum[3]=serial[board-1][3];
   snum[4]=serial[board-1][4];
   snum[5]=serial[board-1][5];
   snum[6]='\0';
   printf("serial=%c%c%c%c%c%c\n",
    snum[0],snum[1],snum[2],snum[3],snum[4],snum[5]);
   ttcvi_map(ttcadr);
   reset_ctrl_can();    /* set up the TIP810 canbus controller */
   can_reset(ttcadr);  
   printf("calling can_init\n");
   can_init(board,snum);  
   for(k=0;k<49;k++) 
     { if(cardchk(k) == 1) 
          printf(" card %d found\n",k);
       else  
          if(sw(8) == 1) printf("no card %d\n",k);
     }
  }
     if(sw(1) == 1) 
     { printf(" \n");
       sleep(1);
       goto strt;
     }    
}
cardchk(k)
int k;
  {int i,status,ok;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   status=0;
   ok=0;
   can_multi_low(id);
   can_bkld_low(id);
   can_tube(id,k);
   can_intg_gain(id,5);   /* s1=s3=1 s2=s4=0 */
   can_itr_high(id);           /* itr=1 */
   can_intg_rd_low(id);        /* ire=0 */
   can_mse_high(id);           /* mse=1 */
   can_small_cap_disable(id);  /* tplo=0 */
   can_large_cap_enable(id);   /* tphi=1 */
   can_trig_disable(id);       /* trig=0 */
   can_bkld_high(id);
   can_bkld_low(id);
   status=can_3in1set(id,0xd400);   /* fetch from 3in1 */
   status=can_3in1get(id,&shrt);    /* and load can output reg */
   i = (int)shrt;
   trig = i & 1;
   s1 = (i>>1) & 1;
   s2 = (i>>2) & 1;
   s3 = (i>>3) & 1;
   s4 = (i>>4) & 1;
   itr = (i>>5) & 1;
   ire = (i>>6) & 1;
   mse = (i>>7) & 1;
   tplo = (i>>8) & 1;
   tphi = (i>>9) & 1;
   if((s1==1) && (s2==0)  && (s3==1) && (s4==0) && (itr==1) &&
      (ire==0) && (mse==1) && (tplo==0) && (tphi==1) && 
      (trig==0) ) ok++;
   can_intg_gain(id,0xa);   /* s1=s3=0 s2=s4=1 */
   can_itr_low(id);           /* itr=1 */
   can_intg_rd_high(id);        /* ire=1 */
   can_mse_low(id);           /* mse=0 */
   can_small_cap_enable(id);  /* tplo=1 */
   can_large_cap_disable(id);   /* tphi=0 */
   can_trig_enable(id);       /* trig=1 */
   can_bkld_high(id);
   can_bkld_low(id);
   status=can_3in1set(id,0xd400);   /* fetch from 3in1 */
   status=can_3in1get(id,&shrt);    /* and load can output reg */
   i = (int)shrt;
   trig = i & 1;
   s1 = (i>>1) & 1;
   s2 = (i>>2) & 1;
   s3 = (i>>3) & 1;
   s4 = (i>>4) & 1;
   itr = (i>>5) & 1;
   ire = (i>>6) & 1;
   mse = (i>>7) & 1;
   tplo = (i>>8) & 1;
   tphi = (i>>9) & 1;
   if((s1==0) && (s2==1)  && (s3==0) && (s4==1) && (itr==0) &&
      (ire==1) && (mse==0) && (tplo==1) && (tphi==0) && 
      (trig==1) ) ok++;
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
   if(ok == 2) status=1;
   return(status);
  }
