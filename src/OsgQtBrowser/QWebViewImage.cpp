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
#include <QDebug>
namespace OsgQtBrowser {

QWebViewImage::QWebViewImage()
{
	// make sure we have a valid QApplication before we start creating widgets.
	OsgQtBrowser::QGraphicsViewAdapter::getOrCreateQApplication();

	_webView = new QWEBVIEW;

	_webPage = new QLogWebPage;
	_webPage->settings()->setAttribute( QWEBSETTING::JavascriptEnabled, true );
#if defined(QTWEBENGINE_PLUGINS)
	// missing in Qt 5.5
	// reintroduced in Qt 5.6
	// http://doc.qt.io/qt-5/qwebenginesettings.html
	_webPage->settings()->setAttribute( QWEBSETTING::PluginsEnabled, true );
#endif

	_webView->setPage( _webPage );

	_adapter = new QGraphicsViewAdapter( this, _webView.data() );
}

void QWebViewImage::navigateTo( const std::string& url )
{
	_webView->load( QUrl( url.c_str() ) );
}

void QWebViewImage::showTemplate( const std::string& templateName, Lua::LuaValueMap models, const std::string& templateType )
{
	qDebug() << templateName.c_str() << templateType.c_str();
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
//        baseUrl.setFragment( "git" );
	}

	qDebug() << "Webview url: " << baseUrl;

	// Set html and baseUrl working directory
	_webView->setHtml( html.c_str(), baseUrl );

//    qDebug() << _webView->page()->currentFrame()->toHtml();
}

void QWebViewImage::showGitTemplate( const std::string& templateName, const std::string& templateType, QMap<QString, int>* changedMetrics )
{
	// Create relative webview dir url
	QString appPath = QCoreApplication::applicationDirPath();
	QString webviewPath = appPath.append( "/../share/3dsoftviz/webview/index.html" );
	QUrl baseUrl = QUrl::fromLocalFile( webviewPath );

	// Set angular template type using query string
	if ( !templateType.empty() ) {

		// Fragment represents value after # hash in url. For example: http://something/index.html#<fragment>
		baseUrl.setFragment( QString::fromStdString( templateType ) );
	}

	qDebug() << "Webview url: " << baseUrl;

	// Set html and baseUrl working directory
//	_webView->setHtml( html.c_str(), baseUrl );
	_webView->setHtml( createGitHtml( changedMetrics ), baseUrl );
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

QString QWebViewImage::createGitHtml( QMap<QString, int>* changedMetrics )
{
	QString html = "";
	html += "<!DOCTYPE html>\n";
	html += "<html>\n";
	html += "<head lang='en'>\n";
	html += "<meta charset='UTF-8'>\n";
	html += "<!-- CSS Dependencies -->\n";
	html += "<link rel='stylesheet' href='bower_components/bootstrap/dist/css/bootstrap.min.css'>\n";
	html += "<link rel='stylesheet' href='bower_components/bootstrap/dist/css/bootstrap-theme.min.css'>\n";
	html += "<link rel='stylesheet' href='bower_components/angular-chart.js/dist/angular-chart.css'>\n";
	html += "<link rel='stylesheet' href='bower_components/parallel-coordinates/d3.parcoords.css'>\n";
	html += "<link rel='stylesheet' href='css/style.css'>\n";
	html += "</head>\n";
	html += "<body ng-app='app'>\n";
	html += "<div ui-view></div>\n";
	html += "<script type='text/javascript'>\n";
	html += "var models =\n";
	html += "{\n";
	html += "'22':\n";
	html += "{\n";
	html += "'metrics':\n";
	html += "{\n";
	html += "'changedLines': '" + QString::number( changedMetrics->value( "lines" ) ) + "',\n";
	html += "'changedLinesCode': '" + QString::number( changedMetrics->value( "linesCode" ) ) + "',\n";
	html += "'changedBlank': '" + QString::number( changedMetrics->value( "linesBlank" ) ) + "',\n";
	html += "'changedLinesComment': '" + QString::number( changedMetrics->value( "linesComment" ) ) + "',\n";
	html += "'changedNonEmpty': '" + QString::number( changedMetrics->value( "linesNonEmpty" ) ) + "'\n";
	html += "}\n";
	html += "}\n";
	html += "}\n";
	html += ";\n";
	html += "</script>\n";
	html += "<script src='bower_components/jquery/dist/jquery.min.js'></script>\n";
	html += "<script src='bower_components/bootstrap/dist/js/bootstrap.min.js'></script>\n";
	html += "<script src='bower_components/Chart.js/Chart.min.js'></script>\n";
	html += "<script src='bower_components/angular/angular.min.js'></script>\n";
	html += "<script src='bower_components/angular-ui-router/release/angular-ui-router.min.js'></script>\n";
	html += "<script src='bower_components/angular-chart.js/dist/angular-chart.min.js'></script>\n";
	html += "<script src='bower_components/d3/d3.min.js'></script>\n";
	html += "<script src='bower_components/parallel-coordinates/d3.parcoords.js'></script>\n";
	html += "<script src='src/app.js'></script>\n";
	html += "<script src='src/services/Service.js'></script>\n";
	html += "<script src='src/controllers/SingleController.js'></script>\n";
	html += "<script src='src/controllers/Single2Controller.js'></script>\n";
	html += "<script src='src/controllers/MultiController.js'></script>\n";
	html += "<script src='src/controllers/GitController.js'></script>\n";
	html += "<script src='src/directives/ParCoordsDirective.js'></script>\n";
	html += "</body>\n";
	html += "</html>\n";
	html = html.replace( "'", "\"" );
//    qDebug() << html;
	return html;
}

}
