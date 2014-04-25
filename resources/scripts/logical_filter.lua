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
local RelOp = C(P"==" + P"~=" + P"<=" + P"<" + P">=" + P">")
local AndOp = C(P"and")
local OrOp = C(P"or")
local Open = "(" * Space
local Close = ")" * Space
local QuotString = P'"' * C(( (P'\\' * 1) + (1 - (S'"\n\r\f')) )^0) * P'"'
local AposString = P"'" * C(( (P'\\' * 1) + (1 - (S"'\n\r\f")) )^0) * P"'"
local LiteralString = QuotString + AposString
local Like = C(P"like")

local Exp, And, Or, Rel, Regex = V"Exp", V"And", V"Or", V"Rel", V"Regex"
G = lpeg.P{ Exp,
  Exp = Ct(-1) + Ct(Or * (Space * OrOp * Space * Or)^0),
  Or = Ct(And * (Space * AndOp * Space * And)^0),
  And = Rel + Regex + Open * Exp * Close,
  Rel = Ct(Id * Space * RelOp * Space * Number),
  Regex = Ct(Id * Space * Like * Space * LiteralString)
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
    local possible = false
    for i2, n in pairs(fullGraph[e]) do
      if node ~= n then
        if edgeOpRemoteCorrect(edgeOp, i2) and nodeAccepted(n, newExp) then 
          if possible then return true
          else possible = true end
        end
      else
        if edgeOpLocalCorrect(edgeOp, i2) then 
          if possible then return true
          else possible = true end
        end
      end
    end
  end
  return false
end

function nodeAccepted(node, expTree)
  if expTree[2] == 'or' then
    return nodeAccepted(node, expTree[1]) or nodeAccepted(node, expTree[3])
  elseif expTree[2] == 'and' then
    return nodeAccepted(node, expTree[1]) and nodeAccepted(node, expTree[3])
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

local function copyNode(node)
  local result = {}
  for k, v in pairs(node) do
    result[k] = v
  end
  result.params = {}
  for k, v in pairs(node.params) do
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
  if exp[2] == 'or' then
    return hasEdgeOperator(exp[1]) or hasEdgeOperator(exp[3])
  elseif exp[2] == 'and' then
    return hasEdgeOperator(exp[1]) or hasEdgeOperator(exp[3])
  elseif #exp == 1 then
    return hasEdgeOperator(exp[1])
  else
    return next(exp[1][1]) ~= nil
  end
end

local function filterGraph(s)
  if getFullGraph == nil then return end
  fullGraph = getFullGraph()
  filteredgraph = {}
  if s == "" then 
    filteredgraph = fullGraph
    graphChangedCallback()
    return
  end
  checkedNodes = {}
  local t = lpeg.match(G, s)
  print("has edgeop", hasEdgeOperator(t))
  if hasEdgeOperator(t) then
    invertedgraph = getInvertedGraph(fullGraph)
  end
  print(invertedgraph)
 
  for e, is in pairs(fullGraph) do
    local incidences = {}
    for i, n in pairs(is) do
      local node = copyNode(n)
      if checkedNodes[n] == nil then
        if nodeAccepted(n, t) then
          node.params.colorA = 1
          node.params.size = node.params.size * 2
        else
          node.label = ''
          node.params.colorA = 0.1
        end
        checkedNodes[n] = node
      else
        node = checkedNodes[n]
      end
      incidences[i] = node
    end
    filteredgraph[e] = incidences
  end
  graphChangedCallback()
end

local function validQuery(s)
  return lpeg.match(G, s) ~= nil
end

local function getGraph()
  return filteredgraph
end

return {validQuery = validQuery,
  filterGraph = filterGraph,
  getGraph = getGraph}
