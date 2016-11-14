/*
 * buttons.c - simple button box
 */
 
/*
 * so that we can use fprintf:
 */
#include <stdio.h>
#include <sys/types.h>
/*
 * standard motif indlude file:
 */
#include <Xm/Xm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <X11/Xresource.h>


/*
 * public include files for widgets used in this file
 */
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
/*
 * "Quit" button callback function
 */
/*ARGSUSED*/
void Quit(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	exit(0);
}

/*
 * "Press me!" button callback function
 */
/*ARGSUSED*/
void PressMe(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
	fprintf(stderr, "Thank you!\n");
}

main(argc, argv)
int argc;
char **argv;

{
	XtAppContext app_context;
	Widget rowColumn, quit,  pressme, topLevel;
	
	 XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);
	
	topLevel = XtVaAppInitialize(
		&app_context,	/*Application Context*/
		"XRowColumn1",	/*Application class*/
		NULL, 0,	/*command line option list*/
		&argc, argv,	/*command line args*/
		NULL,		/*for missing app-defaults file*/
		NULL);		/*terminate varargs list*/
	
	rowColumn = XtVaCreateManagedWidget(
		"rowColumn",		/*widget name*/
		xmRowColumnWidgetClass,	/*widget class*/
		topLevel,		/*parent widget*/
		NULL);			/*terminate varargs list*/
	
	pressme = XtVaCreateManagedWidget(
		"pressme",		/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		rowColumn,		/*parent widget*/
		NULL);			/*terminate varargs list*/
		
	quit = XtVaCreateManagedWidget(
		"quit",			/*widget name*/
		xmPushButtonWidgetClass,/*widget class*/
		rowColumn,		/*parent widget*/
		NULL);			/*terminate varargs list*/
	
	XtAddCallback(pressme, XmNactivateCallback, PressMe, 0); 
	XtAddCallback(quit, XmNactivateCallback, Quit, 0);
		
	XtRealizeWidget(topLevel);
	
	XtAppMainLoop(app_context);
}				
