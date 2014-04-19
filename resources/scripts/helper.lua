local function vardump(value, depth, key, visited)
  local linePrefix = ""
  local spaces = ""
  
  if key ~= nil then
    linePrefix = "["..tostring(key).."] = "
  end
  
  if depth == nil then
    depth = 0
  else
    depth = depth + 1
    for i=1, depth do spaces = spaces .. "  " end
  end

  if visited == nil then 
    visited = {}
  end
  
  if type(value) == 'table' and visited[value] == nil then
    visited[value] = true    
    print(spaces ..linePrefix.."(table) "..tostring(value))
    mTable = getmetatable(value)

    if mTable ~= nil then
      vardump(mTable, depth, '(metatable)', visited)
    end		
    for tableKey, tableValue in pairs(value) do
      vardump(tableValue, depth, tableKey, visited)
    end
  elseif type(value)	== 'function' or 
      type(value)	== 'thread' or 
      type(value)	== 'userdata' or
      value		== nil
  then
    print(spaces..linePrefix..tostring(value))
  else
    print(spaces..linePrefix.."("..type(value)..") "..tostring(value))
  end
end

return {vardump = vardump}
