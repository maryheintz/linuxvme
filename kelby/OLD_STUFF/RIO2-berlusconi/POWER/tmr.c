/* buttons.c
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
#include <Xm/XmStrDefs.h>
/*
 * public include files for widgets used in this file
 */
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>

#define OFFSET 10



static int mask = 0,ient=0,ibad[5]={1,1,1,1,1};
char string[800];
int gpib_adr;
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
	set_on(1);
}

/*
 * "Drawer 2" button callback function
 */
/*ARGSUSED*/
void Drawer2(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_on(2);
}

/*
 * "Drawer 3" button callback function
 */
/*ARGSUSED*/
void Drawer3(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_on(3);
}

/*
 * "Drawer 4" button callback function
 */
/*ARGSUSED*/
void Drawer4(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_on(4);
}

/*
 * "Drawer 5" button callback function
 */
/*ARGSUSED*/
void Drawer5(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_on(5);
}

/*
 * "allon" button callback function
 */
/*ARGSUSED*/
void ALLON(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_on(0);
}

/*
 * "Off" button callback function
 */
/*ARGSUSED*/
void Off1(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_off(1);
}

/*
 * "Off" button callback function
 */
/*ARGSUSED*/
void Off2(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_off(2);
}

/*
 * "Off" button callback function
 */
/*ARGSUSED*/
void Off3(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_off(3);
}

/*
 * "Off" button callback function
 */
/*ARGSUSED*/
void Off4(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_off(4);
}

/*
 * "Off" button callback function
 */
/*ARGSUSED*/
void Off5(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_off(5);
}
/*
 * "offall" button callback function
 */
/*ARGSUSED*/
void ALLOFF(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	set_off(0);
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

{	
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
set_on(int k)
 { printf("set_on(%d) called\n",k);
   if(k != 0) ibad[k-1]=0;
   if(k == 0)
     { ibad[0]=0;
       ibad[1]=0;
       ibad[2]=0;
       ibad[3]=0;
       ibad[4]=0;
     }
   voltcheck(); /* to change button color */
 }
set_off(int k)
 { printf("set_off(%d) called\n",k);
   if(k != 0) ibad[k-1]=1;
   if(k == 0)
     { ibad[0]=1;
       ibad[1]=1;
       ibad[2]=1;
       ibad[3]=1;
       ibad[4]=1;
     }
   voltcheck(); /* to change button color */
 }
voltcheck()
 {  set_button_colors();
 }
set_button_colors()
 { 
 if(ibad[0]==1) /* drawer voltage 1 is off */
      {	strcpy(string,"green");
	XtVaSetValues(drawer1,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"red");
	XtVaSetValues(off1,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(ibad[0]==0) /* drawer voltage 1 is on */
      {	strcpy(string,"red");
	XtVaSetValues(drawer1,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"green");
	XtVaSetValues(off1,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }

 if(ibad[1]==1) /* drawer voltage 2 is off */
      {	strcpy(string,"green");
	XtVaSetValues(drawer2,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"red");
	XtVaSetValues(off2,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(ibad[1]==0) /* drawer voltage 2 is on */
      {	strcpy(string,"red");
	XtVaSetValues(drawer2,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"green");
	XtVaSetValues(off2,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(ibad[2]==1) /* drawer voltage 3 is off */
      {	strcpy(string,"green");
	XtVaSetValues(drawer3,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"red");
	XtVaSetValues(off3,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(ibad[2]==0) /* drawer voltage 3 is on */
      {	strcpy(string,"red");
	XtVaSetValues(drawer3,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"green");
	XtVaSetValues(off3,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(ibad[3]==1) /* drawer voltage 4 is off */
      {	strcpy(string,"green");
	XtVaSetValues(drawer4,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"red");
	XtVaSetValues(off4,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(ibad[3]==0) /* drawer voltage 4 is on */
      {	strcpy(string,"red");
	XtVaSetValues(drawer4,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"green");
	XtVaSetValues(off4,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(ibad[4]==1) /* drawer voltage 5 is off */
      {	strcpy(string,"green");
	XtVaSetValues(drawer5,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"red");
	XtVaSetValues(off5,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 if(ibad[4]==0) /* drawer voltage 5 is on */
      {	strcpy(string,"red");
	XtVaSetValues(drawer5,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      	strcpy(string,"green");
	XtVaSetValues(off5,XtVaTypedArg,XmNbackground,XtRString,string,
	  strlen(string)+1,NULL);
      }
 }     
