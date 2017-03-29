-----------------------------------------------
-- Main Extractor
-- @release 2014/04/18, Tomas Filcak
-----------------------------------------------

local hypergraph    = require "luadb.hypergraph"
local utils         = require "luadb.utils"
local filestree     = require "luadb.extraction.filestree"
local functioncalls = require "luadb.extraction.functioncalls"
local lfs           = require "lfs"
local logger        = utils.logger


local function getFilesTree(graph, path)
  filestree.extract(path, graph)
end


local function getFunctionCalls(graph, luaFileNodes)  
  for i,luaFileNode in pairs(luaFileNodes) do
    local functionCalls = functioncalls.extract(luaFileNode, graph)
    luaFileNode.functionNodes = functionCalls.nodes
    luaFileNode.functionCalls = functionCalls.edges
    
    -- connect all function nodes to file node
    for j,functionNode in pairs(luaFileNode.functionNodes) do
      local connection = hypergraph.edge.new()
      connection.label = "declares"
      connection:addSource(luaFileNode)
      connection:addTarget(functionNode)
      graph:addEdge(connection)
    end
    
    -- connect all root function calls to module file node
    for k,functionCallEdge in pairs(luaFileNode.functionCalls) do
      if utils.isEmpty(functionCallEdge.from) then
      functionCallEdge.label = "calls"
      functionCallEdge:addSource(luaFileNode)
      end
    end
  end
end


local function getFunctionWithName(functionsList, name)
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
    newGlobalModuleNode.meta.type = "module"
    newGlobalModuleNode.functionNodes = newGlobalModuleNode.functionNodes or {}
    newGlobalModuleNode.data.name = moduleName

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
    table.insert(moduleNode.functionNodes, newFunctionNode)
    functionNode = newFunctionNode
    graph:addNode(newFunctionNode)
  end
  
  -- connect global module node with his function node
  local connection = hypergraph.edge.new()
  connection.label = "declares"
  connection:addSource(globalModuleNodes[moduleName])
  connection:addTarget(functionNode)
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
      newGlobalFunctionNode.meta.type = "global function"
      newGlobalFunctionNode.data.name = globalFunctionCall
      graph:addNode(newGlobalFunctionNode)
      functionNode = newGlobalFunctionNode
    end
    
    for i,globalFunctionCallEdge in pairs(edges) do
      -- add target connection for each global function call
      globalFunctionCallEdge:addTarget(functionNode)
    end
    
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

local function extract(sourcePath)
  assert(sourcePath and utils.isDir(sourcePath), "wrong path passed")
  assert(not utils.isDirEmpty(sourcePath), "directory is empty")
  
  local label = "ExtractionGraph"
  local description = "hierarchical function calls graph"
  local graph = hypergraph.graph.new({ description =  description, label = label })
  local sourceDirectory = sourcePath or lfs.currentdir()
  
  getFilesTree(graph, sourceDirectory)
  getFunctionCalls(graph, graph.luaFileNodes)

  connectModuleCalls(graph)
  assignGlobalCalls(graph)
  
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
