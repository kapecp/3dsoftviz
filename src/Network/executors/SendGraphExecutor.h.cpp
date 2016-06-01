#include "Network/executors/SendGraphExecutor.h"
#include "Network/Server.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"

namespace Network {


void SendGraphExecutor::execute_server()
{

	Server* server = Server::getInstance();
	QTcpSocket* senderClient = ( QTcpSocket* ) stream->device();
	server->sendGraph( senderClient );

}

void SendGraphExecutor::execute_client()
{
	return;
}

} // namespace Network

#pragma GCC diagnostic pop
