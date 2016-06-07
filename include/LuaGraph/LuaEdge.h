#ifndef LUAEDGE_H
#define LUAEDGE_H

#include "LuaGraphObject.h"

#include <QList>

namespace Lua {

/**
 * \class LuaEdge
 * \brief Class for representing of hypergraph edge in c++
 *
 * \author Frantisek Nagy
 * \date 19.5.2014
 */
class LuaEdge : public LuaGraphObject
{
public:

	/**
	 * @brief addIncidence add incidence to edge
	 * @param id id of incidence to be added
	 */
	void addIncidence( qlonglong id );

	/**
	 * @brief getIncidences get list of incidences
	 * @return list of incidences' ids
	 */
	QList<qlonglong> getIncidences() const;

private:
	QList<qlonglong> incidences;
};

}

#endif // LUAEDGE_H
