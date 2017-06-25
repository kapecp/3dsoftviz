/*!
 * GraphLayout.cpp
 * Projekt 3DVisual
 */

#ifndef DATA_GRAPHLAYOUT_DEF
#define DATA_GRAPHLAYOUT_DEF 1

#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QtSql>

namespace Data {
class Graph;

/**
    *  \class GraphLayout
    *  \brief GraphLayout representing one layout of the Graph.
    *  \author Aurel Paulovic
    *  \date 29. 4. 2010
    */
class GraphLayout
{
private:

	/**
	    *  Data::Graph * graph
	    *  \brief Graph to which the GraphLayout belongs
	    */
	Data::Graph* graph;

	/**
	    *  qlonglong layout_id
	    *  \brief ID of the GraphLayout
	    */
	qlonglong layout_id;

	/**
	    *  QString name
	    *  \brief Name of the GraphLayout
	    */
	QString name;

	/**
	    *  QSqlDatabase * conn
	    *  \brief Connection to the database
	    */
	QSqlDatabase* conn;

	/**
	    *  bool inDB
	    *  \brief Flag, true if the GraphLayout is in database
	    */
	bool inDB;

	/**
	    *  QMap<QString,QString> * metaSettings
	    *  \brief MetaSettings of the GraphLayout
	    *
	    * MetaSettings are settings of the GraphLayout that are used by the data structure and application for layouting, etc. They do not hold any information intended for use by the user.
	    */
	QMap<QString,QString>* metaSettings;
public:

	/**
	    *  const QString META_NODE_TYPE
	    *  \brief Constant identifier of MetaType name used for meta-Nodes as identified by MetaSettings of the GraphLayout
	    */
	static const QString META_NODE_TYPE;

	/**
	    *  const QString MULTI_NODE_TYPE
	    *  \brief Constant identifier of MultiType name used for multi-Nodes as identified by MetaSettings of the GraphLayout
	    */
	static const QString MULTI_NODE_TYPE;

	/**
	    *  const QString SPLITTER_NODE_TYPE
	    *  \brief Constant identifier of SplitterType name used for splitter-Nodes (they are used for splitting edges) as identified by MetaSettings of the GraphLayout
	    */
	static const QString SPLITTER_NODE_TYPE;

	/**
	    *  const QString EDGE_TYPE
	    *  \brief Constant identifier of Type name used for Edges as identified by Settings of the GraphLayout
	    */
	static const QString EDGE_TYPE;

	/**
	    *  const QString META_EDGE_TYPE
	    *  \brief Constant identifier of MetaType name used for meta-Edges as identified by MetaSettings of the GraphLayout
	    */
	static const QString META_EDGE_TYPE;

	/**
	    *  const QString EDGE_PIECE_TYPE
	    *  \brief Constant identifier of MetaType name used for piece of edge as identified by MetaSettings of the GraphLayout
	    */
	static const QString EDGE_PIECE_TYPE;

	/**
	    *  const QString MULTI_EDGE_TYPE
	    *  \brief Constant identifier of MultiType name used for multi-Edges as identified by MetaSettings of the GraphLayout
	    */
	static const QString MULTI_EDGE_TYPE;

	/**
	    *  const QString NESTED_NODE_TYPE
	    *  \brief Constant identifier of MultiType name used for nested-nodes as identified by MetaSettings of the GraphLayout
	    */
	static const QString NESTED_NODE_TYPE;

	/**
	    *  const QString NESTED_EDGE_TYPE
	    *  \brief Constant identifier of MultiType name used for nested-edges as identified by MetaSettings of the GraphLayout
	    */
	static const QString NESTED_EDGE_TYPE;

	/**
	    *  const QString NESTED_META_EDGE_TYPE
	    *  \brief Constant identifier of MultiType name used for meta-nested-edges as identified by MetaSettings of the GraphLayout
	    */
	static const QString NESTED_META_EDGE_TYPE;

	/**
	    *  const QString HYPER_NODE_TYPE
	    *  \brief Constant identifier of HyperType name used for hyper nodes as identified by MetaSettings of the GraphLayout
	    */
	static const QString HYPER_NODE_TYPE;

	/**
		*  const QString HIERARCHY_EDGE_TYPE
		*  \brief Constant identifier of HierarchyType name used for hierarchy edges in moduleGraph by MetaSettings of the GraphLayout
		*/
	static const QString HIERARCHY_EDGE_TYPE;

	/**
		*  const QString ARC_EDGE_TYPE
		*  \brief Constant identifier of ArcType name used for arc edges in moduleGraph by MetaSettings of the GraphLayout
		*/
	static const QString ARC_EDGE_TYPE;

	static const QString RESTRICTION_NODE_TYPE;


	/**
	    *  const QString MERGED_NODE_TYPE
	    *  \brief Constant identifier of MergedType name used for merged-Nodes as identified by MetaSettings of the GraphLayout
	    */
	static const QString MERGED_NODE_TYPE;

	/**
	    *  const QString MERGED_EDGE_TYPE
	    *  \brief Constant identifier of MergedType name used for merged-Edges as identified by MetaSettings of the GraphLayout
	    */
	static const QString MERGED_EDGE_TYPE;

	/**
	    *  \fn public constructor  GraphLayout(qlonglong layout_id, Data::Graph* graph, QString name, QSqlDatabase* conn)
	    *  \brief Creates new GraphLayout object
	    *  \param   layout_id 		ID of the GraphLayout
	    *  \param   graph Graph to wchich the Graphlayout belongs
	    *  \param   name  name of the GraphLayout
	    *  \param   conn  connection to the database
	    */
	GraphLayout( qlonglong layout_id, Data::Graph* graph, QString name, QSqlDatabase* conn );

	/**
	    *  \fn public destructor  ~GraphLayout
	    *  \brief Destroys the GraphLayout object
	    */
	~GraphLayout( void );

	/**
	    *  \fn inline public  getName
	    *  \brief Returns name of the GraphLayout
	    *  \return QString name of the GraphLayout
	    */
	QString getName()
	{
		return name;
	}

	/**
	    *  \fn public  setName(QString name)
	    *  \brief Sets new name of the GraphLayout
	    *  \param       name     new name
	    *  \return QString resultant name of the GraphLayout
	    */
	QString setName( QString name );


	/**
	    *  \fn inline public  getGraph
	    *  \brief Returns Graph to which the GraphLayout belongs
	    *  \return Data::Graph * Graph object
	    */
	Data::Graph* getGraph()
	{
		return graph;
	}

	/**
	    *  \fn public  getGraphId
	    *  \brief Returns ID of the Graph to which the GraphLayout belongs
	    *
	    * This is a shortcut to GraphLayout->getGraph()->getId()
	    *
	    *  \return qlonglong ID of the Graph
	    */
	qlonglong getGraphId();


	/**
	    *  \fn inline public  getId
	    *  \brief Returns ID of the GraphLayout
	    *  \return qlonglong ID of the GraphLayout
	    */
	qlonglong getId()
	{
		return layout_id;
	}


	/**
	    *  \fn inline public  isInDB
	    *  \brief Returns true if the GraphLayout is already in database
	    *  \return bool true, if the GraphLayout is in database
	    */
	bool isInDB()
	{
		return inDB;
	}

	/**
	    *  \fn inline public  setIsInDB
	    *  \brief Sets this GraphLayout inDB flag to true, meaning that the GraphLayout is in database
	    */
	void setIsInDB()
	{
		inDB = true;
	}


	/**
	    *  \fn public constant  toString
	    *  \brief Returns human-readable string representing the GraphLayout
	    *  \return QString
	    */
	QString toString() const;


	/**
	    *  \fn public  loadMetaSettings
	    *  \brief Loads MetaSettings of the GraphLayout from database
	    *
	    * DUMMY - NOT IMPLEMENTED
	    *
	    *  \return bool true, if the MetaSettings were successfully loaded from database
	    */
	bool loadMetaSettings();

	/**
	    *  \fn public  setMetaSetting(QString key, QString value)
	    *  \brief Sets (replaces) new meta setting
	    *  \param     key     key of the setting
	    *  \param     value     value of the setting
	    */
	void setMetaSetting( QString key, QString value );

	/**
	    *  \fn public  getMetaSetting(QString key)
	    *  \brief Returns meta setting associated with the key
	    *  \param       key     key of the setting
	    *  \return QString value of the setting associated with the key
	    */
	QString getMetaSetting( QString key );

	/**
	    *  \fn public  getMetaSetting(QString key)
	    *  \brief Removes meta setting associated with the key
	    *  \param     key     key of the setting
	    */
	void removeMetaSetting( QString key );
};
}

#endif
