local lpeg = require "lpeg"
local helper = require "helper"

local P, R, S, V, C, Ct = lpeg.P, lpeg.R, lpeg.S, lpeg.V, lpeg.C, lpeg.Ct
local locale, match = lpeg.locale(), lpeg.match

-- Lexical Elements
local Space = S(" \n\t")^0
local Digit = R("09")
local Integer = S("+-")^-1 * Digit^1
local Fractional = P(".") * Digit^1
local Decimal =	(Integer * Fractional^-1) + (S("+-") * Fractional)
local Scientific = Decimal * S("Ee") * Integer
local Number = C(Decimal + Scientific)
local Name = C((locale.alpha + P "_") * (locale.alnum + P "_")^0)
local EdgeOp = C(S("><-")) * Space
local Id = Ct(Ct(EdgeOp^0) * Name * (P"." * Name)^0)
local EdgeId = Ct(Ct(EdgeOp^-1) * Name * (P"." * Name)^0)
local RelOp = C(P"==" + P"~=" + P"<=" + P"<" + P">=" + P">")
local AndOp = C(P"and")
local OrOp = C(P"or")
local NotOp = C(P"not")
local Open = "(" * Space
local Close = ")" * Space
local QuotString = P'"' * C(( (P'\\' * 1) + (1 - (S'"\n\r\f')) )^0) * P'"'
local AposString = P"'" * C(( (P'\\' * 1) + (1 - (S"'\n\r\f")) )^0) * P"'"
local LiteralString = QuotString + AposString
local Like = C(P"like")

local Exp, And, Or, Rel, Regex, Not = V"Exp", V"And", V"Or", V"Rel", V"Regex", V"Not"
local G = lpeg.P{ Exp,
  Exp = Ct(-1) + Not + Ct(Or * (Space * OrOp * Space * Or)^0),
  Not = Ct(NotOp * Space * Open * Exp * Close),
  Or = Ct(And * (Space * AndOp * Space * And)^0),
  And = Rel + Regex +  Open * Exp * Close,
  Rel = Ct(Id * Space * RelOp * Space * Number),
  Regex = Ct(Id * Space * Like * Space * LiteralString)
}

local G2 = lpeg.P{ Exp,
  Exp = Ct(-1) + Not + Ct(Or * (Space * OrOp * Space * Or)^0),
  Not = Ct(NotOp * Space * Open * Exp * Close),
  Or = Ct(And * (Space * AndOp * Space * And)^0),
  And = Rel + Regex +  Open * Exp * Close,
  Rel = Ct(EdgeId * Space * RelOp * Space * Number),
  Regex = Ct(EdgeId * Space * Like * Space * LiteralString)
}

local nodeAccepted
local invertedgraph = {}
local filteredgraph = {}
local fullGraph = {}
local checkedNodes = {}

local function deleteFirstEdgeOp(expTree)
  local newExp = {}
  local i
  newExp[1] = {}
  newExp[2] = expTree[2]
  newExp[3] = expTree[3]
  newExp[1][1] = {}
  for i = 2,#expTree[1][1] do
    newExp[1][1][i-1] = expTree[1][1][i]
  end
  for i = 2,#expTree[1] do
    newExp[1][i] = expTree[1][i]
  end
  return newExp
end

local function edgeOpLocalCorrect(edgeOp, inc)
  if edgeOp == '-' then 
    return true
  elseif edgeOp == '>' and inc.direction == 'in' then
    return true
  elseif edgeOp == '<' and inc.direction == 'out' then
    return true
  end 
  return false 
end

local function edgeOpRemoteCorrect(edgeOp, inc)
  if edgeOp == '-' then 
    return true
  elseif edgeOp == '<' and inc.direction == 'in' then
    return true
  elseif edgeOp == '>' and inc.direction == 'out' then
    return true
  end 
  return false 
end

local function checkNeighbors(node, expTree)
  local edgeOp = expTree[1][1][1]
  local newExp = deleteFirstEdgeOp(expTree)
  for i1, e in pairs(invertedgraph[node]) do
    local possible1, possible2 = false, false
    for i2, n in pairs(fullGraph[e]) do
      if node ~= n then
        if edgeOpRemoteCorrect(edgeOp, i2) and nodeAccepted(n, newExp) then 
          if possible1 then return true
          else possible2 = true end
        end
      else
        if edgeOpLocalCorrect(edgeOp, i2) then 
          if possible2 then return true
          else possible1 = true end
        end
      end
    end
  end
  return false
end

local function checkEdgeNeighbors(is, expTree)
  local edgeOp = expTree[1][1][1]
  local newExp = deleteFirstEdgeOp(expTree)
  print("edgeop", edgeOp, is.direction, edgeOpRemoteCorrect(edgeOp, is))
  for inc, node in pairs(is) do
    if edgeOpRemoteCorrect(edgeOp, is) and nodeAccepted(node, newExp) then
      return true
    end
  end
  return false
end

function nodeAccepted(node, expTree)
  if #expTree == 0 then
    return true 
  elseif expTree[2] == 'or' then
    local result = false
    local i = 1
    for i = 1, #expTree, 2 do
      result = result or nodeAccepted(node, expTree[i])
    end
    return result
  elseif expTree[2] == 'and' then
    local result = true
    local i = 1
    for i = 1, #expTree, 2 do
      result = result and nodeAccepted(node, expTree[i])
    end
    return result
  elseif expTree[1] == 'not' then
    return not nodeAccepted(node, expTree[2])
  elseif #expTree == 1 then
    return nodeAccepted(node, expTree[1])
  else
    if next(expTree[1][1]) ~= nil then 
      return checkNeighbors(node, expTree)
    end
    local var = node
    local i
    for i = 2,#expTree[1] do
      var = var[expTree[1][i]]
      if var == nil then return false end
    end
    local op = expTree[2]
    if op == 'like' then
      local patt = expTree[3] 
      return string.find(var, patt) ~= nil
    else
      local s = 'return ' .. var .. expTree[2] .. expTree[3]
      return loadstring(s)()
    end
  end
end

local function copyObj(obj)
  local result = {}
  for k, v in pairs(obj) do
    result[k] = v
  end
  result.params = {}
  for k, v in pairs(obj.params) do
    result.params[k] = v
  end
  return result
end

local function getInvertedGraph(g)
  local result = {}
  for e, is in pairs(g) do
    for i, n in pairs(is) do
      result[n] = result[n] or {}
      result[n][i] = e
    end
  end
  return result
end

local function hasEdgeOperator(exp)
  if #exp == 0 then
    return false
  elseif exp[2] == 'or' then
    local result = false
    local i = 1
    for i = 1, #exp, 2 do
      result = result or hasEdgeOperator(exp[i])
    end
    return result
  elseif exp[2] == 'and' then
    local result = true
    local i = 1
    for i = 1, #exp, 2 do
      result = result and hasEdgeOperator(exp[i])
    end
    return result
  elseif exp[1] == 'not' then
    return hasEdgeOperator(exp[2])
  elseif #exp == 1 then
    return hasEdgeOperator(exp[1])
  else
    return next(exp[1][1]) ~= nil
  end
end

local function edgeAccepted(edge, expTree, is)
  if expTree[2] == 'or' then
    local result = false
    local i = 1
    for i = 1, #expTree, 2 do
      result = result or edgeAccepted(edge, expTree[i], is)
    end
    return result
  elseif expTree[2] == 'and' then
    local result = true
    local i = 1
    for i = 1, #expTree, 2 do
      result = result and edgeAccepted(edge, expTree[i], is)
    end
    return result
  elseif expTree[1] == 'not' then
    return not edgeAccepted(edge, expTree[2], is)
  elseif #expTree == 1 then
    return edgeAccepted(edge, expTree[1], is)
  else
    if next(expTree[1][1]) ~= nil then 
      print("edge", edge.id)
      return checkEdgeNeighbors(is, expTree)
    end
    local var = edge
    local i
    for i = 2,#expTree[1] do
      var = var[expTree[1][i]]
      if var == nil then return false end
    end
    local op = expTree[2]
    if op == 'like' then
      local patt = expTree[3] 
      return string.find(var, patt) ~= nil
    else
      local s = 'return ' .. var .. expTree[2] .. expTree[3]
      return loadstring(s)()
    end
  end
end

local function filterEdges(graph, query)
  local t = lpeg.match(G2, query)
  helper.vardump(t)
  for e,is in pairs(graph) do
    if edgeAccepted(e, t, is) then
      e.params.colorA = 1
      e.label = e.params.type
    else
      e.params.colorA = 0.1
      e.label = ''
    end
  end
end

local function filterGraph(nodeFilter, edgeFilter)
  if getFullGraph == nil then return end
  fullGraph = getFullGraph()
  filteredgraph = {}
  checkedNodes = {}
  local t = lpeg.match(G, nodeFilter)
  local hasEdgeOp = hasEdgeOperator(t)
  if hasEdgeOp then
    invertedgraph = getInvertedGraph(fullGraph)
  end
  print(invertedgraph)
 
  local acceptedNodes = {}
  for e, is in pairs(fullGraph) do
    local edge = copyObj(e)
    edge.params.colorA = 0.1
    edge.label = ''
    local incidences = {}
    local hasAccepted = false
    for i, n in pairs(is) do
      local node
      if checkedNodes[n] == nil then
        node = copyObj(n)
        if nodeAccepted(n, t) then
          node.params.colorA = 1
          acceptedNodes[n] = true
        else
          node.label = ''
          node.params.colorA = 0.3
        end
        checkedNodes[n] = node
      else
        node = checkedNodes[n]
      end
      if edgeFilter ~= nil then
        if hasAccepted then
          if acceptedNodes[n] then
            edge.params.colorA = 1
            edge.label = e.label
          end
        else
          hasAccepted = acceptedNodes[n]
        end
      end
      incidences[i] = node
    end
    filteredgraph[edge] = incidences
  end
  if edgeFilter ~= "" then 
    filterEdges(filteredgraph, edgeFilter)
  end
  graphChangedCallback()
end

local function validNodeQuery(s)
  return lpeg.match(G, s) ~= nil
end

local function validEdgeQuery(s)
  return lpeg.match(G2, s) ~= nil
end

local function getGraph()
  return filteredgraph
end

return {validNodeQuery = validNodeQuery,
  validEdgeQuery = validEdgeQuery,
  filterGraph = filterGraph,
  getGraph = getGraph}
