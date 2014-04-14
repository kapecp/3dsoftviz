local function inc()
  a = a or 0
  a = a + 1
  return a
end

local N1 = {type = "node", id = inc(), label = "printMessage", params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
local N2 = {type = "node", id = inc(), label = "fileExists", params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
local N3 = {type = "node", id = inc(), label = "readFile", params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
local N4 = {type = "node", id = inc(), label = "tblPrint", params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
local N5 = {type = "node", id = inc(), label = "printKeys", params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
local N6 = {type = "node", id = inc(), label = "printValues", params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
local N7 = {type = "node", id = inc(), label = "mergeTbls", params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
local N8 = {type = "node", id = inc(), label = "testTbl", params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
local N9 = {type = "node", id = inc(), label = "proceedMerge", params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
local N10 = {type = "node", id = inc(), label = "print", params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}

graph = {
[{type = "edge", id = inc(), label = "", params = {size = 0}}] = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] = N3, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N2},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] = N2, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N1},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] = N5, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N1},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] =  N6, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N1},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] =  N7, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N1},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] =  N7, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N1},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] =  N4, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N5},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] =  N7, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N8},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] =  N7, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N8},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] =  N9, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N8},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] =  N4, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N6},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] =  N1, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N10},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] =  N7, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N9},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] =  N7, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N4},
[{type = "edge", id = inc(), label = "", params = {size = 0}}]  = {[{type = "edge_part", id = inc(), direction = "in", label = "", params = {size = 0}}] =  N7, [{type = "edge_part", id = inc(), direction = "out", label = "", params = {size = 0}}] = N4},
}
