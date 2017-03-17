-------------------------------------------------------------------------------
-- Metrics - utilities for common actions when working with created AST tree
-- @release 2011/05/04 , Ivan Simko
-------------------------------------------------------------------------------

local pairs, table, type = pairs, table, type

module('metrics.utils')

-----------------------------------------
-- Returns node with specified tag from an array of nodes
-- @param tagname the name of tag to be searched for
-- @param data an array of nodes to be searched
-- @return node or nil
function searchForTagItem(tagname, data)
	for j,l in pairs(data) do
		if (l.tag == tagname) then
			return l	
		end
	end
	return nil
end

-----------------------------------------
-- Returns table of nodes with specified tag from an array of nodes
-- @param tagname the name of tag to be searched for
-- @param data an array of nodes to be searched
-- @return table containing nodes
function searchForTagArray(tagname, data)
	local nodes = {}
	for j,l in pairs(data) do
		if (l.tag == tagname) then
			table.insert(nodes, l)	
		end
	end
	return nodes
end

-----------------------------------------
-- Returns table of nodes with specified key from an array of nodes
-- @param key the name of key to be searched for
-- @param data an array of nodes to be searched
-- @return table containing nodes
function searchForTagArray_key(key, data)
	local nodes = {}
	for j,l in pairs(data) do
		if (l.key == key) then
			table.insert(nodes, l)	
		end
	end
	return nodes
end

------------------------------------------
-- Recursive search for first node with specified tagname in children of specified node
-- @param tagname the name of tag to be searched for
-- @node node which children are searched
-- @maxdepth number specifying the maximum depth the algorithm searches in, nil is equal to unlimited depth
-- @return node or nil
function searchForTagItem_recursive(tagname, node, maxdepth)
	if (node == nil) then return nil end
	local j,l
	if (maxdepth ~= nil) then maxdepth = maxdepth -1 end
	for j,l in pairs(node.data) do
		if (l.tag == tagname) then
			return l
		elseif (l.data ~= nil) then
			if (maxdepth == nil or maxdepth > 0) then
				local tmp = searchForTagItem_recursive(tagname, l, maxdepth)
				if (tmp ~= nil) then return tmp end
			end
		end
	end
	return nil
end

-- Helper function for recursive search
local function searchForTagArray_recursive_helper(tagname, node, maxdepth, tbl) 	
	if (maxdepth ~= nil) then 
    maxdepth = maxdepth -1 
  end
  
  if (node.data == nil) then
    return tbl
  end
    
  for j,l in pairs(node.data) do
      if (l.tag == tagname) then
        table.insert(tbl or {}, l)
      elseif (l.data ~= nil) then
        if (maxdepth == nil or maxdepth > 0) then
          tbl = searchForTagArray_recursive_helper(tagname, l, maxdepth, tbl)      
        end
      end    
  end
  return tbl
end


-- Recursive search for every node with specified tagname in children of specified node
-- @param tagname the name of tag to be searched for
-- @node node which children are searched
-- @maxdepth number specifying the maximum depth the algorithm searches in, nil is equal to unlimited depth
-- @return array of nodes or nil
function searchForTagArray_recursive(tagname, node, maxdepth)
	if (node == nil) then 
    return nil 
  end
   
  local tbl = searchForTagArray_recursive_helper(tagname, node, maxdepth, {})
    
  return tbl;
end

-----------------------------
-- Search for nodes with tag 'Name' or 'Var' in nameList
-- returns a table with nodes
-- @param nameList node to be searched
-- @return table containing nodes
function getNamesFromNameList(nameList)
	local names = {}
	for i,j in pairs(nameList.data) do
		if (j.tag == 'Name' or j.tag == 'Var') then
			table.insert(names, j)
		end
	end
	return names
end

-----------------------------
-- Search for nodes with tag 'Exp' expList
-- returns a table with nodes
-- @param expList node to be searched
-- @return table containing nodes
function getExpsFromExpsList(expList)
	local names = {}
	for i,j in pairs(expList.data) do
		if (j.tag == 'Exp') then
			table.insert(names, j)
		end
	end
	return names
end

------------------------------
-- Find the 'Block' node that belongs to the corresponding function
-- @param func the function to be looked in
-- @return node with tag='Block'
function getBlockFromFunction(func)
	local block = nil
	for k, v in pairs(func.data) do
		if (v.tag == 'FuncBody') then
			for i,j in pairs(v.data) do
				if (j.tag == 'Block') then
					block = j	
					break
				end
			end
			break
		end
	end		
	return block
end

-------------------------------
-- Match comment before given node, works using .parent property of AST node
-- @param node search for comment for this node
-- @return comment as string or empty string
function getComment(node)
	
	local comment = ''
	local fullIGNORED = ''
	local ignore
	local order = node.order
	local currNode = node
	local comment_table = {}
	
	while (true) do
		while (order == 1 or order == 0) do
			currNode = currNode.parent
			if (currNode == nil) then return nil end
			order = currNode.order			
		end	
		
		if (currNode.tag == 'STARTPOINT') then return nil end
	
		order = order - 1

		while (order ~= 0) do
			ignore = currNode.parent.data[order]
			if (ignore.tag == 'IGNORED') then
				for k,v in pairs(ignore.data) do
					if (type(v) == 'table') then
						if (v.tag == 'COMMENT') then
							comment = comment .. v.text .. '\n'
						end
						fullIGNORED = fullIGNORED .. v.text
					end
				end
				order = order - 1
			else
				break
			end
		end
		
		if (order ~= 0) then
			break
		end	
			
	end
	
	if (comment == '') then comment = nil end
	return comment, fullIGNORED
end

-----------------------------
-- compares two functions by their function.name property
-- @param function1 first function
-- @param function2 second function
function compare_functions_by_name(function1, function2)
	return function1.name<function2.name	
end
