/**
*  AdapterWidget.h
*  Projekt 3DVisual
*/
#ifndef QOSG_ADAPTER_WIDGET_H
#define QOSG_ADAPTER_WIDGET_H 1

#include <osgViewer/Viewer>

#include <QtGlobal>

#include <QKeyEvent>
#include <QGLWidget>

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
class AdapterWidget : public QGLWidget
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

	explicit AdapterWidget( QWidget* parent = 0, const char*   name = 0, const QGLWidget* shareWidget = 0, WindowFlags f = 0 );


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
		*  osg::ref_ptr _gw
		*  \brief
		*/
	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gw;

};
}

#endif
