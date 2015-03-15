#include "LuaGraph/LuaNode.h"

void Lua::LuaNode::addIncidence( const qlonglong id )
{
	incidences.append( id );
}

QList<qlonglong> Lua::LuaNode::getIncidences() const
{
	return incidences;
}
