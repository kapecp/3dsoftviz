package.path = package.path .. ";../share/3dsoftviz/scripts/?.lua"

filetree = require "filetree"

function inc()
  a = a or 0
  a = a + 1
  return a
end

local function get_graph_from_tree(tree, graph, parent, min_size, max_size)
  print ("traversing " .. tree.name)
  local graph = graph or {}
  min_size = min_size or tree.attributes.size
  max_size = max_size or tree.attributes.size
  if tree.attributes.size < min_size then min_size = tree.attributes.size end
  if tree.attributes.size > max_size then max_size = tree.attributes.size end
  local node = {type = "node", id = inc(), label = tree.name, params={size = tree.attributes.size, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
  if parent ~= nil then
    local edge = {type = "edge", label = "", id = inc()}
    local incid1 = {type = "edge_part", label = "", id = inc(), direction = "in"}
    local incid2 = {type = "edge_part", label = "", id = inc(), direction = "out"}
    graph[edge] = {[incid1] = parent, [incid2] = node}
  else
    node.params.colorG = 1  
  end    
  local contents = tree.contents or {}
  for k, v in pairs(contents) do
    _, min_size, max_size = get_graph_from_tree(v, graph, node, min_size, max_size)
  end
  return graph, min_size, max_size
end

function create_file_graph(path)
  print "creating file graph"
  tree = filetree.load(path)
  graph, min, max = get_graph_from_tree(tree)
  print ("sizes " .. min .. "-" .. max)
  for _, v in pairs(graph) do
    for _, n in pairs(v) do
      if n.size_counted == nil then
        n.params.size = 8 + 50 * ((n.params.size - min) / (max - min)) 
        n.size_counted = true
      end   
    end
  end
end
