-----------------------------------------------
-- UTILS - module
-- @release 2013/12/03, Tomas Filcak
-----------------------------------------------

local lfs = require "lfs"
require "logging"

-----------------------------------------------
-- Funcion to cover logging functionality
-- Manual http://www.keplerproject.org/lualogging/manual.html
local logger = logging.new(function(self, level, message)
                             print(level, message)
                             return true
                           end)

-----------------------------------------------
-- Set default minimum level to show
-- @param logging.DEBUG, INFO, WARN, ERROR, FATAL
--logger:setLevel(logging.INFO)


local function getLogger()
  return logger
end


local function isLuaFile(file)
  if type(file) == "string" then
    local found = file:find("%plua", -4)
    return found ~= nil
  else
    error("input parameter expected to be string")
  end
end


local function isHidden(file)
  if type(file) == "string" then
    return file:sub(1,1) == "."
  else
    error("input parameter expected to be string")
  end
end


local function isTable(var)
  return (type(var) == "table") and true or false
end


local function isArray(var)
  if type(var) ~= "table" then return nil end

  local count = 0
  for key,value in pairs(var) do
    if type(key) ~= "number" then return false else count = count + 1 end
  end

  for i = 1, count do
    if not var[i] and type(var[i]) ~= "nil" then return false end
  end
  return true
end


local function isEmpty(tbl)
  if next(tbl) ~= nil then return false else return true end
end


local function fileExists(file)
  local f = io.open(file, "rb")
  if f then f:close() end
  return f ~= nil
end

local function isDir(dir)
  if type(dir)~="string" then return false end
  if lfs.attributes(dir, 'mode') == "directory" then return true end    
  return false
end

local function isFile(file)
  if type(file)~="string" then return false end
  if lfs.attributes(file, 'mode') == "file" then return true end    
  return false
end

local function isDirEmpty(dir)
  if lfs.dir(dir) == nil then return true end
  return false
end


local function trim(str)
  return str:gsub("%s+", "")
end


local function lowerCase(str)
  return str:lower()
end


local function firstToUpper(str)
  return (str:gsub("^%l", string.upper))
end


local function split(str, pat)
   local t = {}
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = str:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
        table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = str:find(fpat, last_end)
   end
   if last_end <= #str then
      cap = str:sub(last_end)
      table.insert(t, cap)
   end
   return t
end

local function splitAndGetLast(str, pat)
  local splitted = split(str, pat)
  return splitted[#splitted]
end

local function splitAndGetFirst(str, pat)
  local splitted = split(str, pat)
  return splitted[1]
end

local function deepCopy(t)
    local lookup_table = {}
    local function _copy(t)
        if type(t) ~= "table" then
          return t
        elseif lookup_table[t] then
            return lookup_table[t]
        end
        local new_table = {}
        lookup_table[t] = new_table
        for index, value in pairs(t) do
            new_table[_copy(index)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(t))
    end
    return _copy(t)
end

local function shallowCopy(t)
    if type(t) ~= "table" then
      return t 
    end
    local meta = getmetatable(t)
    local target = {}
    for k, v in pairs(t) do 
      target[k] = v 
    end
    setmetatable(target, meta)
    return target
end


local function tblPrint(s, l, i) -- recursive Print (structure, limit, indent)
	l = (l) or 100
	i = i or "" -- default item limit, indent string
	if (l < 1) then
		print "ERROR: Item limit reached."
		return l-1
	end
	local ts = type(s)
	if (ts ~= "table") then
		print (i,ts,s)
		return l - 1
	end
	print (i,ts) -- print "table"
	for k,v in pairs(s) do  -- print "[KEY] VALUE"
		l = tblPrint(v, l, i.."\t["..tostring(k).."]")
		if (l < 0) then break end
	end
	return l
end


local function printStructure(tbl)
	if (tbl == nil) then return end
	for k,v in pairs(tbl) do
		if (v == nil) then
			print(k.." --> nil")
		elseif (type(v) == "table") then
			print(k.." --> table")
		else
			print(k.." --> "..v)
		end
	end
end


local function printKey(tbl, key)
	if (tbl[key] ~= nil) then
		print(tbl[key])
	else
		print(key, " doesn't exist.")
	end
end


local function printKeys(tbl)
	if (tbl == nil) then return end
	for k,v in pairs(tbl) do
		print(k)
	end
end


local function printDivider(msg)
	print('--------------------------------------')
	print('-- '..msg)
	print('--------------------------------------')
end


local function joinArrays(tbl1, tbl2)
	if (type(tbl1) == "table") and (type(tbl2) == "table") then
		for i,value in pairs(tbl2) do table.insert(tbl1, value) end
		return tbl1
	else
		return {}
	end
end


local function serializeTable(val, name, skipnewlines, depth)
    skipnewlines = skipnewlines or true
    depth = depth or 0

    -- indent text by depth value
    local tmp = string.rep(" ", depth)

    if name then tmp = tmp .. name .. " = " end

    if type(val) == "table" then
        tmp = tmp .. "{" .. (not skipnewlines and "\n" or "")

        for k, v in pairs(val) do
            tmp =  tmp .. serializeTable(v, k, skipnewlines, depth + 1) .. "," .. (not skipnewlines and "\n" or "")
        end

        tmp = tmp .. string.rep(" ", depth) .. "}"
    elseif type(val) == "number" then
        tmp = tmp .. tostring(val)
    elseif type(val) == "string" then
        tmp = tmp .. string.format("%q", val)
    elseif type(val) == "boolean" then
        tmp = tmp .. (val and "true" or "false")
    else
        tmp = tmp .. "\"[inserializeable datatype:" .. type(val) .. "]\""
    end

    return tmp
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  logger = logger,
  logging = logging,
  split  = split,
  splitAndGetFirst = splitAndGetFirst,
  splitAndGetLast = splitAndGetLast,
  trim = trim,
  lowerCase = lowerCase,
  firstToUpper = firstToUpper,
  isTable = isTable,
  isArray = isArray,
  isEmpty = isEmpty,
  isHidden = isHidden,
  isLuaFile = isLuaFile,
  fileExists = fileExists,
  isDir = isDir,
  isFile = isFile,
  isDirEmpty = isDirEmpty,
  deepCopy = deepCopy,
  shallowCopy = shallowCopy,
  tblPrint = tblPrint,
  printStructure = printStructure,
  printKey = printKey,
  printKeys = printKeys,
  printDivider = printDivider,
  joinArrays = joinArrays,
  serializeTable = serializeTable
}