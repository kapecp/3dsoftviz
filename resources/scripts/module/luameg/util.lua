--------
-- @name trim
-- @author Matus Stefanik
-- @param str - [string] string
-- @return [string]
local function trim(str)
	if type(str) == "string" then
		return str:gsub("^%s*(.-)%s*$", "%1")
	end

	return str
end

---------
-- Replace some characters as ", _, \n, \r\n, [, ], <, >
-- @name replaceSpecialChar
-- @author Matus Stefanik
-- @param str - [string] string
-- @return [string]
local function replaceSpecialChar(str)
	if type(str) == "string" then
		return str:gsub("\"", "'"):gsub("_", " "):gsub("\n", "\\n"):gsub("\r\n", "\\n"):gsub('%[', "("):gsub('%]', ")"):gsub('>', 'gt'):gsub('<', 'lt')
	end

	return str
end


---------------------------
-- Helper function to visualise ast tree in external web service. This function return text in special format.
-- @name getAST_treeSyntax
-- @author Matus Stefanik
-- @param ast - [table] ast from luameg (moonscript)
-- @param showText - [numeric] optional number parameter.
-- 			nil or 1 - do not show text from ast in tree.
--			2 - show text only in leaf node;
--			3 - show text in all nodes;
--			4 - show text in all nodes below Line node. Text is missing in first four nodes.
-- 			all texts are modified (replaced characters as [, ], >, ", etc.) and trimed
-- @return [table] Tree in special text format
-- Return something like:
--    "[1 [File [Block [Line [CheckIndent ] [Statement [ExpList [Exp [Value [ChainValue [Callable [Name ] ] ] ] ] ] [Assign [ExpListLow [Exp [Value [SimpleValue ] ] ] ] ] ] ] [Line ] ] ] ]"
--
-- String put to:
--  http://ironcreek.net/phpsyntaxtree/					-- fast, export to PNG, SVG
--  http://www.yohasebe.com/rsyntaxtree/				-- slow, export to PNG, SVG, PDF
--  http://mshang.ca/syntree/							-- problem with big tree
local function getAST_treeSyntax(ast, showText)
	local showText = showText or 1

	local newout = ""

	if (ast == nil) then
		return ""
	end

	newout = "[" .. ast["key"]

	-- show all text
	if (showText == 3) then
		newout = newout .. ' [ "' .. replaceSpecialChar(trim(ast["text"])) .. '"] '
	end

	-- show all text better
	if (showText == 4) then
		if ast["key"] ~= "Line" and ast["key"] ~= "Block" and ast["key"] ~= "File" 
			and ast["key"] ~= 1 and ast["key"] ~= "CHUNK" then
			newout = newout .. ' [ "' .. replaceSpecialChar(trim(ast["text"])) .. '"] '
		end
	end

	-- show text only from leaf
	if (showText == 2 and #ast["data"] == 0) then
		newout = newout .. ' [ " ' .. replaceSpecialChar(trim(ast["text"])).. '"] '
	end

	for i=1,#ast["data"] do
		-- show all text
		newout = newout .. " " .. getAST_treeSyntax(ast["data"][i], showText)
	end
	newout = newout .. " ]"
	
	return newout
end


return {
	getAST_treeSyntax = getAST_treeSyntax
}
