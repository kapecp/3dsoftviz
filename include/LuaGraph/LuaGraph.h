#ifndef LUAGRAPH_H
#define LUAGRAPH_H

#include "LuaGraph/LuaNode.h"
#include "LuaGraph/LuaEdge.h"
#include "LuaGraph/LuaIncidence.h"
#include "LuaTypes/LuaValueList.h"

#include <QMap>

namespace Lua {

class LuaGraphObserver;

/**
 * @class LuaGraph
 * @brief Representation of hzpergraph in c++
 *
 * \author Frantisek Nagy
 * \date 19.5.2014
 */
class LuaGraph
{
public:
	/**
	 * @brief getInstance as in singleton pattern
	 * @return single instance of hypergraph
	 */
	static LuaGraph* getInstance();

	/**
	 * @brief loadGraph load graph from Lua, must be returned from function getGraph
	 * @return representation of loaded hypergraph
	 */
	static LuaGraph* loadGraph();

	static LuaGraph* loadEvoGraph( QString repoFilepath );

	/**
	 * @brief destructor
	 */
	~LuaGraph();

	/**
	 * @brief getNodes getter for nodes
	 * @return nodes map with ids as keys and LuaNode values
	 */
	QMap<qlonglong, LuaNode*>* getNodes() const;

	/**
	 * @brief getEdges getter for edges
	 * @return edges map with ids as keys and LuaEdge values
	 */
	QMap<qlonglong, LuaEdge*>* getEdges() const;

	/**
	 * @brief getIncidences getter for incidences
	 * @return incidence map with ids as keys and LuaIncidence values
	 */
	QMap<qlonglong, LuaIncidence*>* getIncidences() const;

	/**
	 * @brief hasObserver check if graph has observer set
	 * @return true if graph has observer set
	 */
	static bool hasObserver();

	/**
	 * @brief getObserver getter for observer
	 * @return observer
	 */
	Lua::LuaGraphObserver* getObserver() const;

	/**
	 * @brief setObserver setter for observer
	 * @param value
	 */
	void setObserver( Lua::LuaGraphObserver* value );

	/**
	 * @brief clearGraph delete all nodes, edges and incidences from graph
	 */
	void clearGraph();

	/**
	 * @brief printGraph print graph in text form
	 */
	void printGraph();

	Lua::LuaNode* findNodeByLuaIdentifier( QString identifier );

	Lua::LuaEdge* findEdgeByLuaIdentifier( QString identifier );
private:
	/**
	 * @brief LuaGraph private constructor as in Singleton
	 */
	LuaGraph();

	/**
	 * @brief instance Singleton instance
	 */
	static LuaGraph* instance;

	/**
	 * @brief observer LuaGraphObserver to be notified
	 */
	Lua::LuaGraphObserver* observer;

	/**
	 * @brief nodes map of id => LuaNode
	 */
	QMap<qlonglong, LuaNode*>* nodes;

	/**
	 * @brief edges map of id => LuaEdge
	 */
	QMap<qlonglong, LuaEdge*>* edges;

	/**
	 * @brief incidences map of id => LuaIncidence
	 */
	QMap<qlonglong, LuaIncidence*>* incidences;



};

}

/**
 * @brief luaCallback function to be registered in Lua - for notyfying observer from Lua
 * @param params
 * @return
 */
Lua::LuaValueList luaCallback( const Lua::LuaValueList& params );

#endif // LUAGRAPH_H
