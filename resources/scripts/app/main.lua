graph_importer = require "softviz/graph_importer"
logical_filter = require "softviz/logical_filter"
slt2_renderer = require "softviz/slt2_renderer"


-- support for better stac trace repports
local STP = require "StackTracePlus"
debug.traceback = STP.stacktrace

--[[
-- simple test to test StackTracePlus
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

-- singleton for remote debbuging with ZeroBrane Studio
theDebugger = require("mobdebug")
theDebugger.start() 
