local artifactsExtractor = require "luadb.extraction.extractor"
local lfs                = require "lfs"
local utils              = require "luadb.utils"

local graph = {}

local function inc()
  a = a or 0
  a = a + 1
  return a
end

local function extractNode(v, nodes, rootcandidates, minComplexity, maxComplexity, minLines, maxLines)
  local origname = v.data.name or v.id
  if string.sub(v.id, 1, 1) == 'N' then
    local newnode = {type = "node", id = inc(), label = origname, params={size = 8, colorA = 1, colorR = 1, colorG = 1, colorB = 1, name = v.name, origid = v.id, nodetype = "file"}}
    nodes[v.id] = newnode
    rootcandidates[newnode] = true
  else
    local subnodes = {}
    for _, v1 in ipairs(v.nodes) do     
      local origname = v1.data.name or v1.id
      local newnode = {type = "node", id = inc(), label = origname, params={size = 8, colorA = 1, colorR = 0, colorG = 0, colorB = 1, name = v1.name, origid = v1.id, nodetype = "function"}}
      newnode.params.tag = v1.data.tag       
      if v1.data.metrics ~= nil then
        newnode.params.metrics = {}
        newnode.params.metrics.halstead = v1.data.metrics.halstead
        newnode.params.metrics.cyclomatic = v1.data.metrics.cyclomatic
        newnode.params.metrics.LOC = v1.data.metrics.LOC
        newnode.params.metrics.infoflow = v1.data.metrics.infoflow

        minComplexity = (minComplexity and (minComplexity < newnode.params.metrics.cyclomatic.upperBound and minComplexity)) or newnode.params.metrics.cyclomatic.upperBound
        maxComplexity = (maxComplexity and (maxComplexity > newnode.params.metrics.cyclomatic.upperBound and maxComplexity)) or newnode.params.metrics.cyclomatic.upperBound
        minLines = (minLines and (minLines < newnode.params.metrics.LOC.lines and minLines)) or newnode.params.metrics.LOC.lines
        maxLines = (maxLines and (maxLines > newnode.params.metrics.LOC.lines and maxLines)) or newnode.params.metrics.LOC.lines
      end
      subnodes[v1.id] = newnode
    end
    nodes[v.id] = {subnodes = subnodes, subedges = v.edges}
  end
  return minComplexity, maxComplexity, minLines, maxLines
end

local function extractEdge(v, existingedges, nodes, rootcandidates)
  if string.sub(v.to[1], 1, 1) == 'N' then
    rootcandidates[nodes[v.to[1]]] = nil
    local edge = {type = "edge", id = inc(), label = '', params = { origid = v.id}}
    local incid1 = {type = "edge_part", id = inc(), label = '', direction = "in"}
    local incid2 = {type = "edge_part", id = inc(), label = '', direction = "out"}
    graph[edge] = {[incid1] = nodes[v.from[1]], [incid2] = nodes[v.to[1]]}
  else
    for _, v1 in pairs(nodes[v.to[1]].subnodes) do
      local edge1 = {type = "edge", id = inc(), label = ''}
      local incid11 = {type = "edge_part", id = inc(), label = '', direction = "in"}
      local incid12 = {type = "edge_part", id = inc(), label = '', direction = "out"}
      graph[edge1] = {[incid11] = nodes[v.from[1]], [incid12] = v1}
    end
    for _, v1 in pairs(nodes[v.to[1]].subedges) do
      if v1.from[1] ~= v1.to[1] then 
        if #v1.from ~= 1 then print('from', #v1.from, v1.id, v.from[1], v1.from[1], v.from[2]) end 
        if #v1.to ~= 1 then print('to', #v1.to, v1.id, v.from[1], v1.to[1], v1.to[2]) end 
        local ind = (v1.from[1] or '') .. "|" .. v1.to[1]
        if existingedges[ind] ~= nil then
          existingedges[ind].params.count = existingedges[ind].params.count + 1
        else
          local edge2 = {type = "edge", id = inc(), label = '', params = { origid = v1.id, count = 1, colorA = 1, colorR = 1, colorG = 1, colorB = 0}}
          local incid21 = {type = "edge_part", id = inc(), label = '', direction = "in"}
          local incid22 = {type = "edge_part", id = inc(), label = '', direction = "out"}
          local node1 = nodes[v.to[1]].subnodes[v1.from[1]]
          local node2 = nodes[v.to[1]].subnodes[v1.to[1]]
          graph[edge2] = {[incid21] = node1, [incid22] = node2}
          existingedges[ind] = edge2
        end
      end
    end
  end
end

local function doVisualMapping(nodes, minComplexity, maxComplexity, minLines, maxLines)
  local minSize = 8
  local maxSize = 50
  for k, v in pairs(nodes) do
    if string.sub(k, 1, 1) == 'G' then
      for _, n in pairs(v.subnodes) do
        if n.params.metrics ~= nil then
          n.params.size = minSize + (n.params.metrics.LOC.lines - minLines) / (maxLines - minLines) * (maxSize - minSize)
          local complexRatio = n.params.metrics.cyclomatic.upperBound * minComplexity / maxComplexity
          n.params.colorG = 1 - complexRatio
          n.params.colorR = complexRatio
          n.params.colorB = 0
        else
          n.params.colorG = 0
          n.params.colorR = 0
          n.params.colorB = 1
        end
      end
    end
  end
end

local function extractGraph(absolutePath)
  graph = {}
  utils.logger:setLevel(utils.logging.INFO)

  utils.logger:info("started extraction")
  local extractedGraph = artifactsExtractor.extract(absolutePath)
  utils.logger:info("extraction successfully finished")

  local nodes = {}
  local rootcandidates = {}
  local minComplexity, maxComplexity
  local minLines, maxLines

  for _, v in ipairs(extractedGraph.nodes) do
    minComplexity, maxComplexity, minLines, maxLines = extractNode(v, nodes, rootcandidates, minComplexity, maxComplexity, minLines, maxLines)
  end

  print("complexity", minComplexity, maxComplexity, minLines, maxLines)

  local existingedges = {}

  for _, v in ipairs(extractedGraph.edges) do
    extractEdge(v, existingedges, nodes, rootcandidates)
  end

  for k, _ in pairs(rootcandidates) do
    k.params.root = true
    break
  end
  
  doVisualMapping(nodes, minComplexity, maxComplexity, minLines, maxLines)
end

local function getGraph()
  return graph
end

return {extractGraph = extractGraph,
  getGraph = getGraph}
