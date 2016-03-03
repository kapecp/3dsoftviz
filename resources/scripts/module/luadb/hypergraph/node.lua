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

-------------------------------- Find functions

local function findByName(nodes, name)
  local occurrences = {}
  for i,node in pairs(nodes) do
    if node.data.name and node.data.name == name then
      table.insert(occurrences, node)
    end
  end
  return occurrences
end


-- get all ids for nodes with selected name
local function findIdsByName(nodes, name)
  local occurrenceIds = {}
  for i,node in pairs(nodes) do
    if node.data.name and node.data.name == name then
      table.insert(occurrenceIds, node.id)
    end
  end
  return occurrenceIds
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
	new = N,
	findByName = findByName,
  findIdsByName = findIdsByName
}
