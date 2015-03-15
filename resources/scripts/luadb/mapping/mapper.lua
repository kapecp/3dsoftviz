-----------------------------------------------
-- MAPPER - luadb ABSTRACT data mapper
-- @release 2014/04/2, Tomas Filcak
-----------------------------------------------

local utils = require "luadb.utils"

local pMapper = {
  _type = "Mapper"
}

local function construct(arguments)
  local arguments = arguments or {}
  
  assert(arguments.entity, "entity property must be defined, singular is recommended")
  assert(arguments.adapter, "database adapter property must be assigned")
  
  local o = {
    filters = {}
  }
  setmetatable(o,
    {
      __index = pMapper,
      __tostring = function()
        return "data mapper"
      end
    }
  )
  
  -- passing arguments
  for key,value in pairs(arguments) do
    o[key] = value
  end

  return o
end

-----------------------------------------------
-- Filter registration
-----------------------------------------------

function pMapper:register(filter)
  assert(filter._type == "Filter", "object must be a filter")
  assert(self.filters[filter:getIdentifier()] == nil, "filter already registrated")
  self.filters[filter:getIdentifier()] = filter
  filter:visit(self)
end

function pMapper:registerToFilters(object)
  -- save to set for filter functionality
  for filterName,filter in pairs(self.filters) do
    filter:register(object)
  end
end


function pMapper:unregisterFromFilters(object)
  -- delete filter data about object
  for filterName,filter in pairs(self.filters) do
    filter:unregister(object)
  end
end


function pMapper:identifyFilterByCall(call)
  for i,filter in pairs(self.filters) do
    if filter[call.name] then
      return filter  
    end
  end
  return nil
end

-----------------------------------------------
-- Helper functions
-----------------------------------------------

function pMapper:getEntityName()
  return "s_"..self.entity
end


function pMapper:getObjectIdentifier(nodeId)
  return self.entity..":"..nodeId
end
 
 
function pMapper:getStructuresIdentifier(nodeId)
  return self.entity..":"..nodeId..":structures"
end


function pMapper:getStructureIdentifier(nodeId, key)
  return self.entity..":"..nodeId..":structure"..":"..key
end


function pMapper:validatePreconditions()
  assert(self.entity, "entity property must be defined, singular is recommended")
  assert(self.adapter, "no database adapter attached")
end

-----------------------------------------------
-- Model functions
-----------------------------------------------

function pMapper:existsWithId(id)
  self:validatePreconditions()
  assert(id, "id must be specified")
  return self.adapter:isMember(self:getEntityName(), id)
end


function pMapper:count()
  self:validatePreconditions()
  return self.adapter:count(self:getEntityName())
end


function pMapper:execute(query)
  assert(self.adapter, "no database adapter attached")
  assert(query, "id must be specified")
  return self.adapter:execute(query)
end

-----------------------------------------------
-- Manipulation functions
-----------------------------------------------

function pMapper:save(object)
  self:validatePreconditions()
  assert(utils.isTable(object), "object must be a table")
  assert(object.id, "object id must be specified")
  
  -- create or update
  if self:existsWithId(object.id) then
    self.adapter:deleteAll(self:getObjectIdentifier(object.id))
    self.adapter:removeAll(self:getStructuresIdentifier(object.id))
  else
    self.adapter:add(self:getEntityName(), object.id)
  end
  
  -- find nested structures in first depth level
  local objectStructuresKeys = {}
  for k,v in pairs(object) do 
    if utils.isTable(v) then
      table.insert(objectStructuresKeys,k)
    end
  end
  
  -- save meta data for created filters
  self:registerToFilters(object)
  -- save root primitive attributes
  self.adapter:setMultiple(self:getObjectIdentifier(object.id), object)
  -- save keys of tables in first depth level
  self.adapter:addMultiple(self:getStructuresIdentifier(object.id), objectStructuresKeys)
  -- save primitiv values for nested tables in first depth level
  for i,k in pairs(objectStructuresKeys) do
    self.adapter:setMultiple(self:getStructureIdentifier(object.id, k), object[k])
  end
end


function pMapper:remove(object)
  self:validatePreconditions()
  assert(utils.isTable(object), "object must be a table")
  assert(object.id, "object id must be specified")
  
  self.adapter:remove(self:getEntityName(), object.id)
  self.adapter:deleteAll(self:getObjectIdentifier(object.id))
  
  -- remove meta data for created filters
  self:unregisterFromFilters(object)
  -- remove nested structures in first depth level
  local objectStructuresKeys = self.adapter:members(self:getStructuresIdentifier(object.id))
  for i,k in pairs(objectStructuresKeys) do
    self.adapter:deleteAll(self:getStructureIdentifier(object.id, k))
  end
  self.adapter:removeAll(self:getStructuresIdentifier(object.id))
end


function pMapper:removeById(id)
  local object = self:loadById(id)
  self:remove(object)
end


function pMapper:loadById(id)
  self:validatePreconditions()
  assert(id, "id must be specified")
  
  if self:existsWithId(id)  then
    local object = self.adapter:getAll(self:getObjectIdentifier(id))
    
    local objectStructuresKeys = self.adapter:members(self:getStructuresIdentifier(id))
    -- assign nested structures data in first depth level
    for i,k in pairs(objectStructuresKeys) do
      object[k] = self.adapter:getAll(self:getStructureIdentifier(id, k))
    end
        
    return object
  else
    return nil
  end 
end


function pMapper:loadAll(ids)
  self:validatePreconditions()
  local objectIds = ids or self.adapter:members(self:getEntityName())
  
  local objects = {}
  if utils.isTable(objectIds) then
    for index,objectId in pairs(objectIds) do
      table.insert(objects, self:loadById(objectId))
    end
  end
  
  return objects
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = construct
}