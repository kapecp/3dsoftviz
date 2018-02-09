#include "QOSG/AdapterWidget.h"
#include "QOSG/CoreWindow.h"

#include <leathers/push>
#include <leathers/switch-enum>

#include <easylogging++.h>

namespace QOSG {

AdapterWidget::AdapterWidget( const QGLFormat& format, QWidget* parent, const char* name, const QGLWidget* shareWidget, WindowFlags f ) :
#if QT_VERSION > 0x050000
	QOpenGLWidget( parent, f )
#elif QT_VERSION > 0x040000
	QGLWidget( format, parent, shareWidget, f )
#else
	QGLWidget( parent, shareWidget, f )
#endif
{
	//setting nescesary attributes for multi-touch interaction
	setAttribute( Qt::WA_AcceptTouchEvents );
	setAttribute( Qt::WA_StaticContents );
	_time.start();
	_rightMouse = false;
	_selectionMode = false;
	_counter = 0;
	_whatSelect = 0;


	_gw = new osgViewer::GraphicsWindowEmbedded( 0,0,width(),height() );
	setFocusPolicy( Qt::StrongFocus );
}

void AdapterWidget::initializeGL()
{
#if QT_VERSION > 0x050000
	this->initializeOpenGLFunctions();
#endif
	// Set up the rendering context, define display lists etc.:

	qDebug() << "OpenGL Versions Supported: " << QGLFormat::openGLVersionFlags();
	qDebug() << "Current Context:" << this->format();

	auto context = this->context();

	qDebug() << "Context valid: " << context->isValid();
	qDebug() << "Really used OpenGl: " << context->format().majorVersion() << "." << context->format().minorVersion();
	qDebug() << "Profile: " << context->format().profile();

	qDebug() << "OpenGl information: VENDOR:       " << reinterpret_cast<const char*>( glGetString( GL_VENDOR ) );
	qDebug() << "                    RENDERDER:    " << reinterpret_cast<const char*>( glGetString( GL_RENDERER ) );
	qDebug() << "                    VERSION:      " << reinterpret_cast<const char*>( glGetString( GL_VERSION ) );
	qDebug() << "                    GLSL VERSION: " << reinterpret_cast<const char*>( glGetString( GL_SHADING_LANGUAGE_VERSION ) );
	qDebug() << "endstuff\n";

}



void AdapterWidget::resizeGL( int width, int height )
{
	// FIX: for HiDPI displays
	// without this fix, OpenGL context on retina displays is small,
	// causing to display a small OpenGL context only in the lower-left corner
	// (1/4 of the desired size)
	// NOTE: on non-retina displays devicePixelRatio() is 1, on retina 2
#if QT_VERSION > 0x050000
	width *= QApplication::desktop()->devicePixelRatio();
	height *= QApplication::desktop()->devicePixelRatio();
#endif
	// end FIX

	//zmena velkosti widgetu
	_gw->getEventQueue()->windowResize( 0, 0, width, height );
	_gw->resized( 0,0,width,height );
}

void AdapterWidget::keyPressEvent( QKeyEvent* event )
{
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
	_gw->getEventQueue()->mouseMotion( static_cast<float>( event->x() ),static_cast<float>( event->y() ) );
}

void AdapterWidget::wheelEvent( QWheelEvent* event )
{
	_gw->getEventQueue()->mouseScroll( ( event->delta()>0 ) ?
									   osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN );
}

bool AdapterWidget::event( QEvent* event )
{
	//different types of touch event
	switch ( event->type() ) {
		//first touch if previous touches were zero
		case QEvent::TouchBegin: {
			break;
		}
		//if the touch is still going on
		case QEvent::TouchUpdate: {
			break;
		}
		//we changed the position of a touch
		case QEvent::TouchEnd: {
			//list of all touch points
			QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent*>( event )->touchPoints();
			//reset interaction to navigation of the scene
			if ( touchPoints.size() == 5 ) {
				CoreWindow* coreWindow = dynamic_cast<CoreWindow*>( parent() );
				coreWindow->noSelectClicked( true );
				_selectionMode = false;
				_rightMouse = false;
				_counter = 10;
				_difference = _time.elapsed();
				_time.restart();

				return true;
			}
			//selection mode of touch interaction
			if ( _selectionMode == true ) {
				//the touch is a left mouse button, that is how driver interprets it
				if ( ( touchPoints.size() == 1 ) ) {
					_counter = 1;
				}
				//centering the camera
				if ( touchPoints.size() == 2 ) {
					if ( _counter >= 0 ) {
						_counter--;
						return true;
					}
					CoreWindow* coreWindow = dynamic_cast<CoreWindow*>( parent() );
					coreWindow->centerView( true );
					coreWindow->multiSelectClicked( true );
					_counter = 2;
					return true;
				}
				//for sensitivity issues
				if ( touchPoints.size() == 3 ) {
					_counter = 2;
				}
				//change the filter of selection
				if ( touchPoints.size() == 4 ) {
					if ( _counter >= 0 ) {
						_counter--;
						return true;
					}
					CoreWindow* coreWindow = dynamic_cast<CoreWindow*>( parent() );
					const QTouchEvent::TouchPoint touchPoint = touchPoints.takeLast();
					if ( touchPoint.pos().y() > touchPoint.lastPos().y() ) {
						_whatSelect++;
						if ( _whatSelect == 4 ) {
							_whatSelect = 0;
						}
					}
					if ( touchPoint.pos().y() < touchPoint.lastPos().y() ) {
						_whatSelect--;
						if ( _whatSelect == -1 ) {
							_whatSelect = 3;
						}
					}
					coreWindow->selectionTypeComboBoxChanged( _whatSelect );
					_counter = 2;
					return true;
				}
			}
			//navigation mode of touch interaction
			else {
				//either move camera by X/Y axis, or rotate
				if ( touchPoints.size() == 1 ) {
					const QTouchEvent::TouchPoint touchPoint = touchPoints.takeFirst();
					//last time since single touch was recorded
					_difference = _time.elapsed();
					_time.restart();

					//checking for double-tap
					if ( ( ( _difference < 200 ) && ( _difference > 50 ) ) || ( _difference > 200 ) ) {
						//a lot of time has passed, cancel double-tap
						if ( _difference > 200 ) {
							_gw->getEventQueue()->mouseButtonRelease( static_cast<float>( touchPoint.pos().x() ),static_cast<float>( touchPoint.pos().y() ), 3 );
							_rightMouse = false;
						}
						else {
							//double-tap at approximately the same place
							if ( ( std::abs( _lastSingleTouch.pos().x() - touchPoint.pos().x() ) < 10 )
									&& ( std::abs( _lastSingleTouch.pos().y() - touchPoint.pos().y() ) < 10 ) ) {
								_gw->getEventQueue()->mouseButtonPress( static_cast<float>( touchPoint.pos().x() ),static_cast<float>( touchPoint.pos().y() ), 3 );
								_rightMouse = true;
							}
						}
					}
					_lastSingleTouch = touchPoint;
					//rotate camera
					if ( _rightMouse == true ) {
						_gw->getEventQueue()->mouseMotion( static_cast<float>( touchPoint.pos().x() ),static_cast<float>( touchPoint.pos().y() ) );
					}
					//move camera on axis X/Y
					else {
						//check the difference, has to be greater for sensitivity
						if ( std::abs( touchPoint.pos().x()-touchPoint.lastPos().x() ) > 5 ) {
							if ( touchPoint.pos().x() < touchPoint.lastPos().x() ) {
								_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Right );
								_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Right );
							}
							else {
								_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Left );
								_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Left );
							}
						}
						//check the difference, has to be greater for sensitivity
						if ( std::abs( touchPoint.pos().y()-touchPoint.lastPos().y() ) > 5 ) {
							if ( touchPoint.pos().y() < touchPoint.lastPos().y() ) {
								_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Down );
								_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Down );
							}
							else {
								_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Up );
								_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Up );
							}
						}
					}
				}
				//Zoom, or move camera on axis X/Y/Z
				if ( touchPoints.size() == 2 ) {

					const QTouchEvent::TouchPoint _t_0 = touchPoints.takeFirst();
					const QTouchEvent::TouchPoint _t_1 = touchPoints.takeFirst();
					// move camera on axis Z, have to be in close proximity
					if ( ( std::abs( _t_0.pos().y() - _t_0.lastPos().y() ) <= 25 )
							&& ( std::abs( _t_0.pos().x() - _t_0.lastPos().x() ) <= 25 ) ) {
						//first touch is stationary and second is used for direction of movement
						if ( std::abs( _t_1.pos().y() - _t_1.lastPos().y() ) >= 10 ) {
							if ( _t_1.pos().y() > _t_1.lastPos().y() ) {
								_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Down );
								_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Down );
							}
							else {
								_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Up );
								_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Up );
							}

						}
					}
					//Zoom, distances based on previous position and curent position
					double _x_last = std::abs( _t_0.lastPos().rx()-_t_1.lastPos().rx() );
					double _y_last = std::abs( _t_0.lastPos().ry()-_t_1.lastPos().ry() );
					double _v_last = sqrt( ( _x_last*_x_last )+( _y_last*_y_last ) );

					double _x_now = std::abs( _t_0.pos().rx()-_t_1.pos().rx() );
					double _y_now = std::abs( _t_0.pos().ry()-_t_1.pos().ry() );
					double _v_now = sqrt( ( _x_now*_x_now )+( _y_now*_y_now ) );

					//maximal distance for gesture for camera movement on axis X/Y
					if ( _v_now < 75 ) {
						//maximal difference for gesture to activate
						if ( std::abs( _v_now - _v_last ) < 25 ) {
							//same as with one touch
							if ( std::abs( _t_0.pos().x() - _t_0.lastPos().x() ) > 5 ) {
								if ( _t_0.pos().x() < _t_0.lastPos().x() ) {
									_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Right );
									_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Right );
								}
								else {
									_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Left );
									_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Left );
								}
							}
							if ( std::abs( _t_0.pos().y() - _t_0.lastPos().y() ) > 5 ) {
								if ( _t_0.pos().y() < _t_0.lastPos().y() ) {
									_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Down );
									_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Down );
								}
								else {
									_gw->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Up );
									_gw->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Up );
								}
							}
							return true;
						}
					}
					//Zoom, gesture pinch and spread
					if ( std::abs( _v_now - _v_last ) > 25 ) {
						if ( _v_now > _v_last ) {
							_gw->getEventQueue()->mouseScroll( osgGA::GUIEventAdapter::SCROLL_UP );
						}
						if ( _v_now < _v_last ) {
							_gw->getEventQueue()->mouseScroll( osgGA::GUIEventAdapter::SCROLL_DOWN );
						}
					}

				}
				//change mode to selection mode
				if ( touchPoints.size() == 3 ) {
					if ( _counter != 0 ) {
						_counter--;
						return true;
					}
					CoreWindow* coreWindow = dynamic_cast<CoreWindow*>( parent() );
					coreWindow->multiSelectClicked( true );
					_selectionMode = true;
					_rightMouse = false;
					_counter = 2;
					_difference = _time.elapsed();
					_time.restart();
				}
			}
			break;
		}
		default:
			return QWidget::event( event );
	}
	return true;
}

} // namespace QOSG

#include <leathers/pop>
