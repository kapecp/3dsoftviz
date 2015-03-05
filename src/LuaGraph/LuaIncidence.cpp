#include "LuaGraph/LuaIncidence.h"

void Lua::LuaIncidence::setEdgeNode( const qlonglong edgeId, const qlonglong nodeId )
{
	edgeNodePair.first = edgeId;
	edgeNodePair.second = nodeId;
}
QPair<qlonglong, qlonglong> Lua::LuaIncidence::getEdgeNodePair() const
{
	return edgeNodePair;
}
bool Lua::LuaIncidence::getOutGoing() const
{
	return outGoing;
}

void Lua::LuaIncidence::setOutGoing( bool value )
{
	outGoing = value;
}

bool Lua::LuaIncidence::getOriented() const
{
	return oriented;
}

void Lua::LuaIncidence::setOriented( bool value )
{
	oriented = value;
}


