/**
*  GraphSerializer.h
*  Projekt 3DVisual
*/
#ifndef GRAPH_SER_DEF
#define GRAPH_SER_DEF 1

#include <vector>
#include <cstdlib>
#include <ctime>

#include "Data/Node.h"
#include "Data/Edge.h"
#include "Data/Type.h"

#include <QMap>
#include <QString>

typedef QMap<QString, long> MapSL;



namespace Data
{
	class Node;
	class Edge;
	/**
	*  \class GraphSerializer
	*  \brief Hypergraph serialzer
	*  \author Michal Paprcka 
	*  \date 29. 4. 2010
	*
	*	This class is obsolete and not used anymore. It should be functional after some rewriting though.
	*/
	class GraphSerializer
	{
	public:

		/**
		*  \fn public constructor  GraphSerializer(std::vector<Data::Node*> *in_nodes,std::vector<Data::Edge*> *in_edges,std::vector<Data::Type*> *in_types)
		*  \brief Creates serializer layer
		*  \param  in_nodes  serialized nodes storage
		*  \param  in_nodes  serialized edges storage
		*  \param  in_nodes  serialized types storage
		*/
		GraphSerializer(std::vector<Data::Node*> *in_nodes,std::vector<Data::Edge*> *in_edges,std::vector<Data::Type*> *in_types);

		/**
		*  \fn public destructor  ~GraphSerializer
		*  \brief
		*/
		~GraphSerializer(void);


		/**
		*  \fn public  addEntry
		*  \brief Serializes new graph entry
		*/
		void addEntry(QString edgeId, QString edgeData, QString incId, QString incData, QString nodeId, QString nodeData);

	private:
		std::vector<Data::Node*> *nodes;

		std::vector<Data::Edge*> *edges;

		std::vector<Data::Type*> *types;


		/**
		*  MapSL nodeIdMap
		*  \brief Node id map
		*/
		MapSL nodeIdMap;

		/**
		*  MapSL edgeIdMap
		*  \brief edge id map
		*/
		MapSL edgeIdMap;


		/**
		*  long nodeIdSeq
		*  \brief Node id sequence
		*/
		long nodeIdSeq;

		/**
		*  long edgeIdSeq
		*  \brief Edge id sequence
		*/
		long edgeIdSeq;


		/**
		*  \fn private  getRandomPosition
		*  \brief Returns random position in space
		*  \return osg::Vec3f random position in space
		*/
		osg::Vec3f getRandomPosition();

		/**
		*  \fn private  createNode
		*  \brief Creates new node with given id and data
		*  \return Data::Node * new node
		*/
		Data::Node* createNode(QString id, QString data);

		/**
		*  \fn private  edgeExists(Data::Edge * e)
		*  \brief Check edge existance
		*  \param  e  checked edge   
		*  \return bool True, if edge exists
		*/
		bool edgeExists(Data::Edge * e);
	};
}

#endif