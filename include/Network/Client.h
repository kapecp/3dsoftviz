/*
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include <QTcpSocket>
#include <QStringList>

namespace Network{

class Client : public QObject
{
    Q_OBJECT

    public:

        Client(QObject *parent=0);

        void ServerConnect(QString nick);

        void send_message(QString message);

    private slots:
        // This is a function we'll connect to a socket's readyRead()
        // signal, which tells us there's something to be read from the server.
        void readyRead();

        // This function gets called when the socket tells us it's connected.
        void connected();

    private:
        QString clientNick;

        // This is the socket that will let us communitate with the server.
        QTcpSocket *socket;
    };

}
