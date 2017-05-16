-----------------------------------------------
-- INIT luadb
-- @release 2013/12/03, Tomas Filcak
-----------------------------------------------

local utils              = require "luadb.utils"
local funcCallsExtractor = require "luadb.extraction.functioncalls.extractor"

utils.logger:info('require function calls extractor - done.')

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  extract = funcCallsExtractor.extract,
  extractFromString = funcCallsExtractor.extractFromString,
  extractFromAST = funcCallsExtractor.extractFromAST
}
