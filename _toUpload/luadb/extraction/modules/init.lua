-----------------------------------------------
-- INIT luadb.extraction.modules
-- @release 2016/15/11, Denis Illes
-----------------------------------------------

local utils            = require "luadb.utils"
local modulesExtractor = require "luadb.extraction.modules.extractor"

utils.logger:info('require modules extractor - done.')

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract = modulesExtractor.extract
}