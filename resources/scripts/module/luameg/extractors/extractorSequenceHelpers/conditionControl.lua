local luadb = require 'luadb.hypergraph'

local function isConditionBlock (node)
	return (node.key == "If")
end

local function findConditionNode(statement)
  local conditionNode = nil

  for i, node in pairs(statement.data) do
    if (isConditionBlock(node)) then
      conditionNode = node
      break
    else
      conditionNode = findConditionNode(node)
    end
  end

  return conditionNode
end

local function insertCentralConditionNodeWithEdge(hypergraph, conditionNode, graphSourceNode)

	-- create new luadb node, setup with proper values and insert into hypergraph
	local newCondNode = luadb.node.new()
	newCondNode.meta = newCondNode.meta or {}
	newCondNode.meta.type = "condition"
	newCondNode.data.name = "condition"
	newCondNode.data.astNodeId = conditionNode.nodeid

	hypergraph:addNode(newCondNode)
  -- condition node creation end

  -- create new edge to connect method with created condition node
  local edge = luadb.edge.new()
  edge.label = "executes"
  edge:setSource(graphSourceNode)
  edge:setTarget(newCondNode)
  edge:setAsOriented()

  hypergraph:addEdge(edge)
  -- edge between method and conditon node creation end

  return hypergraph, newCondNode
end

local function setupConditionBranch(nodeName, nodeAstId, graphSource, hypergraph)
  -- create first condition branch node
  local newCondBranch = luadb.node.new()
  newCondBranch.meta = newCondBranch.meta or {}
  newCondBranch.meta.type = "conditionBranch"
  newCondBranch.data.name = nodeName
  newCondBranch.data.astNodeId = nodeAstId

  hypergraph:addNode(newCondBranch)

  -- create also edge to connect these two nodes
  local branchEdge = luadb.edge.new()
  branchEdge.label = "hasBranch"
  branchEdge:setSource(graphSource)
  branchEdge:setTarget(newCondBranch)
  branchEdge:setAsOriented()

  hypergraph:addEdge(branchEdge)
  -- end

  return hypergraph, newCondBranch
end

return {
	isConditionBlock = isConditionBlock,
	findConditionNode = findConditionNode,
	insertCentralConditionNodeWithEdge = insertCentralConditionNodeWithEdge,
	setupConditionBranch = setupConditionBranch
}