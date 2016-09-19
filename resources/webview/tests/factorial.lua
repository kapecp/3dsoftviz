-- Prints table keys and their values
function printTable(t)

  -- Loop over table key and value pairs
  for key,value in pairs(t) do

    -- Print key and value
    print(key,value)
  end
end

-- Iterates over numbers in range from to and add these numbers to table and return it.
-- Each table record consists of iterated numbers as keys and its factorial as values
function multiFactorial(from,to)

  -- Validate input
  if from < 0 then
    error("from must be < 0")
  end

  if to < 0 then
    error("to must be < 0")
  end

  if from > to then
    error("from must be < to")
  end

  -- Create new array
  arr = {}

  -- Loop over numbers in range from to
  for num=from,to do

    -- Add number to table with its initial value
    arr[num] = num

    -- Iterate over each num and in descending order calculate its factorial
    for factNum = num - 1, 1, -1 do
      arr[num] = arr[num] * factNum
    end
  end

  return arr
end

printTable(multiFactorial(0,10))