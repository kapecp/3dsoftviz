#include "Mouse3d/Connector.h"
#include <QDebug>
Mouse3d::Connector::Connector(){

}

Mouse3d::Connector::~Connector(){

}

/* @author Michal Fašánek
 * @date 22.11.2015
 * @brief Called to detect a device and start listening thread
 */
void Mouse3d::Connector::ConnectMouse3d(){

    qDebug() << "Create connection call - Connector";
    LibMouse3d::ConnectionInterface conInt = LibMouse3d::ConnectionInterface();
    conInt.CreateConnection();

}
