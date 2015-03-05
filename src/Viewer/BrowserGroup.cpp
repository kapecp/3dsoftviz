#include "Viewer/BrowserGroup.h"
#include "OsgQtBrowser/QWebViewImage.h"

#include <osg/ValueObject>

#include <LuaGraph/LuaGraph.h>

#include <math.h>

namespace Vwr {

BrowserGroup::BrowserGroup()
{
	this->group = new osg::Group;
	this->browsersTransforms = new QList<osg::ref_ptr<osg::AutoTransform> >;
	this->browsersGrouping = false;
	this->selectedNodesModels = new QMap<qlonglong, Lua::LuaGraphTreeModel*>;
	this->selectedNodes = new QMap<qlonglong, osg::ref_ptr<Data::Node> >;
}

BrowserGroup::~BrowserGroup( void )
{
}

void BrowserGroup::setSelectedNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selected )
{
	// std::cout << "Selected nodes size: " << selected->size() << "\n" << std::flush;

	// No nodes are selected
	if ( selected->size() == 0 ) {
		// Remove all browsers & models when no nodes are selected
		this->clearBrowsers();
		this->clearModels();

		// Nothing to do here
		return;
	}

	QLinkedList<osg::ref_ptr<Data::Node> >::iterator i;
	Data::Node* node;

	// Iterate over each selected node
	for ( i = selected->begin(); i != selected->end(); i++ ) {
		node = *i;

		// If node was not already previously selected & its model initialized & node exists in lua model
		if ( !selectedNodesModels->contains( node->getId() ) && Lua::LuaGraph::getInstance()->getNodes()->contains( node->getId() ) ) {

			// Get lua node model and add it to model map
			Lua::LuaNode* luaNode = Lua::LuaGraph::getInstance()->getNodes()->value( node->getId() );
			Lua::LuaGraphTreeModel* luaModel = new Lua::LuaGraphTreeModel( luaNode );
			selectedNodesModels->insert( node->getId(), luaModel );
			//std::cout << "Selecting node: " << node->getId() << "\n" << std::flush;

			// Remember node in nodes map
			selectedNodes->insert( node->getId(), node );

			// If grouping is not enabled, then add browser for each newly selected node
			if ( !browsersGrouping ) {
				QList<Lua::LuaGraphTreeModel*>* models = new QList<Lua::LuaGraphTreeModel*>();
				models->push_back( luaModel );
				this->addBrowser( node->getCurrentPosition(), models );
			}
		}
	}

	// If grouping is enabled
	if ( browsersGrouping ) {
		this->clearBrowsers();
		this->initGroupedBrowser();
	}
}

void BrowserGroup::setBrowsersGrouping( bool browsersGrouping )
{
	// If grouping setting changed
	if ( this->browsersGrouping != browsersGrouping ) {

		this->browsersGrouping = browsersGrouping;

		// Clear all browsers & selected nodes data
		this->clearBrowsers();

		// Initialize browsers all over again
		if ( browsersGrouping ) {
			initGroupedBrowser();
		}
		else {
			initBrowsers();
		}
	}

	this->browsersGrouping = browsersGrouping;
}

void BrowserGroup::initBrowsers()
{
	QMapIterator<qlonglong, osg::ref_ptr<Data::Node> > i( *selectedNodes );
	Data::Node* node;
	Lua::LuaGraphTreeModel* model;

	// Iterate over all selected nodes and create sepparate browsers for each node
	while ( i.hasNext() ) {
		i.next();

		node = i.value();
		model = selectedNodesModels->value( i.key() );

		QList<Lua::LuaGraphTreeModel*>* models = new QList<Lua::LuaGraphTreeModel*>();
		models->push_back( model );
		this->addBrowser( node->getCurrentPosition(), models );
	}
}

void BrowserGroup::initGroupedBrowser()
{
	QMapIterator<qlonglong, osg::ref_ptr<Data::Node> > i( *selectedNodes );
	Data::Node* node;
	Lua::LuaGraphTreeModel* model;
	float xSum = 0, ySum = 0, zSum = 0;
	osg::Vec3f pos;

	// Iterate over all selected nodes and show one browser in their center using list of all selected nodes models
	while ( i.hasNext() ) {
		i.next();

		node = i.value();
		model = selectedNodesModels->value( i.key() );

		pos = node->getCurrentPosition();

		xSum += pos.x();
		ySum += pos.y();
		zSum += pos.z();
	}

	// Calculate centroid
	pos = osg::Vec3f(
			  xSum / selectedNodes->size(),
			  ySum / selectedNodes->size(),
			  zSum / selectedNodes->size()
		  );

	QList<Lua::LuaGraphTreeModel*>* models = new QList<Lua::LuaGraphTreeModel*>( selectedNodesModels->values() );
	this->addBrowser( pos, models );
}

void BrowserGroup::addBrowser( osg::Vec3 position, QList<Lua::LuaGraphTreeModel*>* models )
{
	// qDebug() << "Adding browser";

	// Create webView
	osg::ref_ptr<OsgQtBrowser::QWebViewImage> webView = new OsgQtBrowser::QWebViewImage();
	webView->navigateTo( "../share/3DSOFTVIZ/webview/index.html" );
	webView->setModels( models );

	// Add it to browser
	osgWidget::GeometryHints hints( osg::Vec3( 0.0f,0.0f,0.0f ),
									osg::Vec3( 150.0f,0.0f,0.0f ),
									osg::Vec3( 0.0f,150.0f,0.0f ),
									osg::Vec4( 0.0f,0.0f,0.0f,0.0f ) );

	osg::ref_ptr<osgWidget::Browser> browser = new osgWidget::Browser;
	browser->assign( webView, hints );

	// Wrap browser to transform
	osg::ref_ptr<osg::AutoTransform> transform = new osg::AutoTransform;
	transform->setPosition( position );
	transform->setAutoRotateMode( osg::AutoTransform::ROTATE_TO_SCREEN );
	transform->addChild( browser );

	// Set initial scale & set animation start frame to help us calculate interpolation value
	transform->setScale( 0 );
	transform->setUserValue( "frame", 0 ); // TODO fix error with undefined symbols for architecture........

	// Add transform to group
	this->group->addChild( transform );

	// Remember transform
	this->browsersTransforms->append( transform );
}

void BrowserGroup::clearBrowsers()
{
	this->group->removeChildren( 0, this->group->getNumChildren() );
	this->browsersTransforms->clear();
}

void BrowserGroup::clearModels()
{
	this->selectedNodesModels->clear();
	this->selectedNodes->clear();

	// TODO make sure no memory is leaking ...
	// UPDATE: fix memory leaking ....
}

double BrowserGroup::interpolate( long currentFrame, long endFrame, double startValue, double endValue )
{
	double value = endValue * ( pow( ( currentFrame / ( float )endFrame ) - 1, 5 ) + 1 + startValue );

	// std::cout << value << " " << currentFrame << " " << endFrame << " " << startValue << " " << endValue << endl;
	// std::cout << flush;

	return value;
}

void BrowserGroup::updateBrowsers()
{
	QList<osg::ref_ptr<osg::AutoTransform> >::iterator i;
	osg::ref_ptr<osg::AutoTransform> transform;
	int frame;

	// Interpolate each browser transform scale using interpolation function
	for ( i = this->browsersTransforms->begin(); i != this->browsersTransforms->end(); i++ ) {

		transform = *i;

		if ( transform->getScale().x() < 1 ) {

			// Get animation frame for current transform
			transform->getUserValue( "frame", frame );

			// Apply interpolation function
			transform->setScale( this->interpolate( frame, 20, 0, 1 ) );

			// Increment transform animation time
			transform->setUserValue( "frame", frame + 1 );
		}
	}
}

} // namespace Vwr
