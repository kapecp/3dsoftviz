package.path = package.path .. ";../share/3dsoftviz/scripts/?.lua"

filetree = require "filetree"

function inc()
  a = a or 0
  a = a + 1
  return a
end

local function get_graph_from_tree(tree, graph, parent)
  print ("traversing " .. tree.name)
  local graph = graph or {}
  local node = {type = "node", id = inc(), label = tree.name, params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
  if parent ~= nil then
    local edge = {type = "edge", label = "", id = inc()}
    local incid1 = {type = "edge_part", id = inc()}
    local incid2 = {type = "edge_part", id = inc()}
    graph[edge] = {[incid1] = parent, [incid2] = node}
  end  
  local contents = tree.contents or {}
  for k, v in pairs(contents) do
    get_graph_from_tree(v, graph, node)
  end
  return graph
end

function create_file_graph(path)
  print "creating file graph"
  tree = filetree.load(path)
  graph = get_graph_from_tree(tree)
end
