-------------------------------------------------------------------------------
-- Metrics - lookup functions for the created AST tree
-- @release 2017/03/07 , Denis Illes
-------------------------------------------------------------------------------

local pairs, table, assert = pairs, table, assert

local lookup = {}

-----------------------------
-- Search for nodes with tag '_SimpleExp' in exp
-- returns a table with nodes
-- @param exp node to be searched
-- @return table containing nodes
function lookup.getSimpleExpsFromExp(exp)
  assert(exp.tag == "Exp", "[table].tag is not Exp")
  local tbl = {}
    -- [Exp][_SimpleExp]
  for _,simpleExp in pairs(exp.data) do
    if (simpleExp.tag == '_SimpleExp') then
      table.insert(tbl, simpleExp)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'ExpList' in assign
-- returns a table with nodes
-- @param assign node to be searched
-- @return table containing nodes
function lookup.getExpListsFromAssign(assign)
  assert(assign.tag == "LocalAssign" or assign.tag == "Assign", "[table].tag is not LocalAssign nor Assign")
  local tbl = {}
    -- ([LocalAssign] | [Assign]) [ExpList]
  for _,expList in pairs(assign.data) do
    if (expList.tag == 'ExpList') then
      table.insert(tbl, expList)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'NameList' in localAssign
-- returns a table with nodes
-- @param localAssign node to be searched
-- @return table containing nodes
function lookup.getNameListsFromLocalAssign(localAssign)
  assert(localAssign.tag == "LocalAssign", "[table].tag is not LocalAssign")
  local tbl = {}
    -- [LocalAssign][NameList]
  for _,nameList in pairs(localAssign.data) do
    if (nameList.tag == 'NameList') then
      table.insert(tbl, nameList)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'VarList' in assign
-- returns a table with nodes
-- @param assign node to be searched
-- @return table containing nodes
function lookup.getVarListsFromAssign(assign)
  assert(assign.tag == "Assign", "[table].tag is not Assign")
  local tbl = {}
    -- [Assign][VarList]
  for _,varList in pairs(assign.data) do
    if (varList.tag == 'VarList') then
      table.insert(tbl, varList)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Name' in nameList
-- returns a table with nodes
-- @param nameList node to be searched
-- @return table containing nodes
function lookup.getNamesFromNameList(nameList)
  assert(nameList.tag == "NameList", "[table].tag is not NameList")
  local tbl = {}
    -- [NameList][Name]
  for _,name in pairs(nameList.data) do
    if (name.tag == 'Name') then
      table.insert(tbl, name)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Var' in varList
-- returns a table with nodes
-- @param nameList node to be searched
-- @return table containing nodes
function lookup.getVarsFromVarList(varList)
  assert(varList.tag == "VarList", "[table].tag is not VarList")
  local tbl = {}
    -- [VarList][Var]
  for _,var in pairs(varList.data) do
    if (var.tag == 'Var') then
      table.insert(tbl, var)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Name' or 'Var' in list
-- returns a table with nodes
-- @param list node to be searched
-- @return table containing nodes
function lookup.getNamesOrVarsFromList(list)
  assert(list.tag == "NameList" or list.tag == "VarList", "[table].tag is not NameList nor VarList")
  local tbl = {}
    -- ([NameList][Name]) | ([VarList][Var])
  for _,nameVar in pairs(list.data) do
    if (nameVar.tag == 'Name' or nameVar.tag == 'Var') then
      table.insert(tbl, nameVar)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Exp' in expList
-- returns a table with nodes
-- @param expList node to be searched
-- @return table containing nodes
function lookup.getExpsFromExpsList(expList)
  assert(expList.tag == "ExpList", "[table].tag is not ExpList")
  local tbl = {}
    -- [ExpList][Exp]
  for _,exp in pairs(expList.data) do
    if (exp.tag == 'Exp') then
      table.insert(tbl, exp)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag '_PrefixExp' in functionCall
-- returns a table with nodes
-- @param functionCall node to be searched
-- @return table containing nodes
function lookup.getPrefixExpFromFunctionCall(functionCall)
  assert(functionCall.tag == "FunctionCall", "[table].tag is not FunctionCall")
  local tbl = {}
    -- [FunctionCall][_PrefixExp]
  for _,prefixExp in pairs(functionCall.data) do
    if (prefixExp.tag == '_PrefixExp') then
      table.insert(tbl, prefixExp)
    end       
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Name' in prefixExp
-- returns a table with nodes
-- @param prefixExp node to be searched
-- @return table containing nodes
function lookup.getNamesFromPrefixExp(prefixExp)
  assert(prefixExp.tag == "_PrefixExp", "[table].tag is not _PrefixExp")
  local tbl = {}
    -- [_PrefixExp][Name]
  for _,name in pairs(prefixExp.data) do
    if (name.tag == 'Name') then
      table.insert(tbl, name)
    end       
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'FuncBody' in func
-- returns a table with nodes
-- @param func node to be searched
-- @return table containing nodes
function lookup.getFuncBodyFromFunction(func)
  assert(func.tag == "Function", "[table].tag is not Function")
  local tbl = {}
    -- [Function][FuncBody]
  for _,funcBody in pairs(func.data) do
    if (funcBody.tag == 'FuncBody') then
      table.insert(tbl, funcBody)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag '_PrefixExp' in simpleExp
-- returns a table with nodes
-- @param simpleExp node to be searched
-- @return table containing nodes
function lookup.getPrefixExpsFromSimpleExp(simpleExp)
  assert(simpleExp.tag == "_SimpleExp", "[table].tag is not _SimpleExp")
  local tbl = {}
    -- [_SimpleExp][_PrefixExp]
  for _,prefixExp in pairs(simpleExp.data) do
    if (prefixExp.tag == '_PrefixExp') then
      table.insert(tbl, prefixExp)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'TableContructor' in simpleExp
-- returns a table with nodes
-- @param simpleExp node to be searched
-- @return table containing nodes
function lookup.getTableConstructorsFromSimpleExp(simpleExp)
  assert(simpleExp.tag == "_SimpleExp", "[table].tag is not _SimpleExp")
  local tbl = {}
    -- [_SimpleExp][TableContructor]
  for _,tblConstr in pairs(simpleExp.data) do
    if (tblConstr.tag == 'TableConstructor') then
      table.insert(tbl, tblConstr)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'ParList' in funcBody
-- returns a table with nodes
-- @param funcBody node to be searched
-- @return table containing nodes
function lookup.getParListsFromFuncBody(funcBody)
  assert(funcBody.tag == "FuncBody", "[table].tag is not FuncBody")
  local tbl = {}
    -- [FuncBody][ParList]
  for _,parList in pairs(funcBody.data) do
    if (parList.tag == 'ParList') then
      table.insert(tbl, parList)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Function' in simpleExp
-- returns a table with nodes
-- @param simpleExp node to be searched
-- @return table containing nodes
function lookup.getFunctionsFromSimpleExp(simpleExp)
  assert(simpleExp.tag == "_SimpleExp", "[table].tag is not _SimpleExp")
  local tbl = {}
    -- [_SimpleExp][Function]
  for _,func in pairs(simpleExp.data) do
    if (func.tag == 'Function') then
      table.insert(tbl, func)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Block' in func
-- returns a table with nodes
-- @param func node to be searched
-- @return table containing nodes
function lookup.getBlockFromFunction(func)
  local tbl = {}
    -- [Function][FuncBody][Block]
  for _,funcBody in pairs(func.data) do
    if (funcBody.tag == 'FuncBody') then
      for _,block in pairs(funcBody.data) do
        if (block.tag == 'Block') then
          tbl = block
          break
        end
      end
      break
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'ID' in field
-- returns a table with nodes
-- @param field node to be searched
-- @return table containing nodes
function lookup.getIDsFromField(field)
  assert(field.tag == "Field", "[table].tag is not Field")
  local tbl = {}
    -- [Field][_FieldID][ID]  
  for _,fieldID in pairs(field.data) do
    if (fieldID.tag == '_FieldID') then
      for _,id in pairs(fieldID.data) do
        if (id.tag == 'ID') then
          table.insert(tbl, id)
        end
      end
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Args' in prefixExp
-- returns a table with nodes
-- @param prefixExp node to be searched
-- @return table containing nodes
function lookup.getArgsFromPrefixExp(prefixExp)
  assert(prefixExp.tag == "_PrefixExp", "[table].tag is not _PrefixExp")
  local tbl = {}
    -- [_PrefixExp][_PrefixExpArgs][Args]
  for _,prefixExpArg in pairs(prefixExp.data) do
    if (prefixExpArg.tag == '_PrefixExpArgs') then
      for _,arg in pairs(prefixExpArg.data) do
        if (arg.tag == 'Args') then
          table.insert(tbl, arg)
        end
      end
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Name' in simpleExp
-- returns a table with nodes
-- @param simpleExp node to be searched
-- @return table containing nodes
function lookup.getNamesFromSimpleExp(simpleExp)
  assert(simpleExp.tag == "_SimpleExp", "[table].tag is not _SimpleExp")
  local tbl = {}
    -- [_SimpleExp][_PrefixExp][Name]
  local prefixExps = lookup.getPrefixExpsFromSimpleExp(simpleExp)
  for _,prefixExp in pairs(prefixExps) do
    local names = lookup.getNamesFromPrefixExp(prefixExp)
    for _,name in pairs(names) do
      if (name.tag == 'Name') then
        table.insert(tbl, name)
      end
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Name' in localAssign
-- returns a table with nodes
-- @param localAssign node to be searched
-- @return table containing nodes
function lookup.getLeftNamesFromLocalAssign(localAssign)
  assert(localAssign.tag == "LocalAssign", "[table].tag is not LocalAssign")
  local tbl = {}  
    -- [LocalAssign][NameList][Name]
  local nameLists = lookup.getNameListsFromLocalAssign(localAssign)  
  for _,nameList in pairs(nameLists) do
    local names = lookup.getNamesOrVarsFromList(nameList)
    for _,name in pairs(names) do
      table.insert(tbl, name)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Var' in assign
-- returns a table with nodes
-- @param localAssign node to be searched
-- @return table containing nodes
function lookup.getLeftVarsFromAssign(assign)
  assert(assign.tag == "Assign", "[table].tag is not Assign")
  local tbl = {}  
    -- [Assign][VarList][Var]
  local varLists = lookup.getVarListsFromAssign(assign)  
  for _,varList in pairs(varLists) do
    local vars = lookup.getNamesOrVarsFromList(varList)
    for _,var in pairs(vars) do
      table.insert(tbl, var)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'ParList' in func
-- returns a table with nodes
-- @param func node to be searched
-- @return table containing nodes
function lookup.getParListsFromFunction(func)
  assert(func.tag == "Function", "[table].tag is not Function")
  local tbl = {}
    -- [Function][FuncBody][ParList]
  local funcBodies = lookup.getFuncBodyFromFunction(func)  
  for _,funcBody in pairs(funcBodies) do
    local parLists = lookup.getParListsFromFuncBody(funcBody)
    for _,parList in pairs(parLists) do
      table.insert(tbl, parList)
    end 
  end    
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Field' in simpleExp
-- returns a table with nodes
-- @param simpleExp node to be searched
-- @return table containing nodes
function lookup.getFieldsFromSimpleExp(simpleExp)
  assert(simpleExp.tag == "_SimpleExp", "[table].tag is not _SimpleExp")
  local tbl = {}
    -- [_SimpleExp][TableConstructor][FieldList][Field]
  for _,tblConstr in pairs(simpleExp.data) do
    if (tblConstr.tag == 'TableConstructor') then
      for _,fieldList in pairs(tblConstr.data) do
        if (fieldList.tag == 'FieldList') then
          for _,field in pairs(fieldList.data) do
            if (field.tag == 'Field') then
              table.insert(tbl, field)
            end
          end
        end
      end
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag '_SimpleExp' in lastStat
-- returns a table with nodes
-- @param lastStat node to be searched
-- @return table containing nodes
function lookup.getSimpleExpsFromLastStat(lastStat)
  assert(lastStat.tag == "LastStat", "[table].tag is not LastStat")
  local tbl = {}
    -- [LastStat][ExpList][Exp][_SimpleExp]
  for _,expList in pairs(lastStat.data) do
    if (expList.tag == 'ExpList') then
      local exps = lookup.getExpsFromExpsList(expList)
      for _,exp in pairs(exps) do
        local simpleExps = lookup.getSimpleExpsFromExp(exp)
        for _,simpleExp in pairs(simpleExps) do
          table.insert(tbl, simpleExp)
        end
      end
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag '_SimpleExp' in assign
-- returns a table with nodes
-- @param assign node to be searched
-- @return table containing nodes
function lookup.getSimpleExpsFromAssign(assign)
  assert(assign.tag == "LocalAssign" or assign.tag == "Assign", "[table].tag is not LocalAssign nor Assign")
  local tbl = {}
    -- ([LocalAssign] | [Assign]) [ExpList][Exp][_SimpleExp]
  local expLists = lookup.getExpListsFromAssign(assign)
  for _,expList in pairs(expLists) do    
    local exps = lookup.getExpsFromExpsList(expList)
    for _,exp in pairs(exps) do
      local simpleExps = lookup.getSimpleExpsFromExp(exp)
      for _,simpleExp in pairs(simpleExps) do
        table.insert(tbl, simpleExp)
      end
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'ParList' in simpleExp
-- returns a table with nodes
-- @param simpleExp node to be searched
-- @return table containing nodes
function lookup.getParListsFromSimpleExp(simpleExp)
  assert(simpleExp.tag == "_SimpleExp", "[table].tag is not _SimpleExp")
  local tbl = {}
    -- [_SimpleExp][Function][FuncBody][ParList]
  local functions = lookup.getFunctionsFromSimpleExp(simpleExp)
  for _,func in pairs(functions) do
    local parLists = lookup.getParListsFromFunction(func)
    for _,parList in pairs(parLists) do
      table.insert(tbl, parList)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag '_PrefixExp' in assign
-- returns a table with nodes
-- @param assign node to be searched
-- @return table containing nodes
function lookup.getPrefixExpsFromAssign(assign)
  assert(assign.tag == "LocalAssign" or assign.tag == "Assign", "[table].tag is not LocalAssign nor Assign")
  local tbl = {}
    -- ([LocalAssign] | [Assign]) [ExpList][Exp][_SimpleExp][_PrefixExp]
  local simpleExps = lookup.getSimpleExpsFromAssign(assign)
  for _,simpleExp in pairs(simpleExps) do
    local prefixExps = lookup.getPrefixExpsFromSimpleExp(simpleExp)
    for _,prefixExp in pairs(prefixExps) do
      table.insert(tbl, prefixExp)
    end    
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Name' in field
-- returns a table with nodes
-- @param field node to be searched
-- @return table containing nodes
function lookup.getNamesFromField(field)
  assert(field.tag == "Field", "[table].tag is not Field")
  local tbl = {}
    -- [Field][_FieldID][Exp][_SimpleExp][_PrefixExp][Name]
  for _,fieldID in pairs(field.data) do
    if (fieldID.tag == '_FieldID') then
      for _,exp in pairs(fieldID.data) do
        if (exp.tag == 'Exp') then
          local simpleExps = lookup.getSimpleExpsFromExp(exp)
          for _,simpleExp in pairs(simpleExps) do
            local names = lookup.getNamesFromSimpleExp(simpleExp)
            for _,name in pairs(names) do
              table.insert(tbl, name)
            end
          end
        end
      end
    end
  end 
  return tbl
end

-----------------------------
-- Search for nodes with tag 'Name' in assign
-- returns a table with nodes
-- @param assign node to be searched
-- @return table containing nodes
function lookup.getRightNamesFromAssign(assign)
  assert(assign.tag == "LocalAssign" or assign.tag == "Assign", "[table].tag is not LocalAssign nor Assign")
  local tbl = {}
    -- ([LocalAssign] | [Assign]) [ExpList][Exp][_SimpleExp][_PrefixExp][Name]  
  local prefixExps = lookup.getPrefixExpsFromAssign(assign)
  for _,prefixExp in pairs(prefixExps) do
    local names = lookup.getNamesFromPrefixExp(prefixExp)
    for _,name in pairs(names) do
      table.insert(tbl, name)
    end
  end
  return tbl
end

-----------------------------
-- Search for nodes with tag 'TableConstructor' in assign
-- returns a table with nodes
-- @param assign node to be searched
-- @return table containing nodes
function lookup.getTableConstructorsFromAssign(assign)
  assert(assign.tag == "LocalAssign" or assign.tag == "Assign", "[table].tag is not LocalAssign nor Assign")
  local tbl = {}
    -- ([LocalAssign] | [Assign]) [ExpList][Exp][_SimpleExp][TableConstructor]
  local simpleExps = lookup.getSimpleExpsFromAssign(assign)
  for _,simpleExp in pairs(simpleExps) do
    local tblConstrs = lookup.getTableConstructorsFromSimpleExp(simpleExp)
    for _,tblConstr in pairs(tblConstrs) do
      table.insert(tbl, tblConstr)
    end
  end
  return tbl
end

-----------------------------
-- Search for parent node with tag 'LocalAssign' in prefixExp
-- returns a table with one parent node
-- @param prefixExp node to be searched
-- @return table containing parent node
function lookup.getLocalAssignsFromPrefixExp(prefixExp)
  assert(prefixExp.tag == "_PrefixExp", "[table].tag is not _PrefixExp")
  
    -- [LocalAssign][ExpList][Exp][_SimpleExp][_PrefixExp]
  local simpleExp = prefixExp.parent
  if (simpleExp.tag ~= '_SimpleExp') then return nil end
  
  local exp = simpleExp.parent
  if (exp.tag ~= 'Exp') then return nil end
  
  local expList = exp.parent
  if (expList.tag ~= 'ExpList') then return nil end
  
  local localAssign = expList.parent
  if (localAssign.tag ~= 'LocalAssign') then return nil end

  return localAssign
end

-----------------------------
-- Search for parent node with tag 'LocalAssign' in prefixExp
-- returns a table with one parent node
-- @param prefixExp node to be searched
-- @return table containing parent node
function lookup.getLocalAssignsFromPrefixExp1(prefixExp)
  assert(prefixExp.tag == "_PrefixExp", "[table].tag is not _PrefixExp")
  local tbl = {}
    -- [LocalAssign][ExpList][Exp][_SimpleExp][_PrefixExp]  
  local localAssign = prefixExp.parent.parent.parent.parent
  if (localAssign.tag ~= 'LocalAssign') then return tbl end
  
  tbl = localAssign  
  
  return tbl
end

return lookup