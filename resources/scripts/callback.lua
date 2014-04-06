if graph ~= nil then
  for e, is in pairs(graph) do
    for i, v in pairs(is) do
      is[i] = nil
      break
    end
    break
  end
end
if callback ~= nil then
  callback()
end


