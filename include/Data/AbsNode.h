#ifndef ABSNODE_H
#define ABSNODE_H

#include <QMap>
#include <QString>

namespace Data {
class Node;
class Graph;
class Type;


class AbsNode
{
public:

	AbsNode( qlonglong id, QString name, Data::Type* type,Data::Graph* graph );

	// ~AbsNode( void );

	qlonglong getId() const
	{
		return id;
	}

	/**
	    *  \fn inline public constant  getName
	    *  \brief Returns name of the Node
	    *  \return QString name of the Node
	    */
	QString getName() const
	{
		return name;
	}

	void setId( qlonglong id )
	{
		this->id=id;
	}

	void setName( QString name )
	{
		this->name=name;
	}

	/**
	    *  \fn inline public constant  getType
	    *  \brief Returns Type of the Node
	    *  \return Data::Type * Type of the Node
	    */
	Data::Type* getType() const
	{
		return type;
	}

	/**
	    *  \fn inline public  setType(Data::Type* val)
	    *  \brief Sets new Type of the Node
	    *  \param   val  new Type
	    */
	void setType( Data::Type* val )
	{
		type = val;
	}

	/**
	    *  \fn inline public  getGraph
	    *  \brief Returns Graph to which the Nodes belongs
	    *  \return Data::Graph * Graph
	    */
	Data::Graph* getGraph()
	{
		return graph;
	}

	/**
	    *  \fn inline public constant  getSettings
	    *  \brief Returns settings of the Node
	    *  \return QMap<QString,QString> * settings of the Node
	    */
	QMap<QString, QString>* getSettings() const
	{
		return settings;
	}

	/**
	    *  \fn inline public  setSettings(QMap<QString, QString> * val)
	    *  \brief Sets (overrides) new settings of the Node
	    *  \param   val    new settings
	    */
	void setSettings( QMap<QString, QString>* val )
	{
		settings = val;
	}

	bool isParentNode()
	{
		return hasNestedNodes;
	}

	void setAsParentNode()
	{
		hasNestedNodes = true;
	}

	float getNodeBetweeness()
	{
		return nodeBetweeness;
	}

	void setNodeBetweeness( float nodeBetweeness )
	{
		this->nodeBetweeness = nodeBetweeness;
	}

	float getNodeCloseness()
	{
		return nodeCloseness;
	}

	void setNodeCloseness( float nodeCloseness )
	{
		this->nodeCloseness = nodeCloseness;
	}

	float getNodeDegree()
	{
		return nodeDegree;
	}

	void setNodeDegree( float nodeDegree )
	{
		this->nodeDegree = nodeDegree;
	}

	bool isNodeMatched()
	{
		return nodeMatched;
	}
	void setNodeMatched( bool nodeMatched )
	{
		this->nodeMatched = nodeMatched;
	}

	void setRemovableByUser( bool b )
	{
		removableByUser = b;
	}

	bool isRemovableByUser()
	{
		return removableByUser;
	}

	/**
	    *  \fn public  equals(Node* node)
	    *  \brief Checks if the node and this object is the same object
	    *  \param   node     Node to compare
	    *  \return bool true, if this object and node are the same object
	    */
	bool equals( AbsNode* node );

	float nodeBetweeness;
	float nodeCloseness;
	float nodeDegree;
	bool nodeMatched;

	/**
	     * \brief If the node is removable by the user (using GUI).
	     * Used to make some nodes not removable (e.g. nodes used for manipulating layout restrictions - their
	     * existence is bound to the existence of the layout restriction and their removal is managed
	     * elsewhere, so they can not be removed by user).
	     */
	bool removableByUser;

protected:
	qlonglong id;

	QString name;

	/**
	    *  Data::Type * type
	    *  \brief Type of the Node
	    */
	Data::Type* type;

	/**
	    *  Data::Graph * graph
	    *  \brief Graph object to which the Node belongs
	    */
	Data::Graph* graph;

	/**
	    *  QMap<QString,QString> * settings
	    *  \brief Settings of the Node
	    */
	QMap<QString, QString>* settings;

	bool hasNestedNodes;

private:

};
}


#endif // ABSNODE_H
