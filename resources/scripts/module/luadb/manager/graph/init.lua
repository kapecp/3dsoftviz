-----------------------------------------------
-- INIT luadb Graph Manager
-- @release 2017/03/17, Denis Illes
-----------------------------------------------

local graphManager = require "luadb.manager.graph.manager"
local utils        = require "luadb.utils"

utils.logger:info('require graph manager - done.')

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = graphManager.new
}