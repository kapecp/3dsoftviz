i1 = {type = "edge_part", id = 1}
i2 = {type = "edge_part", id = 2}
i3 = {type = "edge_part", id = 3}
i4 = {type = "edge_part", id = 4}
i5 = {type = "edge_part", id = 5}
i6 = {type = "edge_part", id = 6}
i7 = {type = "edge_part", id = 7}
i8 = {type = "edge_part", id = 15}
i9 = {type = "edge_part", id = 17}
i10 = {type = "edge_part", id = 18}
i11 = {type = "edge_part", id = 19}

F = {type = "node", id = 8, params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
X = {type = "node", id = 9, params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
Y = {type = "node", id = 10, params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
Z = {type = "node", id = 11, params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}
G = {type = "node", id = 12, params={size = 8, colorA = 1, colorR = 1, colorG = 0, colorB = 0}}

E1 = {type = "edge", note = "temp", id = 13}
E2 = {type = "edge", id = 14}
E3 = {type = "edge", id = 16}

mt = {__index = {orientation = "unoriented"}}
setmetatable(E1, mt)
setmetatable(E2, mt)

w = {
[E1] = {[i1] = F, [i2] = X, [i3] = Y, [i4] = Z},
[E2] = {[i5] = G, [i6] = Y, [i7] = Z},
[E3] = {[i8] = F, [i9] = Z}
}

