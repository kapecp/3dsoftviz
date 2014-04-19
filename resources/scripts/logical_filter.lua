--package.path = package.path .. ";../share/3dsoftviz/scripts/?.lua"

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
local Id = Ct(Name * (P"." * Name)^0)
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
  Exp = Ct(Or * (Space * OrOp * Space * Or)^0),
  Or = Ct(And * (Space * AndOp * Space * And)^0),
  And = Rel + Regex + Open * Exp * Close,
  Rel = Ct(Id * Space * RelOp * Space * Number),
  Regex = Ct(Id * Space * Like * Space * LiteralString)
}

local function nodeAccepted(node, expTree)
  if expTree[2] == 'or' then
    return nodeAccepted(node, expTree[1]) or nodeAccepted(node, expTree[3])
  elseif expTree[2] == 'and' then
    return nodeAccepted(node, expTree[1]) and nodeAccepted(node, expTree[3])
  elseif #expTree == 1 then
    return nodeAccepted(node, expTree[1])
  else
    local var = node
    for _, v in ipairs(expTree[1]) do
      var = var[v]
      if var == nil then return false end
    end
    local op = expTree[2]
    if op == 'like' then
      local patt = expTree[3] 
      if patt:sub(2,2) ~= '^' then patt = patt:sub(1,1) .. '^' .. patt:sub(2) end
      if patt:sub(-2,-2) ~= '$' then patt = patt:sub(1,-2) .. '$' .. patt:sub(-1,-1) end
      local s = loadstring('return ' .. patt)
      return string.find(var, s()) ~= nil
    else
      local s = 'return ' .. var .. expTree[2] .. expTree[3]
      return loadstring(s)()
    end
  end
end

local function filterGraph(g, s)
  local t = lpeg.match(G, s)
  for _, is in pairs(g) do
    for _, n in pairs(is) do
      n.params = n.params or {}
      if nodeAccepted(n, t) then
        n.params.colorA = 1
      else
        n.params.colorA = 0.2
      end
    end
  end
  return g
end

local graph = {
[{}] = {[{}] = {id = 1}},
[{}] = {[{}] = {id = 2}}
}

local s = 'id ~= 1'
local t = lpeg.match(G, s)
helper.vardump(s)
--helper.vardump(t)

helper.vardump(filterGraph(graph, s))
