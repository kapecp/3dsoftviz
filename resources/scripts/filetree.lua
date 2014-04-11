local lfs = require"lfs"

local filetree = {}

local function traverse(file, path)
	local result = {name = file}
	local f = path..'/'..file
	result.fullpath = f
	local attr = lfs.attributes(f)
	if attr == nil then
		return nil
	end
  result.attributes = attr
	if attr.mode == "file" then
		return result
	elseif attr.mode == "directory" then
		local contents = {}
		for child in lfs.dir(f) do
			if child ~= "." and child ~= ".." then
				local res = traverse(child, f)
				table.insert(contents, res)
			end
		end
		result.contents = contents
		return result
	end
end

local function isabsolute(path)
	if path:find("\\") == 1 then
		return true
	elseif path:find("[A-Za-z]:\\") == 1 then
		return true
	elseif path:find("/") == 1 then
		return true
	end
	return false
end

local function split(s)
	local result = {}
	for i in string.gmatch(s, "[^\\/]+") do
		table.insert(result, i)
	end
	return result
end

local function getabsolute(path)
	if isabsolute(path) then
    return split(path)
	end
	local current = lfs.currentdir()
	local abspath = current .. "/" .. path
	local result = {}
	for _,i in ipairs(split(abspath)) do
		if i == ".." then
			table.remove(result)
		elseif i ~= "." then
			table.insert(result, i)
		end
	end
	return result
end

--- Creates file tree for given folder.
-- Create file tree starting at given path. Format of returned tree is
-- following:
--     name : name of file/directory
--     fullpath : absolute path to file/directory
--     contents : (only for directories) array of directory subtrees
-- @param file Path to the file to start from.
-- @return Table containing file tree.
-- @usage filetree.load(".")
function filetree.load(path)
	local abspath = getabsolute(path)
	local file = table.remove(abspath)
	local dirpath = table.concat(abspath, "/")
  dirpath = "/" .. dirpath
	return traverse(file, dirpath)
end

return filetree
