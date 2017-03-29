-----------------------------------------------
-- Function Calls Extractor
-- @release 2013/12/03, Tomas Filcak
-----------------------------------------------

local hypergraph = require "luadb.hypergraph"
local utils      = require "luadb.utils"
local ast        = require "luadb.ast"
local logger     = utils.logger


local function extractFunctions(AST, graph, path)
  local nodes = {}
  local functions = ast.getFunctions(AST)
  logger:debug("importing nodes")
  
  for i,func in pairs(functions) do
    logger:debug('adding node '..func.name)
    local newNode = hypergraph.node.new()
    newNode.meta = newNode.meta or {}
    newNode.meta.type = "function"
    newNode.meta.modulePath = path
    newNode.data.metrics = func.metrics
    newNode.data.name = func.name
    newNode.data.position = func.position
    newNode.data.tag = func.tag
    graph:addNode(newNode)
    table.insert(nodes, newNode)
  end
  
  return nodes
end


local function addGlobalCall(graph, newEdge, call, calledFunction)
  -- module function call
  if call.metrics.module_functionCall then
    calledFunction = call.metrics.module_functionCall.functionName
    
    graph.moduleCalls = graph.moduleCalls or {}
    graph.moduleCalls[calledFunction] = graph.moduleCalls[calledFunction] or {}
    
    newEdge.meta.calledFunction = calledFunction
    newEdge.meta.calledFunctionModule = call.metrics.module_functionCall.moduleName
    newEdge.meta.calledFunctionModulePath = newEdge.meta.calledFunctionModule:gsub('%.', '/')..".lua"
    table.insert(graph.moduleCalls[calledFunction], newEdge)
  
  -- global function call
  else            
    graph.globalCalls = graph.globalCalls or {}
    graph.globalCalls[calledFunction] = graph.globalCalls[calledFunction] or {}   
    table.insert(graph.globalCalls[calledFunction], newEdge)  
  end
end


local function getCalleeFunctionName(call)
  local from = ast.getParentNodesByTag(call, "LocalFunction", nil)
  if not utils.isEmpty(from) then
    return ast.getChildNodesByTag(from[1], "Name", nil)[1].str
  else
    from = ast.getParentNodesByTag(call, "GlobalFunction", nil)
    if not utils.isEmpty(from) then return from[1].name end
  end
end


local function extractFunctionCalls(AST, graph, nodes)
  local edges = {}
  local functions = ast.getFunctions(AST)
  local functionsCalls = ast.getFunctionsCalls(AST)
  
  for calledFunction,functionCalls in pairs(functionsCalls) do
    for index,call in pairs(functionCalls) do
      local newEdge = hypergraph.edge.new()
      newEdge.label = "calls"
      newEdge.meta  = newEdge.meta or {}
      newEdge.meta.calleeFunction = getCalleeFunctionName(call)
      newEdge.meta.calledFunction = calledFunction
      newEdge.data.text = call.text
      newEdge.data.position = call.position
      newEdge.data.tag = call.tag
      newEdge:addSource(hypergraph.node.findByName(nodes, newEdge.meta.calleeFunction))
      newEdge:addTarget(hypergraph.node.findByName(nodes, calledFunction))
              
      -- function's declaration not found
      if utils.isEmpty(newEdge.to) then
        logger:debug('found undeclared function '..calledFunction)
        addGlobalCall(graph, newEdge, call, calledFunction)
      end
      
      newEdge:setAsOriented()
      graph:addEdge(newEdge)
      table.insert(edges, newEdge)
    end
  end
  return edges
end

-- extract function from 3DSoftViz's LuaDB module
local function extract(luaFileNode, graph)  
  local path = luaFileNode.data.path
  local graph = graph or hypergraph.graph.new()
  local AST = ast.getAST(path)
  local nodes = extractFunctions(AST, graph, path)
  local edges = extractFunctionCalls(AST, graph, nodes)
  return { nodes = nodes, edges = edges }
end


-----------------------------------------------
-- @author Michael Scholtz
-----------------------------------------------
local function extractFromString(string, graph)
  local graph = graph or hypergraph.graph.new()
  local AST   = ast.getASTFromString(string)
  local nodes = extractFunctions(AST, graph, "source:string")
  local edges = extractFunctionCalls(AST, graph, nodes)
  return { nodes = nodes, edges = edges }
end

-----------------------------------------------
-- @author Michael Scholtz
-----------------------------------------------
local function extractFromAST(ast, graph)
  local graph = graph or hypergraph.graph.new()
  local nodes = extractFunctions(ast, graph, "source:ast")
  local edges = extractFunctionCalls(ast, graph, nodes)
  return { nodes = nodes, edges = edges }
end


-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract = extract,
  extractFromString = extractFromString,
  extractFromAST = extractFromAST,
  extractNodes = extractFunctions,
  extractEdges = extractFunctionCalls
}
