#ifndef ABSNODE_H
#define ABSNODE_H


#include <QString>

namespace Data {
class Node;
class Graph;
class Type;


class AbsNode
{
public:

    AbsNode(void){};

    AbsNode( qlonglong id, QString name);

   // ~AbsNode( void );

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


#endif // ABSNODE_H
