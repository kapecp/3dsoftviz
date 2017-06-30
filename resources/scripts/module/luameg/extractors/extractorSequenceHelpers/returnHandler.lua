local luadb = require 'luadb.hypergraph'

local function insertReturnNode(hypergraph, returnAstNode, methodSourceNode)

	-- create new luadb node, setup with proper values and insert into hypergraph
	local newReturnNode = luadb.node.new()
	newReturnNode.meta = newReturnNode.meta or {}
	newReturnNode.meta.type = "return"
	newReturnNode.data.name = "return"
	newReturnNode.data.astNodeId = returnAstNode.nodeid

	hypergraph:addNode(newReturnNode)

  -- create new edge to connect method node with newly created return node
  local edge = luadb.edge.new()
  edge.label = "executes"
  edge:setSource(methodSourceNode)
  edge:setTarget(newReturnNode)
  edge:setAsOriented()

  hypergraph:addEdge(edge)
  -- edge between method and return node creation end

  return hypergraph
end

return {
	insertReturnNode = insertReturnNode
}