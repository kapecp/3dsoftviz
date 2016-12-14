#include "Network/executors/SetRestrictionExecutor.h"
#include "Manager/Manager.h"
#include "Layout/RestrictionRemovalHandler_RestrictionNodesRemover.h"
#include "Layout/ShapeGetter_SphereSurface_ByTwoNodes.h"
#include "Layout/ShapeGetter_Sphere_ByTwoNodes.h"
#include "Layout/ShapeGetter_Plane_ByThreeNodes.h"
#include "Network/Server.h"
#include "Network/Client.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

void SetRestrictionExecutor::execute_client()
{
	int id;
	float x,y,z;
	QString name;

	osg::ref_ptr<Data::Node> node1;
	osg::ref_ptr<Data::Node> node2;
	osg::ref_ptr<Data::Node> node3;

	quint8 type;
	*stream >> type;

	*stream >> id >> name >> x >> y >> z;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	node1 = currentGraph->addRestrictionNode( name, osg::Vec3f( x,y,z ), id );

	*stream >> id >> name >> x >> y >> z;
	node2 = currentGraph->addRestrictionNode( name, osg::Vec3f( x,y,z ), id );

	if ( type > 2 ) {
		*stream >> id >> name >> x >> y >> z;
		node3 = currentGraph->addRestrictionNode( name, osg::Vec3f( x,y,z ), id );
	}

	Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;
	restrictionNodes.push_back( node1 );
	restrictionNodes.push_back( node2 );

	Client* client = Client::getInstance();
	QOSG::CoreWindow* cw = ( QOSG::CoreWindow* ) client->getCoreWindowReference();

	QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict = new QLinkedList<osg::ref_ptr<Data::Node> >();

	int count;

	*stream >> count;

	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph->getNodes();
	for ( int i = 0; i < count; i++ ) {
		*stream >> id;
		if ( nodes->contains( id ) ) {
			nodesToRestrict->append( *nodes->find( id ) );
		}
	}

	Layout::ShapeGetter* shapeGetter;

	switch ( type ) {
		case 1:
			shapeGetter = new Layout::ShapeGetter_SphereSurface_ByTwoNodes( node1, node2 );
			break;
		case 2:
			shapeGetter = new Layout::ShapeGetter_Sphere_ByTwoNodes( node1, node2 );
			break;
		case 3:
			shapeGetter = new Layout::ShapeGetter_Plane_ByThreeNodes( node1, node2, node3 );
			break;
		default:
			delete nodesToRestrict;
			return;
			break;
	}

	cw->setRestrictionToSelectedNodes(
		QSharedPointer<Layout::ShapeGetter> (
			shapeGetter
		),
		currentGraph,
		QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
			new Layout::RestrictionRemovalHandler_RestrictionNodesRemover(
				*currentGraph,
				restrictionNodes
			)
		),
		nodesToRestrict
	);

}

void SetRestrictionExecutor::execute_server()
{
	int id;
	float x,y,z;
	QString name;

	osg::ref_ptr<Data::Node> node1;
	osg::ref_ptr<Data::Node> node2;
	osg::ref_ptr<Data::Node> node3;

	osg::Vec3f position_node1;
	osg::Vec3f position_node2;
	osg::Vec3f* position_node3 = NULL;

	quint8 type;
	*stream >> type;

	*stream >> name >> x >> y >> z;
	position_node1 = osg::Vec3f( x,y,z );
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	node1 = currentGraph->addRestrictionNode( name, position_node1 );

	*stream >> name >> x >> y >> z;
	position_node2 = osg::Vec3f( x,y,z );
	node2 = currentGraph->addRestrictionNode( name, position_node2 );

	if ( type > 2 ) {
		*stream >> name >> x >> y >> z;
		position_node3 = new osg::Vec3f( x,y,z );
		node3 = currentGraph->addRestrictionNode( name, *position_node3 );
	}

	Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;
	restrictionNodes.push_back( node1 );
	restrictionNodes.push_back( node2 );

	Server* server = Server::getInstance();
	QOSG::CoreWindow* cw = ( QOSG::CoreWindow* ) server->getCoreWindowReference();

	QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict = new QLinkedList<osg::ref_ptr<Data::Node> >();

	int count;

	*stream >> count;

	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph->getNodes();
	for ( int i = 0; i < count; i++ ) {
		*stream >> id;
		if ( nodes->contains( id ) ) {
			nodesToRestrict->append( *nodes->find( id ) );
		}
	}

	Layout::ShapeGetter* shapeGetter;

	switch ( type ) {
		case 1:
			shapeGetter = new Layout::ShapeGetter_SphereSurface_ByTwoNodes( node1, node2 );
			break;
		case 2:
			shapeGetter = new Layout::ShapeGetter_Sphere_ByTwoNodes( node1, node2 );
			break;
		case 3:
			shapeGetter = new Layout::ShapeGetter_Plane_ByThreeNodes( node1, node2, node3 );
			break;
		default:
			delete nodesToRestrict;
			return;
			break;
	}

	cw->setRestrictionToSelectedNodes(
		QSharedPointer<Layout::ShapeGetter> (
			shapeGetter
		),
		currentGraph,
		QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
			new Layout::RestrictionRemovalHandler_RestrictionNodesRemover(
				*currentGraph,
				restrictionNodes
			)
		),
		nodesToRestrict
	);

	server->sendSetRestriction( type, node1, position_node1, node2, position_node2, nodesToRestrict, node3, position_node3 );
}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
