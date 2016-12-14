-----------------------------------------------
-- Main Extractor
-- @release 2014/04/18, Tomas Filcak
-----------------------------------------------

local hypergraph    = require "luadb.hypergraph"
local utils         = require "luadb.utils"
local filestree     = require "luadb.extraction.filestree"
local modules       = require "luadb.extraction.modules"
local lfs           = require "lfs"
local logger        = utils.logger
local debug         = require "dbg2"



local function getFilesTree(graph, path)
  filestree.extract(path, graph)
end


local function getModules(graph, luaFileNodes)  
  for i,luaFileNode in pairs(luaFileNodes) do
    -- for every lua file do the following
    local modules = modules.extract(luaFileNode.data.path, graph)
    --luaFileNode.moduleNodes = modules.nodes //no need for now
    --luaFileNode.moduleCalls = modules.edges //no need for now
    
    -- connect all variables to file node
    for _,moduleNode in pairs(modules.nodes) do
      if moduleNode.data.type == "variable" then
        local connection = hypergraph.edge.new()
        connection.label = "InFile"
        connection:addSource(luaFileNode)
        connection:addTarget(moduleNode)
        connection:setAsOriented()
        graph:addEdge(connection)
      end
    end    

  end
  --debug.Save("graph", graph)
end

local function mergeModuleMetrics(metrics1, metrics2)
  local tempTable = utils.shallowCopy(metrics1)
  for key,value in pairs(metrics2) do 
    if tempTable[key] == nil then
      tempTable[key] = value    
    elseif type(metrics1[key]) == "number" then
      tempTable[key] = tempTable[key] + value
    end
  end
  return tempTable
end


local function mergeModules(graph)  
  local addedNodeIds = {}
  for i,node1 in pairs(graph.nodes) do
    --get only module types
    if node1.data.type == "module" then
      for j,node2 in pairs(graph.nodes) do
        local haveToAdd = false;
        -- dont compare the same nodes
        if i < j and node2.data.type == "module" then
          --print("i = "..i)
          --print("j = "..j)
          if node2.data.modulePath == node1.data.modulePath then
            --duplicate found can start merging
            print("found module duplicate = ".. node1.data.modulePath)
            
            local mergedModule = {}
                        
            local sameModules = graph:findNodeByType("mergedModule")

            --search through graph if partialy mergedModule exists
            local found = false
            for _,node in pairs(sameModules) do
              assert(node.data.type == "mergedModule")
              if node.data.modulePath == node1.data.modulePath then
                mergedModule = graph:findNodeById(node.id)
                found = true
                break                
              end
            end
            
            if not found then
              print("we have to add it")
              mergedModule = hypergraph.node.new()            
              mergedModule.data = utils.shallowCopy(node1.data)
              --print("i = " .. i .. ", j = " .. j)
              --print("Before: " .. node1.data.name)
              mergedModule.data.type = "mergedModule"
              mergedModule.data.name = "merged_" .. mergedModule.data.name
              --print("After: " .. node1.data.name)
              --there were no mergedModules -> create new one                
              haveToAdd = true
              table.insert(addedNodeIds, node1.id)
              graph:addNode(mergedModule) 
            end
            
            --search through all edges in graph
            for key,edge in pairs(graph.edges) do
              local connection = hypergraph.edge.new()
              
              if edge.to[1].id == node1.id then
                connection.label = "RequireCallMerged"
                connection:addSource(edge.from[1])
                connection:addTarget(mergedModule)
                connection:setAsOriented()
                --print("adjusting edge to new node")
                graph:addEdge(connection)
                break
              end              
            end
            
            --search through all edges in graph
            for key,edge in pairs(graph.edges) do
              local connection = hypergraph.edge.new()
              
              if edge.to[1].id == node2.id then
                connection.label = "RequireCallMerged"
                connection:addSource(edge.from[1])
                connection:addTarget(mergedModule)
                connection:setAsOriented()
                --print("adjusting edge to new node")
                graph:addEdge(connection)
                break
              end              
            end
            
            -- check if node was already merged
            local alreadyAdded = false
            for _,id in pairs(addedNodeIds) do
              if id == node2.id then
                alreadyAdded = true
                break                
              end
            end
            
            -- if not -> merge
            if not alreadyAdded then              
              --merge node2 and node1 metrics into mergedModule
              mergedModule.data.metrics = mergeModuleMetrics(mergedModule.data.metrics, node2.data.metrics)
              table.insert(addedNodeIds, node2.id)
            end
          end
        end
      end
    end
  end
end

local function connectModuleImplementations(graph)
  for i,moduleNode in pairs(graph.nodes) do
    --get only module types for i cycle
    if moduleNode.data.type == "module" then
      for j,fileNode in pairs(graph.nodes) do
        --get only file types for j cycle
        if fileNode.data.type == "file" then
          local fileName = utils.splitAndGetFirst(fileNode.data.name, "%.")          
          local moduleName = moduleNode.data.name
          
          --if they are the same, connect them
          if fileName == moduleName then
            local connection = hypergraph.edge.new()
            connection.label = "Implements"
            connection:addSource(fileNode)
            connection:addTarget(moduleNode)
            connection:setAsOriented()
            --print("adjusting edge to new node")
            graph:addEdge(connection)
          end
        end
      end
    end
  end
end

local function mergeUnassignedVariables(graph)
end

  

local function clearTmpVars(graph)
  graph.luaFileNodes = nil
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
  
  --get lua files in dir recursively and save as graph.luaFileNodes[]
  getFilesTree(graph, sourceDirectory)
    
  getModules(graph, graph.luaFileNodes)
  connectModuleImplementations(graph)
  --mergeUnassignedVariables(graph)
  --mergeModules(graph)
  
  --connectModuleCalls(graph)
  --assignGlobalCalls(graph)
  
  clearTmpVars(graph)
  debug.Save("graph", graph)

  return graph
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract = extract
}
