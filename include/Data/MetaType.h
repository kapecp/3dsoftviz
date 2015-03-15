/*!
 * MetaType.h
 * Projekt 3DVisual
 */

#ifndef DATA_METATYPE_DEF
#define DATA_METATYPE_DEF 1


#include "Data/Type.h"

#include <QString>

namespace Data {
class Graph;
class GraphLayout;
class Type;


/**
		*  \class MetaType
		*  \brief MetaType is a special Type Object used as a type in meta-Nodes and meta-Edges
		*  \author Aurel Paulovic
		*  \date 29. 4. 2010
		*/
class MetaType : public Type
{
public:


	/**
		*  \fn inline public constructor  MetaType(qlonglong id, QString name, Data::Graph* graph, Data::GraphLayout* layout, QMap<QString, QString> * settings = 0):Data::Type(id,name,graph,settings)
		*  \brief Creates new MetaType object
		*  \param  id     ID of the MetaType
		*  \param  name     name of the MetaType
		*  \param  graph    Graph object to which the MetaType is assigned
		*  \param  layout    GraphLayout of the Graph to which the MetaType belongs
		*  \param  settings     settings of the MetaType
		*/
	//MetaType(qlonglong id, QString name, Data::Graph* graph, Data::GraphLayout* layout, QMap<QString, QString> * settings = 0):Data::Type(id,name,graph,settings) { this->layout = layout; this->meta = true; }
	MetaType( qlonglong id, QString name, Data::Graph* graph, Data::GraphLayout* layout, QMap<QString, QString>* settings = 0 );

	/**
		*  \fn public destructor  ~MetaType
		*  \brief Destroys MetaType object
		*/
	~MetaType( void );


	/**
		*  \fn inline public  getLayout
		*  \brief Returns GraphLayout to which the MetaType belongs
		*  \return Data::GraphLayout *
		*/
	Data::GraphLayout* getLayout()
	{
		return layout;
	}
private:

	/**
		*  Data::GraphLayout * layout
		*  \brief GraphLayout to which the MetaType belongs
		*/
	Data::GraphLayout* layout;
};
}
#endif
