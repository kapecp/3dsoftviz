#include "Layout/RadialLayout.h"

#include "Layout/ShapeGetter_Sphere_AroundNode.h"
#include "Layout/ShapeGetter_Circle_ByThreeNodes.h"
#include "Layout/RestrictionsManager.h"

#include "Data/GraphSpanningTree.h"
#include "Data/Graph.h"
#include "Data/Node.h"

#include "QDebug"

namespace Layout {

//instance of static members
RadialLayout* RadialLayout::selectedRadialLayout = NULL;
int RadialLayout::lastLayerID = 0;

void RadialLayout::changeSize( float size )
//change size of radial layout
{
	RadialLayout::mSize = size;
	update();
}

void RadialLayout::select()
//select this radial layout
{
	selectedRadialLayout = this; //selectedRadialLayout is static member
}

void RadialLayout::unselect()
//unselect this radial layout
{
	if ( selectedRadialLayout == this ) { //selectedRadialLayout is static member
		selectedRadialLayout = NULL;
	}
}

RadialLayout::~RadialLayout()
//destructor
{
	unselectNodes();
	unselect();
}

RadialLayout* RadialLayout::getSelectedRadialLayout()
//selectedRadialLayout is static member
{
	return selectedRadialLayout;
}

void RadialLayout::changeCenterPosition( osg::Vec3f centerPosition )
//set center of radial layout
{
	mCenterPosition = centerPosition;
	update();
}

void RadialLayout::changeCenterNode( osg::ref_ptr<Data::Node> centerNode )
//set node from argument as center node
{
	mCenterNode = centerNode;
	update();
}

void RadialLayout::update()
{
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = mCurrentGraph->getNodes();

	//return, when inputs are incorrect
	if ( mSelectedNodes.empty() ) {
		return;
	}
	if ( mCenterNode == NULL ) {
		return;
	}

	Data::GraphSpanningTree* spanningTree = mCurrentGraph->getSpanningTree( mCenterNode->getId() ); //get spanning tree

	int maxDepth = spanningTree->getMaxDepth();
	if ( maxDepth == 0 ) {
		return;
	}

	mSphereCount = maxDepth; //sphere count is equal to max depth of spaning tree

	osg::Vec4 color( 0.0, 0.0, 0.0, 1.0 ); //center node is black
	osg::Vec4 d_color( static_cast<float>( 1.0/maxDepth ), static_cast<float>( 1.0/maxDepth ), 0.0, 1.0 );

	//temporary variables
	osg::ref_ptr<Data::Node> node;
	QList<qlonglong> groups;
	QList<qlonglong>::iterator groupIt;
	QList<qlonglong> nodes;
	QList<qlonglong>::iterator nodeIt;

	mCenterNode->setFixed( true );   //don't move with radial layout

	for ( int depth = 0; depth <= maxDepth; depth++ ) {
		float radius = static_cast<float>( depth )*mSize/ static_cast<float>( maxDepth ); //radius of sphere or circle
		groups = spanningTree->getGroupsInDepth( depth ); //groups are set of nodes with same parent
		QSet<Data::Node*>  pickedNodes; //nodes in same depth
		lastLayerID++; //because every layer need id, cause repulsive forces
		for ( groupIt=groups.begin(); groupIt!=groups.end(); ++groupIt ) { //iterating groups from current depth
			nodes = spanningTree->getNodesInGroup( *groupIt );
			for ( nodeIt=nodes.begin(); nodeIt!=nodes.end(); ++nodeIt ) { //iterating nodes from current group
				node = allNodes->value( *nodeIt );

				if ( mSelectedNodes.contains( node ) ) { //set attributes of current node
					node->setColor( color ); //for debugging
					node->setLayerID( lastLayerID ); //because repulsive forces
					node->setRadialLayout( this ); //because repulsive forces
					pickedNodes.insert( node );
				}
			}
		}
		Layout::ShapeGetter*  sphereSurfaceGetter;
		if ( m2D == false ) { //sphere surface constrain, if mode is 3D
			sphereSurfaceGetter = new Layout::ShapeGetter_Sphere_AroundNode( mCenterNode, radius, Shape_Sphere::SURFACE, ShapeGetter_Sphere_AroundNode::NODE_TARGET_POSITION );
		}
		else {
			//because circle constrain is defined by tree nodes
			osg::Vec3f positionNode1 = mCenterPosition;
			osg::Vec3f positionNode2 = mCenterPosition + osg::Vec3f( radius, 0.f, 0.f );
			osg::Vec3f positionNode3 = mCenterPosition + osg::Vec3f( 0.f, radius, 0.f );

			mNode1 = mCurrentGraph->addRestrictionNode( "plane_node_1", positionNode1 );
			mNode2 = mCurrentGraph->addRestrictionNode( "plane_node_2", positionNode2 );
			mNode3 = mCurrentGraph->addRestrictionNode( "plane_node_2", positionNode3 );

			mNode1->setInvisible( true );
			mNode2->setInvisible( true );
			mNode3->setInvisible( true );

			sphereSurfaceGetter = new Layout::ShapeGetter_Circle_ByThreeNodes( mNode1, mNode2, mNode3 );
		}

		QSharedPointer <Layout::ShapeGetter> shapeGetter( sphereSurfaceGetter );
		QSharedPointer<Layout::Shape> shape = sphereSurfaceGetter->getShape();
		//try cast to sphere shape or circle shape
		QSharedPointer<Layout::Shape_Sphere> sphere = shape.dynamicCast<Shape_Sphere>();
		QSharedPointer<Layout::Shape_Intersection> circle = shape.dynamicCast<Shape_Intersection>();
		if ( sphere != NULL ) { //shape is sphere
			sphere->setRenderType( mRenderType );
			if ( static_cast<float>( depth ) / static_cast<float>( maxDepth ) <= mVisibleSpheres && mVisibleSpheres > 0.0 ) {
				sphere->setAlpha( mAlpha );
			}
			else {
				sphere->setAlpha( 0 );
			}
		}
		if ( circle != NULL ) { //shape is circle
			circle->setRenderType( mRenderType );
			if ( static_cast<float>( depth )/static_cast<float>( maxDepth ) <= mVisibleSpheres && mVisibleSpheres > 0.0 ) {
				circle->setAlpha( mAlpha );
			}
			else {
				circle->setAlpha( 0 );
			}
		}
		mCurrentGraph->getRestrictionsManager().setRestrictions( pickedNodes, shapeGetter ); //apply restriction
		//for next iteration
		pickedNodes.clear();
		groups.clear();
		color += d_color;
	}


}

osg::Vec3f RadialLayout::getCenterPosition()
//get center position of radial layout
{
	return mCenterPosition;
}

osg::ref_ptr<Data::Node> RadialLayout::getCenterNode()
{
	return mCenterNode;
}

qlonglong RadialLayout::getSpeheCount()
//get radial layout depth
{
	return mSphereCount;
}

RadialLayout::RadialLayout( Data::Graph* currentGraph, QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, float size, osg::ref_ptr<Data::Node> centerNode, osg::Vec3f centerPosition )
{
	for ( QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator it = selectedNodes->constBegin(); it != selectedNodes->constEnd(); ++it ) {
		mSelectedNodes.insert( it->get() );
		//qDebug()<<"inserting: "<<it->get();
	}
	mSize = size;
	mCenterNode = centerNode;
	mCenterPosition = centerPosition;
	mCurrentGraph = currentGraph;

	//default values
	mVisibleSpheres = 1.0;
	m2D = false;
	mForceScale = 1000.0;
	mForceSphereScale = 1000.0;
	mAlpha = 0.04f;
	mRenderType = Shape_AbstractSphere::WIREFRAME;


	update();
}

void RadialLayout::setAlpha( float alpha )
{
	mAlpha = alpha;
	update();
}

float RadialLayout::getAlpha()
{
	return mAlpha;
}

void RadialLayout::setVisibleSpheres( float visibleSpheres )
//visibleSpheres real number in interval 0.0 - 1.0
//  0.0 all spheres are invisible
//  1.0 all spheres are visible
{
	mVisibleSpheres = visibleSpheres;
	update();
}

float RadialLayout::getVisibleSpheres()
{
	return mVisibleSpheres;
}

void RadialLayout::unselectNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes )
{
	//because default color of node is white and radial layout change this color
	osg::Vec4 color( 1.0, 1.0, 1.0, 1.0 );
	osg::ref_ptr<Data::Node> node;

	//unselect nodes from argument
	for ( QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator it = selectedNodes->constBegin(); it != selectedNodes->constEnd(); ++it ) {
		QSet<Data::Node*>::iterator sit = qFind( mSelectedNodes.begin(), mSelectedNodes.end(), it->get() );
		if ( sit != mSelectedNodes.constEnd() ) { //check if node is already erased from radial layout
			//restore values of nodes
			node = it->get();
			node->setColor( color );
			node->setLayerID( 0 );
			node->setRadialLayout( NULL );
			if ( node == mCenterNode ) {
				node->setFixed( false );
			}
			mSelectedNodes.erase( sit );
		}

	}
}


void RadialLayout::unselectNodes() //unselect all nodes
{
	osg::Vec4 color( 1.0, 1.0, 1.0, 1.0 );
	osg::ref_ptr<Data::Node> node;
	for ( QSet<Data::Node*>::iterator it = mSelectedNodes.begin(); it != mSelectedNodes.end(); it = mSelectedNodes.erase( it ) ) {
		node = *it;
		//restrore valies
		node->setColor( color );
		node->setLayerID( 0 );
		node->setRadialLayout( NULL );
	}
	if ( mCenterNode != NULL ) {
		mCenterNode->setFixed( false );
	}

}

void RadialLayout::setForceScale( float forceScale )
//set multiplier of repulsive forces between two nodes, which are in same layer
{
	mForceScale = forceScale;
}

float RadialLayout::getForceScale()
//get multiplier of repulsive forces between two nodes, which are in same layer
{
	return mForceScale;
}

void RadialLayout::setForceSphereScale( float forceSphereScale )
//set multiplier of repulsive forces between two nodes, which are not in same layer
{
	mForceSphereScale = forceSphereScale;
}

float RadialLayout::getForceSphereScale()
//get multiplier of repulsive forces between two nodes, which are in same layer
{
	return mForceSphereScale;
}

void RadialLayout::setRenderType( Shape_AbstractSphere::RenderType RenderType )
{
	mRenderType = RenderType;
	update();
}

Shape_AbstractSphere::RenderType RadialLayout::getRenderType()
{
	return mRenderType;
}

void RadialLayout::changeRenderType()
{
	switch ( mRenderType ) {
		case Shape_AbstractSphere::WIREFRAME:
			mRenderType = Shape_AbstractSphere::SOLID;
			break;
		case Shape_AbstractSphere::SOLID:
			mRenderType = Shape_AbstractSphere::WIREFRAME;
			break;
		case Shape_AbstractSphere::CIRCLE:  //not implemented yet
			break;
		default:
			break;
	}
	update();
}

void RadialLayout::changeMode()
//2d mode is set of circles constraints
//3d is set of sphere constraints
{
	m2D = !m2D;
	update();
}

} // namespace Layout
