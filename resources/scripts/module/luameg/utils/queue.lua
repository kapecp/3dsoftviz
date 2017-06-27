
local pQueue = {
    _type = 'Queue'
}

local function Q()
	local arguments = arguments or {}
	
	local o = {
		list = {},
		first_ = 0,
		last_ = -1
	}

	setmetatable(o,
	{
		__index = pQueue,
		__tostring = function(t)
			local out = {}
			table.insert(out, tostring(t._type) .. " with " .. tostring(t:size()) .. " elements from left (first) to right (last): ")
			if t.first_ <= t.last_ then
				for i=t.first_, t.last_ do
					table.insert(out, tostring(t.list[i]) .. ", ")
				end
			end
			return table.concat(out)
		end
		}
	)

	return o
end

function pQueue:pushLeft(value)
	self.first_ = self.first_ - 1
	self.list[self.first_] = value
end

function pQueue:pushRight(value)
	self.last_ = self.last_ + 1
	self.list[self.last_] = value
end

function pQueue:popLeft()
	if self.first_ > self.last_ then
		error("list is empty")
	end

	local value = self.list[self.first_]
	self.list[self.first_] = nil
	self.first_ = self.first_ + 1
	return value
end

function pQueue:popRight()
	if self.first_ > self.last_ then
		error("list is empty")
	end

	local value = self.list[self.last_]
	self.list[self.last_] = nil
	self.last_ = self.last_ - 1
	return value
end

function pQueue:push(value)
	self.pushRight(self, value)
end

function pQueue:pop()
	return self.popLeft(self)
end

function pQueue:right()
	if self.first_ > self.last_ then
		error("list is empty")
	end
	return self.list[self.last_]
end

function pQueue:left()
	if self.first_ > self.last_ then
		error("list is empty")
	end
	return self.list[self.first_]
end

function pQueue:first()
	return self.left(self)
end

function pQueue:last()
	return self.right(self)
end

function pQueue:isEmpty()
	if self.first_ > self.last_ then
		return true
	end

	return false
end

function pQueue:size()
	if self.first_ > self.last_ then
		return 0
	end

	return (self.last_ - self.first_) + 1
end


return {
	new = Q
}
