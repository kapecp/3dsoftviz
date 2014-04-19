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


local function extractArtifacts(sourcePath)
  assert(sourcePath and utils.fileExists(sourcePath), "wrong path passed")
  local sourceDirectory = sourcePath or lfs.currentdir()
  local artifactsGraph = hypergraph.graph.new()
  filestree.extract(artifactsGraph, sourceDirectory)
  
  for i,node in pairs(artifactsGraph.nodes) do
    if node.data and node.data.type == "file" and utils.isLuaFile(node.data.name) then
      -- extract function calls
      local functionCallsGraph = hypergraph.graph.new()
      functioncalls.extract(functionCallsGraph, node.data.path)
      artifactsGraph:addNode(functionCallsGraph)
      
      -- add connection to new graph
      local connection = hypergraph.edge.new()
      connection.label = "nested graph"
      connection:addSource(node)
      connection:addTarget(functionCallsGraph)
      artifactsGraph:addEdge(connection)
    end
  end
  
  return artifactsGraph
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract = extractArtifacts
}