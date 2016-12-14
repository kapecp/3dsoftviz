#include "Network/executors/SendGraphExecutor.h"
#include "Network/Server.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

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

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
