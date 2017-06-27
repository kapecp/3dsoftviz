-----------------------------------------------
-- INIT luadb AST Manager
-- @release 2017/03/17, Denis Illes
-----------------------------------------------

local astManager = require "luadb.manager.AST.manager"
local utils      = require "luadb.utils"

utils.logger:info('require AST manager - done.')

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = astManager.new
}