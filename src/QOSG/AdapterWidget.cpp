 #include "QOSG/AdapterWidget.h"
#include "QOSG/CoreWindow.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

namespace QOSG {

AdapterWidget::AdapterWidget( QWidget* parent, const char* name , const QGLWidget* shareWidget, WindowFlags f ) :
#if QT_VERSION > 0x040000
	QGLWidget( QGLFormat( QGL::DoubleBuffer | QGL::DepthBuffer | QGL::Rgba | QGL::StencilBuffer | QGL::AlphaChannel | QGL::StereoBuffers ), parent, shareWidget, f )
#else
	QGLWidget( parent, shareWidget, f )
#endif
{
    setAttribute(Qt::WA_AcceptTouchEvents);
    setAttribute(Qt::WA_StaticContents);

    _time.start();
    _rightMouse = false;
    _selectionMode = false;
    _counter = 0;
    _whatSelect = 0;

	_gw = new osgViewer::GraphicsWindowEmbedded( 0,0,width(),height() );
	setFocusPolicy( Qt::StrongFocus );
}



void AdapterWidget::resizeGL( int width, int height )
{
	//zmena velkosti widgetu
	_gw->getEventQueue()->windowResize( 0, 0, width, height );
	_gw->resized( 0,0,width,height );
}

void AdapterWidget::keyPressEvent( QKeyEvent* event )
{

	//odchytavanie udalosti klavesnice
	switch ( event->key() ) {
		case ( Qt::Key_Up ): {
			_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Up );
			break;
		}
		case ( Qt::Key_Down ): {
			_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Down );
			break;
		}
		case ( Qt::Key_Right ): {
			_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Right );
			break;
		}
		case ( Qt::Key_Left ): {
			_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Left );
			break;
		}
		case ( Qt::Key_PageDown ): {
			_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Down );
			break;
		}
		case ( Qt::Key_PageUp ): {
			_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Up );
			break;
		}
		case ( Qt::Key_Alt ): {
			_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Alt_L );
			break;
		}
		case ( Qt::Key_Control ): {
			_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Control_L );
			break;
		}
		case ( Qt::Key_Shift ): {
			_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Shift_L );
			break;
		}
		case ( Qt::Key_O ): {
			_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_O );
			break;
		}
		case ( Qt::Key_P ): {
			_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_P );
			break;
		}
		default: {
			_gw->getEventQueue()->keyPress( static_cast<osgGA::GUIEventAdapter::KeySymbol>( *( event->text().TOASCII().data() ) ) );
			break;
		}
	}
}

void AdapterWidget::keyReleaseEvent( QKeyEvent* event )
{
	//odchytavanie udalosti klavesnice
	switch ( event->key() ) {
		case ( Qt::Key_Up ): {
			_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Up );
			break;
		}
		case ( Qt::Key_Down ): {
			_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Down );
			break;
		}
		case ( Qt::Key_Right ): {
			_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Right );
			break;
		}
		case ( Qt::Key_Left ): {
			_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Left );
			break;
		}
		case ( Qt::Key_PageDown ): {
			_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Down );
			break;
		}
		case ( Qt::Key_PageUp ): {
			_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Up );
			break;
		}
		case ( Qt::Key_Alt ): {
			_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Alt_L );
			break;
		}
		case ( Qt::Key_Control ): {
			_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Control_L );
			break;
		}
		case ( Qt::Key_Shift ): {
			_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Shift_L );
			break;
		}
		case ( Qt::Key_AsciiTilde ): {
			CoreWindow* coreWindow = dynamic_cast<CoreWindow*>( parent() );
			if ( coreWindow ) {
				coreWindow->addSQLInput();
			}
			break;
		}
		default: {
			_gw->getEventQueue()->keyRelease( static_cast<osgGA::GUIEventAdapter::KeySymbol>( *( event->text().TOASCII().data() ) ) );
			break;
		}
	}
}

void AdapterWidget::mousePressEvent( QMouseEvent* event )
{
    //odchytavanie udalosti mys
	unsigned int button = 0;
	switch ( event->button() ) {
		case ( Qt::LeftButton ):
			button = 1;
			break;
		case ( Qt::MidButton ):
			button = 2;
			break;
		case ( Qt::RightButton ):
			button = 3;
			break;
		case ( Qt::NoButton ):
			button = 0;
			break;
		default:
			button = 0;
			break;
	}

	_gw->getEventQueue()->mouseButtonPress( static_cast<float>( event->x() ),static_cast<float>( event->y() ), button );
}

void AdapterWidget::mouseDoubleClickEvent( QMouseEvent* event )
{
	//odchytavanie udalosti mysi

	unsigned int button = 0;
	switch ( event->button() ) {
		case ( Qt::LeftButton ):
			button = 1;
			break;
		case ( Qt::MidButton ):
			button = 2;
			break;
		case ( Qt::RightButton ):
			button = 3;
			break;
		case ( Qt::NoButton ):
			button = 0;
			break;
		default:
			button = 0;
			break;
	}

	_gw->getEventQueue()->mouseDoubleButtonPress( static_cast<float>( event->x() ),static_cast<float>( event->y() ), button );

}

void AdapterWidget::mouseReleaseEvent( QMouseEvent* event )
{
	//odchytavanie udalosti mysi

	unsigned int button = 0;
	switch ( event->button() ) {
		case ( Qt::LeftButton ):
			button = 1;
			break;
		case ( Qt::MidButton ):
			button = 2;
			break;
		case ( Qt::RightButton ):
			button = 3;
			break;
		case ( Qt::NoButton ):
			button = 0;
			break;
		default:
			button = 0;
			break;
	}    
	_gw->getEventQueue()->mouseButtonRelease( static_cast<float>( event->x() ),static_cast<float>( event->y() ), button );
}

void AdapterWidget::mouseMoveEvent( QMouseEvent* event )
{

	//odchytavanie udalosti mysi

	_gw->getEventQueue()->mouseMotion( static_cast<float>( event->x() ),static_cast<float>( event->y() ) );
}

void AdapterWidget::wheelEvent( QWheelEvent* event )
{
	//odchytavanie udalosti mysi
    //osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gw;    
	_gw->getEventQueue()->mouseScroll( ( event->delta()>0 ) ?
									   osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN );
}

bool AdapterWidget::event(QEvent* event)
{    
    switch (event->type()) {
    case QEvent::TouchBegin:
    {
    }
    case QEvent::TouchUpdate:
    {
    }
    case QEvent::TouchEnd:
    {
        /*
         * Sem pridať funkcionalitu
        */
        QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent *>(event)->touchPoints();

        if(touchPoints.size() == 5)
        {

            CoreWindow* coreWindow = dynamic_cast<CoreWindow*>( parent() );
            coreWindow->noSelectClicked(true);
            _selectionMode = false;
            _rightMouse = false;            
            _counter = 10;
            _difference = _time.elapsed();
            _time.restart();

            return true;
        }
        if(_selectionMode == true)
        {


            if((touchPoints.size() == 1)){
                _counter = 1;
            }
            if(touchPoints.size() == 2){
                if(_counter >= 0)
                {
                    _counter--;
                    return true;
                }
                CoreWindow* coreWindow = dynamic_cast<CoreWindow*>( parent() );
                coreWindow->centerView(true);
                coreWindow->multiSelectClicked(true);
                _counter = 2;
                return true;
            }
            if(touchPoints.size() == 3){
                _counter = 2;
            }
            if(touchPoints.size() == 4){

                if(_counter >= 0)
                {
                    _counter--;
                    return true;
                }                                
                CoreWindow* coreWindow = dynamic_cast<CoreWindow*>( parent() );
                const QTouchEvent::TouchPoint touchPoint = touchPoints.takeLast();
                if(touchPoint.pos().y() > touchPoint.lastPos().y()){
                    _whatSelect++;
                    if(_whatSelect == 4){_whatSelect = 0;}
                }
                if(touchPoint.pos().y() < touchPoint.lastPos().y()){
                    _whatSelect--;
                    if(_whatSelect == -1){_whatSelect = 3;}
                }
                coreWindow->selectionTypeComboBoxChanged(_whatSelect);
                _counter = 2;
                return true;
            }

        }
        else
        {
            if(touchPoints.size() == 1)
            {                
                const QTouchEvent::TouchPoint touchPoint = touchPoints.takeFirst();
                _difference = _time.elapsed();
                _time.restart();
                if((_difference < 200) && (_difference > 50) || (_difference > 200))
                {

                    if(_difference > 200)
                    {
                        _gw->getEventQueue()->mouseButtonRelease( static_cast<float>( touchPoint.pos().x() ),static_cast<float>( touchPoint.pos().y() ), 3 );
                        _rightMouse = false;

                    }
                    else
                    {
                        if((abs(_lastSingleTouch.pos().x() - touchPoint.pos().x()) < 10) &&
                        (abs(_lastSingleTouch.pos().y() - touchPoint.pos().y()) < 10)  )

                        {

                               _gw->getEventQueue()->mouseButtonPress( static_cast<float>( touchPoint.pos().x() ),static_cast<float>( touchPoint.pos().y() ), 3 );
                               _rightMouse = true;

                        }
                    }

                }

                _lastSingleTouch = touchPoint;

                if(_rightMouse == true)
                {
                     _gw->getEventQueue()->mouseMotion( static_cast<float>( touchPoint.pos().x() ),static_cast<float>( touchPoint.pos().y() ) );
                }
                else
                {

                    if(abs(touchPoint.pos().x()-touchPoint.lastPos().x()) > 5)
                    {
                        if(touchPoint.pos().x() < touchPoint.lastPos().x())
                        {
                            _gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Right );
                            _gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Right );
                        }
                        else
                        {
                            _gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Left );
                            _gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Left );
                        }
                    }
                    if(abs(touchPoint.pos().y()-touchPoint.lastPos().y()) > 5)
                    {
                        if(touchPoint.pos().y() < touchPoint.lastPos().y())
                        {
                            _gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Down );
                            _gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Down );
                        }
                        else
                        {
                            _gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Up );
                            _gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Up );
                        }
                    }
                }
            }
            if(touchPoints.size() == 2)
            {


                const QTouchEvent::TouchPoint _t_0 = touchPoints.takeFirst();
                const QTouchEvent::TouchPoint _t_1 = touchPoints.takeFirst();

                if((abs(_t_0.pos().y() - _t_0.lastPos().y() ) <= 25) && (abs(_t_0.pos().x() - _t_0.lastPos().x() ) <= 25))
                {
                    if(abs(_t_1.pos().y() - _t_1.lastPos().y()) >= 10)
                    {
                        if(_t_1.pos().y() > _t_1.lastPos().y())
                        {
                            _gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Down );
                            _gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Down );
                        }
                        else
                        {
                            _gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Up );
                            _gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Up );
                        }

                    }
                }

                double _x_last = abs(_t_0.lastPos().rx()-_t_1.lastPos().rx());
                double _y_last = abs(_t_0.lastPos().ry()-_t_1.lastPos().ry());
                double _v_last = sqrt((_x_last*_x_last)+(_y_last*_y_last));

                double _x_now = abs(_t_0.pos().rx()-_t_1.pos().rx());
                double _y_now = abs(_t_0.pos().ry()-_t_1.pos().ry());
                double _v_now = sqrt((_x_now*_x_now)+(_y_now*_y_now));                


                if(_v_now < 75){
                    if(abs(_v_now - _v_last) < 25)
                    {
                        if(abs(_t_0.pos().x() - _t_0.lastPos().x()) > 5)
                        {
                            if(_t_0.pos().x() < _t_0.lastPos().x())
                            {
                                _gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Right );
                                _gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Right );
                            }
                            else
                            {
                                _gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Left );
                                _gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Left );
                            }
                        }
                        if(abs(_t_0.pos().y() - _t_0.lastPos().y()) > 5)
                        {
                            if(_t_0.pos().y() < _t_0.lastPos().y())
                            {
                                _gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Down );
                                _gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Down );
                            }


                            else
                            {
                                _gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Up );
                                _gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Up );
                            }

                        }
                        return true;
                    }


                }
                if(abs(_v_now - _v_last) > 25)
                {                    
                    if(_v_now > _v_last)
                    {

                        _gw->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_UP);

                    }
                    if(_v_now < _v_last)
                    {

                        _gw->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_DOWN);

                    }
                }

            }
            if(touchPoints.size() == 3)
            {
                if(_counter != 0)
                {
                    _counter--;
                    return true;
                }
                CoreWindow* coreWindow = dynamic_cast<CoreWindow*>( parent() );
                coreWindow->multiSelectClicked(true);
                _selectionMode = true;
                _rightMouse = false;                
                _counter = 2;
                _difference = _time.elapsed();
                _time.restart();
            }
        }

    }
    default:
        return QWidget::event(event);
    }
    return true;
}

} // namespace QOSG

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
