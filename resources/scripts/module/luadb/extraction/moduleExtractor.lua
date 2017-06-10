-----------------------------------------------
-- Main Module Extractor
-- @release 2016/12/2, Denis Illes
-----------------------------------------------

local hypergraph    = require "luadb.hypergraph"
local utils         = require "luadb.utils"
local filestree     = require "luadb.extraction.filestree"
local functioncalls = require "luadb.extraction.functioncalls"
local modules       = require "luadb.extraction.modules"
local lfs           = require "lfs"
local logger        = utils.logger

-----------------------------------------------
-- Helper functions
-----------------------------------------------

----------------------------------------------
-- Find function in functionList
-- @param functionList table of functions
-- @return name name of searched function
local function getFunctionWithName(functionsList, name)
  if functionsList ~= nil then
    for i,functionNode in pairs(functionsList) do
      if functionNode.data.name == name then
        return functionNode
      end
    end
  end
  return nil
end

----------------------------------------------
-- Create module node (if not found) and connects with function
-- @param graph luadb type of graph
-- @param moduleName name of module
-- @param moduleFunctionCall
local function registerGlobalModule(graph, moduleName, moduleFunctionCall)
  graph.globalModuleNodes = graph.globalModuleNodes or {} 
  local globalModuleNodes = graph.globalModuleNodes
  
  -- add global module node
  if not globalModuleNodes[moduleName] then
    local newGlobalModuleNode = hypergraph.node.new()
    newGlobalModuleNode.meta  = newGlobalModuleNode.meta or {}    
    newGlobalModuleNode.meta.type = "module"    
    newGlobalModuleNode.data.name = moduleName
    newGlobalModuleNode.data.path = ""
    newGlobalModuleNode.data.astID = -1
    newGlobalModuleNode.data.astNodeID = -1
    newGlobalModuleNode.functionNodes = newGlobalModuleNode.functionNodes or {}

    globalModuleNodes = globalModuleNodes or {}
    globalModuleNodes[moduleName] = newGlobalModuleNode
    graph:addNode(newGlobalModuleNode) 
  end
  
  -- add global module node function
  local moduleNode = globalModuleNodes[moduleName]
  local functionNode = getFunctionWithName(moduleNode.functionNodes, moduleFunctionCall)
  if not functionNode then
    local newFunctionNode = hypergraph.node.new()
    newFunctionNode.meta  = newFunctionNode.meta or {}
    newFunctionNode.meta.type = "global function"
    newFunctionNode.data.name = moduleFunctionCall
    newFunctionNode.data.astID = -1
    newFunctionNode.data.astNodeID = -1
    table.insert(moduleNode.functionNodes, newFunctionNode)
    functionNode = newFunctionNode
    graph:addNode(newFunctionNode)
  end
  
  -- connect global module node with his function node
  local connection = hypergraph.edge.new()
  connection.label = "declares"
  connection:addSource(globalModuleNodes[moduleName])
  connection:addTarget(functionNode)
  connection:setAsOriented()
  graph:addEdge(connection)
end

----------------------------------------------
-- Find and return table of function of a module
-- @param graph luadb type of graph
-- @param moduleName name of searched module
-- @return table of functions or empty table
local function getGlobalModuleFunctions(graph, moduleName)
  local globalNodes = graph.globalModuleNodes or {}
  if globalNodes[moduleName] and globalNodes[moduleName].functionNodes then
    return globalNodes[moduleName].functionNodes
  else
    return {}  
  end
end


-----------------------------------------------
-- Main functions
-----------------------------------------------

----------------------------------------------
-- Extract files tree
-- save as graph.luaFilesNodes
-- @param graph luadb type of graph
-- @param path absolute path
local function getFilesTree(graph, path)
  filestree.extract(path, graph)
end

----------------------------------------------
-- Create modules from lua files
-- @param graph luadb type of graph
local function getModuleFromFile(graph)
  local luaFileNodes = graph.luaFileNodes
  for i,luaFileNode in pairs(luaFileNodes) do
    local modulePath = luaFileNode.data.path
    local moduleName = utils.splitAndGetFirst(luaFileNode.data.name, "%.")  --scanner.lua -> scanner
        
    local newModuleNode = hypergraph.node.new()
    newModuleNode.meta  = {}
    newModuleNode.meta.type = "module"
    newModuleNode.data.name = moduleName
    newModuleNode.data.path = modulePath
    newModuleNode.data.astID = -1
    newModuleNode.data.astNodeId = -1
    newModuleNode.functionNodes = {}
    
    graph:addNode(newModuleNode)
    
    graph.globalModuleNodes = graph.globalModuleNodes or {} 
    local globalModuleNodes = graph.globalModuleNodes
    globalModuleNodes[moduleName] = newModuleNode
    
    -- connect module node with his file node
    local connection = hypergraph.edge.new()
    connection.label = "implements"
    connection:addSource(luaFileNode)
    connection:addTarget(globalModuleNodes[moduleName])
    connection:setAsOriented()
    graph:addEdge(connection)    
  end  
end

----------------------------------------------
-- Extract function calls
-- @param graph luadb type of graph
-- @param astManager manager of ASTs
local function getFunctionCalls(graph, astManager)
  local luaFileNodes = graph.luaFileNodes
  graph.globalModuleNodes = graph.globalModuleNodes or {} 
  local globalModuleNodes = graph.globalModuleNodes
      
  for i,luaFileNode in pairs(luaFileNodes) do    
    local functionCalls = functioncalls.extract(luaFileNode, graph, astManager)
    luaFileNode.functionNodes = functionCalls.nodes
    luaFileNode.functionCalls = functionCalls.edges
    
    local moduleName = utils.splitAndGetFirst(luaFileNode.data.name, "%.")
    
    -- connect all function nodes to file node
    for j,functionNode in pairs(luaFileNode.functionNodes) do
      local connection = hypergraph.edge.new()
      connection.label = "declares"
      connection:addSource(globalModuleNodes[moduleName])
      connection:addTarget(functionNode)
      connection:setAsOriented()
      graph:addEdge(connection)
    end
    
    -- connect all root function calls to module file node
    for k,functionCallEdge in pairs(luaFileNode.functionCalls) do
      if utils.isEmpty(functionCallEdge.from) then
      functionCallEdge.label = "calls"
      functionCallEdge:addSource(globalModuleNodes[moduleName])
      end
    end
  end
end

----------------------------------------------
-- Connect function with required modules
-- @param graph luadb type of graph
local function connectModuleCalls(graph)
  local moduleCalls = graph.moduleCalls or {}
  local globalCalls = graph.globalCalls or {}
  local luaFileNodes = graph.luaFileNodes or {}
  
  for moduleFunctionCall,edges in pairs(moduleCalls) do
    
    local functionNodes = {}
    local moduleName = edges[1].meta.calledFunctionModule
    local modulePath = edges[1].meta.calledFunctionModulePath
    
    -- get all function nodes for searched module file
    for i,luaFileNode in pairs(luaFileNodes) do
      if luaFileNode.data.path:find(modulePath) and getFunctionWithName(luaFileNode.functionNodes, moduleFunctionCall) ~= nil then 
        functionNodes = luaFileNode.functionNodes
      end
    end
    
    -- found global module usage
    if utils.isEmpty(functionNodes) then
      registerGlobalModule(graph, moduleName, moduleFunctionCall)
      functionNodes = getGlobalModuleFunctions(graph, moduleName)
    end
    
    -- assign target node to module function call edge
    local functionNode = getFunctionWithName(functionNodes, moduleFunctionCall)
    if functionNode then
      for j,moduleFunctionCallEdge in pairs(edges) do
        -- add target connection for each module function call
        moduleFunctionCallEdge:addTarget(functionNode)
      end
    end
    
  end
end

----------------------------------------------
-- Assign connection of global function calls
-- @param graph luadb type of graph
local function assignGlobalCalls(graph)
  local globalCalls = graph.globalCalls or {}
  
  for globalFunctionCall,edges in pairs(globalCalls) do
    local functionNode = nil
    local parts = utils.split(globalFunctionCall, "%.")
    if table.getn(parts) == 2 then
      local moduleName = parts[1]
      local moduleCall = parts[2]
      registerGlobalModule(graph, moduleName, moduleCall)
      local functionNodes = getGlobalModuleFunctions(graph, moduleName)
      functionNode = getFunctionWithName(functionNodes, moduleCall)
    else
      local newGlobalFunctionNode = hypergraph.node.new()
      newGlobalFunctionNode.meta  = newGlobalFunctionNode.meta or {}
      newGlobalFunctionNode.meta.type = "global function"
      newGlobalFunctionNode.data.name = globalFunctionCall
      newGlobalFunctionNode.data.astID = -1
      newGlobalFunctionNode.data.astNodeId = -1
      graph:addNode(newGlobalFunctionNode)
      functionNode = newGlobalFunctionNode
    end
    
    for i,globalFunctionCallEdge in pairs(edges) do
      -- add target connection for each global function call
      globalFunctionCallEdge:addTarget(functionNode)
    end
    
  end
end

----------------------------------------------
-- Extract nodes and edges to create a module graph
-- @param graph luadb type of graph
-- @param astManager manager of ASTs
local function getAssignsAndReturnValues(graph, astManager)
  local luaFileNodes = graph.luaFileNodes
  for i,luaFileNode in pairs(luaFileNodes) do
    local extractedNodes = modules.extract(luaFileNode, graph, astManager)
  end
  
end

----------------------------------------------
-- Join variable and function nodes if belongs to its table
-- @param graph luadb type of graph
local function connectFunctionsToVariables(graph)
  local wantedEdges = {}
  local assignEdges = graph:findEdgesByLabel("assigns")  
  for i,edge in pairs(assignEdges) do
    if(edge.to[1].data.name == "{}") then
      table.insert(wantedEdges, edge)
    end
  end
  -- edge.from[1] is a local/global variable like 'local ret = {}'
  local functions = graph:findNodesByType("function")
  -- functions[1] is like 'ret.Save()'
  for _,func in pairs(functions) do
    local funcDotName = utils.splitAndGetFirst(func.data.name, "%.")
    
    for _,edge in pairs(wantedEdges) do
      local variableName = edge.from[1].data.name
      if(variableName == funcDotName) then
        local connection = hypergraph.edge.new()
        connection.label = "contains"
        connection:addSource(edge.from[1])
        connection:addTarget(func)
        connection:setAsOriented()
        graph:addEdge(connection)
      end
    end
  end  
end

----------------------------------------------
-- Clear unused tables from luadb graph 
-- @param graph luadb type of graph
local function clearTmpVars(graph)
  graph.globalCalls = nil
  graph.moduleCalls = nil
  graph.luaFileNodes = nil
  graph.globalModuleNodes = nil
end

-----------------------------------------------
-- Extract
-----------------------------------------------

----------------------------------------------
-- Extract module graph from given project path
-- @param sourcePath string of absolute path
-- @param astManager manager of ASTs
-- @return graph luadb type of graph
local function extract(sourcePath, astManager)
  assert(sourcePath and utils.isDir(sourcePath), "wrong path passed")
  assert(not utils.isDirEmpty(sourcePath), "directory is empty")
  
  local label = "ExtractionGraph"
  local description = "hierarchical hybrid graph"
  local graph = hypergraph.graph.new({ description =  description, label = label })
  local sourceDirectory = sourcePath or lfs.currentdir()
  
  getFilesTree(graph, sourceDirectory)
  
  getModuleFromFile(graph)
  
  getFunctionCalls(graph, astManager)
  connectModuleCalls(graph)
  assignGlobalCalls(graph)
  
  getAssignsAndReturnValues(graph, astManager)
  connectFunctionsToVariables(graph)
    
  clearTmpVars(graph)
  return graph
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract = extract
}
