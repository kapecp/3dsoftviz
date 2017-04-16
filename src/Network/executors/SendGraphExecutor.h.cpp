#include "Network/executors/SendGraphExecutor.h"
#include "Network/Server.h"

#include <leathers/push>
#include <leathers/useless-cast>
#include <leathers/old-style-cast>
#include <leathers/sign-conversion>

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

#include <leathers/pop>
