-----------------------------------------------
-- Modules Extractor
-- @release 2016/12/11, Denis Illes
-----------------------------------------------

local hypergraph = require "luadb.hypergraph"
local utils      = require "luadb.utils"
local ast        = require "luadb.ast"
local mutils     = require "metrics.utils"
local logger     = utils.logger

local function findNodeByNameAndType(name, nodeType, graph)
  local nodes = graph:findNodeByType(nodeType)
  for _,node in pairs(nodes) do
    if(node.data.name == name) then
      print("found in " .. nodeType)
      return node
    end
  end
  return nil
end


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

local function extractAssigns(AST, AST_ID, graph, fileName, nodes, edges)
  local nodes = nodes or {}
  local edges = edges or {}
  
  local localAssigns = ast.getLocalAssigns(AST)
  local globalAssigns = ast.getAssigns(AST)
  
  graph.globalModuleNodes = graph.globalModuleNodes or {} 
  local globalModuleNodes = graph.globalModuleNodes
  
  local moduleName = utils.splitAndGetFirst(fileName, "%.")
  
  logger:debug("importing nodes")
  -- local assigns:
  for i,localAssign in pairs(localAssigns) do
    local nodeName, node
    if (localAssign.assignedNode) then
      if (localAssign.assignedNode.functionName) then
        nodeName = localAssign.assignedNode.functionName
        node = localAssign.assignedNode.node
      else
        nodeName = localAssign.assignedNode.text
        node = localAssign.assignedNode
      end
    else
      nodeName = nil
      node = {}
    end
    
    logger:debug('adding var node '..i)
    local varNode = hypergraph.node.new()
    varNode.meta = {}
    --varNode.data.metrics = localAssign.idNode.data.metrics
    varNode.data.name = localAssign.idNode.text
    varNode.data.type = "local variable"
    varNode.data.position = localAssign.idNode.position
    varNode.data.astID = AST_ID
    graph:addNode(varNode)
    table.insert(nodes, varNode)
    
    logger:debug('adding var init edge')
    local newEdge = hypergraph.edge.new()
    newEdge.label = "initializes"
    newEdge:addSource(globalModuleNodes[moduleName])
    newEdge:addTarget(varNode)
    newEdge:setAsOriented()
    graph:addEdge(newEdge)
    table.insert(edges, newEdge)
    
    logger:debug('adding var node '..i)
    local assignNode = hypergraph.node.new()
    assignNode.meta = {}
    --assignNode.data.metrics = node.data or {}
    assignNode.data.name = nodeName
    assignNode.data.type = "N/A"
    assignNode.data.position = node.position or {}
    assignNode.data.astID = AST_ID
    graph:addNode(assignNode)
    table.insert(nodes, assignNode)
    
    logger:debug('adding var assign edge')
    local newEdge = hypergraph.edge.new()
    newEdge.label = "assigns"
    newEdge:addSource(varNode)
    newEdge:addTarget(assignNode)
    newEdge:setAsOriented()
    graph:addEdge(newEdge)
    table.insert(edges, newEdge)    
  end
  
  -- global assigns:
  for i,globalAssign in pairs(globalAssigns) do
    local nodeName, node
    if (globalAssign.assignedNode) then
      if (globalAssign.assignedNode.functionName) then
        nodeName = globalAssign.assignedNode.functionName
        node = globalAssign.assignedNode.node
      else
        nodeName = globalAssign.assignedNode.text
        node = globalAssign.assignedNode
      end
    else
      nodeName = nil
      node = {}
    end
    
    logger:debug('adding var node '..i)
    local varNode = hypergraph.node.new()
    --varNode.data.metrics = globalAssign.idNode.data.metrics
    varNode.data.name = globalAssign.idNode.text
    varNode.data.type = "global variable"
    varNode.data.position = globalAssign.idNode.position
    varNode.data.astID = AST_ID
    graph:addNode(varNode)
    table.insert(nodes, varNode)
    
    logger:debug('adding var init edge')
    local newEdge = hypergraph.edge.new()
    newEdge.label = "initializes"
    newEdge:addSource(globalModuleNodes[moduleName])
    newEdge:addTarget(varNode)
    newEdge:setAsOriented()
    graph:addEdge(newEdge)
    table.insert(edges, newEdge)
    
    logger:debug('adding var node '..i)
    local assignNode = hypergraph.node.new()
    --assignNode.data.metrics = node.data.metrics or {}
    assignNode.data.name = nodeName
    assignNode.data.type = "something"
    assignNode.data.position = node.position or {}
    assignNode.data.astID = AST_ID
    graph:addNode(assignNode)
    table.insert(nodes, assignNode)
    
    logger:debug('adding var assign edge')
    local newEdge = hypergraph.edge.new()
    newEdge.label = "assigns"
    newEdge:addSource(varNode)
    newEdge:addTarget(assignNode)
    newEdge:setAsOriented()
    graph:addEdge(newEdge)
    table.insert(edges, newEdge)    
  end
  return nodes, edges
end


local function extractRequireCalls(AST, AST_ID, graph, fileName, nodes, edges)
  local nodes = nodes or {}
  local edges = edges or {}
  local requireCalls = ast.getRequireCalls(AST)
  local localRequireCalls = requireCalls.localRequires
  local globalRequireCalls = requireCalls.globalRequires
  
  graph.globalModuleNodes = graph.globalModuleNodes or {} 
  local globalModuleNodes = graph.globalModuleNodes
  
  local moduleName = utils.splitAndGetFirst(fileName, "%.")
  
  logger:debug("importing nodes")
  -- local requires:
  for i,localRequireCall in pairs(localRequireCalls) do
    logger:debug('adding var node '..i)
    local varNode = hypergraph.node.new()
    --varNode.data.metrics = localRequireCall.node.data.metrics
    varNode.data.name = localRequireCall.fullVarName
    varNode.data.type = "local variable"
    varNode.data.position = localRequireCall.node.position
    varNode.data.astID = AST_ID
    graph:addNode(varNode)
    table.insert(nodes, varNode)
    
    logger:debug('adding var init edge')
    local newEdge = hypergraph.edge.new()
    newEdge.label = "initializes"
    newEdge:addSource(globalModuleNodes[moduleName])
    newEdge:addTarget(varNode)
    newEdge:setAsOriented()
    graph:addEdge(newEdge)
    table.insert(edges, newEdge)
    
    local requiredNode_temp = localRequireCall.moduleName:gsub("'", "")
    local requiredNode = globalModuleNodes[requiredNode_temp]
    if (not requiredNode) then
      logger:debug('adding var node '..i)
      requiredNode = hypergraph.node.new()
      requiredNode.meta = {}
      --requiredNode.data.metrics = localRequireCall.node.metrics
      requiredNode.data.name = requiredNode_temp
      
      requiredNode.data.type = "module"
      requiredNode.data.position = localRequireCall.node.position
      requiredNode.data.astID = AST_ID
      graph:addNode(requiredNode)
      table.insert(nodes, requiredNode)
      globalModuleNodes[requiredNode.data.name] = requiredNode
    end    
    
    logger:debug('adding require assign edge')
    local newEdge = hypergraph.edge.new()
    newEdge.label = "requires"
    newEdge:addSource(varNode)
    newEdge:addTarget(globalModuleNodes[requiredNode_temp])
    newEdge:setAsOriented()
    graph:addEdge(newEdge)
    table.insert(edges, newEdge) 
  end
  
  -- global requires
  for i,globalRequireCall in pairs(globalRequireCalls) do
    logger:debug('adding var node '..i)
    local varNode = hypergraph.node.new()
    --varNode.data.metrics = globalRequireCall.node.data.metrics
    varNode.data.name = globalRequireCall.fullVarName
    varNode.data.type = "global variable"
    varNode.data.position = globalRequireCall.node.position
    varNode.data.astID = AST_ID
    graph:addNode(varNode)
    table.insert(nodes, varNode)
    
    logger:debug('adding var init edge')
    local newEdge = hypergraph.edge.new()
    newEdge.label = "initializes"
    newEdge:addSource(globalModuleNodes[moduleName])
    newEdge:addTarget(varNode)
    newEdge:setAsOriented()
    graph:addEdge(newEdge)
    table.insert(edges, newEdge)
    
    local requiredNode = globalModuleNodes[globalRequireCall.moduleName]
    if (not requiredNode) then
      logger:debug('adding var node '..i)
      requiredNode = hypergraph.node.new()
      requiredNode.meta = {}
      --requiredNode.data.metrics = globalRequireCall.node.metrics
      requiredNode.data.name = globalRequireCall.moduleName
      requiredNode.data.type = "module"
      requiredNode.data.position = globalRequireCall.node.position
      requiredNode.data.astID = AST_ID
      graph:addNode(requiredNode)
      table.insert(nodes, requiredNode)
      globalModuleNodes[requiredNode.data.name] = requiredNode
    end    
    
    logger:debug('adding require assign edge')
    local newEdge = hypergraph.edge.new()
    newEdge.label = "requires"
    newEdge:addSource(varNode)
    newEdge:addTarget(globalModuleNodes[globalRequireCall.moduleName])
    newEdge:setAsOriented()
    graph:addEdge(newEdge)
    table.insert(edges, newEdge) 
  end
  
  return nodes, edges
end

local function extraModuleReturnValues(AST, AST_ID, graph, fileName, nodes, edges)
  local nodes = nodes or {}
  local edges = edges or {}
  local returnValues = ast.getModuleReturnValues(AST)
  
  graph.globalModuleNodes = graph.globalModuleNodes or {}
  local globalModuleNodes = graph.globalModuleNodes
  
  local moduleName = utils.splitAndGetFirst(fileName, "%.")
  if (not utils.isEmpty(returnValues)) then
    -- add return node
    logger:debug('adding return node')
    retNode = hypergraph.node.new()
    retNode.meta = {}
    retNode.data.name = "return"
    retNode.data.type = "interface"
    --retNode.data.position = ?
    retNode.data.astID = AST_ID
    graph:addNode(retNode)
    table.insert(nodes, retNode)
    
    -- connect return node to module
    logger:debug('adding provides edge')
    local newEdge = hypergraph.edge.new()
    newEdge.label = "provides"
    newEdge:addSource(globalModuleNodes[moduleName])
    newEdge:addTarget(retNode)
    newEdge:setAsOriented()
    graph:addEdge(newEdge)
    table.insert(edges, newEdge) 
  end 
  
  logger:debug("importing nodes")
  for _,retValue in pairs(returnValues) do
    local idName = retValue.id or retValue.functionName or retValue.name
    local name = retValue.name or retValue.functionName
    logger:debug('adding return node')
    interfaceNode = hypergraph.node.new()
    interfaceNode.meta = {}
    interfaceNode.data.name = idName
    interfaceNode.data.type = "interface"
    interfaceNode.data.position = retValue.node.position
    interfaceNode.data.astID = AST_ID
    graph:addNode(interfaceNode)
    table.insert(nodes, interfaceNode)
    
    --find node that represents this interface
    print("searching for " .. name)
    representsNode = getRepresentsNodeByName(name, graph)
    if(representsNode) then
      -- connect return node to representing node
      logger:debug('adding represents edge')
      local newEdge = hypergraph.edge.new()
      newEdge.label = "represents"
      newEdge:addSource(representsNode)
      newEdge:addTarget(interfaceNode)
      newEdge:setAsOriented()
      graph:addEdge(newEdge)
      table.insert(edges, newEdge)
    end
        
    -- connect return node to module
    logger:debug('adding provides edge')
    local newEdge = hypergraph.edge.new()
    newEdge.label = "provides"
    newEdge:addSource(retNode)
    newEdge:addTarget(interfaceNode)
    newEdge:setAsOriented()
    graph:addEdge(newEdge)
    table.insert(edges, newEdge)
  end
  

  return nodes, edges
end



local function extractNodesAndEdges(AST, AST_ID, graph, fileName)
  local nodes = {}
  local edges = {}
  nodes, edges = extractAssigns(AST, AST_ID, graph, fileName, nodes, edges)
  nodes, edges = extractRequireCalls(AST, AST_ID, graph, fileName, nodes, edges)
  nodes, edges = extraModuleReturnValues(AST, AST_ID, graph, fileName, nodes, edges)
  
  return nodes, edges
end



local function extract(luaFileNode, graph, astManager)
  local path = luaFileNode.data.path
  local fileName = luaFileNode.data.name
  --print(fileName)
  local graph = graph or hypergraph.graph.new()
  
  local AST, AST_ID = astManager:findASTByPath(path)
  if(AST == nil) then
    AST = ast.getAST(path)
    AST_ID = astManager:addAST(AST, path)
  end
    
  
  --local locAssigns = ast.getLocalAssigns(AST)
  --[[
  for i,j in pairs(locAssigns) do
    local name
    if (not j.AssignedNode) then
      name = ""
    elseif (j.AssignedNode.text) then
      name = j.AssignedNode.text
    else
      --is function
      name = j.AssignedNode.FunctionName
    end
    
    print(i .. ": " .. j.IDNode.text .. " = " .. name)
  end
  ]]--
  --[[
  local assigns = ast.getAssigns(AST)
  for i,j in pairs(assigns) do
    local name
    if (not j.AssignedNode) then
      name = ""
    elseif (j.AssignedNode.text) then
      name = j.AssignedNode.text
    else
      --is function
      name = j.AssignedNode.FunctionName
    end
    
    print(i .. ": " .. j.IDNode.text .. " = " .. name)
  end
  ]]--

  --[[
  local requireCalls = ast.getRequireCalls(AST)
  
  local reqCallLoc = requireCalls.localRequires
  local reqCallGlob = requireCalls.globalRequires
  ]]--
  
  --debug.Save("nodes", localassign)
  local nodes, edges = extractNodesAndEdges(AST, AST_ID, graph, fileName)
  --local nodes = extractNodes(AST, graph, fileName)
  --local edges = extractEdges(AST, graph, nodes)
  return { nodes = nodes, edges = edges }
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract = extract
}
