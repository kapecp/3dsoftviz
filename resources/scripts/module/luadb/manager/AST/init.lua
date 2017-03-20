-----------------------------------------------
-- INIT luadb Manager
-- @release 2017/03/17, Denis Illes
-----------------------------------------------

local astManager = require "luadb.manager.AST.manager"
local utils      = require "luadb.utils"

utils.logger:info('require manager - done.')

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = astManager.new
}