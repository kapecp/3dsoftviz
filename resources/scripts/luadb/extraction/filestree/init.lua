-----------------------------------------------
-- INIT luadb
-- @release 2014/04/18, Tomas Filcak
-----------------------------------------------

local utils              = require "luadb.utils"
local filesTreeExtractor = require "luadb.extraction.filestree.extractor"

utils.logger:info('require filestree extractor - done.')

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract = filesTreeExtractor.extract
}