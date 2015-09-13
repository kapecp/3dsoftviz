-----------------------------------------------
-- INCIDENCE - hypergraph incidence
-- @release 2014/03/18, Tomas Filcak
-----------------------------------------------

local pIncidence = {
		_type = 'Incidence',
}

local n = 0
local function inc()
	n = n + 1
	return n
end

local function I(arguments)
  local arguments = arguments or {} 
	
	local o = {
	  id    = "I"..inc(),
	  label = nil,
	  graph = nil,
	  node  = nil,
	  data  = {}
	}   
	
	setmetatable(o,  
		{	
			__index = pIncidence, 
			__tostring = function(t)
			 	if t.id and t.label then return "incidence "..t.id..":"..t.label
        elseif t.id then return "incidence "..t.id
        else return "incidence" end
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
  newIncidence = I
}
