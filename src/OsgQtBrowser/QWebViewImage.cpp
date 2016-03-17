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

#include "OsgQtBrowser/QWebViewImage.h"
#include "LuaInterface/LuaInterface.h"
#include "LuaTypes/LuaValueList.h"
#include "OsgQtBrowser/QLogWebPage.h"

namespace OsgQtBrowser {

QWebViewImage::QWebViewImage()
{
	// make sure we have a valid QApplication before we start creating widgets.
	OsgQtBrowser::QGraphicsViewAdapter::getOrCreateQApplication();

	_webView = new QWebView;

	_webPage = new QLogWebPage;
	_webPage->settings()->setAttribute( QWebSettings::JavascriptEnabled, true );
	_webPage->settings()->setAttribute( QWebSettings::PluginsEnabled, true );

	_webView->setPage( _webPage );

	_adapter = new QGraphicsViewAdapter( this, _webView.data() );
}

void QWebViewImage::navigateTo( const std::string& url )
{
	_webView->load( QUrl( url.c_str() ) );
}

void QWebViewImage::showTemplate( const std::string& templateName, Lua::LuaValueMap models, const std::string& templateType )
{
	// Initialize lua interface to call slt2 renderer
	Lua::LuaInterface* lua = Lua::LuaInterface::getInstance();
	QString renderer[] = {"slt2_renderer", "render"};

	// Prepare parameters to be passed to template renderer
	Lua::LuaValueList params;
	params.push_back( templateName );
	params.push_back( models.getValue() );

	// Call slt2 renderer
	std::string html = lua->callFunction( 2, renderer, params.getValue() )[0].asString();
	// qDebug() << html.c_str();

	// Create relative webview dir url
	QString appPath = QCoreApplication::applicationDirPath();
	QString webviewPath = appPath.append( "/../share/3dsoftviz/webview/index.html" );
	QUrl baseUrl = QUrl::fromLocalFile( webviewPath );

	// Set angular template type using query string
	if ( !templateType.empty() ) {

		// Fragment represents value after # hash in url. For example: http://something/index.html#<fragment>
		baseUrl.setFragment( QString::fromStdString( templateType ) );
	}

	// qDebug() << "Webview url: " << baseUrl;

	// Set html and baseUrl working directory
	_webView->setHtml( html.c_str(), baseUrl );
}

void QWebViewImage::focusBrowser( bool focus )
{
//	QFocusEvent event( focus ? QEvent::FocusIn : QEvent::FocusOut, Qt::OtherFocusReason );
//	QCoreApplication::sendEvent( _webPage, &event );
}

void QWebViewImage::clearWriteBuffer()
{
	_adapter->clearWriteBuffer();
}

void QWebViewImage::render()
{
	_adapter->render();
}

void QWebViewImage::setFrameLastRendered( const osg::FrameStamp* frameStamp )
{
	_adapter->setFrameLastRendered( frameStamp );
}

bool QWebViewImage::sendPointerEvent( int x, int y, int buttonMask )
{
	// TODO interaction could be enabled but we need to check if camera is rotating or moving
	// (returned value indicates if we are going to handle this event here)
	//return _adapter->sendPointerEvent( x,y,buttonMask );
	return false;
}

bool QWebViewImage::sendKeyEvent( int key, bool keyDown )
{
	//return QWebViewImage::_adapter->sendKeyEvent( key, keyDown );
	return false;
}

}
