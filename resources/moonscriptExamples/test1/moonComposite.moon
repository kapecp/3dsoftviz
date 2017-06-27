class Component
	new: (title) =>
		@name = title

class Composite extends Component
	new: (title) =>
		@children = {}
		super(title)

	operation: (indent=1) =>
		print "Composite: " .. @name
		for item in *@children do 
			for i = 1, indent
				io.write("   ")
			item\operation(indent+1)

	add: (component) =>
		table.insert(@children, component)


class Leaf extends Component
	new: (title) =>
		super(title)

	operation: => 
		print "Leaf: " .. tostring(@name)

--------------------------------
a = Composite("a")
b = Leaf("b")
c = Composite("c")
d = Composite("d")
e = Leaf("e")
f = Leaf("f")
g = Leaf("g")

a\add(b)
c\add(d)
a\add(c)
a\add(e)
c\add(f)
d\add(g)

print "==============="
a\operation()
print "==============="
b\operation()
print "==============="
c\operation()
print "==============="
d\operation()
print "==============="

-------------------

classDecl = Composite("ClassDecl")
classBlock = Composite("ClassBlock")
PopIndent = Leaf("PopIndent")
Exp = Composite("Exp")
PreventIndent = Leaf("PreventIndent")
Extends = Leaf("EXTENDS")
Assignable = Composite("Assignable")
Name = Leaf("Name")
Class = Leaf("CLASS")
value = Composite("Value")
chainValue = Composite("ChainValue")
callable = Composite("Callable")
Name2 = Leaf("Name")


classDecl\add(Class)
classDecl\add(Assignable)
classDecl\add(Extends)
classDecl\add(PreventIndent)
classDecl\add(Exp)
classDecl\add(PopIndent)
classDecl\add(classBlock)
Assignable\add(Name)
Exp\add(value)
value\add(chainValue)
chainValue\add(callable)
callable\add(Name2)

classDecl\operation()
