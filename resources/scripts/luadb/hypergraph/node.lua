-----------------------------------------------
-- NODE - hypergraph node
-- @release 2014/03/13, Tomas Filcak
-----------------------------------------------

local utils = require "luadb.utils"

local pNode = {
		_type = 'Node'
}

local n = 0
local function inc()
	n = n + 1
	return n
end

local function N(arguments)
  local arguments = arguments or {}
  
	local o = {
    id    = "N"..inc(),
  	data  = {},
  	label = nil
	}

	setmetatable(o,
		{
			__index = pNode,
			__tostring = function(t)
				if t.id and t.label then return "node "..t.id..":"..t.label
        elseif t.id then return "node "..t.id
        else return "node" end
			end
		}
	)
	
	-- passing arguments
  for key,value in pairs(arguments) do
    o[key] = value
  end

	return o
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
	new = N
}
