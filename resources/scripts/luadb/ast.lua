-----------------------------------------------
-- AST - AST utilities
-- @release 2013/12/03, Tomas Filcak
-----------------------------------------------

local utils         = require 'luadb.utils'
local metrics       = require 'metrics'
local metrics_utils = require 'metrics.utils'
local logger        = utils.logger


local function fileExists(file)
  local f = io.open(file, "rb")
  if f then f:close() end
  return f ~= nil
end


local function readFile(file)
  if not fileExists(file) then
    logger:debug(file.." doesn't exist")
    return nil 
  end
  local f = io.open(file, "rb")
  local content = f:read("*a")
  logger:debug("reading "..file)
  f:close()
  return content
end


local function getAST(file_name)
  local code = readFile(file_name)
  if code then
    logger:debug("generating AST from "..file_name)
    return metrics.processText(code)  
  else
    return nil
  end
end


local function getParentNodesByTag(tbl, tag_name, tbl_out)
  tbl_out = (tbl_out) or {}

  for k,v in pairs(tbl) do
    if v == "IGNORED" then return tbl_out end
    if k == "tag" then
      if v == tag_name then table.insert(tbl_out, tbl) end
    end

    if (k == "parent") and (type(v) == "table") then
      tbl_out = getParentNodesByTag(v, tag_name, tbl_out);
    end
  end
  return tbl_out
end


local function getChildNodesByTag(tbl, tag_name, tbl_out)
  tbl_out = (tbl_out) or {}

  for k,v in pairs(tbl) do
    if v == "IGNORED" then return tbl_out end
    if k == "tag" then
      if v == tag_name then table.insert(tbl_out, tbl) end
    end

    if (k == "data") and (type(v) == "table") then
      for k2,v2 in pairs(v) do
        if type(v[k2]) == "table" then
          tbl_out = getChildNodesByTag(v[k2], tag_name, tbl_out);
          end
      end
    end
  end
  return tbl_out
end


local function getFunctions(AST)
  return AST.metrics.functionDefinitions
end


local function getFunctionsCalls(AST)
  return AST.metrics.functionExecutions
end


-- return first occurrency of node name
local function getName(node, maxdepth)
  node = metrics_utils. TagItem_recursive("Name", node, maxdepth)
  if node then return node['str'] end
  return nil
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  getAST = getAST,
  getName = getName,
  getFunctions = getFunctions,
  getFunctionsCalls = getFunctionsCalls,
  getChildNodesByTag = getChildNodesByTag,
  getParentNodesByTag = getParentNodesByTag
}