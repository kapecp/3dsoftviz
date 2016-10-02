#include "Network/executors/SendGraphExecutor.h"
#include "Network/Server.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
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
