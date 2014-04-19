-----------------------------------------------
-- EDGE - hypergraph edge
-- @release 2014/03/13, Tomas Filcak
-----------------------------------------------

local utils = require "luadb.utils"

local pEdge = {
		_type = 'Edge'
}

local n = 0
local function inc()
  n = n + 1
  return n
end

local function E(arguments)
  local arguments = arguments or {}
  
	local o = {
    id = "E"..inc(),
    label = nil,
    orientation = nil,
    incidences  = {},
    from  = {},
    to    = {},
    data  = {}
	}

	setmetatable(o,
		{	
			__index = pEdge, 
			__tostring = function(t)
				if t.id and t.label then return "edge "..t.id..":"..t.label
        elseif t.id then return "edge "..t.id
        else return "edge" end
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
-- Edge helper functions
-----------------------------------------------

-- edge_data & nodes expect to be table structure
function pEdge:setAsUnoriented()
  self.orientation = "Unoriented"
end


-- edge_data & from & to expect to be table structure
function pEdge:setAsOriented()
  self.orientation = "Oriented"
end


function pEdge:isOriented()
  return self.orientation and self.orientation == "Oriented" and true or false;
end


function pEdge:addTarget(node)
  if utils.isTable(node) then
    assert(node.id, "id must be defined")
    table.insert(self.to, node.id)
  else
    table.insert(self.to, node)
  end
end


function pEdge:addSource(node)
  if utils.isTable(node) then
    assert(node.id, "id must be defined")
    table.insert(self.from, node.id)
  else
    table.insert(self.from, node)
  end
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = E
}
