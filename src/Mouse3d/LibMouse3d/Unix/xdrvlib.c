/*|--------------------------------------------------------------------------

 05-Oct-95  G-01-08  VS   $$1 Magellan 3D Controller support for X Windows

 MAGELLAN X-Window Driver Library                       Version 4.00 09.06.97

 Copyright (C) 1984, 2002 3Dconnexion GmbH / Inc.

 An der Hartmuehle 8
 D - 82229 Seefeld

 mailto:development@3dconnexion.com

----------------------------------------------------------------------------|*/
#include <string.h>
#include <math.h>
#include <stdio.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#define MagellanDEBUG
#include "xdrvlib.h"


#ifndef XDRVLIB_H
#define XDRVLIB_H
/*|--------------------------------------------------------------------------

 MAGELLAN X-Window Driver Header                        Version 4.00 09.06.97

 Copyright (C) 1984, 2002 3Dconnexion GmbH / Inc.

 An der Hartmuehle 8
 D - 82229 Seefeld

 mailto:development@3dconnexion.com

----------------------------------------------------------------------------|*/

#ifndef TRUE
#define TRUE	(1==1)
#endif

#ifndef FALSE
#define FALSE	(1==0)
#endif

#ifdef SGI
#define ParameterCheck
#endif

#ifdef IBM
#define ParameterCheck
#endif

/* Data Structure for CADDS5 implementation */
#define MagellanInputMotionEvent 1
#define MagellanInputButtonPressEvent 2
#define MagellanInputButtonReleaseEvent 3

union _MagellanInputEventUnion_ {
 int data[7];
 int button;
};
typedef union _MagellanInputEventUnion_ MagellanIntUnion;

struct _MagellanIntEvent_ {
 int type;
 MagellanIntUnion u;
};
typedef struct _MagellanIntEvent_ MagellanIntEvent;


/* Data Structure for Pro/ENGINEER implementation */
enum _MagellanData_ { MagellanX,MagellanY,MagellanZ,MagellanA,MagellanB,MagellanC };

struct _MagellanFloatEvent_ {
 int MagellanType;
 int MagellanButton;
 double MagellanData[6];
 int MagellanPeriod;
};
typedef struct _MagellanFloatEvent_ MagellanFloatEvent;



int MagellanInit(
#ifdef ParameterCheck
Display *display,
Window window
#endif
);

int MagellanSetWindow(
#ifdef ParameterCheck
Display *display,
Window window
#endif
);

int MagellanApplicationSensitivity(
#ifdef ParameterCheck
Display *display,
double Sensitivity
#endif
);

static int MagellanErrorHandler(
#ifdef ParameterCheck
Display *display,
XErrorEvent *Error
#endif
);

int MagellanInputEvent(
#ifdef ParameterCheck
Display *display,
XEvent *Event,
MagellanIntEvent *MagellanEvent
#endif
);

int MagellanTranslateEvent(
#ifdef ParameterCheck
Display *display,
XEvent *Event,
MagellanFloatEvent *MagellanEvent,
double MagellanTranslationScale,
double MagellanRotationScale
#endif
);

int MagellanClose(
#ifdef ParameterCheck
Display *display
#endif
);

Bool MagellanCheckMotionEvent(
#ifdef ParameterCheck
Display *display,
XEvent *Event,
char *Arg
#endif
);

int MagellanRemoveMotionEvents(
#ifdef ParameterCheck
Display *display
#endif
);

int MagellanRotationMatrix(
#ifdef ParameterCheck
double Rotate[4][4],
double c,
double b,
double a
#endif
);

int MagellanMultiplicationMatrix(
#ifdef ParameterCheck
double MatrixA[4][4],
double MatrixB[4][4],
double MatrixC[4][4] 
#endif 
);

extern Atom MagellanMotionEvent, MagellanButtonPressEvent, MagellanReleaseEvent;
extern Window MagellanWindow;
extern int MagellanExist;

enum _CommandMessages_ { NoCommandMessage, CommandMessageApplicationWindow = 27695, 
                                           CommandMessageApplicationSensitivity };
union _MagellanTypeConversion_ {
 float Float;
 short Short[ 2 ];
};
typedef union _MagellanTypeConversion_ MagellanTypeConversion;

#define XHigh32( Value )        (((Value)>>16)&0x0000FFFF)
#define XLow32( Value )         ((Value)&0x0000FFFF)

#endif  /* Magellan X Window Driver Header */



/* Define printf Function */
#define cprintf printf

Atom MagellanMotionEvent;		/* MotionEvent */
Atom MagellanButtonPressEvent;		/* ButtonPressEvent */
Atom MagellanButtonReleaseEvent;	/* ButtonReleaseEvent */ 
Atom MagellanCommandEvent;		/* CommandEvent */

Window MagellanWindow = InputFocus;	/* Magellan Driver Window */ 
int MagellanExist = FALSE; /* Magellan Exist Flag */

int MagellanInit( display, window )
Display *display;
Window window; 
{

/* Define the Magellan Event types */
 MagellanMotionEvent       =XInternAtom( display, "MotionEvent", TRUE );
 MagellanButtonPressEvent  =XInternAtom( display, "ButtonPressEvent", TRUE );
 MagellanButtonReleaseEvent=XInternAtom( display, "ButtonReleaseEvent",TRUE);
 MagellanCommandEvent      =XInternAtom( display, "CommandEvent", TRUE );

#ifdef MagellanDEBUG
 cprintf("Magellan: xdrvlib.c \n" );
 cprintf("MagellanMotionEvent = %d \n", MagellanMotionEvent );
 cprintf("MagellanButtonPressEvent = %d \n", MagellanButtonPressEvent );
 cprintf("MagellanButtonReleaseEvent = %d \n", MagellanButtonReleaseEvent );
 cprintf("MagellanCommandEvent = %d \n", MagellanCommandEvent );
 cprintf("\n");
#endif

/* TRUE if Driver exist */
 MagellanExist = (MagellanMotionEvent != 0) && (MagellanButtonPressEvent != 0) &&
                 (MagellanButtonReleaseEvent != 0) && (MagellanCommandEvent != 0);
 if ( MagellanExist == FALSE )
  return FALSE;

 MagellanExist = MagellanSetWindow( display, window );
 if ( MagellanExist == FALSE )
  return FALSE;

 
#ifdef MagellanDEBUG
 cprintf("Magellan: xdrvlib.c \n" );
 cprintf("Magellan Display = %s \n", XDisplayName( (char *)NULL ) );
 cprintf("Magellan Driver Window = %08X \n", MagellanWindow );
 cprintf("Magellan Application Window = %08X \n", window );
 cprintf("\n");
#endif

 return MagellanExist; 
}


static int MagellanErrorHandler( display, Error )
Display *display;
XErrorEvent *Error;
{
char Buffer[ 128 ];

 switch( Error->error_code )
  {
   case BadWindow : break;

   default : XGetErrorText(display,Error->error_code,Buffer,sizeof(Buffer));
             fprintf( stderr, "Magellan: Error = %s. Exit ... \n", Buffer );
             break;
  };
 return 0;
}


int MagellanApplicationSensitivity( display, Sensitivity )
Display *display;
double Sensitivity;
{
XWindowAttributes MagellanWindowAttributes;
XTextProperty MagellanWindowName;
XEvent CommandMessage;
Atom ActualType;
int ActualFormat;
unsigned long NItems, BytesReturn;
unsigned char *PropReturn;
Window root;
int (*ApplicationErrorHandler)(
#ifdef ParamterCheck
Display *,
XErrorEvent *
#endif
);
MagellanTypeConversion MagellanType;
int result;

 ApplicationErrorHandler = XSetErrorHandler( MagellanErrorHandler );
 
/* Read the window of the Magellan Driver */
 root = RootWindow( display, DefaultScreen(display) );

#ifdef MagellanDEBUG 
 cprintf("Magellan: xdrvlib.c \n" );
 cprintf("Magellan Root Window Number: %08X \n\n", root );
#endif

 PropReturn = NULL;
 XGetWindowProperty( display, root, MagellanCommandEvent, 0,1, FALSE,
                     AnyPropertyType, &ActualType, &ActualFormat, &NItems,
                     &BytesReturn, &PropReturn );

 MagellanWindow = InputFocus;
 if ( PropReturn != NULL )
  {
   MagellanWindow = *(Window *) PropReturn;
   XFree( PropReturn );                            

#ifdef MagellanDEBUG 
   cprintf("Magellan Window Number: %08X \n\n", MagellanWindow );
#endif

   if ( XGetWMName( display, MagellanWindow, &MagellanWindowName ) != 0 )
    {
#ifdef MagellanDEBUG
     cprintf("Magellan Window Name: %s \n\n", MagellanWindowName.value );
#endif

     if ( strcmp( (char *) "Magellan Window", (char *) MagellanWindowName.value) == 0 )
      {
/* Send the application window to the Magellan X Window Driver */
       CommandMessage.type = ClientMessage;
       CommandMessage.xclient.format = 16;
       CommandMessage.xclient.send_event = FALSE;
       CommandMessage.xclient.display = display;
       CommandMessage.xclient.window = MagellanWindow;
       CommandMessage.xclient.message_type = MagellanCommandEvent;

       MagellanType.Float = (float) Sensitivity;
       CommandMessage.xclient.data.s[0] = (short) MagellanType.Short[0];
       CommandMessage.xclient.data.s[1] = (short) MagellanType.Short[1]; 
       CommandMessage.xclient.data.s[2] = CommandMessageApplicationSensitivity;

       result = XSendEvent( display, MagellanWindow, FALSE, 0x0000, &CommandMessage ) != 0;

#ifdef MagellanDEBUG
       cprintf("Magellan Application Sensitivity: %lf \n\n", Sensitivity );
#endif

       XFlush( display );
      };
    };
  };

 XSetErrorHandler( ApplicationErrorHandler );
 return result;
}

int MagellanSetWindow( display, window )
Display *display;
Window window;
{
XWindowAttributes MagellanWindowAttributes;
XTextProperty MagellanWindowName;
XEvent CommandMessage;
Atom ActualType;
int ActualFormat;
unsigned long NItems, BytesReturn;
unsigned char *PropReturn;
Window root;
int (*ApplicationErrorHandler)(
#ifdef ParameterCheck
Display *,
XErrorEvent *
#endif
);
int result;

 result = FALSE;
 ApplicationErrorHandler = XSetErrorHandler( MagellanErrorHandler );
 
/* Read the window of the Magellan Driver */
 root = RootWindow( display, DefaultScreen(display) );

#ifdef MagellanDEBUG 
 cprintf("Magellan: xdrvlib.c \n" );
 cprintf("Magellan Root Window Number: %08X \n\n", root );
#endif

 PropReturn = NULL;
 XGetWindowProperty( display, root, MagellanCommandEvent, 0,1, FALSE,
                     AnyPropertyType, &ActualType, &ActualFormat, &NItems,
                     &BytesReturn, &PropReturn );

 MagellanWindow = InputFocus;
 if ( PropReturn != NULL )
  {
   MagellanWindow = *(Window *) PropReturn;
   XFree( PropReturn );

#ifdef MagellanDEBUG 
   cprintf("Magellan Window Number: %08X \n\n", MagellanWindow );
#endif

   if ( XGetWMName( display, MagellanWindow, &MagellanWindowName ) != 0 )
    {
#ifdef MagellanDEBUG
     cprintf("Magellan Window Name: %s \n\n", MagellanWindowName.value );
#endif

     if ( strcmp( (char *) "Magellan Window", (char *) MagellanWindowName.value) == 0 )
      {
/* Send the application window to the Magellan X Window Driver */
       CommandMessage.type = ClientMessage;
       CommandMessage.xclient.format = 16;
       CommandMessage.xclient.send_event = FALSE;
       CommandMessage.xclient.display = display;
       CommandMessage.xclient.window = MagellanWindow;
       CommandMessage.xclient.message_type = MagellanCommandEvent;

       CommandMessage.xclient.data.s[0] = (short) XHigh32( window );
       CommandMessage.xclient.data.s[1] = (short) XLow32( window );
       CommandMessage.xclient.data.s[2] = CommandMessageApplicationWindow;

       result = XSendEvent( display, MagellanWindow, FALSE, 0x0000, &CommandMessage ) != 0;

#ifdef MagellanDEBUG
       cprintf("Magellan Send Command Message: %08X \n\n", window );
#endif

       XFlush( display );
      };
    };
  }

 XSetErrorHandler( ApplicationErrorHandler );
 return result;
}

int MagellanClose(display)
Display *display;
{
 MagellanInit( display, (Window)InputFocus );
 MagellanExist = FALSE;
 
 return TRUE;
}

int MagellanInputEvent(display,Event,MagellanEvent)
Display *display;
XEvent *Event;
MagellanIntEvent *MagellanEvent;
{
 if ( MagellanExist == FALSE )
  return FALSE;

 if( Event->type == ClientMessage )
  {
   if( Event->xclient.message_type == MagellanMotionEvent )
    {
     MagellanEvent->type = MagellanInputMotionEvent;
     MagellanEvent->u.data[0] = Event->xclient.data.s[2];
     MagellanEvent->u.data[1] = Event->xclient.data.s[3];
     MagellanEvent->u.data[2] = Event->xclient.data.s[4];
     MagellanEvent->u.data[3] = Event->xclient.data.s[5];
     MagellanEvent->u.data[4] = Event->xclient.data.s[6];
     MagellanEvent->u.data[5] = Event->xclient.data.s[7];
     MagellanEvent->u.data[6] = Event->xclient.data.s[8]*1000/60;
     return MagellanInputMotionEvent;
    }
   else
    if ( Event->xclient.message_type == MagellanButtonPressEvent )
     {
      MagellanEvent->type = MagellanInputButtonPressEvent;
      MagellanEvent->u.button = Event->xclient.data.s[2];
      return MagellanInputButtonPressEvent;
     }
    else
     if ( Event->xclient.message_type == MagellanButtonReleaseEvent )
      {
       MagellanEvent->type = MagellanInputButtonReleaseEvent;
       MagellanEvent->u.button = Event->xclient.data.s[2];
       return MagellanInputButtonReleaseEvent;
      };
  };
 return display == NULL;
}

int MagellanTranslateEvent( display, Event, MagellanEvent,
			       MagellanTranslationScale, MagellanRotationScale )
Display *display;
XEvent *Event;
MagellanFloatEvent *MagellanEvent;
double MagellanTranslationScale;
double MagellanRotationScale;
{
 if ( MagellanExist == FALSE )
  return FALSE;

 if ( Event->type == ClientMessage )
  {
   if ( Event->xclient.message_type == MagellanMotionEvent )
    {
     MagellanEvent->MagellanType = MagellanInputMotionEvent;
     MagellanEvent->MagellanData[ MagellanX ] =
      Event->xclient.data.s[2] * MagellanTranslationScale;
     MagellanEvent->MagellanData[ MagellanY ] =
      Event->xclient.data.s[3] * MagellanTranslationScale;
     MagellanEvent->MagellanData[ MagellanZ ] =
      Event->xclient.data.s[4] * MagellanTranslationScale;
     MagellanEvent->MagellanData[ MagellanA ] =
      Event->xclient.data.s[5] * MagellanRotationScale;
     MagellanEvent->MagellanData[ MagellanB ] =
      Event->xclient.data.s[6] * MagellanRotationScale;
     MagellanEvent->MagellanData[ MagellanC ] =
      Event->xclient.data.s[7] * MagellanRotationScale;
     MagellanEvent->MagellanPeriod = Event->xclient.data.s[8];
     return MagellanInputMotionEvent;
    }
   else
    if ( Event->xclient.message_type == MagellanButtonPressEvent )
     {
      MagellanEvent->MagellanType = MagellanInputButtonPressEvent;
      MagellanEvent->MagellanButton = Event->xclient.data.s[2];
      return MagellanInputButtonPressEvent;
     }
    else
     if ( Event->xclient.message_type == MagellanButtonReleaseEvent )
      {
       MagellanEvent->MagellanType = MagellanInputButtonReleaseEvent;
       MagellanEvent->MagellanButton = Event->xclient.data.s[2];
       return MagellanInputButtonReleaseEvent;
      };
  };
 return display == NULL;
}

int MagellanRotationMatrix( Rotate, c, b, a )
double Rotate[4][4];
double c, b, a;
{
double ca,cb,cc,sa,sb,sc;

 ca = cos( a ); sa = sin( a );
 cb = cos( b ); sb = sin( b );
 cc = cos( c ); sc = sin( c );

 Rotate[0][0] = ca*cb;
 Rotate[1][0] = sa*cb;
 Rotate[2][0] =-sb;

 Rotate[0][1] =-sa*cc+ca*sc*sb;
 Rotate[1][1] = ca*cc+sa*sc*sb;
 Rotate[2][1] = cb*sc;

 Rotate[0][2] = sa*sc+ca*cc*sb;
 Rotate[1][2] =-ca*sc+sa*cc*sb;
 Rotate[2][2] = cb*cc;

 Rotate[0][3] = 0.0;
 Rotate[1][3] = 0.0;
 Rotate[2][3] = 0.0;

 Rotate[3][0] = 0.0;
 Rotate[3][1] = 0.0;
 Rotate[3][2] = 0.0;

 Rotate[3][3] = 1.0;
 return TRUE;
}

int MagellanMultiplicationMatrix( MatrixA, MatrixB, MatrixC )
double MatrixA[4][4], MatrixB[4][4], MatrixC[4][4];
{
double MatrixR[4][4];

 MatrixR[0][0] = MatrixB[0][0]*MatrixC[0][0] + MatrixB[0][1]*MatrixC[1][0] + MatrixB[0][2]*MatrixC[2][0];
 MatrixR[0][1] = MatrixB[0][0]*MatrixC[0][1] + MatrixB[0][1]*MatrixC[1][1] + MatrixB[0][2]*MatrixC[2][1];
 MatrixR[0][2] = MatrixB[0][0]*MatrixC[0][2] + MatrixB[0][1]*MatrixC[1][2] + MatrixB[0][2]*MatrixC[2][2];
 MatrixR[1][0] = MatrixB[1][0]*MatrixC[0][0] + MatrixB[1][1]*MatrixC[1][0] + MatrixB[1][2]*MatrixC[2][0];
 MatrixR[1][1] = MatrixB[1][0]*MatrixC[0][1] + MatrixB[1][1]*MatrixC[1][1] + MatrixB[1][2]*MatrixC[2][1];
 MatrixR[1][2] = MatrixB[1][0]*MatrixC[0][2] + MatrixB[1][1]*MatrixC[1][2] + MatrixB[1][2]*MatrixC[2][2];
 MatrixR[2][0] = MatrixB[2][0]*MatrixC[0][0] + MatrixB[2][1]*MatrixC[1][0] + MatrixB[2][2]*MatrixC[2][0];
 MatrixR[2][1] = MatrixB[2][0]*MatrixC[0][1] + MatrixB[2][1]*MatrixC[1][1] + MatrixB[2][2]*MatrixC[2][1];
 MatrixR[2][2] = MatrixB[2][0]*MatrixC[0][2] + MatrixB[2][1]*MatrixC[1][2] + MatrixB[2][2]*MatrixC[2][2];
 MatrixR[0][3] = MatrixR[1][3] = MatrixR[2][3] = MatrixR[3][0] = MatrixR[3][1] = MatrixR[3][2] = 0.0;
 MatrixR[3][3] = 1.0;

 memcpy( MatrixA, MatrixR, sizeof(MatrixA) );
 return TRUE;
}

Bool MagellanCheckMotionEvent( display, event, arg )
Display *display;
XEvent *event;
char *arg;
{
 if ( event->type == ClientMessage )
  if ( event->xclient.message_type == MagellanMotionEvent )
   return True;
 return False;
}

int MagellanRemoveMotionEvents( display )
Display *display;
{
XEvent event;

 while( XCheckIfEvent(display,&event,MagellanCheckMotionEvent,NULL) == True )
  ;

 return TRUE;
}
