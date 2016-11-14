/* POWER.c
 * creates an interface enabling user to turn on and off power to five drawers
 */
 
/*
 * so that we can use fprintf:
 */
#include <stdio.h>
#include <sys/types.h>

/*
 * standard motif include file:
 */
#include <Xm/Xm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <X11/Xresource.h>
#include <Xm/RepType.h>

/*
 * public include files for widgets used in this file
 */
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>

#define OFFSET 10

static int mask = 0,ient=0,jbad[5],pset[5],ptrip[5];
static int gpib_adr;
static int ipass=0;
double window_h[15] = {8.0,-3.5,16.2,  8.0,-3.5,16.2,  8.0,-3.5,16.2,
                       8.0,-3.5,16.2,  8.0,-3.5,16.2};
double window_l[15] = {3.5,-7.5,13.5,  3.5,-7.5,13.5,  3.5,-7.5,13.5,  
                       3.5,-7.5,13.5,  3.5,-7.5,13.5};  
char string[800];
FILE *fp;

static XtAppContext app_context;

void timer_handler(XtPointer dummy1, XtIntervalId *dummy2)
 { /* reinstall callback */
   (void) XtAppAddTimeOut(app_context,30000,timer_handler,NULL);
   voltcheck();
 }
	
/*
 * "Drawer 1" button callback function
 */
/*ARGSUSED*/
void Drawer1(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_on(1,1);
}

/*
 * "Drawer 2" button callback function
 */
/*ARGSUSED*/
void Drawer2(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_on(2,1);
}

/*
 * "Drawer 3" button callback function
 */
/*ARGSUSED*/
void Drawer3(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_on(3,1);
}

/*
 * "Drawer 4" button callback function
 */
/*ARGSUSED*/
void Drawer4(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_on(4,1);
}

/*
 * "Drawer 5" button callback function
 */
/*ARGSUSED*/
void Drawer5(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_on(5,1);
}

/*
 * "allon" button callback function
 */
/*ARGSUSED*/
void ALLON(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_on(0,1);
}

/*
 * "Off" button callback function
 */
/*ARGSUSED*/
void Off1(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_off(1,1);
}

/*
 * "Off" button callback function
 */
/*ARGSUSED*/
void Off2(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_off(2,1);
}

/*
 * "Off" button callback function
 */
/*ARGSUSED*/
void Off3(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_off(3,1);
}

/*
 * "Off" button callback function
 */
/*ARGSUSED*/
void Off4(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_off(4,1);
}

/*
 * "Off" button callback function
 */
/*ARGSUSED*/
void Off5(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_off(5,1);
}
/*
 * "offall" button callback function
 */
/*ARGSUSED*/
void ALLOFF(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_off(0,1);
}

IceProtocolSetupStatus IceProtocolSetup (
    IceConn     a       /* iceConn */,
    int         b       /* myOpcode */,
    IcePointer  c       /* clientData */,
    Bool        d       /* mustAuthenticate */,
    int        *e       /* majorVersionRet */,
    int        *f       /* minorVersionRet */,
    char      **g       /* vendorRet */,
    char      **h       /* releaseRet */,
    int         i       /* errorLength */,
    char       *j       /* errorStringRet */
)
{
  return 0;
}
int IceRegisterForProtocolSetup (
    char                *l      /* protocolName */,
    char                *m      /* vendor */,
    char                *n      /* release */,
    int                  o      /* versionCount */,
    IcePoVersionRec    * p      /* versionRecs */,
    int                  q      /* authCount */,
    char              ** r      /* authNames */,
    IcePoAuthProc      * t      /* authProcs */,
    IceIOErrorProc       v      /* IOErrorProc */
)
{
  return 0;
}
Widget form, drawer1, drawer2, drawer3, drawer4, drawer5,allon,
	off1, off2, off3, off4, off5,alloff, topLevel;
main(argc, argv)
int argc;
char **argv;

{	 fp = fopen("/home/user/kelby/POWER/power.log","a");
         fprintf(fp,"program restart\n");
         fflush(fp);
         system("date>>/home/user/kelby/POWER/power.log");
         pset[1]=0;
         pset[2]=0;
         pset[3]=0;
         pset[4]=0;
	 ptrip[0]=0;
	 ptrip[1]=0;
	 ptrip[2]=0;
	 ptrip[3]=0;
	 ptrip[4]=0;
	 
	 XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);
	
	topLevel = XtVaAppInitialize(
		&app_context,	/*Application Context*/
		"XForm1",	/*Application class*/
		NULL, 0,	/*command line option list*/
		&argc, argv,	/*command line args*/
		NULL,		/*for missing app-defaults file*/
		NULL);		/*terminate varargs list*/
	
	form = XtVaCreateManagedWidget(
		"form",			/*widget name*/
		xmFormWidgetClass,	/*widget class*/
		topLevel,		/*parent widget*/
		NULL);			/*terminate varargs list*/
		
	drawer1 = XtVaCreateManagedWidget(
		"1 ON",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, OFFSET,
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, OFFSET,
		NULL);			/*terminate varargs list*/
		
	drawer2 = XtVaCreateManagedWidget(
		"2 ON",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, OFFSET,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, drawer1,
		XmNtopOffset, OFFSET,
		NULL);			/*terminate varargs list*/

	drawer3 = XtVaCreateManagedWidget(
		"3 ON",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, OFFSET,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, drawer2,
		XmNtopOffset, OFFSET,
		NULL);			/*terminate varargs list*/

	drawer4 = XtVaCreateManagedWidget(
		"4 ON",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, OFFSET,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, drawer3,
		XmNtopOffset, OFFSET,
		NULL);			/*terminate varargs list*/

	drawer5 = XtVaCreateManagedWidget(
		"5 ON",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, OFFSET,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, drawer4,
		XmNtopOffset, OFFSET,
		NULL);			/*terminate varargs list*/ 
	
	allon = XtVaCreateManagedWidget(
		"ALL ON",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, OFFSET,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, drawer5,
		XmNtopOffset, OFFSET,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, OFFSET,
		NULL);			/*terminate varargs list*/ 
	
	off1 = XtVaCreateManagedWidget(
		"1 OFF",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, OFFSET,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, OFFSET,
		XmNrightAttachment, XmATTACH_WIDGET,	 
		XmNrightWidget, drawer1,	 
		XmNrightOffset, OFFSET,
		NULL);	/*terminate varargs list of resource settings*/
	
	off2 = XtVaCreateManagedWidget(
		"2 OFF",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, off1,
		XmNtopOffset, OFFSET,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, OFFSET,
		XmNrightAttachment, XmATTACH_WIDGET,	 
		XmNrightWidget, drawer2,	 
		XmNrightOffset, OFFSET,
		NULL);	/*terminate varargs list of resource settings*/
	
	off3 = XtVaCreateManagedWidget(
		"3 OFF",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, off2,
		XmNtopOffset, OFFSET,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, OFFSET,
		XmNrightAttachment, XmATTACH_WIDGET,	
		XmNrightWidget, drawer3,	
		XmNrightOffset, OFFSET,
		NULL);	/*terminate varargs list of resource settings*/
	
	off4 = XtVaCreateManagedWidget(
		"4 OFF",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, off3,
		XmNtopOffset, OFFSET,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, OFFSET,
		XmNrightAttachment, XmATTACH_WIDGET,	
		XmNrightWidget, drawer4,
		XmNrightOffset, OFFSET,	
		NULL);	/*terminate varargs list of resource settings*/
		
	off5 = XtVaCreateManagedWidget(
		"5 OFF",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, off4,
		XmNtopOffset, OFFSET,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, OFFSET,
		XmNrightAttachment, XmATTACH_WIDGET,	
		XmNrightWidget, drawer5,
		XmNrightOffset, OFFSET,
		NULL);	/*terminate varargs list of resource settings*/
	alloff = XtVaCreateManagedWidget(
		"ALL OFF ",		
		xmPushButtonWidgetClass,
		form,			
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, off5,
		XmNtopOffset, OFFSET,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, OFFSET,
		XmNrightAttachment, XmATTACH_WIDGET,	
		XmNrightWidget, allon,
		XmNrightOffset, OFFSET,     
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, OFFSET,	 
		NULL);	
		
	XtAddCallback(drawer1, XmNactivateCallback, Drawer1, 0);
	XtAddCallback(drawer2, XmNactivateCallback, Drawer2, 0);
	XtAddCallback(drawer3, XmNactivateCallback, Drawer3, 0);
	XtAddCallback(drawer4, XmNactivateCallback, Drawer4, 0);
	XtAddCallback(drawer5, XmNactivateCallback, Drawer5, 0);
	XtAddCallback(allon, XmNactivateCallback, ALLON, 0);
	
	XtAddCallback(off1, XmNactivateCallback, Off1, 0);
	XtAddCallback(off2, XmNactivateCallback, Off2, 0);
	XtAddCallback(off3, XmNactivateCallback, Off3, 0);
	XtAddCallback(off4, XmNactivateCallback, Off4, 0);
	XtAddCallback(off5, XmNactivateCallback, Off5, 0);
	XtAddCallback(alloff, XmNactivateCallback, ALLOFF, 0);   
	
	(void) XtAppAddTimeOut(app_context,30000,timer_handler,NULL);
	XtRealizeWidget(topLevel);
	
/* set main background blue */
	strcpy(string,"blue");
	XtVaSetValues(form,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
/* set onall background yellow */
	strcpy(string,"yellow");
	XtVaSetValues(allon,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
/* set alloff background yellow */
	strcpy(string,"yellow");
	XtVaSetValues(alloff,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
	
        voltcheck();  /* to initialize the button colors */

	XtAppMainLoop(app_context);
}				
set_on(int k,int l)
 { if(ient == 0) 
    { gpib_reset();
      ient = 1;
    }
   fprintf(fp,"drawer %d set on\n",k);
   fflush(fp);
   system("date>>/home/user/kelby/POWER/power.log");
   if(k==0 && l==1)
    { pset[0]=1;
      pset[1]=1;
      pset[2]=1;
      pset[3]=1;
      pset[4]=1;
      ptrip[0]=0;
      ptrip[1]=0;
      ptrip[2]=0;
      ptrip[3]=0;
      ptrip[4]=0;
      ptrip[5]=0;
     }
   if(k>0 && k<5 && l==1)
    { pset[k-1]=1;
      ptrip[k-1]=0;
    }
   if(k == 0) mask = 0xf8;
   if(k == 1) mask = mask | 0x8;
   if(k == 2) mask = mask | 0x10;
   if(k == 3) mask = mask | 0x20;
   if(k == 4) mask = mask | 0x40;
   if(k == 5) mask = mask | 0x80;
   sprintf(string,"D%d X",mask);
   gpib_adr=2;
   sendmessage(gpib_adr,string);
   voltcheck();  /* update the button colors */
 }
set_off(int k,int l)
 { if(ient == 0) 
    { gpib_reset();
      ient = 1;
    }
   fprintf(fp,"drawer %d set off\n",k);
   fflush(fp);
   system("date>>/home/user/kelby/POWER/power.log");
   if(k==0 && l==1)
     { pset[0]=0;
       pset[1]=0;
       pset[2]=0;
       pset[3]=0;
       pset[4]=0;
     }
   if(k>0 && k<5 && l==1) pset[k-1]=0;
   if(k == 0) mask = 0;
   if(k == 1) mask = mask & 0xf7;
   if(k == 2) mask = mask & 0xef;
   if(k == 3) mask = mask & 0xdf;
   if(k == 4) mask = mask & 0xbf;
   if(k == 5) mask = mask & 0x7f;
   sprintf(string,"D%d X",mask);
   gpib_adr=2;
   sendmessage(gpib_adr,string);
   voltcheck();  /* update the button colors */
 }
  
voltcheck()
 {
 /* called every 30 seconds */
   double volts[16];
   int i,dval,bad[15],ibad1,ibad2,ibad3,ibad4,ibad5;
   static int ipass = 0;
   if(ient == 0) 
    { gpib_reset();
      ient = 1;
    }
   gpib_adr=2;
   for(i=0;i<8;i++)
     { go_get(i+1,&volts[i]);
       /*printf("i=%d  gpib_adr=%d  volts=%f\n",i,gpib_adr,volts[i]);  */
       if(i == 2) volts[i] = 3.0*volts[i];
       if(i == 5) volts[i] = 3.0*volts[i];
       if(i == 8) volts[i] = 3.0*volts[i];
       
     }

   gpib_adr=3;
   for(i=0;i<8;i++)
     { go_get(i+1,&volts[i+8]);
       /*printf("i=%d  gpib_adr=%d  volts=%f\n",i,gpib_adr,volts[i]);  */
       if(i == 0) volts[i+8] = 3.0*volts[i+8];
       if(i == 3) volts[i+8] = 3.0*volts[i+8];
       if(i == 6) volts[i+8] = 3.0*volts[i+8];
     }
   for(i=0;i<15;i++)
   { bad[i] = 0;
     /*printf(" i=%d  low=%f  volts=%f  high=%f\n",
        i,window_l[i],volts[i],window_h[i]);  */  
    if(volts[i] < window_l[i] || volts[i] > window_h[i]) bad[i] = 1;
    /*printf("i=%d  volt=%f  bad=%d\n",i,volts[i],bad[i]);  */ 
    }
   ibad1 = bad[0]+bad[1]+bad[2];
   ibad2 = bad[3]+bad[4]+bad[5];
   ibad3 = bad[6]+bad[7]+bad[8];
   ibad4 = bad[9]+bad[10]+bad[11];
   ibad5 = bad[12]+bad[13]+bad[14];
   
   if(ibad1>0 && ibad1 <3) 
    { alarm(1,ibad1);
      fprintf(fp,"drawer 1 set off   ibad1 = %d  volts=%f %f %f\n",
       ibad1,volts[0],volts[1],volts[2]);
       fflush(fp);
       system("date>>/home/user/kelby/POWER/power.log");
    }
   if(ibad2>0 && ibad2 <3)
    { alarm(2,ibad2);
      fprintf(fp,"drawer 2 set off   ibad2 = %d  volts=%f %f %f\n",
       ibad2,volts[3],volts[4],volts[5]);
      fflush(fp);
      system("date>>/home/user/kelby/POWER/power.log");
    }
   if(ibad3>0 && ibad3 <3)
    { alarm(3,ibad3);
      fprintf(fp,"drawer 3 set off   ibad3 = %d  volts=%f %f %f\n",
       ibad3,volts[6],volts[7],volts[8]);
      fflush(fp);
      system("date>>/home/user/kelby/POWER/power.log");
    }
   if(ibad4>0 && ibad4 <3)
    { alarm(4,ibad4);
      fprintf(fp,"drawer 4 set off   ibad4 = %d  volts=%f %f %f\n",
       ibad4,volts[9],volts[10],volts[11]);
      fflush(fp);
      system("date>>/home/user/kelby/POWER/power.log");
    }
   if(ibad5>0 && ibad5 <3)
    { alarm(5,ibad5);
      fprintf(fp,"drawer 5 set off   ibad5 = %d  volts=%f %f %f\n",
       ibad5,volts[12],volts[13],volts[14]);
      fflush(fp);
      system("date>>/home/user/kelby/POWER/power.log");
    }
  jbad[0]=0;
  if(ibad1 !=0) jbad[0]=1;

  jbad[1]=0;
  if(ibad2 !=0) jbad[1]=1;

  jbad[2]=0;
  if(ibad3 !=0) jbad[2]=1;

  jbad[3]=0;
  if(ibad4 !=0) jbad[3]=1;

  jbad[4]=0;
  if(ibad5 !=0) jbad[4]=1;

  set_button_colors();
 }
  alarm(int i,int ibad)
  { int k;
    if(ptrip[i-1] > 20) set_off(i,0);
    if(ptrip[i-1] < 21) set_on(i,0);
    ptrip[i-1]++;
 }

    go_get(channel,v)
      int channel;
      double *v;
      {static int i2=0,i3=0;
       double volts;
       int i;

       if(gpib_adr==2 && i2==0)
           {set_gpib();
	    i2=1;
	   }

       if(gpib_adr==3 && i3==0)
           {set_gpib();
	    i3=1;
	   }

       sprintf(string,"C%d X",channel);  /* scan 1 channel */
       sendmessage(gpib_adr,string);
       usleep((time_t)5); 
  
       sendmessage(gpib_adr,"T6 X");   /* trigger voltage reading */
       usleep((time_t)5); 
       getmessage(gpib_adr, &string[0] );

/*     printf("readback %d = %s\n",i,string);  */  
       sscanf(string,"%le",&volts);
/*       printf("i=%d  volts=%f\n",i,volts);   */
       *v = volts;   
}
set_gpib()
 {     sendmessage(gpib_adr,"A0 X");  /* set 8 diff inputs - master */
       usleep((time_t)5); 
	  
       sendmessage(gpib_adr,"R3,3,3,3,3,3,3,3 X");  
       usleep((time_t)5); 

       sendmessage(gpib_adr,"N1 X");
       usleep((time_t)5); 

       sendmessage(gpib_adr,"G0 X");
       usleep((time_t)5); 
  
       sendmessage(gpib_adr,"Q0 X");
       usleep((time_t)5); 

       sendmessage(gpib_adr,"I15 X");
       usleep((time_t)5); 
 }

on(int k)
 { blank();
   bgyellow();
   red();
   printf("%d-ON",k);
   bgdefault();  /* restore default background */
   fgdefault();  /* restore default foreground */
 }
off(int k)
 { blank();
   blue(); 
   bgdefault();
   printf("%d-OFF",k);
   bgdefault();  /* restore default background */
   fgdefault();  /* restore default foreground */
 }
red()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(51);   /*     3     */ 
   putchar(49);   /*     1     */
   putchar(109);  /*     m     */ 
 }
 black()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(51);   /*     3     */ 
   putchar(57);   /*     9     */
   putchar(109);  /*     m     */ 
 }
 blue()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(51);   /*     3     */ 
   putchar(52);   /*     4     */
   putchar(109);  /*     m     */ 
 }
 white()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(57);   /*     5     */ 
   putchar(52);   /*     4     */
   putchar(109);  /*     m     */ 
 }
 bgyellow()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(52);   /*     4     */ 
   putchar(51);   /*     3     */
   putchar(109);  /*     m     */ 
 }
 bgdefault()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(52);   /*     4     */ 
   putchar(57);   /*     9     */
   putchar(109);  /*     m     */ 
 }
 fgdefault()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(51);   /*     3     */ 
   putchar(57);   /*     9     */
   putchar(109);  /*     m     */ 
 }
blank()
  { bgdefault();
    putchar(32);
  }
set_button_colors()
 { 
 if(jbad[0]==1) /* drawer voltage 1 is off */
      {	strcpy(string,"green");
	XtVaSetValues(drawer1,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"red");
	XtVaSetValues(off1,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(jbad[0]==0) /* drawer voltage 1 is on */
      {	strcpy(string,"red");
	XtVaSetValues(drawer1,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"green");
	XtVaSetValues(off1,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }

 if(jbad[1]==1) /* drawer voltage 2 is off */
      {	strcpy(string,"green");
	XtVaSetValues(drawer2,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"red");
	XtVaSetValues(off2,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(jbad[1]==0) /* drawer voltage 2 is on */
      {	strcpy(string,"red");
	XtVaSetValues(drawer2,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"green");
	XtVaSetValues(off2,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(jbad[2]==1) /* drawer voltage 3 is off */
      {	strcpy(string,"green");
	XtVaSetValues(drawer3,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"red");
	XtVaSetValues(off3,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(jbad[2]==0) /* drawer voltage 3 is on */
      {	strcpy(string,"red");
	XtVaSetValues(drawer3,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"green");
	XtVaSetValues(off3,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(jbad[3]==1) /* drawer voltage 4 is off */
      {	strcpy(string,"green");
	XtVaSetValues(drawer4,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"red");
	XtVaSetValues(off4,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(jbad[3]==0) /* drawer voltage 4 is on */
      {	strcpy(string,"red");
	XtVaSetValues(drawer4,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"green");
	XtVaSetValues(off4,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(jbad[4]==1) /* drawer voltage 5 is off */
      {	strcpy(string,"green");
	XtVaSetValues(drawer5,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"red");
	XtVaSetValues(off5,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(jbad[4]==0) /* drawer voltage 5 is on */
      {	strcpy(string,"red");
	XtVaSetValues(drawer5,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"green");
	XtVaSetValues(off5,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 }     
