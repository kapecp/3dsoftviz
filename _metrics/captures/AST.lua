-------------------------------------------------------------------------------
-- Metrics captures - generating AST tree
-- @release 2010/05/04 02:45:00, Gabriel Duchon
-- @release 2011/05/04, Ivan Simko
-------------------------------------------------------------------------------

local utils = require 'metrics.utils'

local pairs, type, print, table = pairs, type, print, table

module('metrics.captures.AST')

-- set correct order and parent values for each child
local function processChildren(data)
	for k,v in pairs(data.data) do
		v.parent = data
		v.order = k
	end
end

local node_references = {}

local id_generator = (function()
	local id = 0;
	return function(node)
		id = id + 1
		node_references[id] = node
		return id
	end
end)()

----------------------------
-- Table containing key values for each node type
-- @class table
-- @name keys
local keys = {
	  IGNORED = 'IGNORED'
	, EPSILON = 'EPSILON'
	, EOF = 'EOF'
	, BOF = 'BOF'
	, NUMBER = 'NUMBER'
	, ID = 'ID'
	, STRING = 'STRING'
	, Name = 'Name'

-- CHUNKS
	, [1] = 'STARTPOINT'
	, CHUNK = 'CHUNK'
	, Chunk = 'Chunk'
	, Block = 'Block'

-- STATEMENTS
	, Stat = 'Stat'
	, Assign = 'Assign'
	, Do = 'Do'
	, While = 'While'
	, Repeat = 'Repeat'
	, If = 'If'
	, NumericFor = 'NumericFor'
	, GenericFor = 'GenericFor'
	, GlobalFunction = 'GlobalFunction'
	, LocalFunction = 'LocalFunction'
	, LocalAssign = 'LocalAssign'
	, LastStat = 'LastStat'

-- LISTS
	, VarList = 'VarList'
	, NameList = 'NameList'
	, ExpList = 'ExpList'

-- EXPRESSIONS
	, Exp = 'Exp'
	, _SimpleExp = '_SimpleExp'
	, _PrefixExp = '_PrefixExp'
	, _PrefixExpParens = '_PrefixExpParens'
	, _PrefixExpSquare = '_PrefixExpSquare'
	, _PrefixExpDot = '_PrefixExpDot'
	, _PrefixExpArgs = '_PrefixExpArgs'
	, _PrefixExpColon = '_PrefixExpColon'

	-- solving the left recursion problem
	, Var = 'Var'
	, FunctionCall = 'FunctionCall'

-- FUNCTIONS
	, Function = 'Function'
	, FuncBody = 'FuncBody'
	, FuncName = 'FuncName'
	, Args = 'Args'
	, ParList = 'ParList'

-- TABLES
	, TableConstructor = 'TableConstructor'
	, FieldList = 'FieldList'
	, Field = 'Field'
	, _FieldSquare = '_FieldSquare'
	, _FieldID = '_FieldID'
	, _FieldExp = '_FieldExp'
	, FieldSep = 'FieldSep'

-- OPERATORS
	, BinOp = "BinOp"
	, UnOp = "UnOp"

-- KEYWORDS
	, FALSE = "keyword"
	, TRUE = "keyword"
	, NIL = "keyword"

	, AND = "keyword"
	, NOT = "keyword"
	, OR = "keyword"

	, DO = "keyword"
	, IF = "keyword"
	, THEN = "keyword"
	, ELSE = "keyword"
	, ELSEIF = "keyword"
	, END = "keyword"

	, FOR = "keyword"
	, IN = "keyword"
	, UNTIL = "keyword"
	, WHILE = "keyword"
	, REPEAT = "keyword"
	, BREAK = "keyword"

	, LOCAL = "keyword"
	, FUNCTION = "keyword"
	, RETURN = "keyword"
 
 -- WHITESPACE
	, COMMENT = 'COMMENT'
	, SPACE = 'SPACE'
	, NEWLINE = 'NEWLINE'

-- SYMBOLS
	, ['+'] = "symbol"
	, ['-'] = "symbol"
	, ['*'] = "symbol"
	, ['/'] = "symbol"
	, ['%'] = "symbol"
	, ['^'] = "symbol"
	, ['#'] = "symbol"
	, ['=='] = "symbol"
	, ['~='] = "symbol"
	, ['<='] = "symbol"
	, ['>='] = "symbol"
	, ['<'] = "symbol"
	, ['>'] = "symbol"
	, ['='] = "symbol"
	, ['('] = "symbol"
	, [')'] = "symbol"
	, ['{'] = "symbol"
	, ['}'] = "symbol"
	, ['['] = "symbol"
	, [']'] = "symbol"
	, [';'] = "symbol"
	, [':'] = "symbol"
	, [','] = "symbol"
	, ['.'] = "symbol"
	, ['..'] = "symbol"
	, ['...'] = "symbol"

}

--------------------------------------------
-- Captures table for lpeg parsing - generates AST tree
-- @class table
-- @name captures
captures = (function()
	local key,value
	local new_table = {}
	for key,value in pairs(keys) do
		new_table[key] = function (pos, s, ...) 
			local data = {tag=value, key=key, data={...}, position=pos, text=s, str=s} 
			data.nodeid = id_generator(data)
			processChildren(data)
			if (key == 1) then
				data.nodeid_references = node_references
			end
			return data 
		end
	end

	-- set various node properties for easier manipulation later
	new_table.GlobalFunction = function (pos, s, ...) local 
		data = {tag='GlobalFunction', key='GlobalFunction', data={...}, name=utils.searchForTagItem('FuncName',{...}).text, position=pos, text=s, str=s}
		data.nodeid = id_generator(data)
		processChildren(data) 
		return data 
	end
	
	new_table.LocalFunction = function (pos, s, ...) 
		local data = {tag='LocalFunction', key='LocalFunction', data={...}, name=utils.searchForTagItem('Name',{...}).text, position=pos, text=s, str=s} 
		data.nodeid = id_generator(data)
		processChildren(data) 
		return data 
	end
	
	new_table.Assign =  function (pos, s, ...) 
		local data = {tag='Assign', key='Assign', data={...}, position=pos, text=s, str=s, nameList=utils.searchForTagItem('VarList',{...}), expList=utils.searchForTagItem('ExpList',{...})} 
		data.nodeid = id_generator(data)
		processChildren(data) 
		return data 
	end
	
	new_table.LocalAssign = function (pos, s, ...) 
		local data = {tag='LocalAssign', key='LocalAssign', data={...}, position=pos, text=s, str=s, nameList=utils.searchForTagItem('NameList',{...}), expList=utils.searchForTagItem('ExpList',{...})} 
		data.nodeid = id_generator(data)
		processChildren(data) 
		return data 
	end
	
	new_table.LastStat = function (pos, s, ...) 
		local data = {tag='LastStat', key='LastStat', data={...}, position=pos, text=s, str=s, expList=utils.searchForTagItem('ExpList',{...})}
		data.nodeid = id_generator(data) 
		processChildren(data) 
		return data 
	end
	

	return new_table
end)()