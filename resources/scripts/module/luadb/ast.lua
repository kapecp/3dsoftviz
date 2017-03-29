-----------------------------------------------
-- AST - AST utilities
-- @release 2013/12/03, Tomas Filcak
-----------------------------------------------

local utils         = require 'luadb.utils'
local metrics       = require 'metrics'
local metrics_utils = require 'metrics.utils'
local lookup        = require 'metrics.lookup'
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

local function getNodeInASTByID(AST, nodeid)
  return AST.node_references[nodeid]
end

-- return first occurrency of node name
local function getName(node, maxdepth)
  node = metrics_utils.TagItem_recursive("Name", node, maxdepth)
  if node then return node['str'] end
  return nil
end

-- return true if module is returning something
local function isModuleLastReturn(lastStat)
  if (lastStat) then
    local parentNode = lastStat.parent
    if (parentNode.order == 1 and parentNode.position == 1) then
      return true
    end
  end
  return false
end

-- return true if module is returning one or more tables
local function isTableAfterSimpleExp(simpleExp)
  for _,prefixExp in pairs(simpleExp.data) do
    if (prefixExp.tag == '_PrefixExp') then
      return true
    end    
  end
  return false
end

-- return true if module is returning a contructed table
local function isTableConstructorAfterSimpleExp(simpleExp)
  for _,tblConstr in pairs(simpleExp.data) do
    if (tblConstr.tag == 'TableConstructor') then
      return true
    end    
  end
  return false
end

-- return true if module is returning a function
local function isFunctionAfterSimpleExp(simpleExp)
  for _,func in pairs(simpleExp.data) do
    if (func.tag == 'Function') then
      return true
    end    
  end
  return false
end

-- return table of final values after return statement
local function getModuleReturnValues(AST)  
  local lastStats = metrics_utils.searchForTagArray_recursive("LastStat", AST, nil)
  -- the last return statement
  local lastLastStat = lastStats[#lastStats]
  -- is module returning something?
  if (isModuleLastReturn(lastLastStat) == true) then
    local simpleExps = lookup.getSimpleExpsFromLastStat(lastLastStat)    
    local tbl = {}
    
    for _,simpleExp in pairs(simpleExps) do      
      -- what is the module returning?
      if (isTableConstructorAfterSimpleExp(simpleExp) == true) then
        -- get fields from constructed table
        local fields = lookup.getFieldsFromSimpleExp(simpleExp)
        
        for _,field in pairs(fields) do
          local ids = lookup.getIDsFromField(field)
          local id = ids[1].text
          
          local names = lookup.getNamesFromField(field)
          local name = "nested table"
          if (not utils.isEmpty(names)) then
            name = names[1].text
          end
          local idNode = {
            ["name"] = id,
            ["type"] = "interface",
            ["node"] = field
          }
          local assignedNode = {
            ["name"] = name,
            ["type"] = "N/A",
            ["node"] = field
          }
          table.insert(tbl, {["idNode"] = idNode, ["assignedNode"] = assignedNode})
        end
      elseif(isFunctionAfterSimpleExp(simpleExp) == true) then
        -- get functions after return statement
        local funcs = lookup.getFunctionsFromSimpleExp(simpleExp)
        
        for _,func in pairs(funcs) do
          local parLists = lookup.getParListsFromFunction(func)
          local parList = ""
          if (not utils.isEmpty(parLists)) then
            parList = parLists[1].text
          end
          local idNode = {
            ["name"] = "function(" .. parList .. ")",
            ["type"] = "interface",
            ["node"] = func
          }
          table.insert(tbl, {["idNode"] = idNode, ["assignedNode"] = nil})
        end       
        
      else
        -- get tables after return statement
        local names = lookup.getNamesFromSimpleExp(simpleExp)
        local name = names[1].text
        local idNode = {
          ["name"] = name,
          ["type"] = "interface",
          ["node"] = simpleExp
        }
        table.insert(tbl, {["idNode"] = idNode, ["assignedNode"] = nil})
      end      
    end
    
    return tbl
  end  
  -- it has no return at the end (might still be old fashioned: module 'moduleName')
  return {}
end

-- return table of local functions
local function getLocalFunctions(AST)
  local tbl = metrics_utils.searchForTagArray_recursive("LocalFunction", AST, nil)
  return tbl
end

-- return table of global functions
local function getGlobalFunctions(AST)
  local tbl = metrics_utils.searchForTagArray_recursive("GlobalFunction", AST, nil)
  return tbl
end

-- extract only local require calls
local function extractLocalRequireCalls(prefixExps)
  local localRequires = {}
  -- tag: [LocalAssign]{[ExpList][_SimpleExp][_PrefixExp] | [NameList][Name]}
  for _,prefixExp in pairs(prefixExps) do
    
    -- get the localAssign parent node
    local localAssign = lookup.getLocalAssignsFromPrefixExp(prefixExp)
    
    -- if it is a local assign
    if (localAssign ~= nil) then
      local nameLists = lookup.getNameListsFromLocalAssign(localAssign)
      
      for _,nameList in pairs(nameLists) do
        local names = lookup.getNamesFromNameList(nameList)
        
        for _,name in pairs(names) do
          
          -- get the module name            
          local args = lookup.getArgsFromPrefixExp(prefixExp)
          
           -- in case of "local require = require"
          if (not utils.isEmpty(args)) then            
            local fullModuleName = args[1].text:gsub("/", "."):gsub("\"", ""):gsub("'", "")
            local moduleName = utils.splitAndGetLast(fullModuleName, "%.")    --only grammar from leg.grammar
            local idNode = {
              ["name"] = name.text,
              ["fullName"] = "local " .. name.text,
              ["type"] = "local variable",
              ["node"] = localAssign
            }
            local assignedNode = {
              ["name"] = moduleName,
              ["fullName"] = fullModuleName,
              ["type"] = "module",
              ["node"] = localAssign
            }            
            table.insert(localRequires, {["idNode"] = idNode, ["assignedNode"] = assignedNode})
          end
          
        end        
      end      
    end
  end
  return localRequires
end

-- extract only global require calls
local function extractGlobalRequireCalls(prefixExps)
  local globalRequires = {}
  -- tag: [FunctionCall][_PrefixExp]
  for _,prefixExp in pairs(prefixExps) do    
    local functionCall = prefixExp.parent
    
    if (functionCall.tag == "FunctionCall") then      
      local args = lookup.getArgsFromPrefixExp(prefixExp)
      local fullModuleName = args[1].text:gsub("/", "."):gsub("\"", ""):gsub("'", "")
      local moduleName = utils.splitAndGetLast(fullModuleName, "%.")    --only grammar from leg.grammar
      
      local assignedNode = {
        ["name"] = moduleName,
        ["fullName"] = fullModuleName,
        ["type"] = "module",
        ["node"] = functionCall
      }
      table.insert(globalRequires, {["idNode"] = nil, ["assignedNode"] = assignedNode})
    end      
        
  end
  return globalRequires
end

-- return every require call
local function getRequireCalls(AST)
  local prefixExps = metrics_utils.searchForTagArray_recursive("_PrefixExp", AST, nil)
  local requirePrefixExps = {}  
  for _,prefixExp in pairs(prefixExps) do    
    local names = lookup.getNamesFromPrefixExp(prefixExp)
    
    for _,name in pairs(names) do
      if (name.text == "require") then
        -- get 'require' prefixExps only
        table.insert(requirePrefixExps, prefixExp)
      end
    end
  end  
  
  local localRequires = extractLocalRequireCalls(requirePrefixExps)
  local globalRequires = extractGlobalRequireCalls(requirePrefixExps)  
  
  return {localRequires = localRequires, 
          globalRequires = globalRequires}
end

-- return true if local assign is a require call
local function isLocalAssignRequireCall(localAssign)
  local requireCall, hasArgs = false, false;
  
  local prefixExps = lookup.getPrefixExpsFromAssign(localAssign)
  for _,prefixExp in pairs(prefixExps) do
    for _,prefixExpData in pairs(prefixExp.data) do
      -- check if name is 'require'
      if (prefixExpData.tag == 'Name' and prefixExpData.text == 'require') then
        requireCall = true
      end
      -- check if args exist
      if (prefixExpData.tag == '_PrefixExpArgs') then
        hasArgs = true
      end
    end
  end
  return requireCall and hasArgs
end


-- return local variable and its assigns (excluding require calls)
local function getLocalAssigns(AST)
  local localAssigns = metrics_utils.searchForTagArray_recursive("LocalAssign", AST, nil)
  local mainLocalAssigns = {}
  -- get only the main local assigns
  for _,localAssign in pairs(localAssigns) do
    -- [Chunk][Stat][LocalAssign]
    local chunk = localAssign.parent.parent
    if (chunk.key == "Chunk" and chunk.order == 1 and chunk.position == 1) then
      table.insert(mainLocalAssigns, localAssign)
    end    
  end
  
  local tbl = {}
  for _,mainLocalAssign in pairs(mainLocalAssigns) do
    local locNames = lookup.getLeftNamesFromLocalAssign(mainLocalAssign)
    local globNames = lookup.getPrefixExpsFromAssign(mainLocalAssign)
    
    if (utils.isEmpty(globNames)) then
      
      local simpleExps = lookup.getSimpleExpsFromAssign(mainLocalAssign)
      --can be {} or function
      
      for _,simpleExp in pairs(simpleExps) do      
      
        if (isTableConstructorAfterSimpleExp(simpleExp) == true) then
          local tableNode = lookup.getTableConstructorsFromSimpleExp(simpleExp)          
          local assignedNode = {
            ["name"] = tableNode[1].text,
            ["fullName"] = tableNode[1].text,
            ["type"] = "N/A",
            ["node"] = tableNode
          }
          table.insert(globNames, assignedNode)
          
        elseif(isFunctionAfterSimpleExp(simpleExp) == true) then
          -- get functions
          local funcs = lookup.getFunctionsFromSimpleExp(simpleExp)
          
          for _,func in pairs(funcs) do
            local parLists = lookup.getParListsFromFunction(func)
            local parList = ""
            if (not utils.isEmpty(parLists)) then
              parList = parLists[1].text
            end
            
            local assignedNode = {
              ["name"] = "function(" .. parList .. ")",
              ["fullName"] = "function(" .. parList .. ")",
              ["type"] = "N/A",
              ["node"] = func
            }            
            table.insert(globNames, assignedNode)
            
          end
        end
      end    
    end
    
    --remove require calls
    for i=1, #locNames, 1 do
      if (isLocalAssignRequireCall(mainLocalAssign) == true) then
        table.remove(locNames, i)
        table.remove(globNames, i)
      end      
    end
    
    local idNode, assignedNode
    for i=1, #locNames, 1 do
      idNode = {
        ["name"] = locNames[i].text,
        ["fullName"] = locNames[i].text,
        ["type"] = "local variable",
        ["node"] = locNames[i]
      }
      if(globNames[i] == nil) then assignedNode = nil
      elseif(globNames[i].type == nil) then
        assignedNode = {
          ["name"] = globNames[i].text,
          ["fullName"] = globNames[i].text,
          ["type"] = "N/A",
          ["node"] = globNames[i]
        }
      else
        assignedNode = globNames[i]
      end       
      if(assignedNode and assignedNode.fullName:len() > 20) then assignedNode.name = assignedNode.fullName:sub(0, 15) .. "..." end
      table.insert(tbl, {["idNode"] = idNode, ["assignedNode"] = assignedNode})
    end
  end
  return tbl  
end



-- return global variables and its assigns
local function getAssigns(AST)
  local assigns = metrics_utils.searchForTagArray_recursive("Assign", AST, nil)
  local mainAssigns = {}
  -- get only the main assigns
  for _,assign in pairs(assigns) do
    -- [Chunk][Stat][Assign]
    local chunk = assign.parent.parent
    if (chunk.key == "Chunk" and chunk.order == 1 and chunk.position == 1) then
      table.insert(mainAssigns, assign)
    end    
  end
  
  local tbl = {}
  for _,mainAssign in pairs(mainAssigns) do
    local locNames = lookup.getLeftVarsFromAssign(mainAssign)
    local globNames = lookup.getPrefixExpsFromAssign(mainAssign)
        
    local clocNames = #locNames
    local cglobNames = #globNames
    
    if (#locNames < #globNames) then
      globNames[#locNames] = globNames[#locNames].parent.parent
      for i=#locNames+1, #globNames, 1 do
        globNames[i] = nil
      end
      
    end
    
    
    if (utils.isEmpty(globNames)) then
      
      local simpleExps = lookup.getSimpleExpsFromAssign(mainAssign)
      --can be {} or function
      
      for _,simpleExp in pairs(simpleExps) do      
      
        if (isTableConstructorAfterSimpleExp(simpleExp) == true) then
          local tableNode = lookup.getTableConstructorsFromSimpleExp(simpleExp)          
          local assignedNode = {
            ["name"] = tableNode[1].text,
            ["fullName"] = tableNode[1].text,
            ["type"] = "N/A",
            ["node"] = tableNode
          }
          table.insert(globNames, assignedNode)
          
        elseif(isFunctionAfterSimpleExp(simpleExp) == true) then
          -- get functions
          local funcs = lookup.getFunctionsFromSimpleExp(simpleExp)
          
          for _,func in pairs(funcs) do
            local parLists = lookup.getParListsFromFunction(func)
            local parList = ""
            if (not utils.isEmpty(parLists)) then
              parList = parLists[1].text
            end
            
            local assignedNode = {
              ["name"] = "function(" .. parList .. ")",
              ["fullName"] = "function(" .. parList .. ")",
              ["type"] = "N/A",
              ["node"] = func
            }            
            table.insert(globNames, assignedNode)
            
          end
        end
      end
    end
    
    --remove require calls
    for i=1, #locNames, 1 do
      if (isLocalAssignRequireCall(mainAssign) == true) then
        table.remove(locNames, i)
        table.remove(globNames, i)
      end      
    end
    
    local idNode, assignedNode
    for i=1, #locNames, 1 do
      idNode = {
        ["name"] = locNames[i].text,
        ["fullName"] = locNames[i].text,
        ["type"] = "global variable",
        ["node"] = locNames[i]
      }
      if(globNames[i] == nil) then assignedNode = nil
      elseif(globNames[i].type == nil) then
        assignedNode = {
          ["name"] = globNames[i].text,
          ["fullName"] = globNames[i].text,
          ["type"] = "N/A",
          ["node"] = globNames[i]
        }
      else
        assignedNode = globNames[i]
      end
      if(assignedNode and assignedNode.fullName:len() > 20) then assignedNode.name = assignedNode.fullName:sub(0, 15) .. "..." end
      table.insert(tbl, {["idNode"] = idNode, ["assignedNode"] = assignedNode})
    end

  end
  
  
  return tbl
  
end


-- return true if variable is local
local function isVariableLocal(AST, variableName)
  -- tag: [LocalAssign][NameList][Name][ID]
  local localAssigns = metrics_utils.searchForTagArray_recursive("LocalAssign", AST, nil)
  -- for every LocalAssign
  for _,localAssign in pairs(localAssigns) do
    local nameLists = lookup.getNameListsFromLocalAssign(localAssign)
    -- for every NameList
    for _,nameList in pairs(nameLists) do
      local names = lookup.getNamesFromNameList(nameList)
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
  getNodeInASTByID = getNodeInASTByID,
  getModuleReturnValues = getModuleReturnValues,
  getLocalAssigns = getLocalAssigns,
  getAssigns = getAssigns,
  getLocalFunctions = getLocalFunctions,
  getGlobalFunctions = getGlobalFunctions,
  getRequireCalls = getRequireCalls
}
