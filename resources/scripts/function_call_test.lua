package.path = package.path .. ";../share/3dsoftviz/scripts/?.lua;../share/3dsoftviz/scripts/?/init.lua"

local artifactsExtractor = require "luadb.extraction.extractor"
local lfs                = require "lfs"
local utils              = require "luadb.utils"
local helper = require "helper"

utils.logger:setLevel(utils.logging.INFO)

local absolutePath = "/home/ferenc/Plocha/DP/luadist-git-master"
utils.logger:info("started extraction")
local extractedGraph = artifactsExtractor.extract(absolutePath)
utils.logger:info("extraction successfully finished")

--extractedGraph:printNodes()
--extractedGraph:printEdges()

local function inc()
  a = a or 0
  a = a + 1
  return a
end

local nodes = {}
local rootcandidates = {}

for _, v in ipairs(extractedGraph.nodes) do
  local origname = v.data.name or v.id
  if string.sub(v.id, 1, 1) == 'N' then
    local newnode = {type = "node", id = inc(), label = origname, params={size = 8, colorA = 1, colorR = 0, colorG = 0, colorB = 1, name = v.name, origid = v.id, nodetype = "file"}}
    nodes[v.id] = newnode
    rootcandidates[newnode] = true
  else
    local subnodes = {}
    for _, v1 in ipairs(v.nodes) do
      local origname = v1.data.name or v1.id
      subnodes[v1.id] = {type = "node", id = inc(), label = origname, params={size = 8, colorA = 1, colorR = 0, colorG = 1, colorB = 1, name = v1.name, origid = v1.id, nodetype = "function"}}
    end
    nodes[v.id] = {subnodes = subnodes, subedges = v.edges}
  end
end

graph = {}
local existingedges = {}

for _, v in ipairs(extractedGraph.edges) do
  if string.sub(v.to[1], 1, 1) == 'N' then
    rootcandidates[nodes[v.to[1]]] = nil
    local edge = {type = "edge", id = inc(), label = '', params = { origid = v.id}}
    local incid1 = {type = "edge_part", id = inc(), direction = "in"}
    local incid2 = {type = "edge_part", id = inc(), direction = "out"}
    graph[edge] = {[incid1] = nodes[v.from[1]], [incid2] = nodes[v.to[1]]}
  else
    for _, v1 in pairs(nodes[v.to[1]].subnodes) do
      local edge1 = {type = "edge", id = inc(), label = ''}
      local incid11 = {type = "edge_part", id = inc(), direction = "in"}
      local incid12 = {type = "edge_part", id = inc(), direction = "out"}
      graph[edge1] = {[incid11] = nodes[v.from[1]], [incid12] = v1}
    end
    for _, v1 in pairs(nodes[v.to[1]].subedges) do
      if v1.from[1] ~= v1.to[1] then 
        local ind = v1.from[1] .. "|" .. v1.to[1]
        if existingedges[ind] ~= nil then
          existingedges[ind].params.count = existingedges[ind].params.count + 1
        else
          local edge2 = {type = "edge", id = inc(), label = '', params = { origid = v1.id, count = 1}}
          local incid21 = {type = "edge_part", id = inc(), direction = "in"}
          local incid22 = {type = "edge_part", id = inc(), direction = "out"}
          local node1 = nodes[v.to[1]].subnodes[v1.from[1]]
          local node2 = nodes[v.to[1]].subnodes[v1.to[1]]
          graph[edge2] = {[incid21] = node1, [incid22] = node2}
          existingedges[ind] = edge2
        end
      end
    end
  end
end

for k, _ in pairs(rootcandidates) do
  k.params.root = true
  break
end

