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
      nodes = {},
      edges = {},
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
  table.insert(self.nodes, node)
  if self.NodeMapper then
    self.NodeMapper:save(node)
  end
end


function pGraph:addEdge(edge)
  table.insert(self.edges, edge)
  if self.EdgeMapper then
    self.EdgeMapper:save(edge)
  end
end

-------------------------------- Remove functions
--[[
function pGraph:removeNode(node)
  table.insert(self.nodes, node)
  if self.NodeMapper then
    self.NodeMapper:save(node)
  end
end


function pGraph:removeEdge(edge)
  table.insert(self.edges, edge)
  if self.EdgeMapper then
    self.EdgeMapper:save(edge)
  end
end
--]]

-------------------------------- Print functions

function pGraph:printNodes()
  for index,node in pairs(self.nodes) do
    if node.data.name then
      print("NODE with ID: "..node['id'].." NAME: "..node['data']['name'])
    else
      print("NODE with ID: "..node['id'])
    end
  end
end


local function concatNodes(str, nodes)
  for i,node in pairs(nodes) do
    if type(node) == "table" and node.data and node.data.name then
      str = str.." "..node.data.name
    elseif type(node) == "table" and node.id then
      str = str.." "..node.id
    else
      str = str.." "..node
    end
  end
  return str
end


function pGraph:printEdges()
  for index,edge in pairs(self.edges) do
    if edge.from and edge.to then
        local from = concatNodes(" FROM", edge.from)
        local to = concatNodes(" TO", edge.to)
        print("EDGE with ID: ".. edge.id .. from .. to)
    end
  end
end

-------------------------------- Find functions
 
-- get all nodes with selected id
function pGraph:findNodeByID(id)
  for i,node in pairs(self.nodes) do
    if (node.id == id) then
      return node
    end
  end
end

-- get all nodes with selected name
function pGraph:findNodesByName(name)
  local occurrences = {}
  for i,node in pairs(self.nodes) do
    if node.data.name and (node.data.name == name) then
      table.insert(occurrences, node)
    end
  end
  return occurrences
end

 -- get all nodes with selected type
function pGraph:findNodesByType(type)
  local occurrences = {}
  for i,node in pairs(self.nodes) do
    if node.meta.type and (node.meta.type == type) then
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

-- get all edges with selected source ID and selected label
function pGraph:findEdgesBySource(sourceID, label)
  local occurrences = {}
  for i,edge in pairs(self.edges) do
    if (edge.label == label) and (edge.from[1].id == sourceID) then
      table.insert(occurrences, edge)
    end
  end
  return occurrences
end

-- get all edges with selected label
function pGraph:findEdgesByLabel(label)
  local occurrence = {}
  for i,edge in pairs(self.edges) do
    if edge.label and (edge.label == label) then
      table.insert(occurrence, edge)
    end
  end
  return occurrence
end

-- get all edges with selected source ID and selected label
function pGraph:findEdgesBySource(sourceID, label)
  local occurrences = {}
  for i,edge in pairs(self.edges) do
    if (edge.label == label) and (edge.from[1].id == sourceID) then
      table.insert(occurrences, edge)
    end
  end
  return occurrences
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = G
}