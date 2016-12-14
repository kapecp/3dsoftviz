#include "Viewer/BrowserGroup.h"
#include "OsgQtBrowser/QWebViewImage.h"

#include <QLinkedList>

#include <osg/ValueObject>
#include <osg/LineWidth>

#include <LuaGraph/LuaGraph.h>

#include "GitLib/GitEvolutionGraph.h"
#include "Manager/Manager.h"
#include "Repository/Git/GitLuaGraphUtils.h"
#include "GitLib/GitMetaData.h"

#include <math.h>
#include <utility>
#include <string>

namespace Vwr {

BrowserGroup::BrowserGroup() :
	group( new osg::Group ),
	browsersGrouping( false ),
	browsersTransforms( new QList<osg::ref_ptr<osg::AutoTransform> > ),
	connectorsTransforms( new QList<osg::ref_ptr<osg::AutoTransform> > ),
	selectedNodes( new QLinkedList<osg::ref_ptr<Data::Node> > ),
	selectedNodesModels( nullptr ),
	showGit( false )
{
	this->group->getOrCreateStateSet()->setMode( GL_LIGHTING,osg::StateAttribute::OFF );
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
	for ( i = selected->begin(); i != selected->end(); ++i ) {
		node = *i;

		// Ignore meta nodes
		if ( node->Data::AbsNode::getName() == "metaNode" ) {
			continue;
		}

		// If node was not already previously selected & its model initialized & node exists in lua model
		if ( !selectedNodes->contains( node ) && Lua::LuaGraph::getInstance()->getNodes()->contains( node->getId() ) ) {

			// Get lua node model and add it to model map
			Lua::LuaNode* luaNode = Lua::LuaGraph::getInstance()->getNodes()->value( node->getId() );
			Lua::LuaValueMap paramsTable = luaNode->getParams().getValue().asTable();

			// Ignore nodes without models
			if ( paramsTable.find( "metrics" ) == paramsTable.end() ) {
				continue;
			}

			selectedNodes->push_back( node );

			// qDebug() << "Selecting node: " << node->getId() << node->getName();

			// If grouping is not enabled, then add browser for each newly selected node
			if ( !browsersGrouping ) {
				Lua::LuaValueMap models;
				models.insertPair( static_cast<long>( node->getId() ), luaNode->getParams().getValue() );
				if ( !this->showGit ) {
					this->addBrowser( "single", node->getCurrentPosition(), models );
				}
				else {
					Repository::Git::GitLuaGraphUtils luaUtils = Repository::Git::GitLuaGraphUtils( Lua::LuaGraph::getInstance(), Manager::GraphManager::getInstance()->getActiveEvolutionGraph() );

					QMap<QString, int>* map = luaUtils.compareMetrics( luaUtils.getMetrics( luaNode->getIdentifier() ), Manager::GraphManager::getInstance()->getActiveEvolutionGraph()->getMetaDataFromIdentifier( luaNode->getIdentifier() )->getMetric() );

					this->addBrowser( "git", node->getCurrentPosition(), map );
				}
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
	for ( i = selectedNodes->begin(); i != selectedNodes->end(); ++i ) {
		node = *i;

		Lua::LuaNode* luaNode = Lua::LuaGraph::getInstance()->getNodes()->value( node->getId() );
		Lua::LuaValueMap models;
		models.insertPair( static_cast<long>( node->getId() ), luaNode->getParams().getValue() );
		this->addBrowser( "single", node->getCurrentPosition(), models );
	}
}

void BrowserGroup::initGroupedBrowser()
{
	QLinkedList<osg::ref_ptr<Data::Node> >::iterator i;
	Data::Node* node;
	float xSum = 0, ySum = 0, zSum = 0;
	osg::Vec3f pos;
	Lua::LuaValueMap models;
	Lua::LuaNode* luaNode;

	// Iterate over all selected nodes and show one browser in their center using list of all selected nodes models
	for ( i = selectedNodes->begin(); i != selectedNodes->end(); ++i ) {
		node = *i;

		pos = node->getCurrentPosition();

		xSum += pos.x();
		ySum += pos.y();
		zSum += pos.z();

		luaNode = Lua::LuaGraph::getInstance()->getNodes()->value( node->getId() );
		models.insertPair( static_cast<long>( node->getId() ), luaNode->getParams().getValue() );
	}

	// Calculate centroid
	pos = osg::Vec3f(
			  xSum / static_cast<float>( selectedNodes->size() ),
			  ySum / static_cast<float>( selectedNodes->size() ),
			  zSum / static_cast<float>( selectedNodes->size() )
		  );

	this->addBrowser( "multi", pos, models );
}

void BrowserGroup::addBrowser( const std::string& templateType, osg::Vec3 position, Lua::LuaValueMap models )
{
	// qDebug() << "Adding browser";

	// Create webView
	osg::ref_ptr<OsgQtBrowser::QWebViewImage> webView = new OsgQtBrowser::QWebViewImage();

	// Webview position/offset (should have same aspect ratio as 800/600)
	float width = 240;
	float height = 180;
	float offset;

	// Create connectors targets depending on whether grouping is enabled & setup offset
	osg::Vec3Array* targets;
	if ( this->browsersGrouping ) {
		offset = 0;
		targets = new osg::Vec3Array( static_cast<unsigned int>( selectedNodes->size() ) );

		QLinkedList<osg::ref_ptr<Data::Node> >::iterator i;
		Data::Node* node;
		unsigned long pos = 0;

		// Iterate over each selected node and add its position to array
		for ( i = selectedNodes->begin(); i != selectedNodes->end(); ++i ) {
			node = *i;
			( *targets )[pos++].set( node->getCurrentPosition() );
		}

	}
	else {
		offset = 60;
		targets = new osg::Vec3Array( 1 );
		( *targets )[0].set( position );
	}

	// Setup browser pos
	osg::Vec3 bl = osg::Vec3( -width/2, -height/2, 0 ); // Bottom left
	osg::Vec3 br = osg::Vec3( width/2, -height/2, 0 );  // Bottom right
	osg::Vec3 tr = osg::Vec3( width/2,  height/2, 0 );  // Top right
	osg::Vec3 tl = osg::Vec3( -width/2,  height/2, 0 ); // Top left
	osg::Vec3 center = osg::Vec3(
						   position.x() + offset + width/2,
						   position.y() + offset + height/2,
						   position.z()
					   );

	osgWidget::GeometryHints hints( bl,
									osg::Vec3( width, 0,  0 ),
									osg::Vec3( 0,  width, 0 ), // Needs more experimenting
									osg::Vec4( 0,  0,  0, 0 ) );

	osg::ref_ptr<osgWidget::Browser> browser = new osgWidget::Browser;
	browser->assign( webView, hints );

	// Wrap connectors to transform
	osg::ref_ptr<osg::AutoTransform> connectorTransform = new osg::AutoTransform;
	connectorTransform->addChild( this->createConnectorsGeode( center, targets ) ); // Add connecting lines geode

	// Wrap browser and border to transform
	osg::ref_ptr<osg::AutoTransform> browserTransform = new osg::AutoTransform;
	browserTransform->setPosition( center );
	browserTransform->setAutoRotateMode( osg::AutoTransform::ROTATE_TO_SCREEN );
	browserTransform->addChild( browser );
	browserTransform->addChild( this->createBorderGeode( bl, br, tr, tl ) ); // Add border geode
//	browserTransform->getOrCreateStateSet()->setMode();

	// Set initial scale & set animation start frame to help us calculate interpolation value
//	connectorTransform->setScale( 0 ); Disabled since its position is relative to graph, not current node
//	connectorTransform->setUserValue( "frame", 0 );
	browserTransform->setScale( 0 );
	browserTransform->setUserValue( "frame", 0 );

	// Add transform to group
	this->group->addChild( connectorTransform );
	this->group->addChild( browserTransform );

	// Remember transform
	this->connectorsTransforms->append( connectorTransform );
	this->browsersTransforms->append( browserTransform );

	// Display template in webview
	webView->showTemplate( "metrics_template", models, templateType );
}

void BrowserGroup::addBrowser( const std::string& templateType, osg::Vec3 position, QMap<QString, int>* map )
{
	// qDebug() << "Adding browser";

	// Create webView
	osg::ref_ptr<OsgQtBrowser::QWebViewImage> webView = new OsgQtBrowser::QWebViewImage();

	// Webview position/offset (should have same aspect ratio as 800/600)
	float width = 240;
	float height = 180;
	float offset;

	// Create connectors targets depending on whether grouping is enabled & setup offset
	osg::Vec3Array* targets;
	if ( this->browsersGrouping ) {
		offset = 0;
		targets = new osg::Vec3Array( static_cast<unsigned int>( selectedNodes->size() ) );

		QLinkedList<osg::ref_ptr<Data::Node> >::iterator i;
		Data::Node* node;
		unsigned long pos = 0;

		// Iterate over each selected node and add its position to array
		for ( i = selectedNodes->begin(); i != selectedNodes->end(); ++i ) {
			node = *i;
			( *targets )[pos++].set( node->getCurrentPosition() );
		}

	}
	else {
		offset = 60;
		targets = new osg::Vec3Array( 1 );
		( *targets )[0].set( position );
	}

	// Setup browser pos
	osg::Vec3 bl = osg::Vec3( -width/2, -height/2, 0 ); // Bottom left
	osg::Vec3 br = osg::Vec3( width/2, -height/2, 0 );  // Bottom right
	osg::Vec3 tr = osg::Vec3( width/2,  height/2, 0 );  // Top right
	osg::Vec3 tl = osg::Vec3( -width/2,  height/2, 0 ); // Top left
	osg::Vec3 center = osg::Vec3(
						   position.x() + offset + width/2,
						   position.y() + offset + height/2,
						   position.z()
					   );

	osgWidget::GeometryHints hints( bl,
									osg::Vec3( width, 0,  0 ),
									osg::Vec3( 0,  width, 0 ), // Needs more experimenting
									osg::Vec4( 0,  0,  0, 0 ) );

	osg::ref_ptr<osgWidget::Browser> browser = new osgWidget::Browser;
	browser->assign( webView, hints );

	// Wrap connectors to transform
	osg::ref_ptr<osg::AutoTransform> connectorTransform = new osg::AutoTransform;
	connectorTransform->addChild( this->createConnectorsGeode( center, targets ) ); // Add connecting lines geode

	// Wrap browser and border to transform
	osg::ref_ptr<osg::AutoTransform> browserTransform = new osg::AutoTransform;
	browserTransform->setPosition( center );
	browserTransform->setAutoRotateMode( osg::AutoTransform::ROTATE_TO_SCREEN );
	browserTransform->addChild( browser );
	browserTransform->addChild( this->createBorderGeode( bl, br, tr, tl ) ); // Add border geode
//	browserTransform->getOrCreateStateSet()->setMode();

	// Set initial scale & set animation start frame to help us calculate interpolation value
//	connectorTransform->setScale( 0 ); Disabled since its position is relative to graph, not current node
//	connectorTransform->setUserValue( "frame", 0 );
	browserTransform->setScale( 0 );
	browserTransform->setUserValue( "frame", 0 );

	// Add transform to group
	this->group->addChild( connectorTransform );
	this->group->addChild( browserTransform );

	// Remember transform
	this->connectorsTransforms->append( connectorTransform );
	this->browsersTransforms->append( browserTransform );

	// Display template in webview
	webView->showGitTemplate( "metrics_template", templateType, map );
}

osg::Geode* BrowserGroup::createBorderGeode( osg::Vec3 bl, osg::Vec3 br, osg::Vec3 tr, osg::Vec3 tl )
{
	osg::Geometry* linesGeom = new osg::Geometry();
	osg::Vec3Array* vertices = new osg::Vec3Array( 4 );

	( *vertices )[0].set( bl );
	( *vertices )[1].set( br );
	( *vertices )[2].set( tr );
	( *vertices )[3].set( tl );

	linesGeom->setVertexArray( vertices );
	linesGeom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_LOOP,0,static_cast<int>( vertices->size() ) ) );

	return this->createLinesGeode( linesGeom );
}

osg::Geode* BrowserGroup::createConnectorsGeode( osg::Vec3 center, osg::Vec3Array* targets )
{
	osg::Geometry* linesGeom = new osg::Geometry();
	osg::Vec3Array* vertices = new osg::Vec3Array( static_cast<unsigned int>( targets->size() * 2 ) ); // 2 points for each line

	// Iterate over each target node and create corresponding connector line geometry
	for ( std::size_t i=0; i< targets->size(); i++ ) {
		( *vertices )[i*2  ].set( center );
		( *vertices )[i*2+1].set( targets->at( i ) );
	}

	linesGeom->setVertexArray( vertices );
	linesGeom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES,0,static_cast<int>( vertices->size() ) ) );

	return this->createLinesGeode( linesGeom );
}

osg::Geode* BrowserGroup::createLinesGeode( osg::Geometry* linesGeom )
{
	osg::Geode* linesGeode = new osg::Geode();

	// Set line width
	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth( 1.1f );

	// Modify state set
	osg::StateSet* stateSet = linesGeode->getOrCreateStateSet();
	stateSet->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateSet->setMode( GL_LINE_SMOOTH, osg::StateAttribute::ON );
	stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
//    stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON ); // Causing issues with overlying lines & labels
	stateSet->setAttributeAndModes( linewidth, osg::StateAttribute::ON );

	// Set geometry color
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back( osg::Vec4( 1.0f,0.0f,0.0f,1.0f ) ); // config
#ifdef BIND_PER_PRIMITIVE
	linesGeom->setColorArray( colors, osg::Array::BIND_OVERALL );
#else
	linesGeom->setColorArray( colors );
#endif

	// Add geometry to geode
	linesGeode->addDrawable( linesGeom );

	return linesGeode;
}

void BrowserGroup::clearBrowsers()
{
	this->group->removeChildren( 0, this->group->getNumChildren() );
	this->browsersTransforms->clear();
	this->connectorsTransforms->clear();
}

void BrowserGroup::clearModels()
{
	this->selectedNodes->clear();

	// TODO make sure no memory is leaking ...
	// UPDATE: fix memory leaking ....
}

double BrowserGroup::interpolate( long currentFrame, long endFrame, double startValue, double endValue )
{
	double value = endValue * ( pow( ( static_cast<double>( currentFrame ) / static_cast<double>( endFrame ) ) - 1.0, 5.0 ) + 1.0 + startValue );

	// std::cout << value << " " << currentFrame << " " << endFrame << " " << startValue << " " << endValue << endl;
	// std::cout << flush;

	return value;
}

void BrowserGroup::updateBrowsers()
{
	// Animate
	updateTransforms( this->browsersTransforms );
	//updateTransforms(this->connectorsTransforms);
}

void BrowserGroup::updateTransforms( QList<osg::ref_ptr<osg::AutoTransform> >* transforms )
{
	QList<osg::ref_ptr<osg::AutoTransform> >::iterator i;
	osg::ref_ptr<osg::AutoTransform> transform;
	int frame;

	// Interpolate each transform and scale it using interpolation function
	for ( i = transforms->begin(); i != transforms->end(); ++i ) {

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
