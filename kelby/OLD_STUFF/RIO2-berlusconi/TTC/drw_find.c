#include <stdio.h>
#include <math.h>
unsigned int id,ttcadr;
main()
{ 
   int tube,i,k,board;
    char serial[5][6] = {"PS2013","PS2014","PS2016",
                        "PS2018","PS2020"};
    int tadr[5]={0x2001,0x3806,0x2003,0x2007,0x3807};			
    char snum[7]; 
   unsigned int i1,i2,itst;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   long status;
   int good,bad,total,kkk;
    printf("enter board\n");
    fscanf(stdin,"%d",&board);
    ttcadr = tadr[board-1];
    id = (board<<6) | 1;
    snum[0]=serial[board-1][0];
    snum[1]=serial[board-1][1];
    snum[2]=serial[board-1][2];
    snum[3]=serial[board-1][3];
    snum[4]=serial[board-1][4];
    snum[5]=serial[board-1][5];
    snum[6]='\0';
    printf(" board %d  id=%x  ttcadr=%x",board,id,ttcadr);
   printf("  serial=%s\n",snum);
   ttcvi_map(ttcadr);
   reset_ctrl_can();
   printf("calling can_reset\n");
   can_reset(ttcadr);  
   printf("calling can_init\n");
   can_init(board,snum);  
   printf("returned from can_init\n");
strt:
   good=0;
   bad=0;
   for(k=1;k<49;k++) 
     {  kkk=cardchk(k);
        if(kkk == 1)
	  { good++;
            if(sw(7) == 0) printf(" card %d found\n",k);
	  }
 	if(kkk != 1)
	  { bad++; 
            printf("no card %d\n",k);
	  }
     }
   total=good+bad;
   if(sw(7) == 0) printf(" good=%d    bad=%d     %d\n",good,bad,total);
   if(sw(1) == 1) 
     { if(sw(7) == 0) printf(" \n");
       sleep(1);
       goto strt;
     }
      
}
cardchk(k)
int k;
  {int i,status;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,ok;
   ok=0;
   status=0;
   multi_low(ttcadr);
   back_low(ttcadr);
   set_tube(ttcadr,k);
   set_intg_gain(ttcadr,5);   /* s1=s3=1 s2=s4=0 */
   itr_high(ttcadr);           /* itr=1 */
   intg_rd_low(ttcadr);        /* ire=0 */
   mse_high(ttcadr);           /* mse=1 */
   small_cap_disable(ttcadr);  /* tplo=0 */
   large_cap_enable(ttcadr);   /* tphi=1 */
   trig_disable(ttcadr);       /* trig=0 */
   back_high(ttcadr);
   back_low(ttcadr);
   load_can(ttcadr);   /* fetch from 3in1 */
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
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);

   set_intg_gain(ttcadr,0xa);   /* s1=s3=0 s2=s4=1 */
   itr_low(ttcadr);           /* itr=0 */
   intg_rd_high(ttcadr);        /* ire=1 */
   mse_low(ttcadr);           /* mse=0 */
   small_cap_enable(ttcadr);  /* tplo=1 */
   large_cap_disable(ttcadr);   /* tphi=0 */
   trig_enable(ttcadr);       /* trig=1 */
   back_high(ttcadr);
   back_low(ttcadr);
   load_can(ttcadr);   /* fetch from 3in1 */
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
