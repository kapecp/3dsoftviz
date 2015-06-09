#ifndef ABSEDGE_H
#define ABSEDGE_H

#include <QString>

#include "Data/Type.h"

namespace Data {
class Node;
class Graph;
class Type;


class AbsEdge
{
public:

	AbsEdge( void ) {};

	AbsEdge( qlonglong id, QString name, Data::Graph* graph, bool isOriented, Data::Type* type );

	qlonglong getId() const
	{
		return id;
	}

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
	    *  \fn inline public  getGraph
	    *  \brief Returns the Graph to which is the Edge assigned
	    *  \return Data::Graph * Edge's Graph
	    */
	Data::Graph* getGraph()
	{
		return graph;
	}

	/**
	    * \fn inline public constant isOriented
	    * \brief Returns if the Edge is oriented or not
	    * \return bool true, if the Edge is oriented
	    */
	bool isOriented() const
	{
		return oriented;
	}

	/**
	    * \fn inline public setOriented(bool val)
	    * \brief  Sets if the Edge is oriented
	    * \param val 		true, if the Edge is oriented
	    */
	void setOriented( bool val )
	{
		oriented = val;
	}

	/**
	    * \fn inline public constant getType
	    * \brief  Returns the Type of the Edge
	    * \return Data::Type * Type of the Edge
	    */
	Data::Type* getType() const
	{
		return type;
	}

	/**
	    * \fn void setType(Data::Type* val)
	    * \brief  Sets new Type of the Edge
	    *
	    * OBSOLETE - DO NOT USE IT
	    *
	    * \param  val new Type of the Edge
	    */
	void setType( Data::Type* val )
	{
		type = val;
	}

	/**
	    *  \fn inline public constant  getLength
	    *  \brief Returns length of the Edge
	    *  \return float length of the Edge
	    */

	double getLength() const
	{
		return length;
	}

	float getEdgeStrength() const
	{
		return edgeStrength;
	}

	void setEdgeStrength( float value )
	{
		edgeStrength = value;
	}

	int getWeight()
	{
		return 1;
	}

	/**
	    *  \fn inline public constant  isSelected
	    *  \brief Returns selected flag
	    *  \return bool true, if the Edge is selected
	    */
	bool isSelected() const
	{
		return selected;
	}

	/**
	    *  \fn inline public  setSelected(bool val)
	    *  \brief Sets the selected flag of the Edge
	    *  \param       val   true, if the Edge is selected
	    */
	void setSelected( bool val )
	{
		selected = val;
	}

	/**
	    *  \fn inline public constant  getSettings
	    *  \brief Returns current settings of the Edge
	    *  \return QMap<QString,QString> * settings of the Edge
	    */
	QMap<QString, QString>* getSettings() const
	{
		return settings;
	}

	/**
	    *  \fn inline public  setSettings(QMap<QString, QString> * val)
	    *  \brief Sets new settings to the Edge
	    *  \param [in, out]  val QMap<QString,QString> *    new settings
	    */
	void setSettings( QMap<QString, QString>* val )
	{
		settings = val;
	}

protected:
	qlonglong id;

	QString name;

	/**
	    *  Data::Graph * graph
	    *  \brief Graph to which the Edge belongs
	    */
	Data::Graph* graph;

	/**
	    *  bool oriented
	    *  \brief True, if the Edge is oriented
	    */
	bool oriented;

	/**
	    *  Data::Type * type
	    *  \brief Type of the Edge
	    */
	Data::Type* type;

	/**
	    *  float length
	    *  \brief Length of the Edge
	    */
	double length;

	float edgeStrength;

	/**
	    *  bool selected
	    *  \brief True, if edge is picked
	    */
	bool selected;

	/**
	    *  QMap<QString,QString> * settings
	    *  \brief Settings of the Egde
	    */
	QMap<QString, QString>* settings;

private:

};
}
#endif // ABSEDGE_H
