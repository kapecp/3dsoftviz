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
local EdgeOp = C(S("><-")^-1) * Space
local Id = Ct(EdgeOp * Name * (P"." * Name)^0)
local RelOp = C(P"==" + P"~=" + P"<=" + P"<" + P">=" + P">")
local AndOp = C(P"and")
local OrOp = C(P"or")
local Open = "(" * Space
local Close = ")" * Space
local QuotString = P'"' * ( (P'\\' * 1) + (1 - (S'"\n\r\f')) )^0 * P'"'
local AposString = P"'" * ( (P'\\' * 1) + (1 - (S"'\n\r\f")) )^0 * P"'"
local LiteralString = C(QuotString + AposString)
local Like = C(P"like")

local Exp, And, Or, Rel, Regex = V"Exp", V"And", V"Or", V"Rel", V"Regex"
G = lpeg.P{ Exp,
  Exp = Ct(-1) + Ct(Or * (Space * OrOp * Space * Or)^0),
  Or = Ct(And * (Space * AndOp * Space * And)^0),
  And = Rel + Regex + Open * Exp * Close,
  Rel = Ct(Id * Space * RelOp * Space * Number),
  Regex = Ct(Id * Space * Like * Space * LiteralString)
}

local function nodeAccepted(node, expTree, edge)
  if expTree[2] == 'or' then
    return nodeAccepted(node, expTree[1], edge) or nodeAccepted(node, expTree[3], edge)
  elseif expTree[2] == 'and' then
    return nodeAccepted(node, expTree[1], edge) and nodeAccepted(node, expTree[3], edge)
  elseif #expTree == 1 then
    return nodeAccepted(node, expTree[1], edge)
  else
    local var = node
    local edgeOps = {["-"] = true, [">"] = true, ["<"] = true}
    if edgeOps[expTree[1][1]] ~= nil then 
      print"edge operator"
    end
    local i
    for i = 2,#expTree[1] do
      var = var[expTree[1][i]]
      if var == nil then return false end
    end
    local op = expTree[2]
    if op == 'like' then
      local patt = expTree[3] 
      if patt:sub(2,2) ~= '^' then patt = patt:sub(1,1) .. '^' .. patt:sub(2) end
      if patt:sub(-2,-2) ~= '$' then patt = patt:sub(1,-2) .. '$' .. patt:sub(-1,-1) end
      print(patt)
      local s = loadstring('return ' .. patt)
      return string.find(var, s()) ~= nil
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

local filteredgraph = {}
local checkedNodes = {}

local function filterGraph(s)
  if getFullGraph == nil then return end
  local g = getFullGraph()
  filteredgraph = {}
  if s == "" then 
    filteredgraph = g
    graphChangedCallback()
    return
  end
  checkedNodes = {}
  local t = lpeg.match(G, s)
  for e, is in pairs(g) do
    local incidences = {}
    for i, n in pairs(is) do
      local node = copyNode(n)
      if checkedNodes[n] == nil then
        checkedNodes[n] = true
        if nodeAccepted(n, t, is) then
          node.params.colorA = 1
          node.params.size = node.params.size * 2
        else
          node.params.colorA = 0.1
        end
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
