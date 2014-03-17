metrics = require"metrics"

test = { foo = "test1"}

test2 = "test2"

test3 = 42

function bar(param)
  param = param or "not set"
  print("test function: " .. param)
end

test4 = {func = bar}

print"Hello from Lua ;-)"
