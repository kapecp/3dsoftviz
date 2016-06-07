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
	Display* display,
	Window window
#endif
);

int MagellanSetWindow(
#ifdef ParameterCheck
	Display* display,
	Window window
#endif
);

int MagellanApplicationSensitivity(
#ifdef ParameterCheck
	Display* display,
	double Sensitivity
#endif
);

static int MagellanErrorHandler(
#ifdef ParameterCheck
	Display* display,
	XErrorEvent* Error
#endif
);

int MagellanInputEvent(
#ifdef ParameterCheck
	Display* display,
	XEvent* Event,
	MagellanIntEvent* MagellanEvent
#endif
);

int MagellanTranslateEvent(
#ifdef ParameterCheck
	Display* display,
	XEvent* Event,
	MagellanFloatEvent* MagellanEvent,
	double MagellanTranslationScale,
	double MagellanRotationScale
#endif
);

int MagellanClose(
#ifdef ParameterCheck
	Display* display
#endif
);

Bool MagellanCheckMotionEvent(
#ifdef ParameterCheck
	Display* display,
	XEvent* Event,
	char* Arg
#endif
);

int MagellanRemoveMotionEvents(
#ifdef ParameterCheck
	Display* display
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
						 CommandMessageApplicationSensitivity
					   };
union _MagellanTypeConversion_ {
	float Float;
	short Short[ 2 ];
};
typedef union _MagellanTypeConversion_ MagellanTypeConversion;

#define XHigh32( Value )        (((Value)>>16)&0x0000FFFF)
#define XLow32( Value )         ((Value)&0x0000FFFF)

#endif  /* Magellan X Window Driver Header */
