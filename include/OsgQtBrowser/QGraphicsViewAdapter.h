/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2009 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#ifndef QGRAPHICSVIEWADAPTER
#define QGRAPHICSVIEWADAPTER

#include <osg/Image>
#include <osg/observer_ptr>

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QPointer>

namespace OsgQtBrowser {

#define MYQKEYEVENT 2000
#define MYQPOINTEREVENT 2001

struct MyQPointerEvent : public QEvent {
	MyQPointerEvent( int x, int y, unsigned int buttonMask ):
		QEvent( QEvent::Type( MYQPOINTEREVENT ) ),
		_x( x ), _y( y ),_buttonMask( buttonMask ) {}

	int _x, _y;
	unsigned int _buttonMask;
};




/**
*  \class QGraphicsViewAdapter
*  \brief
*  \author Adam Pazitnaj
*  \date 29. 4. 2010
*/
class QGraphicsViewAdapter : public QObject
{
	Q_OBJECT

public:

	/**
		*  \fn public constructor  QGraphicsViewAdapter
		*  \brief
		*  \param [in, out]  image osg::Image *
		*  \param [in, out]  widget QWidget *
		*/
	QGraphicsViewAdapter( osg::Image* image, QWidget* widget );

	static QCoreApplication* getOrCreateQApplication();

	/**
		*  \fn public  setUpKeyMap
		*  \brief
		*/
	void setUpKeyMap();

	/**
		*  \fn public  sendPointerEvent
		*  \brief
		*  \param [in]       x int
		*  \param [in]       y int
		*  \param [in]       buttonMask int
		*  \return bool
		*/
	bool sendPointerEvent( int x, int y, int buttonMask );

	/**
		*  \fn public  sendKeyEvent
		*  \brief
		*  \param [in]       key int
		*  \param [in]       keyDown bool
		*  \return bool
		*/
	bool sendKeyEvent( int key, bool keyDown );



	/**
		*  \fn public  setFrameLastRendered
		*  \brief
		*  \param [in]       frameStamp const osg::FrameStamp *
		*/
	void setFrameLastRendered( const osg::FrameStamp* frameStamp );


	/**
		*  \fn public  clearWriteBuffer
		*  \brief
		*/
	void clearWriteBuffer();


	/**
		*  \fn public  render
		*  \brief
		*/
	void render();


	/**
		*  \fn public  assignImage
		*  \brief
		*  \param [in]       i unsigned int
		*/
	void assignImage( unsigned int i );

protected:


	/**
		*  \fn protected  handlePointerEvent
		*  \brief
		*  \param [in]       x int
		*  \param [in]       y int
		*  \param [in]       buttonMask int
		*  \return bool
		*/
	bool handlePointerEvent( int x, int y, int buttonMask );

	/**
		*  \fn protected  handleKeyEvent
		*  \brief
		*  \param [in]       key int
		*  \param [in]       keyDown bool
		*  \return bool
		*/
	bool handleKeyEvent( int key, bool keyDown );


	/**
		*  osg::observer_ptr<osg::Image> _image
		*  \brief
		*/
	osg::observer_ptr<osg::Image>   _image;


	/**
		*  unsigned int _previousButtonMask
		*  \brief
		*/

	int                    _previousButtonMask;

	/**
		*  int _previousMouseX
		*  \brief
		*/
	int                             _previousMouseX;

	/**
		*  int _previousMouseY
		*  \brief
		*/
	int                             _previousMouseY;

	typedef std::map<int, Qt::Key> KeyMap;

	/**
		*  QGraphicsViewAdapter::KeyMap _keyMap
		*  \brief
		*/
	KeyMap                          _keyMap;

	/**
		*  Qt::KeyboardModifiers _qtKeyModifiers
		*  \brief
		*/
	Qt::KeyboardModifiers           _qtKeyModifiers;


	/**
		*  QColor _backgroundColor
		*  \brief
		*/
	QColor                          _backgroundColor;

	/**
		*  QPointer<QGraphicsView> _graphicsView
		*  \brief
		*/
	QPointer<QGraphicsView>         _graphicsView;

	/**
		*  QPointer<QGraphicsScene> _graphicsScene
		*  \brief
		*/
	QPointer<QGraphicsScene>        _graphicsScene;


	/**
		*  OpenThreads::Mutex _qimagesMutex
		*  \brief
		*/
	OpenThreads::Mutex              _qimagesMutex;

	/**
		*  unsigned int _previousFrameNumber
		*  \brief
		*/
	unsigned int                    _previousFrameNumber;

	/**
		*  bool _newImageAvailable
		*  \brief
		*/
	bool                            _newImageAvailable;

	/**
		*  unsigned int _currentRead
		*  \brief
		*/
	unsigned int                    _currentRead;

	/**
		*  unsigned int _currentWrite
		*  \brief
		*/
	unsigned int                    _currentWrite;

	/**
		*  unsigned int _previousWrite
		*  \brief
		*/
	unsigned int                    _previousWrite;

	/**
		*  QImage [3] _qimages
		*  \brief
		*/
	QImage                          _qimages[3];


	/**
		*  \fn protected virtual  customEvent
		*  \brief
		*  \param [in, out]  event QEvent *
		*/
	virtual void customEvent( QEvent* event ) ;

private slots:


	/**
		*  \fn private  repaintRequestedSlot
		*  \brief
		*  \param [in]       regions const QList<QRectF> &
		*/
	void repaintRequestedSlot( const QList<QRectF>& regions );

};
}
#endif
