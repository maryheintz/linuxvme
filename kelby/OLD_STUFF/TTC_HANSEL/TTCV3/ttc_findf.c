#include <stdio.h>
#include <math.h>
int ttcadr,id;
main()
{ 
   int i,k;
   int good,bad,total,kkk;
   ttcadr = 0x1234;
   printf("enter ttcadr\n");
   fscanf(stdin,"%x",&ttcadr); 
   ttcvi_map(ttcadr);
   printf("ttcvi_map returned\n");
strt:
   good=0;
   bad=0;
   for(k=1;k<49;k++) 
     {  kkk=cardchk(k);
        if(kkk == 1) 
	 { good++;
           printf(" card %d found\n",k);
	 }
	if(kkk != 1) 
	 { bad++; 
           printf("no card %d\n",k);
	 }
     }
   total=good+bad;
   printf(" good=%d    bad=%d     %d\n",good,bad,total);
   if(sw(1) == 1) 
     { printf(" \n");
       sleep(1);
       goto strt;
     }
      
}
cardchk(k)
int k;
  {int i,status;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,ok;
   double a;
/*   printf(" cardchk entered k=%d\n",k);  */
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
   usleep(5);
/*   printf(" cardchk step 1\n");  */
   status=can_3in1get(id,&shrt);    /* and load can output reg */
/*   printf(" shrt=%x\n",shrt);   
   printf(" cardchk step 2\n");
   fflush(stdout);  */
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
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x %d\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i,ok);

/*   printf(" cardchk step 3\n");  */
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
   usleep(5);
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
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x %d\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i,ok);
  if(ok == 2) status=1;
   return(status);
  }
