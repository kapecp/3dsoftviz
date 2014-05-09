local i1 = {type = "edge_part", id = 1}
local i2 = {type = "edge_part", id = 2}
local i3 = {type = "edge_part", id = 3}
local i4 = {type = "edge_part", id = 4}
local i5 = {type = "edge_part", id = 5}
local i6 = {type = "edge_part", id = 6}
local i7 = {type = "edge_part", id = 7}
local i8 = {type = "edge_part", id = 15}
local i9 = {type = "edge_part", id = 17}
local i10 = {type = "edge_part", id = 18}
local i11 = {type = "edge_part", id = 19}

local F = {type = "node", id = 8}
local X = {type = "node", id = 9}
local Y = {type = "node", id = 10}
local Z = {type = "node", id = 11}
local G = {type = "node", id = 12}

local E1 = {type = "edge", id = 13}
local E2 = {type = "edge", id = 14}
local E3 = {type = "edge", id = 16}

local w = {
[E1] = {[i1] = F, [i2] = X, [i3] = Y, [i4] = Z},
[E2] = {[i5] = G, [i6] = Y, [i7] = Z},
[E3] = {[i8] = F, [i9] = Z}
}

local function getGraph()
  return w
end 

return {getGraph = getGraph}

