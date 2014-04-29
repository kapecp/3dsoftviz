-----------------------------------------------
-- INIT luadb HYPERGRAPH
-- @release 2013/12/03, Tomas Filcak
-----------------------------------------------

local graph     = require 'luadb.hypergraph.graph'
local node      = require 'luadb.hypergraph.node'
local edge      = require 'luadb.hypergraph.edge'
local incidence = require 'luadb.hypergraph.incidence'
local utils     = require 'luadb.utils'

utils.logger:info('require hypergraph - done.')

-----------------------------------------------
-- Return
-----------------------------------------------

return {
	graph = graph,
	node = node,
	edge = edge,
	incidence = incidence
}