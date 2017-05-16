
local pStack = {
    _type = 'Stack'
}

local function S()
	local arguments = arguments or {}
	
	local o = {
		list = {},
	}

	setmetatable(o,
	{
		__index = pStack,
		__tostring = function(t)
			local out = {}
			table.insert(out, tostring(t._type) .. " with " .. tostring(t:size()) .. " elements from bottom to top: ")
			for k, v in pairs(t.list) do
				table.insert(out, tostring(v) .. ", ")
			end
			return table.concat(out)
		end
		}
	)

	return o
end

function pStack:push(value)
	table.insert(self.list, value)
end

function pStack:top()
	return self.list[#self.list]
end

function pStack:pop()
	local out = self.list[#self.list]
	self.list[#self.list] = nil
	return out
end

function pStack:isEmpty()
	return #self.list == 0
end

function pStack:size()
	return #self.list
end


return {
	new = S
}
