class   Thing  
  -- static field
  name: "unknown"

class Person extends Thing
  -- static method
  say_name: => print "Hello, I am #{@name}!"

class Inventory
  a: 5
  new: (owner) =>
    @items = {}	-- member field
    @owner = owner

  add_item: (name) =>
    if @items[name]
      @items[name] += 1
    else
      @items[name] = 1

    print @@__name .. "." .. @owner .. " -> added item " .. name .. ". Actual count: " .. @items[name]

    temp = 2

inv = Inventory("A")
inv\add_item "t-shirt"

inv2 = Inventory("B")
inv2.a = 2
inv2\add_item "stick"

inv\add_item "pants"
inv\add_item "stick"
inv2\add_item "pants"
inv2\add_item "coin"
inv2\add_item "coin"
inv\add_item "coin"
