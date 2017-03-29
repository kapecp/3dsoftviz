-----------------------------------------------
-- Files Tree Extractor
-- @release 2014/04/17, Tomas Filcak
-----------------------------------------------

local hypergraph = require "luadb.hypergraph"
local utils      = require "luadb.utils"
local lfs        = require "lfs"


local function getNameFromPath(path, delimiter)
  assert(type(path) == "string", "path must be a string")
  local parts = utils.split(path, delimiter)
  return parts[table.getn(parts)]
end


local function extractFilesTree(graph, path, parent)
  -- root node
  if not parent then
    local newNode = hypergraph.node.new()
    newNode.data.name = getNameFromPath(path, "/")
    newNode.data.path = path
    newNode.meta = newNode.meta or {}
    newNode.meta.type = "directory"
    graph:addNode(newNode)
    parent = newNode
  end

  for file in lfs.dir(path) do
    if file ~= "." and file ~= ".." and not utils.isHidden(file) then
      local fullPath = path..'/'..file
      local attr = lfs.attributes (fullPath)
      assert(type(attr) == "table")
      
      -- new node
      local newNode = hypergraph.node.new()
      newNode.meta = newNode.meta or {}
      if attr.mode == "directory" then
        newNode.data.name = file
        newNode.data.path = fullPath
        newNode.meta.type = "directory"
        -- recursive call
        extractFilesTree(graph, fullPath, newNode)
      else
        newNode.data.name = file
        newNode.data.path = fullPath
        newNode.meta.type = "file"
        graph.luaFileNodes = graph.luaFileNodes or {}
        if utils.isLuaFile(file) then table.insert(graph.luaFileNodes,newNode) end
      end
      
      -- new edge
      local newEdge = hypergraph.edge.new()
      newEdge.label = "contains"
      newEdge:addSource(parent)
      newEdge:addTarget(newNode)
      newEdge:setAsOriented()
      
      graph:addNode(newNode)
      graph:addEdge(newEdge)
    end
  end
end


local function extract(path, graph)
  local graph = graph or hypergraph.graph.new()
  extractFilesTree(graph, path)
  return graph
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract = extract
}
