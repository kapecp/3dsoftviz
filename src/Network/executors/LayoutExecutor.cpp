#include "Network/executors/LayoutExecutor.h"
#include "Manager/Manager.h"

#include "Network/Client.h"
#include "Network/Server.h"

namespace Network {

void LayoutExecutor::execute_client()
{

	int id;

	float x,y,z;

	*stream >> id >> x >> y >> z;

	Client* client = Client::getInstance();
	if ( client->ignoreLayout() ) {
		return;
	}

//	client->thread->pauseAllAlg();

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	if ( currentGraph != NULL ) {
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
		if ( nodes != NULL ) {
			if ( nodes->contains( id ) ) {
				osg::ref_ptr<Data::Node> node = *nodes->find( id );
				if ( !client->selected_nodes.contains( node ) ) {
					node->setTargetPosition( osg::Vec3( x,y,z ) );
					//qDebug()<< "[NEW NODE POS] id: " << id << " [" << x << "," << y << "," << z << "]";
				}
			}
		}
	}
}

void LayoutExecutor::execute_server()
{
	return;
}

} // namespace Network
