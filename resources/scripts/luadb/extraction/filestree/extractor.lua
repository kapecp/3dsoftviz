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
    newNode.data.type = "directory"
    graph:addNode(newNode)
    parent = newNode
  end

  for file in lfs.dir(path) do
    if file ~= "." and file ~= ".." and not utils.isHidden(file) then
      local f = path..'/'..file
      local attr = lfs.attributes (f)
      assert (type(attr) == "table")
      
      -- new node
      local newNode = hypergraph.node.new()
      if attr.mode == "directory" then
        newNode.data.name = file
        newNode.data.path = f
        newNode.data.type = "directory"
        -- recursive call
        extractFilesTree(graph, f, newNode)
      else
        newNode.data.name = file
        newNode.data.path = f
        newNode.data.type = "file"
      end
      
      -- new edge
      local newEdge = hypergraph.edge.new()
      newEdge.label = "subfile"
      newEdge:addSource(parent)
      newEdge:addTarget(newNode)    
      newEdge:setAsOriented()
      
      graph:addNode(newNode)
      graph:addEdge(newEdge)
    end
  end
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract = extractFilesTree
}