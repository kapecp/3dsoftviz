-----------------------------------------------
-- Module graph - module for extracting 
-- module graph from given project and import into 3DSoftViz 
-- @release 2016/12/02, Denis Illes
-----------------------------------------------
local utils               = require "luadb.utils"
local moduleExtractor     = require "luadb.extraction.moduleExtractor"
local artifactsExtractor  = require "luadb.extraction.extractor"
local moonscriptExtractor = require "luameg"
local astManager          = require "luadb.manager.AST"

----------------------------------------------
-- Local graph and AST manager stored after extraction
local graph = {}
local colorTable = {}
local incidTable = {}
local astMan = astManager.new()

----------------------------------------------
-- Increment function for generating of ids
local a = 0
local function inc()
  a = a + 1
  return a
end

----------------------------------------------
-- Initialize colours and incidence labels for nodes and edges
local function addColor(typ, ARGB)
  colorTable[typ] = ARGB
end

local function addIncidLabel(typ, InsOut)
  incidTable[typ] = InsOut
end

local function setSwitchTable()
  colorTable = {
    --nodes
    ['project']         = {A = 1, R = 1, G = 1, B = 0},
    ['directory']       = {A = 1, R = 1, G = 1, B = 0},
    ['file']            = {A = 1, R = 1, G = 0, B = 1},
    ['module']          = {A = 1, R = 1, G = 0, B = 1},
    ['global function'] = {A = 1, R = 0, G = 0, B = 1},
    ['local variable']  = {A = 1, R = 0, G = 1, B = 0},
    ['global variable'] = {A = 1, R = 0, G = 1, B = 0},
    ['interface']       = {A = 1, R = 0, G = 0, B = 0},
    ['argument']        = {A = 1, R = 0, G = 1, B = 1},
    ['method']          = {A = 1, R = 0, G = 1, B = 0},
    ['class']           = {A = 1, R = 1, G = 0, B = 0},
    --edges
    ['calls']           = {A = 1, R = 0.8, G = 0.8, B = 1},
    ['assigns']         = {A = 1, R = 0.8, G = 1, B = 0.8},
    ['contains']        = {A = 1, R = 0.8, G = 1, B = 0.8},
    ['requires']        = {A = 1, R = 1, G = 0.8, B = 1},
    ['implements']      = {A = 1, R = 1, G = 0.8, B = 1},
    ['provides']        = {A = 1, R = 1, G = 0.8, B = 1},
    ['initializes']     = {A = 1, R = 1, G = 0.8, B = 1},
    ['declares']        = {A = 1, R = 1, G = 0.8, B = 1},
    ['represents']      = {A = 1, R = 1, G = 0.8, B = 1},
    ['has']             = {A = 1, R = 1, G = 0.8, B = 1}
  }
  
  incidTable = {
    ['calls']           = {ins = 'callee', out = 'caller'},
    ['assigns']         = {ins = 'assignee', out = 'assigner'},
    ['contains']        = {ins = 'containee', out = 'container'},
    ['requires']        = {ins = 'requiree', out = 'requirer'},
    ['implements']      = {ins = 'implementee', out = 'implementer'},
    ['provides']        = {ins = 'providee', out = 'provider'},
    ['initializes']     = {ins = 'initializee', out = 'initializer'},
    ['declares']        = {ins = 'declaree', out = 'declarer'},
    ['represents']      = {ins = 'representee', out = 'representer'},
    ['extends']         = {ins = 'extendee', out = 'extender'},
    ['has']             = {ins = 'has', out = 'has'}
  }
end

--------------------------------------------------
-- Set incidences based on edge type
local function setIncidence(edge)
  local edgeType = edge.params.type
  local incid1 = {type = 'edge_part', id = inc(), direction = 'in', label = incidTable[edgeType].ins}
  local incid2 = {type = 'edge_part', id = inc(), direction = 'out', label = incidTable[edgeType].out}
  return incid1, incid2
end

--------------------------------------------------
-- Set color for node based on node type
local function setNodeColor(node)
  local nodeType = node.params.type
  if(colorTable[nodeType]) then
    node.params.color = colorTable[nodeType]
  else
    node.params.color = {A = 1, R = 1, G = 1, B = 1}
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
  local newnode = {
    type = "node",
    id = inc(),
    label = origname,
    params = {
      size = 8,
      name = origname,
      origid = v.id,
      type = v.meta.type,
      path = v.data.path,
      modulePath = v.meta.modulePath,
      position = v.data.position
    }
  }
  if(newnode.id == 1)then 
    newnode.params.root = true 
  end
  nodes[v] = newnode  
 
  if(v.meta.type == 'function') then
    newnode.params.tag = v.data.tag       
    if(v.data.metrics ~= nil) then
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
  setNodeColor(newnode)
  return minComplexity, maxComplexity, minLines, maxLines
end

--------------------------------------------------
-- Set color for edge based on edge type
local function setEdgeColor(edge)
  local edgeType = edge.params.type
  if(colorTable[edgeType]) then
    edge.params.color = colorTable[edgeType]
  else
    edge.params.color = {A = 1, R = 1, G = 1, B = 1}
  end
end

-------------------------------------------------------
-- Convert edge from LuaDB format to import 
-- format for 3DSoftviz
-- @param v edge in LuaDB format
-- @param existingedges table with already converted edges
-- @param nodes table of extracted nodes
local function extractEdge(v, existingedges, nodes)
  if(#v.from ~= 1) then print('from', #v.from, v.id, v.from[1], v.from[2]) end
  if(#v.to ~= 1) then print('to', #v.to, v.id, v.to[1], v.to[2]) end  
  local ind = v.from[1].id .. "|" .. v.to[1].id
  if(existingedges[ind] ~= nil) then
    existingedges[ind].params.count = existingedges[ind].params.count + 1
  else
    local edge = {
      type = "edge",
      id = inc(),
      params = {
        origid = v.id,
        count = 1,
        edgeStrength = 2
      }
    }
    if(v.label ~= nil) then
      edge.params.type = v.label
    end
    
    local incid1, incid2 = setIncidence(edge)

    edge.label = edge.params.type
    if(v.from[1].meta.modulePath ~= v.to[1].meta.modulePath) then
      edge.params.edgeStrength = 0.1
    elseif(v.from[1].meta.modulePath ~= nil) then
      edge.params.edgeStrength = 0.5
    end 
    graph[edge] = {
      [incid1] = nodes[v.to[1]],
      [incid2] = nodes[v.from[1]]
    }    
    utils.logger:debug(nodes[v.from[1]].params.type .. " - " .. incid2.label .. " - " .. edge.params.type .. " - " .. incid1.label .. " - " .. nodes[v.to[1]].params.type)
  
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
    if(n.params.type == 'function') then
      if(n.params.metrics ~= nil) then
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
-- @param graphPicker name of desired graph to be extracted
local function extractGraph(absolutePath, graphPicker)
  graph = {}
  
  utils.logger:setLevel(utils.logging.INFO)

  utils.logger:info("started extraction")
  local extractedGraph
  -- for now, it's still nil
  if(graphPicker == "functionCall graph") then
    extractedGraph = artifactsExtractor.extract(absolutePath, astMan)
  elseif(graphPicker == "module graph") then    
    extractedGraph = moduleExtractor.extract(absolutePath, astMan)
  elseif(graphPicker == "moonscript graph") then
    extractedGraph = moonscriptExtractor.getGraphProject(absolutePath, astMan)
  end  
  utils.logger:info("extraction successfully finished")
  
  --extractedGraph:printNodes()
  --extractedGraph:printEdges()
  
  setSwitchTable()

  local nodes = {}
  local minComplexity, maxComplexity
  local minLines, maxLines

  for _, v in ipairs(extractedGraph.nodes) do
    minComplexity, maxComplexity, minLines, maxLines = extractNode(v, nodes, minComplexity, maxComplexity, minLines, maxLines)
  end

  utils.logger:info("complexity", minComplexity, maxComplexity, minLines, maxLines)

  local existingedges = {}

  for _, v in ipairs(extractedGraph.edges) do
    extractEdge(v, existingedges, nodes)
  end
  
  doVisualMapping(nodes, minComplexity, maxComplexity, minLines, maxLines)
end

-------------------------------------
-- Function for retreiving extracted graph
local function getGraph()
  utils.logger:info("getting hybrid graph")
  return graph
end

-------------------------------------
-- Function for retreiving extracted graph
local function getASTManager()
  utils.logger:info("getting AST Manager")
  return astMan
end
-------------------------------------
-- Public interface of module
return {
  extractGraph = extractGraph,
  getGraph = getGraph,
  getASTManager = getASTManager
}
