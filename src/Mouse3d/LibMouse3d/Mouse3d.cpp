#include "Mouse3d/LibMouse3d/Mouse3d.h"
#include "Mouse3d/LibMouse3d/EventThread.h"
#include "QOSG/CoreWindow.h"


namespace LibMouse3d{

    Mouse3dDevice::Mouse3dDevice(QOSG::CoreWindow* window){
#ifdef Q_WS_WIN
        this->mouse = new Mouse3DInput(window);
        QObject::connect(mouse, SIGNAL(Move3d(std::vector<float>&)), window, SLOT(OnMove(std::vector<float>&)));
#elif defined(Q_WS_X11)
        Mouse3DLinux();
#elif defined(Q_WS_MAC)

#endif

    }

    Mouse3dDevice::~Mouse3dDevice(){

    }

#if defined(Q_WS_X11)

    void Mouse3dDevice::Move3d(std::vector<float>& motionData)
    {
        emit Move3d(motionData);
    }

    void Mouse3dDevice::Mouse3DLinux(){
        double MagellanSensitivity = 1.0;

        Display *display;
        Window root, window;

        int screennumber,width,height;
        XSizeHints *sizehints;
        XWMHints *wmhints;
        XClassHint *classhints;

        char *WinName = "Magellan 3D Controller";
        XTextProperty WindowName;
        GC wingc;
        XGCValues xgcvalues;

        XEvent report;
        MagellanFloatEvent MagellanEvent;

        XComposeStatus compose;
        KeySym keysym;

        int MagellanDemoEnd = FALSE;
        char MagellanBuffer[ 256 ];


        /****************** Open a Window ******************************************/
         sizehints  = XAllocSizeHints();
         wmhints    = XAllocWMHints();
         classhints = XAllocClassHint();
         if ( (sizehints==NULL) || (wmhints==NULL) || (classhints==NULL) )
          {
           fprintf( stderr, "Can't allocate memory! Exit ... \n" );
           exit( -1 );
          };

         display = XOpenDisplay( NULL );
         if ( display == NULL )
          {
           fprintf( stderr, "Can't open display! Exit ... \n");
           exit( -1 );
          };

         screennumber = DefaultScreen(display);
         width  = DisplayWidth(display,screennumber);
         height = DisplayHeight(display,screennumber);
         root   = DefaultRootWindow(display);
         window = XCreateSimpleWindow( display, root, 0,0, width/5*3,height/8, 20,
                           BlackPixel(display,screennumber),
                           WhitePixel(display,screennumber) );

         printf("Magellan: xapp.c\n" );
         printf("Magellan Root Window=%08X \nMagellan Application Window=%08X \n\n",
             root, window );

         XStringListToTextProperty( &WinName, 1, &WindowName );

         wmhints->initial_state = NormalState;
         wmhints->input = TRUE;
         wmhints->flags = StateHint | InputHint;

         classhints->res_name = "Magellan 3D Controller";
         classhints->res_class = "BasicWindow";
         int argc = 0;
         char **argv;
         XSetWMProperties( display, window, &WindowName, NULL, argv, argc, sizehints, wmhints, classhints );

         XMapWindow( display, window );
         xgcvalues.foreground = BlackPixel( display, 0 );
         xgcvalues.background = WhitePixel( display, 0 );
         wingc = XCreateGC( display, window, GCForeground | GCBackground, &xgcvalues );

         /************************* Create 3D Event Types ***************************/
         if ( !MagellanInit( display, window ) )
          {
           fprintf( stderr, "No driver is running. Exit ... \n" );
           exit(-1);
          };

         /************************* Main Loop ***************************************/
         XSelectInput( display, window, KeyPressMask | KeyReleaseMask );

         while( MagellanDemoEnd == FALSE )
          {
           XNextEvent( display, &report );
           switch( report.type )
            {
             case KeyRelease : /* ... */
                               break;

             case KeyPress :
              XLookupString( (XKeyEvent*)&report, MagellanBuffer,
                         sizeof(MagellanBuffer), &keysym, &compose );
              MagellanDemoEnd = keysym == XK_Escape;
              break;

             case ClientMessage :
              switch( MagellanTranslateEvent( display, &report, &MagellanEvent, 1.0, 1.0 ) )
               {
                case MagellanInputMotionEvent :
                 MagellanRemoveMotionEvents( display );

                 signal_data.clear();
                 signal_data.push_back(MagellanEvent.MagellanData[ MagellanX ]);
                 signal_data.push_back(MagellanEvent.MagellanData[ MagellanY ]);
                 signal_data.push_back(MagellanEvent.MagellanData[ MagellanZ ]);
                 signal_data.push_back(MagellanEvent.MagellanData[ MagellanA ]);
                 signal_data.push_back(MagellanEvent.MagellanData[ MagellanB ]);
                 signal_data.push_back(MagellanEvent.MagellanData[ MagellanC ]);

                 qDebug() <<  "x=" << signal_data[0] <<
                              "y=" << signal_data[1] <<
                              "z=" << signal_data[2] <<
                              "a=" << signal_data[3] <<
                              "b=" << signal_data[4] <<
                              "c=" << signal_data[5] << endl;

                 //emit Move3d(signal_data);

                 XClearWindow( display, window );
             XDrawString( display, window, wingc, 10,40,
                       MagellanBuffer, (int)strlen(MagellanBuffer) );
                 XFlush( display );
             break;

                case MagellanInputButtonPressEvent :
                 sprintf( MagellanBuffer, "Button pressed [%c]  ",
                   MagellanEvent.MagellanButton ==  9 ? '*' :
                   MagellanEvent.MagellanButton == 10 ? '+' :
                   MagellanEvent.MagellanButton == 11 ? '-' :
                             '0'+MagellanEvent.MagellanButton );
                 XClearWindow( display, window );
                 XDrawString( display, window, wingc, 10,40,
                               MagellanBuffer, (int)strlen(MagellanBuffer) );
                 XFlush( display );

             switch ( MagellanEvent.MagellanButton )
              {
               case 5: MagellanApplicationSensitivity( display, MagellanSensitivity / 2.0 ); break;
               case 6: MagellanApplicationSensitivity( display, MagellanSensitivity * 2.0 ); break;
               case 7: MagellanApplicationSensitivity( display, 1.0 ); break;
              };

             switch( MagellanEvent.MagellanButton )
              {
               case 5: MagellanSensitivity = MagellanSensitivity <= 1.0/32.0 ? 1.0/32.0 : MagellanSensitivity/2.0; break;
               case 6: MagellanSensitivity = MagellanSensitivity >= 32.0 ? 32.0 : MagellanSensitivity*2.0; break;
               case 7: MagellanSensitivity = 1.0;  break;
              };

             switch( MagellanEvent.MagellanButton )
              {
               case 5:
               case 6:
               case 7:
               printf("Application Sensitivity = %lf \n", MagellanSensitivity );
               break;
              };
             break;

                case MagellanInputButtonReleaseEvent :
                 sprintf( MagellanBuffer, "Button released [%c] ",
                   MagellanEvent.MagellanButton == 9  ? '*' :
                   MagellanEvent.MagellanButton == 10 ? '+' :
                   MagellanEvent.MagellanButton == 11 ? '-' :
                             '0'+MagellanEvent.MagellanButton );
                 XClearWindow( display, window );
                 XDrawString( display, window, wingc, 10,40,
                               MagellanBuffer, (int)strlen(MagellanBuffer) );
                 XFlush( display );
                 break;

               default : /* another ClientMessage event */
             break;
               };
              break;
             };
          };

         MagellanClose( display );
         XFree( sizehints );
         XFree( wmhints );
         XFree( classhints );
         XFreeGC( display, wingc );
         XDestroyWindow( display, window );
         XCloseDisplay( display );
    }
#endif
}
