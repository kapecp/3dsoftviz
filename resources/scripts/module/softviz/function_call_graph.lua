-----------------------------------------------
-- Function call graph - module for extracting 
-- function calls graph from given project
-- @release 2014/05/19, Frantisek Nagy
-----------------------------------------------
local artifactsExtractor = require "luadb.extraction.extractor"
local utils              = require "luadb.utils"

----------------------------------------------
-- Local graph stored after extraction
local graph = {}

----------------------------------------------
-- Increment function for generating of ids
local function inc()
  a = a or 0
  a = a + 1
  return a
end

--------------------------------------------------
-- Set color for node based on node type
local function setNodeColor(node)
  node.params.color = {}
  if node.params.type == 'file' or node.params.type == 'module' then 
    node.params.color.A = 1
    node.params.color.R = 1
    node.params.color.G = 0
    node.params.color.B = 1
  end

  if node.params.type == 'directory' then 
    node.params.color.A = 1
    node.params.color.R = 0
    node.params.color.G = 1
    node.params.color.B = 1
  end

  if node.params.type == 'global function' then 
    node.params.color.A = 1
    node.params.color.R = 0
    node.params.color.G = 0
    node.params.color.B = 1
  end
end

----------------------------------------------
-- Convert nodes from LuaDB format to import 
-- format for 3DSoftviz
-- @param v node in LuaDB format
-- @param nodes table with created nodes indexed by id
-- @param minComplexity minimal complexity found in graph so far
-- @param maxComplexity maximum complexity found in graph so far
-- @param minLines minimal LOC found in graph so far
-- @param maxLines maximum LOC found in graph so far
-- @return minComplexity, maxComplexity, minLines, maxLines
local function extractNode(v, nodes, minComplexity, maxComplexity, minLines, maxLines)
  local origname = v.data.name or v.id
  local newnode = {type = "node", id = inc(), label = origname, params={size = 8, name = origname, origid = v.id, type = v.meta.type, path = v.data.path, modulePath = v.meta.modulePath, color = {A = 1, R = 1, G = 1, B = 1}, position = v.data.position}}
  if newnode.id == 1 then newnode.params.root = true end
  nodes[v] = newnode
  setNodeColor(newnode)
 
  if v.meta.type == 'function' then
    newnode.params.tag = v.data.tag       
    if v.data.metrics ~= nil then
      newnode.params.metrics = {}
      newnode.params.metrics.halstead = v.data.metrics.halstead
      newnode.params.metrics.cyclomatic = v.data.metrics.cyclomatic
      newnode.params.metrics.LOC = v.data.metrics.LOC
      newnode.params.metrics.infoflow = v.data.metrics.infoflow

      --! BUG - `used_nodes` have `parent` -> cyclic AST -> crash in Diluculum
      newnode.params.metrics.infoflow.used_nodes = nil
      --! BUG - `hypergraphnode` points to an ATS node, which have `parent` -> cyclic AST -> crash in Diluculum
      newnode.params.metrics.infoflow.hypergraphnode = nil
      newnode.params.metrics.LOC.hypergraphnode = nil

      minComplexity = (minComplexity and (minComplexity < newnode.params.metrics.cyclomatic.upperBound and minComplexity)) or newnode.params.metrics.cyclomatic.upperBound
      maxComplexity = (maxComplexity and (maxComplexity > newnode.params.metrics.cyclomatic.upperBound and maxComplexity)) or newnode.params.metrics.cyclomatic.upperBound
      minLines = (minLines and (minLines < newnode.params.metrics.LOC.lines and minLines)) or newnode.params.metrics.LOC.lines
      maxLines = (maxLines and (maxLines > newnode.params.metrics.LOC.lines and maxLines)) or newnode.params.metrics.LOC.lines
    end
  end
  return minComplexity, maxComplexity, minLines, maxLines
end

--------------------------------------------------
-- Set color for edge based on edge type
local function setEdgeColor(edge)
  edge.params.color = {}
  if edge.params.type == 'calls' then
    edge.params.color.A = 1
    edge.params.color.R = 0.8
    edge.params.color.G = 0.8
    edge.params.color.B = 1
  elseif edge.params.type == 'contains' then
    edge.params.color.A = 1
    edge.params.color.R = 1
    edge.params.color.G = 0.8
    edge.params.color.B = 0.8
  elseif edge.params.type == 'contains' then
    edge.params.color.A = 1
    edge.params.color.R = 0.8
    edge.params.color.G = 1
    edge.params.color.B = 0.8
  end
end

-------------------------------------------------------
-- Convert edge from LuaDB format to import 
-- format for 3DSoftviz
-- @param v edge in LuaDB format
-- @param existingedges table with already converted edges
-- @param nodes table of extracted nodes
local function extractEdge(v, existingedges, nodes)
  if #v.from ~= 1 then print('from', #v.from, v.id, v.from[1], v.from[2]) end 
  if #v.to ~= 1 then print('to', #v.to, v.id, v.to[1], v.to[2]) end    
  local ind = v.from[1].id .. "|" .. v.to[1].id
  if existingedges[ind] ~= nil then
    existingedges[ind].params.count = existingedges[ind].params.count + 1
  else
    local edge = {type = "edge", id = inc(), params = {origid = v.id, count = 1, edgeStrength = 2}}
    local incid1 = {type = "edge_part", id = inc(), label = ''}
    local incid2 = {type = "edge_part", id = inc(), label = ''}
    if v.from[1].meta.type == 'function' or (v.from[1].meta.type == 'file' and v.to[1].meta.type == 'globalFunction') then
      edge.params.type = "calls"
      incid1.direction = 'in'
      incid2.direction = 'out'
    end
    if v.from[1].meta.type == 'file' then 
      edge.params.type = 'contains'
    end
    if v.from[1].meta.type == 'directory' then 
      edge.params.type = 'contains'
    end

    edge.label = edge.params.type
    if v.from[1].meta.modulePath ~= v.to[1].meta.modulePath then
      edge.params.edgeStrength = 0.1
    elseif v.from[1].meta.modulePath ~= nil then
      edge.params.edgeStrength = 0.5
    end 
    graph[edge] = {[incid1] = nodes[v.from[1]], [incid2] = nodes[v.to[1]]}
    existingedges[ind] = edge
    setEdgeColor(edge)
  end
end

-------------------------------------------------------------------------
-- Set color and size for nodes representing functions
-- based on cyclomatic complexity and LOC metrics
local function doVisualMapping(nodes, minComplexity, maxComplexity, minLines, maxLines)
  local minSize = 8
  local maxSize = 100
  for k, n in pairs(nodes) do
    if n.params.type == 'function' then
      if n.params.metrics ~= nil then
        n.params.size = minSize + (n.params.metrics.LOC.lines - minLines) / (maxLines - minLines) * (maxSize - minSize)
        local complexRatio = (n.params.metrics.cyclomatic.upperBound - minComplexity) / (maxComplexity - minComplexity) 
        n.params.color.G = 1 - complexRatio
        n.params.color.R = complexRatio
        n.params.color.B = 0
        
      else
        n.params.color.G = 0
        n.params.color.R = 0
        n.params.color.B = 1
      end
    end
  end
end

-------------------------------------------------
-- Extract function call graph from project and
-- convert it to format for importing to 3DSoftviz
-- @param absolutePath path to project being analysed
local function extractGraph(absolutePath)
  graph = {}
  utils.logger:setLevel(utils.logging.INFO)

  utils.logger:info("started extraction")
  local extractedGraph = artifactsExtractor.extract(absolutePath)
  utils.logger:info("extraction successfully finished")

  --extractedGraph:printNodes()
  --extractedGraph:printEdges()

  local nodes = {}
  local minComplexity, maxComplexity
  local minLines, maxLines

  for _, v in ipairs(extractedGraph.nodes) do
    minComplexity, maxComplexity, minLines, maxLines = extractNode(v, nodes, minComplexity, maxComplexity, minLines, maxLines)
  end

  print("complexity", minComplexity, maxComplexity, minLines, maxLines)

  local existingedges = {}

  for _, v in ipairs(extractedGraph.edges) do
    extractEdge(v, existingedges, nodes)
  end
  
  doVisualMapping(nodes, minComplexity, maxComplexity, minLines, maxLines)
end

-------------------------------------
-- Function for retreiving extracted graph
local function getGraph()
  print"getting function graph"
  return graph
end
-------------------------------------
-- Public interface of module
return {extractGraph = extractGraph,
  getGraph = getGraph}
