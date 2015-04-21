#ifndef ABSEDGE_H
#define ABSEDGE_H

#include <QString>

namespace Data {
class Node;
class Graph;
class Type;


class AbsEdge
{
public:

    AbsEdge(void){};

    AbsEdge( qlonglong id, QString name);

    qlonglong getId() const
    {
        return id;
    }

    QString getName() const
    {
        return name;
    }

    void setId(qlonglong id)
    {
        this->id=id;
    }

    void setName(QString name)
    {
        this->name=name;
    }

protected:
    qlonglong id;
    QString name;

private:

};
}
#endif // ABSEDGE_H
