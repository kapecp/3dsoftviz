-----------------------------------------------
-- FILTER - luadb
-- @release 2014/04/15, Tomas Filcak
-----------------------------------------------

local utils = require "luadb.utils"

local pFilter = {
  _type = "Filter"
}

local function construct(name, attributePath)
  assert(name, "filter name must be defined")
  assert(type(name) == "string", "filter name must be string")
  local attribute = attributePath or name
  
  local o = { 
    name = utils.trim(name),
    attributePath = attribute
  }
  
  setmetatable(o,
    {
      __index = pFilter,
      __tostring = function()
        return "filter for ohm data mapper"
      end
    }
  )

  return o
end

-----------------------------------------------
-- Helper functions
-----------------------------------------------

local function transform(str)
  return utils.firstToUpper(utils.lowerCase(utils.trim(str)))
end


function pFilter:getIdentifier()
  return self.name:lower().."Filter"
end


function pFilter:getObjectIdentifier(value)
  return self:getIdentifier()..":"..value:gsub("%s+", "-")
end


function pFilter:getValueByPath(t_object, s_path)
  assert(type(s_path) == "string", "path must be string")
  assert(utils.isTable(t_object), "passed param must be an object")

  local var = t_object
  local s_path = string.gsub(s_path, "/", "\.")
  local path = utils.split(s_path, '%.')
  for k,v in pairs(path) do var = var[v] end
  return var
end

-----------------------------------------------
-- Provided functionality
-----------------------------------------------

local function load(mapper, param)
  assert(param, "expected to get argument")
  local call  = debug.getinfo(1,"n")
  local filter = mapper:identifyFilterByCall(call) 
  local ids = mapper.adapter:members(filter:getObjectIdentifier(param))
  local result = mapper:loadAll(ids)
  
  if result and utils.isArray(result) and table.getn(result) == 1 then
    return result[1]
  else
    return result
  end
end


local function exists(mapper, param)
  assert(param, "expected to get argument")
  local call  = debug.getinfo(1,"n")
  local filter = mapper:identifyFilterByCall(call)
  if filter then
    return mapper.adapter:isMember(filter:getIdentifier(), filter:getObjectIdentifier(param))
  else
    error("exists function failed, no filter found for function "..call)
  end
end

-----------------------------------------------
-- Filter visitor implementation
-----------------------------------------------

function pFilter:visit(mapper)
  assert(mapper.adapter, "adapter must be defined and assigned")
  self.adapter = mapper.adapter
  
  local loadFunctionName = "loadBy"..transform(self.name)
  local existsFunctionName = "existsWith"..transform(self.name)
  
  mapper[loadFunctionName]   = load
  mapper[existsFunctionName] = exists
  
  self[loadFunctionName]   = true
  self[existsFunctionName] = true
end


function pFilter:register(object)
  local attributeValue = self:getValueByPath(object, self.attributePath)
  self.adapter:add(self:getIdentifier(), self:getObjectIdentifier(attributeValue))
  self.adapter:add(self:getObjectIdentifier(attributeValue), object.id)
end


function pFilter:unregister(object)
  local attributeValue = self:getValueByPath(object, self.attributePath)
  local s_objectsSet   = self:getIdentifier()
  local s_objectIdsSet = self:getObjectIdentifier(attributeValue)
  
  self.adapter:remove(s_objectIdsSet, object.id)
  if self.adapter:count(s_objectIdsSet) == 0 then
    self.adapter:remove(s_objectsSet, s_objectIdsSet)
  end
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = construct
}