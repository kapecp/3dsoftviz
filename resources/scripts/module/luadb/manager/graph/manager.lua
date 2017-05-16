-----------------------------------------------
-- LuaDBGraph Manager
-- @release 2017/03/15, Denis Illes
-----------------------------------------------

local utils = require "luadb.utils"

-----------------------------------------------
-- Graph Manager
-----------------------------------------------
local pGraphManager = {
    _type = 'Graph_Manager'
}

local function GraphManager(arguments)
  local arguments = arguments or {}
  
  local o = {
    label = nil,
    graph = {}
  }

  setmetatable(o,
    {
      __index = pGraphManager,
      __tostring = function(t)
        if t.label then
          return "Grapg Manager: "..t.label
        else
          return "Graph Manager"
        end
      end
    }
  )
  
  -- passing arguments
  for key,value in pairs(arguments) do
    o[key] = value
  end

  return o
end

function pGraphManager:addLabel(label)
  self.label = label  
end

function pGraphManager:addGraph(graph)
  local GraphNode = {
    id = graph['id'],
    root = graph
  }
  table.insert(self.graph, GraphNode)
  return GraphNode.id
end

-----------------------------------------------
-- Print functions
-----------------------------------------------

function pGraphManager:printGraphss()
  for index,graph in pairs(self.graph) do
    print("Graph with ID: "..graph['id'])
  end
end

-----------------------------------------------
-- Remove functions
-----------------------------------------------

-- remove Graph by id
function pGraphManager:removeGraphByID(graphID)
  for index,graph in pairs(self.graph) do
    if graph.id == graphID then
      self.graph[index] = nil
      utils.logger:debug('Graph with ID = ' .. graphID ..' deleted from GraphManager')
      return true
    end
  end
  return false
end

-----------------------------------------------
-- Find functions
-----------------------------------------------

-- get Graph with selected id
function pGraphManager:findGraphByID(id)  
  for _,graph in pairs(self.graph) do
    if graph.id == id then
      return graph.root
    end
  end
  return nil
end

-- get first Graph
function pGraphManager:findFirstGraph()
  if #self.graph > 0 then
    return self.graph[1].root
  end
  utils.logger:debug('GraphManager is empty')
  return nil
end

-- get last Graph
function pGraphManager:findLastGraph()
  if #self.graph > 0 then
    return self.graph[#self.graph].root
  end
  utils.logger:debug('GraphManager is empty')
  return nil
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = GraphManager
}