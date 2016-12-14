-----------------------------------------------
-- Modules Extractor
-- @release 2016/13/11, Denis Illes
-----------------------------------------------

local hypergraph = require "luadb.hypergraph"
local utils      = require "luadb.utils"
local ast        = require "luadb.ast"
local logger     = utils.logger
local debug      = require "dbg2"


local function getModuleNodeByModulePath(nodes, modulePath)
  for i,mod in pairs(nodes) do
    if mod.data.type == "module" then
      if mod.data.modulePath == modulePath then
        return mod   
      end
    end
  end
  return {}
end

local function extractModules(AST, graph, path)
  local nodes = {}
  -- require calls
  local moduleCalls = ast.getModuleCalls(AST)
  --debug.Save("mCalls", moduleCalls)
  
  -- local variables
  local moduleReferences = ast.getModuleReferences(AST)
  --debug.Save("mRef", moduleReferences)
  
  logger:debug("importing nodes")
  
  for i,modCall in pairs(moduleCalls) do
    logger:debug('adding module node '..i)
    local newNode = hypergraph.node.new()
    newNode.data.metrics = modCall    
    newNode.data.name = utils.splitAndGetLast(i, "%.")    --only grammar from leg.grammar
    newNode.data.type = "module"
    --newNode.data.position = ???
    newNode.data.modulePath = i
    graph:addNode(newNode)
    table.insert(nodes, newNode)    
  end
  
  for i,modRef in pairs(moduleReferences) do
    logger:debug('adding var node '..i)
    local newNode = hypergraph.node.new()
    newNode.data.metrics = modRef
    
    newNode.data.name = modRef.variableName   
    newNode.data.type = "variable"
	  --newNode.data.position = ???
    newNode.data.modulePath = path    
    newNode.data.metrics.requires = getModuleNodeByModulePath(nodes, i)
    
    newNode.data.metrics.variableName = nil
    graph:addNode(newNode)
    table.insert(nodes, newNode)
  end
  
  return nodes
end



local function extractModuleCalls(AST, graph, nodes)
  local edges = {}
  
  for _, node in pairs(nodes) do
    if node.data.type == "variable" then
      logger:debug('adding var require edge')
      local newEdge = hypergraph.edge.new()
      newEdge.label = "RequireCall"
      --newEdge.meta  = newEdge.meta or {}
      --newEdge.meta.calleeFunction = getCalleeFunctionName(call)
      --newEdge.meta.calledFunction = calledFunction
      --newEdge.data.text = call.text
      newEdge:addSource(node)
      newEdge:addTarget(node.data.metrics.requires)
      
      node.data.metrics.requires = nil
      
      newEdge:setAsOriented()
      graph:addEdge(newEdge)
      table.insert(edges, newEdge)
    end
  end
  
  return edges
end


local function extract(fileName, graph)
  local graph = graph or hypergraph.graph.new()
  local AST   = ast.getAST(fileName)
  local nodes = extractModules(AST, graph, fileName)
  local edges = extractModuleCalls(AST, graph, nodes)
  --debug.Save("edges", edges)
  return { nodes = nodes, edges = edges }
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract      = extract,
  extractNodes = extractModules,
  extractEdges = extractModuleCalls
}
