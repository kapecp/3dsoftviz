package.path = package.path .. ";../share/3dsoftviz/scripts/?.lua;../share/3dsoftviz/scripts/?/init.lua"

function_call_graph = require "softviz/function_call_graph"
logical_filter = require "softviz/logical_filter"
slt2_renderer = require "softviz/slt2_renderer"
