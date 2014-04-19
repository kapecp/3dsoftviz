-----------------------------------------------
-- GRAPH - luadb graph module
-- @release 2013/12/03, Tomas Filcak
-----------------------------------------------

local pGraph = {
    _type = 'Graph'
}

local n = 0
local function inc()
  n = n + 1
  return n
end

local function G(arguments)
  local arguments = arguments or {}
  
  local o = {
      id = "G"..inc(),
      data = {},
      nodes = {},
      edges = {},
      incidences = {},
      label = nil
  }

  setmetatable(o,
    {
      __index = pGraph,
      __tostring = function(t)
        if t.id and t.label then return "graph "..t.id..":"..t.label
        elseif t.id then return "graph "..t.id
        else return "graph" end
      end
    }
  )
  
  -- passing arguments
  for key,value in pairs(arguments) do
    o[key] = value
  end

  return o
end

-------------------------------- Add functions

function pGraph:addNode(node)
  node.graph = self
  table.insert(self.nodes, node)
  if self.NodeMapper then
    self.NodeMapper:save(node)
  end
end


function pGraph:addEdge(edge)
  edge.graph = self
  table.insert(self.edges, edge)
  if self.EdgeMapper then
    self.EdgeMapper:save(edge)
  end
end

-------------------------------- Print functions

function pGraph:printNodes()
  for index,node in pairs(self.nodes) do
    if node.data.name then
      print("node with id: "..node['id'].." and name: "..node['data']['name'])
    else
      print("node with id: "..node['id'])
    end
  end
end


function pGraph:printEdges()
  for index,edge in pairs(self.edges) do
    -- directed edge
    if edge.from and edge.to then
      
      local from = " from nodes " .. table.concat(edge.from, ", ")
      local to   = " to " .. table.concat(edge.to, ", ")
      print("edge with id: ".. edge.id .. from .. to)
      
    -- undirected edge
    elseif edge.nodes then
      local nodes = "nodes: " .. table.concat(edge.nodes, ", ")
      print("edge with id: ".. edge.id .. "connecting nodes: " .. nodes)
    end
  end
end

-------------------------------- Find functions
 
-- get all nodes with selected name
function pGraph:findNodeByName(name)
  local occurrences = {}
  for i,node in pairs(self.nodes) do
    if node.data.name and (node.data.name == name) then
      table.insert(occurrences, node)
    end
  end
  return occurrences
end
 
 
-- get all ids for nodes with selected name
function pGraph:findNodeIdsByName(name)
  local occurrence_ids = {}
  for i,node in pairs(self.nodes) do
    if node.data.name and (node.data.name == name) then
      table.insert(occurrence_ids, node.id)
    end
  end
  return occurrence_ids
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = G
}