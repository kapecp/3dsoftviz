#include "QOSG/AdapterWidget.h"
#include "QOSG/CoreWindow.h"

using namespace QOSG;

AdapterWidget::AdapterWidget( QWidget * parent, const char * name, const QGLWidget * shareWidget, WindowFlags f) : QGLWidget(parent, shareWidget, f)
{
	_gw = new osgViewer::GraphicsWindowEmbedded(0,0,width(),height());
	setFocusPolicy(Qt::StrongFocus);
}

void AdapterWidget::resizeGL( int width, int height )
{
	//zmena velkosti widgetu
	_gw->getEventQueue()->windowResize(0, 0, width, height );
	_gw->resized(0,0,width,height);
}

void AdapterWidget::keyPressEvent( QKeyEvent* event )
{
	//odchytavanie udalosti klavesnice
	switch(event->key())
	{
	case(Qt::Key_Up):
	{
		_gw->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Up);
		break;
	}
	case(Qt::Key_Down):
	{
		_gw->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Down );
		break;
	}
	case(Qt::Key_Right):
	{
		_gw->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Right );
		break;
	}
	case(Qt::Key_Left):
	{
		_gw->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Left );
		break;
	}
	case(Qt::Key_PageDown):
	{
		_gw->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Page_Down );
		break;
	}
	case(Qt::Key_PageUp):
	{
		_gw->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Page_Up );
		break;
	}
	case(Qt::Key_Alt):
	{
		_gw->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Alt_L );
		break;
	}
	case(Qt::Key_Control):
	{
		_gw->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Control_L );
		break;
	}
	case(Qt::Key_Shift):
	{
		_gw->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Shift_L );
		break;
	}
	default:
	{
		_gw->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
		break;
	}
	}
}

void AdapterWidget::keyReleaseEvent( QKeyEvent* event )
{
	//odchytavanie udalosti klavesnice
	switch(event->key())
	{
	case(Qt::Key_Up):
	{
		_gw->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KEY_Up);
		break;
	}
	case(Qt::Key_Down):
	{
		_gw->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KEY_Down );
		break;
	}
	case(Qt::Key_Right):
	{
		_gw->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KEY_Right );
		break;
	}
	case(Qt::Key_Left):
	{
		_gw->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KEY_Left );
		break;
	}
	case(Qt::Key_PageDown):
	{
		_gw->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KEY_Page_Down );
		break;
	}
	case(Qt::Key_PageUp):
	{
		_gw->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KEY_Page_Up );
		break;
	}
	case(Qt::Key_Alt):
	{
		_gw->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KEY_Alt_L );
		break;
	}
	case(Qt::Key_Control):
	{
		_gw->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KEY_Control_L );
		break;
	}
	case(Qt::Key_Shift):
	{
		_gw->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KEY_Shift_L );
		break;
	}
	case(Qt::Key_AsciiTilde):
	{
		CoreWindow * coreWindow = dynamic_cast<CoreWindow *>(parent());
		if (coreWindow)
		{
			coreWindow->addSQLInput();
		}
	}
	default:
	{
		_gw->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
		break;
	}
	}
}

void AdapterWidget::mousePressEvent( QMouseEvent* event )
{
	//odchytavanie udalosti mysi
	int button = 0;
	switch(event->button())
	{
	case(Qt::LeftButton): button = 1; break;
	case(Qt::MidButton): button = 2; break;
	case(Qt::RightButton): button = 3; break;
	case(Qt::NoButton): button = 0; break;
	default: button = 0; break;
	}
	_gw->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
}

void AdapterWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
	//odchytavanie udalosti mysi
	int button = 0;
	switch(event->button())
	{
	case(Qt::LeftButton): button = 1; break;
	case(Qt::MidButton): button = 2; break;
	case(Qt::RightButton): button = 3; break;
	case(Qt::NoButton): button = 0; break;
	default: button = 0; break;
	}
	_gw->getEventQueue()->mouseDoubleButtonPress(event->x(), event->y(), button);

}

void AdapterWidget::mouseReleaseEvent( QMouseEvent* event )
{
	//odchytavanie udalosti mysi
	int button = 0;
	switch(event->button())
	{
	case(Qt::LeftButton): button = 1; break;
	case(Qt::MidButton): button = 2; break;
	case(Qt::RightButton): button = 3; break;
	case(Qt::NoButton): button = 0; break;
	default: button = 0; break;
	}
	_gw->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
}

void AdapterWidget::mouseMoveEvent( QMouseEvent* event )
{
	//odchytavanie udalosti mysi
	_gw->getEventQueue()->mouseMotion(event->x(), event->y());
}

void AdapterWidget::wheelEvent(QWheelEvent *event)
{
	//odchytavanie udalosti mysi
	_gw->getEventQueue()->mouseScroll((event->delta()>0) ?
										  osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN);
}
