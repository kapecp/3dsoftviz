-----------------------------------------------
-- CONNECTION - luadb TOKYO database ADAPTER
-- @release 2014/04/13, Tomas Filcak
-----------------------------------------------

require "tokyocabinet"
local utils = require "luadb.utils"


local pTokyoAdapter = {
  tokyocabinet = tokyocabinet 
}

local function construct(arguments)
  local arguments = arguments or {}
  local path = utils.fileExists(arguments.targetDirectory) and arguments.targetDirectory or "./"
  local dbName = arguments.databaseName or "casket"
  
  local o = {
     hashDB = tokyocabinet.tdbnew(),
     hashPath = path .. "/" .. dbName .. ".tct",
     collectionDB = tokyocabinet.bdbnew(),
     collectionPath = path .. "/" .. dbName .. ".tcb"
  }

  setmetatable(o,
    {
      __index = pTokyoAdapter,
      __tostring = function()
        return "tokyo connection adapter"
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
-- Database functions
-----------------------------------------------

function pTokyoAdapter:connect()
  assert(tokyocabinet, "tokyocabinet module not properly loaded")
  
  -- collection
  if not self.collectionDB:open(self.collectionPath, self.collectionDB.OWRITER + self.collectionDB.OCREAT) then
     local ecode = self.collectionDB:ecode()
     print("open error: " .. self.collectionDB:errmsg(ecode))
  end
  
  -- hash
  if not self.hashDB:open(self.hashPath, self.hashDB.OWRITER + self.hashDB.OCREAT) then
     local ecode = self.hashDB:ecode()
     print("open error: " .. self.hashDB:errmsg(ecode))
  end
end


function pTokyoAdapter:disconnect()
  assert(tokyocabinet, "tokyocabinet module not properly loaded")
  
  -- collection
  if not self.collectionDB:close() then
     local ecode = self.collectionDB:ecode()
     print("close error: " .. self.collectionDB:errmsg(ecode))
  end
  
  -- hash
  if not self.hashDB:close() then
     local ecode = self.hashDB:ecode()
     print("close error: " .. self.hashDB:errmsg(ecode))
  end
end


function pTokyoAdapter:flush()
  assert(self.hashDB, "no connection detected")
  assert(self.collectionDB, "no connection detected")
  
  assert(self.hashDB:vanish() == true, "failed to execute commnad VANISH on hash db")
  assert(self.collectionDB:vanish() == true, "failed to execute commnad VANISH on collection db")
  
  utils.logger:info("database cleared")
end

-----------------------------------------------
-- Collections functions B+ tree database
-----------------------------------------------

function pTokyoAdapter:add(collection, member)
  assert(self.collectionDB, "no connection detected")
  if not self:isMember(collection, member) then
    assert(self.collectionDB:putdup(collection, member), "failed to execute commnad PUTDUP "..collection)
  end
end


function pTokyoAdapter:addMultiple(collection, members)
  assert(self.collectionDB, "no connection detected")
  for k,v in pairs(members) do
    self:add(collection, v)
  end
end


function pTokyoAdapter:remove(collection, member)
  local resultTable = self:members(collection)
  for i=#resultTable,1,-1 do
    if resultTable[i] == member then
       table.remove(resultTable, i)
       break
    end
  end
  self:removeAll(collection)
  local members = self:members(collection)
  self:addMultiple(collection, resultTable)
end


function pTokyoAdapter:removeAll(collection)
  assert(self.collectionDB, "no connection detected")
  assert(self.collectionDB:outlist(collection), "failed to execute command OUT "..collection)
end


function pTokyoAdapter:members(collection)
  assert(self.collectionDB, "no connection detected")
  return self.collectionDB:getlist(collection) or {}
end


function pTokyoAdapter:isMember(collection, member)
  local members = self:members(collection) or {}
  for key,value in pairs(members) do
    if value == member then return true end
  end
  return false
end


function pTokyoAdapter:count(collection)
  assert(self.collectionDB, "no connection detected")
  local members = self:members(collection) or {}
  return table.getn(members)
end

-----------------------------------------------
-- Hash functions
-----------------------------------------------

function pTokyoAdapter:set(hash, key, value)
  assert(utils.isTable(value) == false, "table structure as value not supported")
  assert(self.hashDB, "no connection detected")
  assert(self.hashDB:put(hash, { key = value }), "failed to execute commnad PUT "..hash)
end


function pTokyoAdapter:get(hash, key)
  assert(hash, "hash name must be defined")
  assert(self.hashDB, "no connection detected")
  local resultSet = self.hashDB:get(hash) or {}
  return resultSet[key]
end


function pTokyoAdapter:getAll(hash)
  assert(hash, "hash name must be defined")
  assert(self.hashDB, "no connection detected")
  return self.hashDB:get(hash) or {}
end


function pTokyoAdapter:setMultiple(hash, t_attributes)
  assert(utils.isTable(value) == false, "table structure as value not supported")
  assert(self.hashDB, "no connection detected")
  assert(self.hashDB:put(hash, t_attributes), "failed to execute commnad PUT "..hash)
end


function pTokyoAdapter:delete(hash, key)
  local resultHash = self:getAll(hash)
  resultHash[key] = nil
  self:deleteAll(hash)
  self:setMultiple(hash, resultHash)
end


function pTokyoAdapter:deleteAll(hash)
  assert(hash, "hash name must be defined")
  assert(self.hashDB, "no connection detected")
  assert(self.hashDB:out(hash), "failed to execute commnad OUT "..hash)
end


function pTokyoAdapter:exists(hash, key)
  local values = self:getAll(hash)
  return values[key] ~= nil
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = construct
}