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

#ifndef QWEBVIEWIMAGE
#define QWEBVIEWIMAGE

#include <osgWidget/Browser>
#include <QtWebKit/QWebSettings>
#include <QtWebKit/QtWebKit>

#include "OsgQtBrowser/QGraphicsViewAdapter.h"
#include "Diluculum/Types.hpp"

#include <QMap>

namespace OsgQtBrowser {
/**
*  \class QWebViewImage
*  \brief
*  \author Adam Pazitnaj, Michael Gloger
*  \date 29. 4. 2010, 12.4.2015
*/
class QWebViewImage : public QObject, public osgWidget::BrowserImage
{

	Q_OBJECT
public:


	/**
		*  \fn public constructor  QWebViewImage
		*  \brief
		*/
	QWebViewImage();


	/**
		*  \fn public virtual  navigateTo
		*  \brief
		*  \param [in]       url const std::string &
		*/
	virtual void navigateTo( const std::string& url );

	/**
		*  \fn public showTemplate
		*  \brief
		*  \param templateName name of the template file
		*  \param models
		*  \param templateType type of the template route
		*/
	void showTemplate( const std::string& templateName, Diluculum::LuaValueMap models, const std::string& templateType );

    void showGitTemplate( const std::string& templateName, const std::string& templateType, QMap<QString, int>* changedMetrics );

	/**
		*  \fn inline public  getQWebView
		*  \brief
		*  \return QWebView *
		*/
	QWebView* getQWebView()
	{
		return _webView;
	}

	/**
		*  \fn inline public  getQWebPage
		*  \brief
		*  \return QWebPage *
		*/
	QWebPage* getQWebPage()
	{
		return _webPage;
	}

	/**

		*  \fn inline public  get
		*
		*  \brief
		*  \return QGraphicsViewAdapter *
		*/
	QGraphicsViewAdapter* getQGraphicsViewAdapter()
	{
		return _adapter;
	}


	/**
		*  \fn public  focusBrowser
		*  \brief
		*  \param [in]       focus bool
		*/
	void focusBrowser( bool focus );


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
		*  \fn public virtual  setFrameLastRendered
		*  \brief
		*  \param [in]       frameStamp const osg::FrameStamp *
		*/
	virtual void setFrameLastRendered( const osg::FrameStamp* frameStamp );


	/**
		*  \fn public virtual  sendPointerEvent
		*  \brief
		*  \param [in]       x int
		*  \param [in]       y int
		*  \param [in]       buttonMask int
		*  \return bool
		*/
	virtual bool sendPointerEvent( int x, int y, int buttonMask );


	/**
		*  \fn public virtual  sendKeyEvent
		*  \brief
		*  \param [in]       key int
		*  \param [in]       keyDown bool
		*  \return bool
		*/
	virtual bool sendKeyEvent( int key, bool keyDown );

    QString createGitHtml( QMap<QString, int>* changedMetrics );

protected:


	/**
		*  QPointer<QGraphicsViewAdapter> _adapter
		*  \brief
		*/
	QPointer<QGraphicsViewAdapter>  _adapter;

	/**
		*  QPointer<QWebView> _webView
		*  \brief
		*/
	QPointer<QWebView>              _webView;

	/**
		*  QPointer<QWebPage> _webPage
		*  \brief
		*/
	QPointer<QWebPage>              _webPage;
};
}
#endif
