#ifndef LUANODE_H
#define LUANODE_H

#include "LuaGraphObject.h"

#include <QList>

namespace Lua {

/**
 * \class LuaNode
 * \brief Class for representing of hypergraph node in c++
 *
 * \author Frantisek Nagy
 * \date 19.5.2014
 */
class LuaNode : public LuaGraphObject
{
public:
	/**
	 * @brief addIncidence add incidence to edge
	 * @param id id of incidence to be added
	 */
	void addIncidence( const qlonglong id );

	/**
	 * @brief getIncidences get list of incidences
	 * @return list of incidences' ids
	 */
	QList<qlonglong> getIncidences() const;

private:
	QList<qlonglong> incidences;

};

}

#endif // LUANODE_H
