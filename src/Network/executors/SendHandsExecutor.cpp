#include "Network/executors/SendHandsExecutor.h"
#include "Network/Server.h"
#include "Network/Client.h"

#include <leathers/push>
#include <leathers/useless-cast>
#include <leathers/old-style-cast>
#include <leathers/sign-conversion>

namespace Network {


void SendHandsExecutor::execute_server()
{
	return;
}

void SendHandsExecutor::execute_client()
{
	Client* client = Client::getInstance();
	QTcpSocket* senderClient = ( QTcpSocket* ) stream->device();
	client->updateHands( senderClient, stream );
}

} // namespace Network

#include <leathers/pop>
