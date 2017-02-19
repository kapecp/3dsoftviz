#include <QStringList>

#include "Network/executors/UsersExecutor.h"
#include "Network/Client.h"
#include "QOSG/CoreWindow.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

void UsersExecutor::execute_client()
{

	Client* client = Client::getInstance();

	QString users_string;
	*stream >> users_string;

	QStringList users = users_string.split( "," );

	QList<int> newClients;

	foreach ( QString user, users ) {
		QStringList args = user.split( "=" );
		int id = args[0].toInt();
		QString nick = args[1];
		newClients << id;

		client->addClient( id,nick );
	}

	//delete disconnected users
	QMap<int, QString>::iterator i = client->userList.begin();
	while ( i != client->userList.end() ) {
		if ( ( !newClients.contains( i.key() ) && i.key() != 0 ) ) {
			if ( i.key() == client->userToSpy() ) {
				QOSG::CoreWindow* cw = ( QOSG::CoreWindow* ) client->getCoreWindowReference();
				cw->chb_spy->setChecked( false );
				client->unSpyUser();
			}
			if ( i.key() == client->getCenterUser() ) {
				QOSG::CoreWindow* cw = ( QOSG::CoreWindow* ) client->getCoreWindowReference();
				cw->chb_center->setChecked( false );
				client->unCenterUser();
			}
			client->removeAvatar( i.key() );
			i = client->userList.erase( i );
		}
		else {
			++i;
		}
	}

	client->updateUserList();
	client->sendMyView();
}

void UsersExecutor::execute_server()
{
	return;
}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
