function_call_graph = require "softviz/function_call_graph"
logical_filter = require "softviz/logical_filter"
slt2_renderer = require "softviz/slt2_renderer"

local STP = require "StackTracePlus"
debug.traceback = STP.stacktrace

--[[
function test()
    local s = "this is a string"
    local n = 42
    local t = { foo = "bar" }
    local co = coroutine
    local cr = coroutine.create

    local t = {}
    local a=nil
    --print(t.b.c)
    error("an error")
end
test()
--]]