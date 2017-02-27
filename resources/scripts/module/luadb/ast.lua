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


local function getModuleReferences(AST)
  return AST.metrics.moduleReferences
end


local function getModuleCalls(AST)
  return AST.metrics.moduleCalls
end

-- return first occurrency of node name
local function getName(node, maxdepth)
  node = metrics_utils. TagItem_recursive("Name", node, maxdepth)
  if node then return node['str'] end
  return nil
end

-- return true if module is returning something
local function isModuleLastReturn(lastStat)
  local parentNode = lastStat.parent
  if (parentNode.order == 1 and parentNode.position == 1) then
    return true
  end
  return false
end

-- return true if module is returning one or more tables
local function isTableAfterReturn(simpleExp)
  for _,prefixExp in pairs(simpleExp.data) do
    if (prefixExp.tag == '_PrefixExp') then
      return true
    end    
  end
  return false
end

-- return true if module is returning a contructed table
local function isTableConstructorAfterReturn(simpleExp)
  for _,tblConstr in pairs(simpleExp.data) do
    if (tblConstr.tag == 'TableConstructor') then
      return true
    end    
  end
  return false
end

-- return final values after return statement
local function getModuleReturnValues(AST)  
  local lastStats = metrics_utils.searchForTagArray_recursive("LastStat", AST, nil)
  local lastLastStat = lastStats[#lastStats]
  -- is module returning something?
  if (isModuleLastReturn(lastLastStat) == true) then
    local simpleExps = metrics_utils.getSimpleExpsFromLastStat(lastLastStat)
    local simpleExpsCount = #simpleExps
    local tbl = {}
    
    for _,simpleExp in pairs(simpleExps) do      
      -- is it returning an existing table or creating one after the return statement?
      if (isTableConstructorAfterReturn(simpleExp) == true) then
        -- get IDs from table constructor
        tbl = metrics_utils.getIDsFromSimpleExp(simpleExp)                
      else
        -- get Names of returning tables
        local tempTbl = metrics_utils.getNamesFromSimpleExp(simpleExp)
        table.insert(tbl, unpack(tempTbl))        
      end      
    end        
    return tbl
  end  
  -- it has no return at the end (might still be old fashioned: module 'moduleName')
  return nil
end

-- return true if variable is local
local function isVariableLocal(AST, variableName)
  -- tag: [LocalAssign][NameList][Name][ID]
  local localAssigns = metrics_utils.searchForTagArray_recursive("LocalAssign", AST, nil)
  -- for every LocalAssign
  for _,localAssign in pairs(localAssigns) do
    local nameLists = metrics_utils.getNameListsFromLocalAssign(localAssign)
    -- for every NameList
    for _,nameList in pairs(nameLists) do
      local names = metrics_utils.getNamesFromNameList(nameList)
      -- for every Name
      for _,name in pairs(names) do
        local tempName = name.str or name.text
        -- check if equals
        if(tempName == variableName) then          
          return true
        end
      end     
    end   
  end
  return false
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
  getParentNodesByTag = getParentNodesByTag,
  getModuleReferences = getModuleReferences,
  getModuleCalls = getModuleCalls,
  getModuleReturnValues = getModuleReturnValues,
  isFinalModuleReturn = isFinalModuleReturn,
  isVariableLocal = isVariableLocal
}