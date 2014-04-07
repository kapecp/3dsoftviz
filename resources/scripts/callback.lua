if graph ~= nil then
  isempty = true
  for e, is in pairs(graph) do
    for i, v in pairs(is) do
      isempty = false
      is[i] = nil
      break
    end
    if isempty then
      graph[e] = nil
    end
    break
--[[
    print("Existing edge before remove", e.id)
    if e.id == 14 then
      print("removing edge", e.id)
      graph[e] = nil
      break
    end]]--
  end
end

if callback ~= nil then
  callback()
end


