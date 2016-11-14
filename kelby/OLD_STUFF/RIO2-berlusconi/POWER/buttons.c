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

/*
 * public include files for widgets used in this file
 */
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>

#define OFFSET 10

static int mask = 0,ient=0;
char string[800];
int gpib_adr;

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
main(argc, argv)
int argc;
char **argv;

{
	XtAppContext app_context;
	Widget form, drawer1, drawer2, drawer3, drawer4, drawer5,
	off1, off2, off3, off4, off5, topLevel;
	
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
		"Drawer 1",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, OFFSET,
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, OFFSET,
		NULL);			/*terminate varargs list*/
		
	drawer2 = XtVaCreateManagedWidget(
		"Drawer 2",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, OFFSET,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, drawer1,
		XmNtopOffset, OFFSET,
		NULL);			/*terminate varargs list*/

	drawer3 = XtVaCreateManagedWidget(
		"Drawer 3",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, OFFSET,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, drawer2,
		XmNtopOffset, OFFSET,
		NULL);			/*terminate varargs list*/

	drawer4 = XtVaCreateManagedWidget(
		"Drawer 4",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, OFFSET,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, drawer3,
		XmNtopOffset, OFFSET,
		NULL);			/*terminate varargs list*/

	drawer5 = XtVaCreateManagedWidget(
		"Drawer 5",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		form,			/*parent widget*/
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, OFFSET,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, drawer4,
		XmNtopOffset, OFFSET,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, OFFSET,
		NULL);			/*terminate varargs list*/ 
	
	off1 = XtVaCreateManagedWidget(
		"Off 1",		/*widget name*/
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
		"Off 2",		/*widget name*/
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
		"Off 3",		/*widget name*/
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
		"Off 4",		/*widget name*/
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
		"Off 5",		/*widget name*/
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
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, OFFSET,	 
		NULL);	/*terminate varargs list of resource settings*/
		
	XtAddCallback(drawer1, XmNactivateCallback, Drawer1, 0);
	XtAddCallback(drawer2, XmNactivateCallback, Drawer2, 0);
	XtAddCallback(drawer3, XmNactivateCallback, Drawer3, 0);
	XtAddCallback(drawer4, XmNactivateCallback, Drawer4, 0);
	XtAddCallback(drawer5, XmNactivateCallback, Drawer5, 0);
	
	XtAddCallback(off1, XmNactivateCallback, Off1, 0);
	XtAddCallback(off2, XmNactivateCallback, Off2, 0);
	XtAddCallback(off3, XmNactivateCallback, Off3, 0);
	XtAddCallback(off4, XmNactivateCallback, Off4, 0);
	XtAddCallback(off5, XmNactivateCallback, Off5, 0);
	
	XtRealizeWidget(topLevel);
	
	XtAppMainLoop(app_context);
}				
set_on(int k)
 { if(ient == 0) 
    { gpib_reset();
      ient = 1;
    }
   if(k == 1) mask = mask | 0x8;
   if(k == 2) mask = mask | 0x10;
   if(k == 3) mask = mask | 0x10;
   if(k == 4) mask = mask | 0x10;
   if(k == 5) mask = mask | 0x10;
   sprintf(string,"D%d X",mask);
   printf(" mask=%x\n",mask);
   gpib_adr=2;
   sendmessage(gpib_adr,string);
   sleep(2);
   sendmessage(gpib_adr,"D? X");
   getmessage(gpib_adr, &string[0] );
   printf("D back=%s\n",string);  
 }
set_off(int k)
 { if(ient == 0) 
    { gpib_reset();
      ient = 1;
    }
   if(k == 1) mask = mask & 0xf7;
   if(k == 2) mask = mask & 0xef;
   if(k == 3) mask = mask & 0xdf;
   if(k == 4) mask = mask & 0xbf;
   if(k == 5) mask = mask & 0x7f;
   sprintf(string,"D%d X",mask);
   printf(" mask=%x\n",mask);
   gpib_adr=2;
   sendmessage(gpib_adr,string);
   sleep(2);
   sendmessage(gpib_adr,"D? X");
   getmessage(gpib_adr, &string[0] );
   printf("D back=%s\n",string);  
 }
  
