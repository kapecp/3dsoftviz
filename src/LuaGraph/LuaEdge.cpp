#include "LuaGraph/LuaEdge.h"

void Lua::LuaEdge::addIncidence( qlonglong id )
{
	incidences.append( id );
}

QList<qlonglong> Lua::LuaEdge::getIncidences() const
{
	return incidences;
}
