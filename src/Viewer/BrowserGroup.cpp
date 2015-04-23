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
	this->selectedNodes = new QLinkedList<osg::ref_ptr<Data::Node> >();
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

		// Ignore meta nodes
		if(node->getName() == "metaNode") {
			continue;
		}

		// If node was not already previously selected & its model initialized & node exists in lua model
		if ( !selectedNodes->contains( node ) && Lua::LuaGraph::getInstance()->getNodes()->contains( node->getId() ) ) {

			// Get lua node model and add it to model map
			Lua::LuaNode* luaNode = Lua::LuaGraph::getInstance()->getNodes()->value( node->getId() );
			Diluculum::LuaValueMap paramsTable = luaNode->getParams().asTable();

			// Ignore nodes without models
			if(paramsTable.find("metrics") == paramsTable.end()) {
				continue;
			}

			selectedNodes->push_back(node);

			// qDebug() << "Selecting node: " << node->getId() << node->getName();

			// If grouping is not enabled, then add browser for each newly selected node
			if ( !browsersGrouping ) {
				Diluculum::LuaValueMap models;
				models.insert(std::pair<Diluculum::LuaValue, Diluculum::LuaValue>((long)node->getId(), luaNode->getParams()));

				this->addBrowser( "single", node->getCurrentPosition(), models );
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

	QLinkedList<osg::ref_ptr<Data::Node> >::iterator i;
	Data::Node* node;

	// Iterate over all selected nodes and create sepparate browsers for each node
	for (i = selectedNodes->begin(); i != selectedNodes->end(); i++) {
		node = *i;

		Lua::LuaNode* luaNode = Lua::LuaGraph::getInstance()->getNodes()->value( node->getId() );
		Diluculum::LuaValueMap models;
		models.insert(std::pair<Diluculum::LuaValue, Diluculum::LuaValue>((long)node->getId(), luaNode->getParams()));
		this->addBrowser( "single", node->getCurrentPosition(), models );
	}
}

void BrowserGroup::initGroupedBrowser()
{
	QLinkedList<osg::ref_ptr<Data::Node> >::iterator i;
	Data::Node* node;
	float xSum = 0, ySum = 0, zSum = 0;
	osg::Vec3f pos;
	Diluculum::LuaValueMap models;
	Lua::LuaNode* luaNode;

	// Iterate over all selected nodes and show one browser in their center using list of all selected nodes models
	for (i = selectedNodes->begin(); i != selectedNodes->end(); i++) {
		node = *i;

		pos = node->getCurrentPosition();

		xSum += pos.x();
		ySum += pos.y();
		zSum += pos.z();

		luaNode = Lua::LuaGraph::getInstance()->getNodes()->value( node->getId() );
		models.insert(std::pair<Diluculum::LuaValue, Diluculum::LuaValue>((long)node->getId(), luaNode->getParams()));
	}

	// Calculate centroid
	pos = osg::Vec3f(
			  xSum / selectedNodes->size(),
			  ySum / selectedNodes->size(),
			  zSum / selectedNodes->size()
		  );

	this->addBrowser( "multi", pos, models );
}

void BrowserGroup::addBrowser(const std::string &templateType, osg::Vec3 position, Diluculum::LuaValueMap models )
{
	// qDebug() << "Adding browser";

	// Create webView
	osg::ref_ptr<OsgQtBrowser::QWebViewImage> webView = new OsgQtBrowser::QWebViewImage();

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

	// Display template in webview
	webView->showTemplate("metrics_template", models, templateType);
}

void BrowserGroup::clearBrowsers()
{
	this->group->removeChildren( 0, this->group->getNumChildren() );
	this->browsersTransforms->clear();
}

void BrowserGroup::clearModels()
{
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
