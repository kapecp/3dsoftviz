/**
*  AdapterWidget.h
*  Projekt 3DVisual
*/
#ifndef QOSG_ADAPTER_WIDGET_H
#define QOSG_ADAPTER_WIDGET_H 1

#include <osgViewer/Viewer>

#include <QtGlobal>
#include <QTime>

#include <QKeyEvent>
#include <QGLWidget>

#if QT_VERSION > 0x050000
#ifdef __APPLE__
/* FIX COMPILE BUG:

	platform: OSX v10.11.6 (15G1611) with Qt v5.9.1 from Homebrew

	this solves the following compilation error:
	/usr/local/Cellar/qt/5.9.1/lib/QtGui.framework/Headers/qopenglversionfunctions.h:1089:23: error: unknown type name 'GLDEBUGPROC'

	somehow the GL_KHR_debug macro is set to 1 in qopengl.h, so
	#ifndef GL_KHR_debug
	typedef void (APIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const GLvoid *userParam);
	#endif
	are removed, causing the error "unknown type name" GLDEBUGPROC in qopenglversionfunctions.h

	possible causes:
		- some change in Qt v5.9.1 (older versions have worked, at least ~5.8 worked)
*/
typedef void ( APIENTRY* GLDEBUGPROC )( GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const GLvoid* userParam );
#endif
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#endif

#if QT_VERSION >= 0x050000
#define TOASCII toLatin1
#else
#define TOASCII toAscii
#endif

using Qt::WindowFlags;

namespace QOSG {
/**
	*  \class AdapterWidget
	*  \brief
	*  \author Adam Pazitnaj
	*  \date 29. 4. 2010
	*/
//

#if QT_VERSION > 0x050000
class AdapterWidget : public QOpenGLWidget, protected QOpenGLFunctions
#elif QT_VERSION > 0x040000
class AdapterWidget : public QGLWidget
#endif
{

public:

	/*!
		* \param parent
		* Rodic widgetu
		*
		* \param name
		* Meno widgetu
		*
		* \param shareWidget
		* Zdielanie widgetu
		*
		* \param f
		* Znacky pre vytvaranie okna.
		*
		*
		* Konstruktor.
		*
		*/

	explicit AdapterWidget( const QGLFormat& format, QWidget* parent = 0, const char*   name = 0, const QGLWidget* shareWidget = 0, WindowFlags f = 0 );


	/*!
		*
		*
		* Destruktor.
		*
		*/
	virtual ~AdapterWidget() {}


	/**
		*  \fn inline public overloaded  getGraphicsWindow
		*  \brief
		*  \return osgViewer::GraphicsWindow *
		*/
	osgViewer::GraphicsWindow* getGraphicsWindow()
	{
		return _gw.get();
	}

	/**
		*  \fn inline public overloaded constant  getGraphicsWindow
		*  \brief
		*  \return const osgViewer::GraphicsWindow *
		*/
	const osgViewer::GraphicsWindow* getGraphicsWindow() const
	{
		return _gw.get();
	}

protected:


	/**
		*  \fn protected  init
		*  \brief
		*/
	void init();

	void initializeGL();

	/**
		*  \fn protected virtual  resizeGL( int width, int height )
		*  \brief
		*  \param width
		*  \param height
		*/
	virtual void resizeGL( int width, int height );

	/**
		*  \fn protected virtual  keyPressEvent( QKeyEvent* event )
		*  \brief
		*  \param event
		*/
	virtual void keyPressEvent( QKeyEvent* event );

	/**
		*  \fn protected virtual  keyReleaseEvent( QKeyEvent* event )
		*  \brief
		*  \param event
		*/
	virtual void keyReleaseEvent( QKeyEvent* event );

	/**
		*  \fn protected virtual  mousePressEvent( QMouseEvent* event )
		*  \brief
		*  \param event
		*/
	virtual void mousePressEvent( QMouseEvent* event );

	/**
		*  \fn protected virtual  mouseDoubleClickEvent( QMouseEvent * event )
		*  \brief
		*  \param event
		*/
	virtual void mouseDoubleClickEvent( QMouseEvent* event );

	/**
		*  \fn protected virtual  mouseReleaseEvent( QMouseEvent * event )
		*  \brief
		*  \param [in, out]  event QMouseEvent *
		*/
	virtual void mouseReleaseEvent( QMouseEvent* event );

	/**
		*  \fn protected virtual  mouseMoveEvent( QMouseEvent * event )
		*  \brief
		*  \param event
		*/
	virtual void mouseMoveEvent( QMouseEvent* event );

	/**
		*  \fn protected virtual  wheelEvent( QWheelEvent * event )
		*  \brief
		*  \param event
		*/
	virtual void wheelEvent( QWheelEvent* event );

	/**
	  *  \fn protected virtual event( QEvent *event )
	  *  \brief
	  *  \param event
	 */
	virtual bool event( QEvent* event );


	/**
		*  osg::ref_ptr _gw
		*  \brief
		*/
	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gw;
	/**
	  *  Moje lokalne premenne pamatanie dotykov
	  */
	QTime _time;
	int _difference;
	int _whatSelect;
	int _counter;
	bool _rightMouse;
	bool _selectionMode;
	QTouchEvent::TouchPoint _lastSingleTouch;

};
}

#endif
