-----------------------------------------------
-- INIT luadb Data Mapper
-- @release 2014/04/02, Tomas Filcak
-----------------------------------------------

local mapper = require "luadb.mapping.mapper"
local filter = require "luadb.mapping.filter"
local utils  = require "luadb.utils"

utils.logger:info('require data mapper - done.')

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = mapper.new,
  filter = filter
}