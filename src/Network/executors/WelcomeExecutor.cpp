#include "Network/executors/WelcomeExecutor.h"
#include "Network/Client.h"

namespace Network {

void WelcomeExecutor::execute_client()
{

	Client* client = Client::getInstance();
	int id;
	*stream >> id;
	client->setMyId( id );
	client->requestGraph();

}

void WelcomeExecutor::execute_server()
{
	return;
}

} // namespace Network
