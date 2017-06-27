-- class Observer ------------
class Observer
	name: "Untitled observer"

	new: (name) =>
		@name = name

	notify: =>

	getPrettyTime: (h, m) =>
		out = ""
		if h < 10
			out = '0'
		out ..= h .. ":"
		if m < 10
			out ..= '0'
		out ..= m
		return out

-- class Subject ------------
class Subject
	hours: 0
	minutes: 0

	new: (hours, minutes) =>
		@observers = {}
		@hours = hours
		@minutes = minutes

	register: (observer) =>
		if observer.__class["__parent"]["__name"] == Observer.__name 	-- iba potomkovia Observer
			table.insert(@observers, observer)

	unregister: (observer) =>
		ind = {}
		for i=1, #@observers
			if @observers[i] == observer
				table.insert(ind, i)
		for i=#ind, 1, -1
			table.remove(@observers, ind[i])


	notifyAll: =>
		for o in *@observers do o\notify(@hours, @minutes)

	setState: (hours, minutes) =>
		@hours = hours
		@minutes = minutes
		@notifyAll()


-- class Observer1 ------------
class ObserverSimpleFormat extends Observer
	new: (name) => super(name)

	notify: (h, m) =>
		io.write @name .. ' was notified; '
		if h < 10
			io.write('0')
		io.write(h .. ":")
		if m < 10
			io.write('0')
		io.write(m .. "\n")


-- class Observer2 ------------
class ObserverSpecialFormat extends Observer
	new: (title) => super(title)

	notify: (h, m) =>
		print @name .. " was notified; " .. h .. "-" .. m


-- class Observer3 ------------
class ObserverAMPMFormat extends Observer
	new: (title) => super(title)

	notify: (h, m) =>
		ampm = "am"
		if h > 12
			h -= 12
			ampm = "pm"
		time = super\getPrettyTime(h, m)
		print @name .. " was notified; " .. time .. " " .. ampm

----------------------------

s = Subject(15, 20)
o1 = ObserverSimpleFormat("First")
o2 = ObserverSpecialFormat("Second")
o3 = ObserverAMPMFormat("AM_PM")

s\register(o1)	-- register

print("1. Set state")
s\setState(12, 12)

-- register
s\register(o2)
s\register(o3)
s\register(o2)

print("\n2. Set state")
s\setState(23, 50)


print("\n3. Set state")
s\setState(3, 0)


print("\n4. Set state")
s\setState(16, 45)

s\unregister(o2)	-- unregister

print("\n5. Set state")
s\setState(16, 50)
