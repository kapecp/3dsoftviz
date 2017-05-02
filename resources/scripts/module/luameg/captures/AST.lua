
local pairs, type, print, table = pairs, type, print, table


-- set correct order and parent values for each child
local function processChildren(data)
	for k,v in pairs(data.data) do
		v.parent = data   -- rodic
		v.order = k       -- kolkaty v poradi
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

-- nazovPravidla = typUzla
local keys = {
	  [1] = 'STARTPOINT',
    File = 'File',
    Block = 'Block',
     CheckIndent = 'CheckIndent', 
    Line = 'Line', 
    Statement = 'Statement', 
    Body = 'Body', 
     Advance = 'Advance',
     PushIndent = 'PushIndent', 
     PreventIndent = 'PreventIndent', 
     PopIndent = 'PopIndent', 
    InBlock = 'InBlock', 
    Local = 'Local', 
    Import = 'Import', 
    ImportName = 'ImportName', 
    ImportNameList = 'ImportNameList', 
    BreakLoop = 'BreakLoop',
    Return = 'Return', 
    WithExp = 'WithExp',
    With = 'With',
    Switch = 'Switch', 
    SwitchBlock = 'SwitchBlock', 
    SwitchCase = 'SwitchCase', 
    SwitchElse = 'SwitchElse', 
    IfCond = 'IfCond', 
    IfElse = 'IfElse', 
    IfElseIf = 'IfElseIf', 
    If = 'If', 
    Unless = 'Unless',
    While = 'While', 
    For = 'For', 
    ForEach = 'ForEach', 
    Do = 'Do', 
    Comprehension = 'Comprehension',
    TblComprehension = 'TblComprehension', 
    CompInner = 'CompInner', 
    CompForEach = 'CompForEach', 
    CompFor = 'CompFor', 
    CompClause = 'CompClause', 
    Assign = 'Assign', 
    Update = 'Update', 
    CharOperators = 'CharOperators',
    WordOperators = 'WordOperators', 
    BinaryOperator = 'BinaryOperator', 
    Assignable = 'Assignable', 
    Exp = 'Exp', 
    SimpleValue = 'SimpleValue',
    ChainValue = 'ChainValue', 
    Value = 'Value', 
    SliceValue = 'SliceValue', 
    String = 'String', 
    SingleString = 'SingleString', 
    DoubleString = 'DoubleString', 
    LuaString = 'LuaString', 
    LuaStringOpen = 'LuaStringOpen', 
    LuaStringClose = 'LuaStringClose', 
    Callable = 'Callable', 
    Parens = 'Parens',
    FnArgs = 'FnArgs', 
    FnArgsExpList = 'FnArgsExpList', 
    Chain = 'Chain', 
    ChainItems = 'ChainItems',
    ChainItem = 'ChainItem', 
    DotChainItem = 'DotChainItem', 
    ColonChainItem = 'ColonChainItem', 
    ColonChain = 'ColonChain', 
    Slice = 'Slice', 
    Invoke = 'Invoke', 
    TableValue = 'TableValue', 
    TableLit = 'TableLit', 
    TableValueList = 'TableValueList', 
    TableLitLine = 'TableLitLine', 
    TableBlockInner = 'TableBlockInner', 
    TableBlock = 'TableBlock', 
    ClassDecl = 'ClassDecl', 
    ClassBlock = 'ClassBlock', 
    ClassLine = 'ClassLine', 
    Export = 'Export', 
    KeyValue = 'KeyValue', 
    KeyValueList = 'KeyValueList',
    KeyValueLine = 'KeyValueLine',
    FnArgsDef = 'FnArgsDef', 
    FnArgDefList = 'FnArgDefList', 
    FnArgDef = 'FnArgDef', 
    FunLit = 'FunLit',
    NameList = 'NameList',
    NameOrDestructure = 'NameOrDestructure',
    AssignableNameList = 'AssignableNameList',
    ExpList = 'ExpList',
    ExpListLow = 'ExpListLow',
    InvokeArgs = 'InvokeArgs',
    ArgBlock = 'ArgBlock',
    ArgLine = 'ArgLine',

    Name = 'Name',
    SelfName = 'SelfName',
    KeyName = 'KeyName',
    VarArg = 'VarArg',

    --KEYWORDS
    LOCAL = 'keyword',
    IMPORT = 'keyword',
    BREAK = 'keyword',
    FROM = 'keyword',
    IF = 'keyword',
    ELSE = 'keyword',
    ELSEIF = 'keyword',
    UNLESS = 'keyword',
    RETURN = 'keyword',
    WITH = 'keyword',
    SWITCH = 'keyword',
    DO = 'keyword',
    WHEN = 'keyword',
    THEN = 'keyword',
    CONTINUE = 'keyword',
    WHILE = 'keyword',
    FOR = 'keyword',
    IN = 'keyword',
    NOT = 'keyword',
    CLASS = 'keyword',
    EXTENDS = 'keyword',
    EXPORT = 'keyword',
    USING = 'keyword',

    --WHITESPACE                 nefunguju
    Comment = 'Comment',
    Space = 'Space',

    --SYMBOLS
    ['\\'] = 'symbol',
    [','] = 'symbol',
    ['='] = 'symbol',
    ['*'] = 'symbol',
    ['['] = 'symbol',
    [']'] = 'symbol',
    ['{'] = 'symbol',
    ['}'] = 'symbol',
    ['('] = 'symbol',
    [')'] = 'symbol',
    ['*'] = 'symbol',
    ['..='] = 'symbol',
    ['+='] = 'symbol',
    ['-='] = 'symbol',
    ['*='] = 'symbol',
    ['/='] = 'symbol',
    ['%='] = 'symbol',
    ['or='] = 'symbol',
    ['and='] = 'symbol',
    ['&='] = 'symbol',
    ['|='] = 'symbol',
    ['>>='] = 'symbol',
    ['<<='] = 'symbol',
    ['-'] = 'symbol',
    ['+'] = 'symbol',
    ['#'] = 'symbol',
    ['~'] = 'symbol',
    ['!'] = 'symbol',
    [':'] = 'symbol',
    ['->'] = 'symbol',
    ['=>'] = 'symbol',
    [';'] = 'symbol'

    --OPERATORS


}

local captures = (function()
  local keyNameRule, valueTypeNode
  local new_table = {}

  for keyNameRule,valueTypeNode in pairs(keys) do
    
    new_table[keyNameRule] = function (pos, s, ...) 
      local nodeInAST = {tag=valueTypeNode, key=keyNameRule, data={...}, position=pos, text=s}    -- data - potomkovia uzla
      nodeInAST.nodeid = id_generator(nodeInAST)
      processChildren(nodeInAST)
      if (keyNameRule == 1) then
        nodeInAST.nodeid_references = node_references
      end
      return nodeInAST 
    end

  end

  return new_table
end)()


return {
  captures = captures
}