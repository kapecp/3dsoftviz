#ifndef LUAINCIDENCE_H
#define LUAINCIDENCE_H

#include "LuaGraphObject.h"

#include <QPair>

namespace Lua {

/**
 * \class LuaIncidence
 * \brief Class for representing of hypergraph incidence in c++
 *
 * \author Frantisek Nagy
 * \date 19.5.2014
 */
class LuaIncidence : public LuaGraphObject
{
public:

	/**
	 * @brief setEdgeNode set edge and node for incidence
	 * @param edgeId id of edge
	 * @param nodeId id of incidence
	 */
	void setEdgeNode( const qlonglong edgeId, const qlonglong nodeId );

	/**
	 * @brief getEdgeNodePair getter for edge an node
	 * @return pair with edge and node ids
	 */
	QPair<qlonglong, qlonglong> getEdgeNodePair() const;

	/**
	 * @brief getOriented getter if the incidence is oriented
	 * @return oriented
	 */
	bool getOriented() const;

	/**
	 * @brief setOriented setter for oriented
	 * @param value oriented
	 */
	void setOriented( bool value );

	/**
	 * @brief getOutGoing getter if the incidence is outgoing from edge
	 * @return outGoing
	 */
	bool getOutGoing() const;

	/**
	 * @brief setOutGoing setter for outgoing
	 * @param value outGoing
	 */
	void setOutGoing( bool value );

private:
	QPair<qlonglong, qlonglong> edgeNodePair;
	bool oriented;
	bool outGoing;
};

}

#endif // LUAINCIDENCE_H
