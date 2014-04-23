package.path = package.path .. ";../share/3dsoftviz/scripts/?.lua;../share/3dsoftviz/scripts/?/init.lua"

test_graph = require "test_graph"
filetree_graph = require "filetree_graph"
function_call_graph = require "function_call_graph"
logical_filter = require "logical_filter"
