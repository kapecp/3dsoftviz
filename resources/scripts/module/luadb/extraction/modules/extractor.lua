-----------------------------------------------
-- Modules Extractor
-- @release 2016/12/11, Denis Illes
-----------------------------------------------

local hypergraph = require "luadb.hypergraph"
local utils      = require "luadb.utils"
local ast        = require "luadb.ast"
local logger     = utils.logger
--local debug      = require "dbg2"
local mutils     = require "metrics.utils"

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

local function getFileNodeByFilePath(graph, filePath)
  local files = graph:findNodeByType('file')
  for i,file in pairs(files) do    
    if file.data.path == filePath then
      return file   
    end    
  end
  return nil
end


local function extractModules(AST, graph, path)
  local nodes = {}
  -- require calls
  local moduleCalls = ast.getModuleCalls(AST)
  --debug.Save("mCalls", moduleCalls)
  
  -- local variables
  local moduleReferences = ast.getModuleReferences(AST)
  --debug.Save("mRef", moduleReferences)
  
  local returnValues = ast.getModuleReturnValues(AST)
  local lastRetVal = returnValues[#returnValues]
  local localFuncs = ast.getLocalFunctions(AST)
  local globalFuncs = ast.getGlobalFunctions(AST)
  
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
    
    -- check if var is local or not
    if ast.isVariableLocal(AST, newNode.data.name) then      
      print(newNode.data.name .. " is local")
      newNode.data.name = "local " .. newNode.data.name
    else
      print(newNode.data.name .. " is not local")
    end    
    
    newNode.data.metrics.variableName = nil
    graph:addNode(newNode)
    table.insert(nodes, newNode)
  end
  
  for i,locFunc in pairs(localFuncs) do
    logger:debug('adding local function node '..i)
    local newNode = hypergraph.node.new()
    newNode.data.metrics = locFunc.metrics  
    newNode.data.name = locFunc.name
    newNode.data.type = "localFunction"
    newNode.data.position = locFunc.position
    newNode.data.metrics.fileNode = getFileNodeByFilePath(graph, path)
    graph:addNode(newNode)
    table.insert(nodes, newNode)
  end
  
  for i,globFunc in pairs(globalFuncs) do
    logger:debug('adding global function node '..i)
    local newNode = hypergraph.node.new()
    newNode.data.metrics = globFunc.metrics  
    newNode.data.name = globFunc.name
    newNode.data.type = "globalFunction"
    newNode.data.position = globFunc.position
    newNode.data.metrics.fileNode = getFileNodeByFilePath(graph, path)
    graph:addNode(newNode)
    table.insert(nodes, newNode)
  end
  
  for i,retVal in pairs(returnValues) do
    logger:debug('adding return value node '..i)
    local newNode = hypergraph.node.new()
    newNode.data.metrics = retVal.metrics  
    newNode.data.name = retVal.text
    newNode.data.type = "interface"
    newNode.data.position = retVal.position
    newNode.data.metrics.filePath = path
    graph:addNode(newNode)
    table.insert(nodes, newNode)
  end
  
  return nodes
end



local function extractModuleCalls(AST, graph, nodes)
  local edges = {}
  
  for _,node in pairs(nodes) do
    if node.data.type == "variable" then
      logger:debug('adding var require edge')
      local newEdge = hypergraph.edge.new()
      newEdge.label = "requires"
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
  
  for _,node in pairs(nodes) do
    if node.data.type == "localFunction" or node.data.type == "globalFunction" then
      logger:debug('adding function contains edge')
      local newEdge = hypergraph.edge.new()
      newEdge.label = "contains"
      --newEdge.meta  = newEdge.meta or {}
      --newEdge.meta.calleeFunction = getCalleeFunctionName(call)
      --newEdge.meta.calledFunction = calledFunction
      --newEdge.data.text = call.text
      newEdge:addSource(node.data.metrics.fileNode)
      newEdge:addTarget(node)
            
      node.data.metrics.fileNode = nil
                  
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
  
  --[[
  local tempp = ast.getModuleReturnValues(AST)
  if (tempp ~= nil) then
    for j,k in pairs(tempp) do
      print(j .. " " .. k.text)
    end
  end
  ]]--

  
  --[[
  local locFunc = mutils.searchForTagArray_recursive("LocalFunction", AST, nil)
  for j,k in pairs(locFunc) do
    print(j .. " " .. k.text)
  end
  ]]--
  
  
  
  --debug.Save("nodes", localassign)
  local nodes = extractModules(AST, graph, fileName)
  --debug.Save("nodes", nodes)
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
