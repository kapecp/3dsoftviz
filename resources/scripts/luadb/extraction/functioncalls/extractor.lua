-----------------------------------------------
-- Function Calls Extractor
-- @release 2013/12/03, Tomas Filcak
-----------------------------------------------

local hypergraph = require "luadb.hypergraph"
local utils      = require "luadb.utils"
local ast        = require "luadb.ast"
local mapper     = require "luadb.mapping"
local logger     = utils.logger

local function extractNodes(AST, graph)
  local functions = ast.getFunctions(AST)
  logger:debug("importing nodes")
  
  for i,func in pairs(functions) do
    logger:debug('adding node '..func.name)
    local newNode = hypergraph.node.new()
    newNode.data.metrics = func.metrics
    newNode.data.name = func.name
    newNode.data.tag = func.tag
    graph:addNode(newNode)
  end
end


local function extractEdges(AST, graph)
  logger:debug("printing all function calls")
  local functions_calls = AST.metrics.functionExecutions
  local functions = ast.getFunctions(AST)
  
  logger:debug("importing edges")
  for i,function_calls in pairs(functions_calls) do
    for j,call in pairs(function_calls) do
      local l_from = ast.getParentNodesByTag(call, "LocalFunction", nil)
      local f_from = ((next(l_from) ~= nil) and l_from) or ast.getParentNodesByTag(call, "GlobalFunction", nil)
      
      if (next(f_from) ~= nil) then
        local s_func_from = ast.getChildNodesByTag(f_from[1], "Name", nil)[1].str
        local s_func_to   = i
        
        local from = graph:findNodeIdsByName(s_func_from)
        local to   = graph:findNodeIdsByName(s_func_to)
        
        -- function definition not found in file
        if (next(to) == nil) then
          logger:debug('found undeclared function '..s_func_to)
          logger:debug('adding node '..s_func_to)
          local newNode = hypergraph.node.new()
          newNode.data.name = s_func_to
          graph:addNode(newNode)
          to = graph:findNodeIdsByName(s_func_to)
        end
        
        local newEdge = hypergraph.edge.new()
        newEdge.label = "FunctionCall"
        newEdge.from  = from
        newEdge.to    = to
        
        newEdge:setAsOriented()
        graph:addEdge(newEdge)
      end
      
    end
  end

end


local function extractFunctionCalls(graph, fileName)
  local AST = ast.getAST(fileName)
  extractNodes(AST, graph)  
  extractEdges(AST, graph)    
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract      = extractFunctionCalls,
  extractEdges = extractEdges,
  extractNodes = extractNodes
}
