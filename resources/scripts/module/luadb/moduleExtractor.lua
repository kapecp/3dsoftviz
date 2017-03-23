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
local function getFunctionWithName(functionsList, name)
  if (functionsList == nil) then
    local sth = functionsList 
  end
  
  for i,functionNode in pairs(functionsList) do
      if functionNode.data.name == name then return functionNode end
  end
  return nil
end

local function registerGlobalModule(graph, moduleName, moduleFunctionCall)
  graph.globalModuleNodes = graph.globalModuleNodes or {} 
  local globalModuleNodes = graph.globalModuleNodes
  
  -- add global module node
  if not globalModuleNodes[moduleName] then
    local newGlobalModuleNode = hypergraph.node.new()
    newGlobalModuleNode.meta  = newGlobalModuleNode.meta or {}
    newGlobalModuleNode.functionNodes = newGlobalModuleNode.functionNodes or {}
    newGlobalModuleNode.data.name = moduleName
    newGlobalModuleNode.data.type = "module"

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
    newFunctionNode.data.name = moduleFunctionCall
    newFunctionNode.data.type = "global function"
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
local function getFilesTree(graph, path)
  filestree.extract(path, graph)
end

local function getModuleFromFile(graph)
  local luaFileNodes = graph.luaFileNodes
  for i,luaFileNode in pairs(luaFileNodes) do
    local moduleName = utils.splitAndGetFirst(luaFileNode.data.name, "%.")  --scanner.lua -> scanner
    --[[
    local moduleName = luaFileNode.data.path
    local moduleName1 = utils.splitAndGetLast(moduleName, "%\\")
    local moduleName2 = utils.splitAndGetFirst(moduleName1,  "%.")
    moduleName = moduleName2:gsub("/", ".")
    ]]--
    
    local newModuleNode = hypergraph.node.new()
    newModuleNode.meta  = luaFileNode.meta or {}
    newModuleNode.functionNodes = newModuleNode.functionNodes or {}
    newModuleNode.data.name = moduleName
    newModuleNode.data.type = "module"
    
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

local function getFunctionCalls(graph, astManager)
  local luaFileNodes = graph.luaFileNodes
  graph.globalModuleNodes = graph.globalModuleNodes or {} 
  local globalModuleNodes = graph.globalModuleNodes
      
  for i,luaFileNode in pairs(luaFileNodes) do
    print(luaFileNode.data.path)
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
      newGlobalFunctionNode.data.name = globalFunctionCall
      newGlobalFunctionNode.data.type = "global function"
      graph:addNode(newGlobalFunctionNode)
      functionNode = newGlobalFunctionNode
    end
    
    for i,globalFunctionCallEdge in pairs(edges) do
      -- add target connection for each global function call
      globalFunctionCallEdge:addTarget(functionNode)
    end
    
  end
end

local function getAssignsAndReturnValues(graph, astManager)
  local luaFileNodes = graph.luaFileNodes
  for i,luaFileNode in pairs(luaFileNodes) do
    local extractedNodes = modules.extract(luaFileNode, graph, astManager)
  end
  
end

  

local function clearTmpVars(graph)
  graph.globalCalls = nil
  graph.moduleCalls = nil
  graph.luaFileNodes = nil
  graph.globalModuleNodes = nil
end

-----------------------------------------------
-- Extract
-----------------------------------------------

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
