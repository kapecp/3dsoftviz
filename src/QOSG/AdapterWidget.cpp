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
	_gw->getEventQueue()->mouseScroll( ( event->delta()>0 ) ?
									   osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN );
}

} // namespace QOSG

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
