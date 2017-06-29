local luadb = require 'luadb.hypergraph'

local function isLoopBlock (node)
	return ( (node.key == "While") or (node.key == "For") )
end

local function findLoopNode(statement)
  local loopNode = nil

  for i, node in pairs(statement.data) do
    if (isLoopBlock(node)) then
      loopNode = node
      break
    else
      loopNode = findLoopNode(node)
    end
  end

  return loopNode
end

local function constructForLoopText(node)

  local numOfNodes = #node.data
  local loopText = ""
  
  for i = 1, numOfNodes - 1 do
    loopText = loopText .. node.data[i].text
  end

  return loopText
end

local function insertCentralLoopNodeWithEdge(hypergraph, loopNode, graphSourceNode)

	-- create new luadb node, setup with proper values and insert into hypergraph
	local newLoopNode = luadb.node.new()
	newLoopNode.meta = newLoopNode.meta or {}
	newLoopNode.meta.type = "loop"
	newLoopNode.data.name = "loop"
	newLoopNode.data.astNodeId = loopNode.nodeid

	hypergraph:addNode(newLoopNode)
    -- condition node creation end

    -- create new edge to connect method with created loop node
    local edge = luadb.edge.new()
    edge.label = "executes"
    edge:setSource(graphSourceNode)
    edge:setTarget(newLoopNode)
    edge:setAsOriented()

    hypergraph:addEdge(edge)
    -- edge between method and loop node creation end

    return hypergraph, newLoopNode
end

local function insertHeaderNodeWithEdge(hypergraph, loopConditionText, newLoopNode)
	-- let's create loop header luadb node
	local loopHeader = luadb.node.new()
	loopHeader.meta = loopHeader.meta or {}
	loopHeader.meta.type = "loopHeader"
	loopHeader.data.name = loopConditionText

	hypergraph:addNode(loopHeader)
    -- condition node creation end

    -- create new edge to connect method with created loop node
    local headerEdge = luadb.edge.new()
    headerEdge.label = "hasHeader"
    headerEdge:setSource(newLoopNode)
    headerEdge:setTarget(loopHeader)
    headerEdge:setAsOriented()

    hypergraph:addEdge(headerEdge)
    -- end

    return hypergraph
end

return {
	isLoopBlock = isLoopBlock,
	findLoopNode = findLoopNode,
	constructForLoopText = constructForLoopText,
	insertCentralLoopNodeWithEdge = insertCentralLoopNodeWithEdge,
	insertHeaderNodeWithEdge = insertHeaderNodeWithEdge
}