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
#include "LuaGraph/LuaGraphTreeItem.h"

namespace OsgQtBrowser {

QWebViewImage::QWebViewImage()
{
	// make sure we have a valid QApplication before we start creating widgets.
	OsgQtBrowser::QGraphicsViewAdapter::getOrCreateQApplication();

	_webView = new QWebView;

	_webPage = new QWebPage;
	_webPage->settings()->setAttribute( QWebSettings::JavascriptEnabled, true );
	_webPage->settings()->setAttribute( QWebSettings::PluginsEnabled, true );

	_webView->setPage( _webPage );
	//_webView->setHtml();

	// Add loadFinished listener to ensure our page is loaded before assigning data objects
	connect( _webPage->mainFrame(), SIGNAL( loadFinished( bool ) ), this, SLOT( loadFinished( bool ) ) );

	_adapter = new QGraphicsViewAdapter( this, _webView.data() );
}

void QWebViewImage::loadFinished( bool ok )
{
	// Clear qData object with empty models array attribute
	_webPage->mainFrame()->evaluateJavaScript( "var qData={models:[]};" );

	// TODO pass variables via Q_PROPERTY and public slots (problems with data retrieving)
	// - currently manually added via js evaluation

	// Iterate over all models and add them one by one to models array
	QList<Lua::LuaGraphTreeModel* >::iterator i;
	Lua::LuaGraphTreeModel* model;

	// Iterate over each selected node
	for ( i = _models->begin(); i != _models->end(); i++ ) {
		model = *i;

		Lua::LuaGraphTreeItem* root = model->getRootItem();

		// Call recursive function to travel through each tree model item
		this->addChildrenToJsModel( root, "curModel" );

		// Add curModel to array of models
		_webPage->mainFrame()->evaluateJavaScript( "qData.models.push(curModel);" );
	}

	// Send signal to browser that qData is ready
	_webPage->mainFrame()->evaluateJavaScript( "qDataReady();" );
}

void QWebViewImage::addChildrenToJsModel( Lua::LuaGraphTreeItem* item, QString path )
{
	// Clear current path variable (for example: "curModel = {};" if is root)
	_webPage->mainFrame()->evaluateJavaScript( path + " = {};" );

	Lua::LuaGraphTreeItem* child;
	QVariant key, value;
	QString escapedValue;

	// Iterate over all item children
	for ( int i=0; i<item->childCount(); i++ ) {
		child = item->child( i );
		key = child->data( 0 );
		value = child->data( 1 );

		// If has children, then append item key to path & call this function recursively
		if ( child->childCount() > 0 ) {

			// Concatenate current js variable path to tree item key
			this->addChildrenToJsModel( child, path + "." + key.toString() );
			// qDebug() << path << "." << key.toString();

		}
		else {
			// Otherwise pass value to current path as model (we need to add strings between "")
			escapedValue = "\"" + value.toString() + "\"";
			QString js = path + "." + key.toString() + " = " + escapedValue + ";";
			_webPage->mainFrame()->evaluateJavaScript( js );
			// qDebug() << js;
		}
	}
}

void QWebViewImage::navigateTo( const std::string& url )
{
	_webView->load( QUrl( url.c_str() ) );
}

void QWebViewImage::focusBrowser( bool focus )
{
	QFocusEvent event( focus ? QEvent::FocusIn : QEvent::FocusOut, Qt::OtherFocusReason );
	QCoreApplication::sendEvent( _webPage, &event );
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
	return _adapter->sendPointerEvent( x,y,buttonMask );
}

bool QWebViewImage::sendKeyEvent( int key, bool keyDown )
{
	return QWebViewImage::_adapter->sendKeyEvent( key, keyDown );
}

}
