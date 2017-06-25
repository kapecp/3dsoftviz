-----------------------------------------------
-- Modules Extractor
-- @release 2016/12/11, Denis Illes
-----------------------------------------------

local hypergraph = require "luadb.hypergraph"
local utils      = require "luadb.utils"
local ast        = require "luadb.ast"
local mutils     = require "metrics.utils"
local logger     = utils.logger

-----------------------------------------------
-- Helper functions
-----------------------------------------------

----------------------------------------------
-- Create new luadb node
-- @param name node name
-- @param type node type
-- @param astID id of node in AST
-- @param astNode table of node AST information
-- @return new node
local function createNode(name, type, astID, astNode)
  local node = hypergraph.node.new()
  node.meta = {}  
  node.meta.type = type
  node.data.name = name
  node.data.astID = astID
  node.data.astNodeID = -1
  if(astNode ~= nil) then
    node.data.metrics = astNode.metrics
    node.data.position = astNode.position
    node.data.tag = astNode.tag
    node.data.astNodeID = astNode.nodeid
  end

  return node
end

----------------------------------------------
-- Create new luadb edge
-- @param label edge name
-- @param sourceNode source node
-- @param targetNode target node
-- @param isOriented boolean, is edge oriented
-- @return new edge
local function createEdge(label, sourceNode, targetNode, isOriented)
  local edge = hypergraph.edge.new()
  edge.label = label
  edge:addSource(sourceNode)
  edge:addTarget(targetNode)
  if(isOriented == true) then edge:setAsOriented() end
  return edge
end

----------------------------------------------
-- Add astID and astNodeID to module nodes
-- @param AST tree of parsed file
-- @param AST_ID id of the current AST
-- @param graph luadb type of graph
-- @param path absolute path of parsed file
local function addIdsToModuleNode(AST, AST_ID, graph, path)
  local modules = graph:findNodesByType("module")
  for i,mod in pairs(modules) do
    if mod.data.path == path then
      mod.data.astID = AST_ID
      mod.data.astNodeID = AST.nodeid
    end
  end
end

----------------------------------------------
-- Find node by name and type
-- @param name name of searched node
-- @param nodeType type of searched node
-- @param graph luadb type of graph
-- @return node or nil
local function findNodeByNameAndType(name, nodeType, graph)
  local nodes = graph:findNodesByType(nodeType)
  for _,node in pairs(nodes) do
    if(node.data.name == name) then
      return node
    end
  end
  return nil
end

----------------------------------------------
-- Find represents node for interface node
-- @param name name of interface node
-- @param graph luadb type of graph
-- @return node which represents the interface node or nil
local function getRepresentsNodeByName(name, graph)
  local globVar = findNodeByNameAndType(name, "global variable", graph)
  if(globVar) then return globVar end
  local locVar = findNodeByNameAndType(name, "local variable", graph)
  if(locVar) then return locVar end
  local globFunct = findNodeByNameAndType(name, "global function", graph)
  if(globFunct) then return globFunct end
  local funct = findNodeByNameAndType(name, "function", graph)
  if(funct) then return funct end
  
  return nil
end

----------------------------------------------
-- Find (or create of not found) and return environment node
-- @param graph luadb type of graph
-- @return environment (_G) node
local function getEnvironmentNode(graph)
  local _GNode = findNodeByNameAndType("_G", "global variable", graph)
  if(_GNode == nil) then _GNode = createNode("_G", "global variable", astID) end
  return _GNode
end

----------------------------------------------
-- Find and return module node from path
-- @param nodes table of nodes
-- @param modulePath absolute path of module
-- @return module node or empty table
local function getModuleNodeByModulePath(nodes, modulePath)
  for i,mod in pairs(nodes) do
    if mod.meta.type == "module" then
      if mod.data.modulePath == modulePath then
        return mod   
      end
    end
  end
  return {}
end

----------------------------------------------
-- Find and return file node from path
-- @param graph luadb type of graph
-- @param filePath absolute path of file
-- @return file node or nil
local function getFileNodeByFilePath(graph, filePath)
  local files = graph:findNodesByType('file')
  for i,file in pairs(files) do    
    if file.data.path == filePath then
      return file   
    end    
  end
  return nil
end

----------------------------------------------
-- Add nodes and edges from assign calls into passed tables
-- @param graph luadb type of graph
-- @param assigns table of structured assign calls
-- @param AST_ID id of the current AST
-- @param fileName name of parsed file
-- @param nodes table of nodes
-- @param edges table of edges
-- @return extracted nodes and edges
local function addAssignsToGraph(graph, assigns, AST_ID, fileName, nodes, edges)
  local assigns = assigns or {}  
  local nodes = nodes or {}
  local edges = edges or {}
  graph.globalModuleNodes = graph.globalModuleNodes or {} 
  local globalModuleNodes = graph.globalModuleNodes
  
  local fileModuleName = utils.splitAndGetFirst(fileName, "%.")
  
  for i,assign in pairs(assigns) do
    --assign.(idNode|assignedNode)
    --assignedNode.(name|fullName|type|node)
    local nodeName, node
    logger:debug('adding variable node '..i)
    local varNode = createNode(assign.idNode.name, assign.idNode.type, AST_ID, assign.idNode.node)
    graph:addNode(varNode)
    table.insert(nodes, varNode)
    
    logger:debug('adding initializes edge')
    local newEdge = createEdge("initializes", globalModuleNodes[fileModuleName], varNode, true)
    graph:addEdge(newEdge)
    table.insert(edges, newEdge)
    
    if(assign.assignedNode ~= nil) then
      local assignNode = createNode(assign.assignedNode.name or "", assign.assignedNode.type or "N/A", AST_ID, assign.assignedNode.node)
      graph:addNode(assignNode)
      table.insert(nodes, assignNode)
      
      logger:debug('adding assigns edge')
      local newEdge = createEdge("assigns", varNode, assignNode, true)
      graph:addEdge(newEdge)
      table.insert(edges, newEdge) 
    end
  end
  return nodes, edges
end

----------------------------------------------
-- Add nodes and edges from require calls into passed tables
-- @param graph luadb type of graph
-- @param requires table of structured require calls
-- @param AST_ID id of the current AST
-- @param fileName name of parsed file
-- @param nodes table of nodes
-- @param edges table of edges
-- @return extracted nodes and edges
local function addRequireNodesToGraph(graph, requires, AST_ID, fileName, nodes, edges)
  local requires = requires or {}  
  local nodes = nodes or {}
  local edges = edges or {}
  graph.globalModuleNodes = graph.globalModuleNodes or {} 
  local globalModuleNodes = graph.globalModuleNodes
  
  local fileModuleName = utils.splitAndGetFirst(fileName, "%.")
  
  for i,req in pairs(requires) do
    --req.(idNode|assignedNode)
    --assignedNode.(name|fullName|type|node)
    local idNode    
    
    if(req.idNode == nil) then
      --case of global require -> connect to _G
      idNode = getEnvironmentNode(graph)      
    else
      idNode = createNode(req.idNode.fullName, req.idNode.type, AST_ID, req.idNode.node)
    end
    logger:debug('adding local variable node '..i)
    graph:addNode(idNode)
    table.insert(nodes, idNode)
        
    logger:debug('adding initializes edge')
    local newEdge = createEdge("initializes", globalModuleNodes[fileModuleName], idNode, true)
    graph:addEdge(newEdge)    
    table.insert(edges, newEdge)
        
    local moduleNode = globalModuleNodes[req.assignedNode.name]
    if (moduleNode == nil) then
      logger:debug('adding module node '..i)
      moduleNode = createNode(req.assignedNode.name, req.assignedNode.type, AST_ID, req.assignedNode.node)      
      graph:addNode(moduleNode)
      table.insert(nodes, moduleNode)
      globalModuleNodes[moduleNode.data.name] = moduleNode
    end
    
    logger:debug('adding requires edge')
    local newEdge = createEdge("requires", idNode, globalModuleNodes[req.assignedNode.name], true)    
    
    graph:addEdge(newEdge)
    table.insert(edges, newEdge)
    
  end
  return nodes, edges  
end

----------------------------------------------
-- Add nodes and edges from return values into passed tables
-- @param graph luadb type of graph
-- @param returns table of structured return values
-- @param AST_ID id of the current AST
-- @param fileName name of parsed file
-- @param nodes table of nodes
-- @param edges table of edges
-- @return extracted nodes and edges
local function addModuleReturnValuesToGraph(graph, returns, AST_ID, fileName, nodes, edges)
  local returns = returns or {}  
  local nodes = nodes or {}
  local edges = edges or {}
  graph.globalModuleNodes = graph.globalModuleNodes or {} 
  local globalModuleNodes = graph.globalModuleNodes
  
  local moduleName = utils.splitAndGetFirst(fileName, "%.")
  
  local retNode
  if (not utils.isEmpty(returns)) then
    -- add returns node
    logger:debug('adding returns node')
    retNode = createNode("Returns", "interface", AST_ID)
    graph:addNode(retNode)
    table.insert(nodes, retNode)
    
    -- connect returns node to module
    logger:debug('adding provides edge')
    local newEdge = createEdge("provides", globalModuleNodes[moduleName], retNode, true)
    
    graph:addEdge(newEdge)
    table.insert(edges, newEdge) 
  end
    
  logger:debug("importing return nodes")
  local interfaceNode
  for _,retValue in pairs(returns) do
    --retValue.(idNode|assignedNode)
    --assignedNode.(name|type|node)
    interfaceNode = createNode(retValue.idNode.name, retValue.idNode.type, AST_ID, retValue.idNode.node)
    graph:addNode(interfaceNode)
    table.insert(nodes, interfaceNode)
    
    local representsNode
    if(retValue.assignedNode ~= nil) then
      --find node that represents this interface      
      representsNode = getRepresentsNodeByName(retValue.assignedNode.name, graph)
    else      
      representsNode = getRepresentsNodeByName(retValue.idNode.name, graph)
    end
      
    if(representsNode ~= nil) then
      -- connect return node to representing node
      logger:debug('adding represents edge')
      local newEdge = createEdge("represents", representsNode, interfaceNode, true)
      graph:addEdge(newEdge)
      table.insert(edges, newEdge)
    end
    
    -- connect return node to module
    logger:debug('adding provides edge')
    local newEdge = createEdge("provides", retNode, interfaceNode, true)
    graph:addEdge(newEdge)
    table.insert(edges, newEdge)
  end
  return nodes, edges
end



-----------------------------------------------
-- Main functions
-----------------------------------------------

----------------------------------------------
-- Extract nodes and edges from assign calls
-- @param AST tree of parsed file
-- @param AST_ID id of the current AST
-- @param graph luadb type of graph
-- @param fileName name of parsed file
-- @param nodes table of nodes
-- @param edges table of edges
-- @return extracted nodes and edges
local function extractAssigns(AST, AST_ID, graph, fileName, nodes, edges)
  local nodes = nodes or {}
  local edges = edges or {}
  local localAssigns = ast.getLocalAssigns(AST)  
  local globalAssigns = ast.getAssigns(AST)
  nodes, edges = addAssignsToGraph(graph, localAssigns, AST_ID, fileName, nodes, edges)
  nodes, edges = addAssignsToGraph(graph, globalAssigns, AST_ID, fileName, nodes, edges)
  return nodes, edges
end

----------------------------------------------
-- Extract nodes and edges from require calls
-- @param AST tree of parsed file
-- @param AST_ID id of the current AST
-- @param graph luadb type of graph
-- @param fileName name of parsed file
-- @param nodes table of nodes
-- @param edges table of edges
-- @return extracted nodes and edges
local function extractRequireCalls(AST, AST_ID, graph, fileName, nodes, edges)
  local nodes = nodes or {}
  local edges = edges or {}
  local requireCalls = ast.getRequireCalls(AST)
  nodes, edges = addRequireNodesToGraph(graph, requireCalls.localRequires, AST_ID, fileName, nodes, edges)
  nodes, edges = addRequireNodesToGraph(graph, requireCalls.globalRequires, AST_ID, fileName, nodes, edges)
  return nodes, edges
end

----------------------------------------------
-- Extract nodes and edges from return values
-- @param AST tree of parsed file
-- @param AST_ID id of the current AST
-- @param graph luadb type of graph
-- @param fileName name of parsed file
-- @param nodes table of nodes
-- @param edges table of edges
-- @return extracted nodes and edges
local function extraModuleReturnValues(AST, AST_ID, graph, fileName, nodes, edges)
  local nodes = nodes or {}
  local edges = edges or {}
  local returnValues = ast.getModuleReturnValues(AST)
  nodes, edges = addModuleReturnValuesToGraph(graph, returnValues, AST_ID, fileName, nodes, edges)
  return nodes, edges
end

----------------------------------------------
-- Extract nodes and edges
-- @param AST tree of parsed file
-- @param AST_ID id of the current AST
-- @param graph luadb type of graph
-- @param fileName name of parsed file
-- @return extracted nodes and edges
local function extractNodesAndEdges(AST, AST_ID, graph, fileName)
  local nodes = {}
  local edges = {}
  nodes, edges = extractAssigns(AST, AST_ID, graph, fileName, nodes, edges)
  nodes, edges = extractRequireCalls(AST, AST_ID, graph, fileName, nodes, edges)
  nodes, edges = extraModuleReturnValues(AST, AST_ID, graph, fileName, nodes, edges)
  return nodes, edges
end


-----------------------------------------------
-- Extract
-----------------------------------------------

----------------------------------------------
-- Extract module artefacts
-- @param luaFileNode file node
-- @param graph luadb type of graph
-- @param astManager manager of ASTs
-- @return table of nodes and edges
local function extract(luaFileNode, graph, astManager)
  local path = luaFileNode.data.path
  local fileName = luaFileNode.data.name
  
  local graph = graph or hypergraph.graph.new()
  
  local AST, AST_ID = astManager:findASTByPath(path)
  if(AST == nil) then
    AST = ast.getAST(path)
    AST_ID = astManager:addAST(AST, path)
  end
  
  --add astID and astNodeID(root) to 'module' nodes
  addIdsToModuleNode(AST, AST_ID, graph, path)
  
  local nodes, edges = extractNodesAndEdges(AST, AST_ID, graph, fileName)

  return { nodes = nodes, edges = edges }
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract = extract
}
