-----------------------------------------------
-- CONNECTION - luadb REDIS database ADAPTER
-- @release 2014/03/31, Tomas Filcak
-----------------------------------------------

local utils   = require "luadb.utils"
local hiredis = require "hiredis"

local HOST = "localhost"
local PORT = 6379

local pRedisAdapter = {
  hiredis = hiredis
}

local function construct(arguments)
  local arguments = arguments or {}
  local port = arguments.port or PORT
  local host = arguments.host or HOST

  local o = {
     port = port,
     host = host, 
     statement = nil
  }

  setmetatable(o,
    {
      __index = pRedisAdapter,
      __tostring = function()
        return "redis connection adapter"
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
-- Helper functions
-----------------------------------------------

local function f_unpack(params)
  local l_unpack = unpack or table.unpack or nil
  assert(l_unpack, "unpack function not known")
  return l_unpack(params)
end

--@param sequence [key1, valu1, key2, value2,...]
--@return associative table
local function f_pack(sequence)
  assert(utils.isTable(sequence), "input param must be a table in '[key1, value1, key2, value2, ...]' format")
  local isKey = true
  local resultHash = {}
  local theKey, theValue
  
  for value in pairs(sequence) do
    if isKey then
      theKey = sequence[value]
      isKey = false
    else
      theValue = sequence[value]
      resultHash[theKey] = theValue
      isKey = true
    end
  end
  
  return resultHash
end


-----------------------------------------------
--Return only params with primitive data types
--@return keyset     = list of all keys
--@return attributes = key/value pairs of params
local function preparePrimitiveAttributes(tbl)
    local attributes = {}
    if utils.isTable(tbl) then
      for key, value in pairs(tbl) do 
        if (utils.isTable(value) == false) then
          table.insert(attributes,key)
          table.insert(attributes,value)
        end
      end
    end
    
    return attributes
end

-----------------------------------------------
-- Database functions
-----------------------------------------------

function pRedisAdapter:connect(host, port)
  local host = host or self.host
  local port = port or self.port
  
  assert(hiredis, "redis module not properly loaded")
  self.connection = hiredis.connect(host, port)
  utils.logger:info("database connection opened")
end


function pRedisAdapter:disconnect()
  assert(hiredis, "redis module not properly loaded")
  if self.connection then
    self.connection:close()
    utils.logger:info("database closed")
  end
end


function pRedisAdapter:flush()
  assert(self.connection, "no connection detected")
  assert(self.connection:command("FLUSHALL") == hiredis.OK, "failed to execute commnad FLUSHALL")
  utils.logger:info("database cleared")
end


function pRedisAdapter:execute(s_query)
  assert(self.connection, "no connection detected")
  
  local preparedQuery = {}
  local query = utils.split(s_query, " ")
  for pos in pairs(query) do
    if query[pos] ~= "" then table.insert(preparedQuery, query[pos]) end
  end
  
  return self.connection:command(f_unpack(preparedQuery))
end

-----------------------------------------------
-- Collections functions
-----------------------------------------------

function pRedisAdapter:add(collection, member)
  assert(self.connection, "no connection detected")
  assert(self.connection:command("SADD", collection, member), "failed to execute commnad SADD "..collection)
end


function pRedisAdapter:addMultiple(collection, members)
  assert(self.connection, "no connection detected")
  assert(self.connection:command("SADD", collection, f_unpack(members)), "failed to execute commnad SADD "..collection)
end


function pRedisAdapter:remove(collection, member)
  assert(self.connection, "no connection detected")
  assert(self.connection:command("SREM", collection, member), "failed to execute command SREM "..collection)
end


function pRedisAdapter:removeAll(collection)
  assert(self.connection, "no connection detected")
  assert(self.connection:command("DEL", collection), "failed to execute command DEL "..collection)
end


function pRedisAdapter:members(collection)
  assert(self.connection, "no connection detected")
  return self.connection:command("SMEMBERS", collection)
end


function pRedisAdapter:isMember(collection, member)
  assert(self.connection, "no connection detected")
  local result = self.connection:command("SISMEMBER", collection, member)
  if result == 0 then return false else return true end
end


function pRedisAdapter:count(collection)
  assert(self.connection, "no connection detected")
  return self.connection:command("SCARD", collection)
end

-----------------------------------------------
-- Hash functions
-----------------------------------------------

function pRedisAdapter:set(hash, key, value)
  assert(utils.isTable(value) == false, "table structure as value not supported")
  assert(self.connection, "no connection detected")
  assert(self.connection:command("HSET", hash, key, value), "failed to execute commnad HMSET "..hash)
end


function pRedisAdapter:get(hash, key)
  assert(hash, "hash name must be defined")
  assert(self.connection, "no connection detected")
  return self.connection:command("HGET", hash, key)
end


function pRedisAdapter:getAll(hash)
  assert(hash, "hash name must be defined")
  assert(self.connection, "no connection detected")
  local resultSet = self.connection:command("HGETALL", hash)
  return f_pack(resultSet)
end


function pRedisAdapter:setMultiple(hash, t_attributes)
  assert(hash, "hash name must be defined")
  assert(self.connection, "no connection detected")
  assert(utils.isTable(t_attributes), "input parameter must be table of key/value pairs")
  local attributes = preparePrimitiveAttributes(t_attributes)
  assert(self.connection:command("HMSET", hash, f_unpack(attributes)), "failed to execute commnad HMSET "..hash)
end


function pRedisAdapter:delete(hash, key)
  assert(hash, "hash name must be defined")
  assert(self.connection, "no connection detected")
  assert(self.connection:command("HDEL", hash, key), "failed to execute commnad HDEL "..hash)
end


function pRedisAdapter:deleteAll(hash)
  assert(hash, "hash name must be defined")
  assert(self.connection, "no connection detected")
  assert(self.connection:command("DEL", hash), "failed to execute commnad DEL "..hash)
end


function pRedisAdapter:exists(hash, key)
  assert(hash, "hash name must be defined")
  assert(self.connection, "no connection detected")
  local result = false
  if hash and key then
    result = self.connection:command("HEXISTS", hash, key)
  else
    result = self.connection:command("EXISTS", hash)
  end
  if result == 0 then return false else return true end
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = construct
}