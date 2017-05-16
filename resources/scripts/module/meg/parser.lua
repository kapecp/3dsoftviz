-- $Id: parser.lua for Moonscript 2016

-- basic modules
local _G     = _G
local table  = table
local string = string

-- local print, assert, type = print, assert, type

-- basic functions
local error   = error
local require = require

-- imported modules
local m       = require 'lpeg'

local grammar = require 'leg.grammar'
local Stack = require 'meg.data'.Stack
local util = require 'meg.util'


m.setmaxstack(10000)

-- vrati pocet prvok vo v (asi)
local L = m.luversion and m.L or function(v)
  return #v
end

local keywords2 = {}

local White = m.S(" \t\r\n") ^ 0
local plain_space = m.S(" \t") ^ 0
local Break = m.P("\r") ^ -1 * m.P("\n")
local Stop = Break + -1
local Comment = m.P("--") * (1 - m.S("\r\n")) ^ 0 * L(Stop)
local Space = plain_space * Comment ^ -1
local SomeSpace = m.S(" \t") ^ 1 * Comment ^ -1
local SpaceBreak = Space * Break
local EmptyLine = SpaceBreak
local AlphaNum = m.R("az", "AZ", "09", "__")
local _Name = m.R("az", "AZ", "__") * AlphaNum ^ 0
local Num = m.P("0x") * m.R("09", "af", "AF") ^ 1 * (m.S("uU") ^ -1 * m.S("lL") ^ 2) ^ -1 + m.R("09") ^ 1 * (m.S("uU") ^ -1 * m.S("lL") ^ 2) + (m.R("09") ^ 1 * (m.P(".") * m.R("09") ^ 1) ^ -1 + m.P(".") * m.R("09") ^ 1) * (m.S("eE") * m.P("-") ^ -1 * m.R("09") ^ 1) ^ -1
local Shebang = m.P("#!") * m.P(1 - Stop) ^ 0
local SpaceName = Space * _Name
local Num = Space * (Num)

local _indent = Stack(0)
local _do_stack = Stack(0)
local state = {
  last_pos = 0
}

local check_indent
check_indent = function(str, pos, indent)
  state.last_pos = pos
  return _indent:top() == indent
end

local advance_indent
advance_indent = function(str, pos, indent)
  local top = _indent:top()
  if top ~= -1 and indent > top then
    _indent:push(indent)
    return true   -- match succeeds without cosuming any input
  end
  -- return no value  -- match fail
end

local push_indent
push_indent = function(str, pos, indent)
    _indent:push(indent)
    return true   -- match succeeds without cosuming any input
end

local pop_indent
pop_indent = function()
  assert(_indent:pop(), "unexpected outdent")
  return true   -- match succeeds without cosuming any input
end

local check_do
check_do = function(str, pos)--, do_node)    -- parametre funkcie: entire subject, current position (after the match ofpatt), plus any capture values produced by patt
  local top = _do_stack:top()
  if top == nil or top then
    return true --, do_node        -- match succeeds without cosuming any input 
  end
  return false    -- match failed
end

local disable_do
disable_do = function()
  _do_stack:push(false)
  return true   -- match succeeds without cosuming any input
end

local pop_do
pop_do = function()
  assert(_do_stack:pop() ~= nil, "unexpected do pop")
  return true   -- match succeeds without cosuming any input
end

local op
op = function(chars)
  local patt = Space * chars
  if chars:match("^%w*$") then
    keywords2[chars] = true   -- poznaci si do keyword aj operatory
    patt = patt * -AlphaNum
  end
  return patt
end

local trim
trim = function(str)
  return str:match("^%s*(.-)%s*$")
end

local Name2 = m.Cmt(SpaceName, function(str, pos, name)
  if keywords2[name] then
    return false  -- match failed; 
  end
  return true   -- match succeeds without cosuming any input
end)

local key
key = function(chars)
  keywords2[chars] = true     -- poznaci si keyword
  return Space * chars * -AlphaNum
end

local sym
sym = function(chars)
  return Space * chars
end

local symx
symx = function(chars)
  return chars
end



local DisableDo = m.Cmt("", disable_do)
local PopDo = m.Cmt("", pop_do)
--local SelfName = Space * "@" * ("@" * (_Name) + _Name)
--local SelfName = Space * "@" * ("@" * (_Name + m.Cc("self.__class")) + _Name + m.Cc("self"))
local SelfName2 = Space * "@" * ("@" * (_Name)^-1 + _Name) ^-1
--local SelfName   = (Space * "@" * ("@" * (_Name ^-1) + _Name) ^-1 )
--local SelfName = Space * "@" * ("@" * (_Name / util.mark("self_class") + m.Cc("self.__class")) + _Name / util.mark("self") + m.Cc("self"))
local KeyName2 = SelfName2 + Space * _Name
local VarArg2 = Space * m.P("...")

local rules = {
    [1] = m.V'File',
    File = Shebang ^ -1 * (m.V'Block' ^-1),
    Block = (m.V'Line' * (Break ^ 1 * m.V'Line') ^ 0),
    CheckIndent = m.Cmt(util.Indent, check_indent),
    Line = (m.V'CheckIndent' * m.V'Statement' + Space * L(Stop)),
    Statement = (m.V'Import' + m.V'While' + m.V'With' + m.V'For' + m.V'ForEach' + m.V'Switch' + m.V'Return' + m.V'Local' + m.V'Export' + m.V'BreakLoop' + m.V'ExpList' * (m.V'Update' + m.V'Assign') ^ -1 ) * Space * ((m.V'IF' * m.V'Exp' * (m.V'ELSE' * m.V'Exp') ^ -1 * Space + m.V'UNLESS' * m.V'Exp' + m.V'CompInner') * Space) ^ -1,
    Body = Space ^ -1 * Break * EmptyLine ^ 0 * m.V'InBlock' + m.V'Statement',
    Advance = L(m.Cmt(util.Indent, advance_indent)),
    PushIndent = m.Cmt(util.Indent, push_indent),
    PreventIndent = m.Cmt(m.Cc(-1), push_indent),
    PopIndent = m.Cmt("", pop_indent),
    InBlock = m.V'Advance' * m.V'Block' * m.V'PopIndent',
    Local = m.V'LOCAL' * ((op("*") + op("^")) + m.V'NameList'),
    Import = m.V'IMPORT' * m.V'ImportNameList' * SpaceBreak ^ 0 * m.V'FROM' * m.V'Exp',
    ImportName = (util.sym("\\") * (m.V'Name') + m.V'Name'),
    ImportNameList = SpaceBreak ^ 0 * m.V'ImportName' * ((SpaceBreak ^ 1 + util.sym(",") * SpaceBreak ^ 0) * m.V'ImportName') ^ 0,
    BreakLoop = m.V'BREAK' + m.V'CONTINUE',
    Return = m.V'RETURN' * (m.V'ExpListLow' ^-1),
    WithExp = m.V'ExpList' * m.V'Assign' ^ -1 ,
    With = m.V'WITH' * DisableDo * util.ensure(m.V'WithExp', PopDo) * m.V'DO' ^ -1 * m.V'Body',
    Switch = m.V'SWITCH' * DisableDo * util.ensure(m.V'Exp', PopDo) * m.V'DO' ^ -1 * Space ^ -1 * Break * m.V'SwitchBlock',
    SwitchBlock = EmptyLine ^ 0 * m.V'Advance' * (m.V'SwitchCase' * (Break ^ 1 * m.V'SwitchCase') ^ 0 * (Break ^ 1 * m.V'SwitchElse') ^ -1) * m.V'PopIndent',
    SwitchCase = m.V'WHEN' * m.V'ExpList' * m.V'THEN' ^ -1 * m.V'Body',
    SwitchElse = m.V'ELSE' * m.V'Body',
    IfCond = m.V'Exp' * m.V'Assign' ^ -1,
    IfElse = (Break * EmptyLine ^ 0 * m.V'CheckIndent') ^ -1 * m.V'ELSE' * m.V'Body',
    IfElseIf = (Break * EmptyLine ^ 0 * m.V'CheckIndent') ^ -1 * m.V'ELSEIF' * m.V'IfCond' * m.V'THEN' ^ -1 * m.V'Body',
    If = m.V'IF' * m.V'IfCond' * m.V'THEN' ^ -1 * m.V'Body' * m.V'IfElseIf' ^ 0 * m.V'IfElse' ^ -1,
    Unless = m.V'UNLESS' * m.V'IfCond' * m.V'THEN' ^ -1 * m.V'Body' * m.V'IfElseIf' ^ 0 * m.V'IfElse' ^ -1,
    While = m.V'WHILE' * DisableDo * util.ensure(m.V'Exp', PopDo) * m.V'DO' ^ -1 * m.V'Body',
    For = m.V'FOR' * DisableDo * util.ensure(m.V'Name' * util.sym("=") * (m.V'Exp' * util.sym(",") * m.V'Exp' * (util.sym(",") * m.V'Exp') ^ -1), PopDo) * m.V'DO' ^ -1 * m.V'Body',
    ForEach = m.V'FOR' * (m.V'AssignableNameList') * m.V'IN' * DisableDo * util.ensure((util.sym("*") * m.V'Exp' + m.V'ExpList'), PopDo) * m.V'DO' ^ -1 * m.V'Body',
    Do = m.V'DO' * m.V'Body',
    Comprehension = util.sym("[") * m.V'Exp' * m.V'CompInner' * util.sym("]"),
    TblComprehension = util.sym("{") * (m.V'Exp' * (util.sym(",") * m.V'Exp') ^ -1) * m.V'CompInner' * util.sym("}"),
    CompInner = ((m.V'CompForEach' + m.V'CompFor') * m.V'CompClause' ^ 0),
    CompForEach = m.V'FOR' * (m.V'AssignableNameList') * m.V'IN' * (util.sym("*") * m.V'Exp' + m.V'Exp') * plain_space,
    CompFor = key("for" * m.V'Name' * util.sym("=") * (m.V'Exp' * util.sym(",") * m.V'Exp' * (util.sym(",") * m.V'Exp') ^ -1)),
    CompClause = m.V'CompFor' + m.V'CompForEach' + m.V'WHEN' * m.V'Exp',
    Assign = util.sym("=") * ((m.V'With' + m.V'If' + m.V'Switch') + (m.V'TableBlock' + m.V'ExpListLow')),
    Update = plain_space * ((util.sym("..=") + util.sym("+=") + util.sym("-=") + util.sym("*=") + util.sym("/=") + util.sym("%=") + util.sym("or=") + util.sym("and=") + util.sym("&=") + util.sym("|=") + util.sym(">>=") + util.sym("<<="))) * plain_space * m.V'Exp',
    CharOperators = Space * m.S("+-*/%^><|&"),
    WordOperators = plain_space * (op("or") + op("and") + op("<=") + op(">=") + op("~=") + op("!=") + op("==") + op("..") + op("<<") + op(">>") + op("//")) * plain_space,
    BinaryOperator = (m.V'WordOperators' + m.V'CharOperators') * SpaceBreak ^ 0,
    Assignable = m.Cmt(m.V'Chain', util.check_assignable) + m.V'Name' + m.V'SelfName',
    Exp = (plain_space * m.V'Value' * (m.V'BinaryOperator' * m.V'Value') ^ 0),
    SimpleValue = m.V'If' + m.V'Unless' + m.V'Switch' + m.V'With' + m.V'ClassDecl' + m.V'ForEach' + m.V'For' + m.V'While' + m.Cmt(m.V'Do', check_do) + util.sym("-") * -SomeSpace * m.V'Exp' + util.sym("#") * m.V'Exp' + util.sym("~") * m.V'Exp' + m.V'NOT' * m.V'Exp' + m.V'TblComprehension' + m.V'TableLit' + m.V'Comprehension' + m.V'FunLit' + Num,
    ChainValue = (m.V'Chain' + m.V'Callable') * (m.V'InvokeArgs' ^ -1),
    Value = (m.V'SimpleValue' + m.V'KeyValueList' + m.V'ChainValue' + m.V'String'),
  --Value = pos(SimpleValue + Ct(KeyValueList) / mark("table") + ChainValue + String),
    SliceValue = m.V'Exp',
    String = Space * m.V'DoubleString' + Space * m.V'SingleString' + m.V'LuaString',
    SingleString = util.simple_string("'"),
    DoubleString = util.simple_string('"', true),
    LuaString = m.C(m.V'LuaStringOpen') * Break ^ -1 * ((1 - m.Cmt(m.C(m.V'LuaStringClose'), util.check_lua_string)) ^ 0) * m.V'LuaStringClose',
  --LuaString = m.V'LuaStringOpen' * Break ^ -1 * m.C((1 - m.Cmt(m.C(m.V'LuaStringClose'), util.check_lua_string)) ^ 0) * m.V'LuaStringClose',
    LuaStringOpen = util.sym("[") * m.P("=") ^ 0 * "[",
    LuaStringClose = "]" * m.P("=") ^ 0 * "]",
    Callable = m.V'Name' + m.V'SelfName' + m.V'VarArg' + m.V'Parens',
    Parens = util.sym("(") * SpaceBreak ^ 0 * m.V'Exp' * SpaceBreak ^ 0 * util.sym(")"),
    FnArgs = util.symx("(") * SpaceBreak ^ 0 * (m.V'FnArgsExpList' ^ -1) * SpaceBreak ^ 0 * util.sym(")") + util.sym("!") * -m.P("="),  -- m.Ct("")
    FnArgsExpList = m.V'Exp' * ((Break + util.sym(",")) * White * m.V'Exp') ^ 0,
    Chain = (m.V'Callable' + m.V'String' + -m.S(".\\")) * m.V'ChainItems' + Space * (m.V'DotChainItem' * m.V'ChainItems' ^ -1 + m.V'ColonChain'),
    ChainItems = m.V'ChainItem' ^ 1 * m.V'ColonChain' ^ -1 + m.V'ColonChain',
    ChainItem = m.V'Invoke' + m.V'DotChainItem' + m.V'Slice' + util.symx("[") * m.V'Exp' * util.sym("]"),
    DotChainItem = util.symx(".") * _Name,
    ColonChainItem = util.symx("\\") * _Name,
    ColonChain = m.V'ColonChainItem' * (m.V'Invoke' * m.V'ChainItems' ^ -1) ^ -1,
    Slice = util.symx("[") * (m.V'SliceValue' ^-1) * util.sym(",") * (m.V'SliceValue' ^-1) * (util.sym(",") * m.V'SliceValue') ^ -1 * util.sym("]"),
  --Slice =      symx("[") * (    SliceValue + Cc(1)) *   sym(",") * (    SliceValue + Cc("")) * ( sym(",") *     SliceValue ) ^ -1 *      sym("]") / mark("slice"),
    Invoke = m.V'FnArgs' + m.V'SingleString' + m.V'DoubleString' + L(m.P("[")) * m.V'LuaString',
    TableValue = m.V'KeyValue' + m.V'Exp',
    TableLit = util.sym("{") * (m.V'TableValueList' ^ -1 * util.sym(",") ^ -1 * (SpaceBreak * m.V'TableLitLine' * (util.sym(",") ^ -1 * SpaceBreak * m.V'TableLitLine') ^ 0 * util.sym(",") ^ -1) ^ -1) * White * util.sym("}"),
    TableValueList = m.V'TableValue' * (util.sym(",") * m.V'TableValue') ^ 0,
    TableLitLine = m.V'PushIndent' * ((m.V'TableValueList' * m.V'PopIndent') + (m.V'PopIndent' * util.Cut)) + Space,
    TableBlockInner = (m.V'KeyValueLine' * (SpaceBreak ^ 1 * m.V'KeyValueLine') ^ 0),
    TableBlock = SpaceBreak ^ 1 * m.V'Advance' * util.ensure(m.V'TableBlockInner', m.V'PopIndent') ,
    
    ClassDecl = m.V'CLASS' * -m.P(":") * (m.V'Assignable') ^-1 *   (m.V'EXTENDS' * m.V'PreventIndent' * util.ensure(m.V'Exp', m.V'PopIndent') ) ^-1 * (m.V'ClassBlock' ^-1),
  --ClassDecl = m.V'CLASS' * -m.P(":") * ((m.V'Assignable') ^-1) * (m.V'EXTENDS' * m.V'PreventIndent' * util.ensure(m.V'Exp', m.V'PopIndent') ^-1) ^-1 * (m.V'ClassBlock' ^-1),
  
  --ClassDecl = key("class") * -P(":") * (Assignable + Cc(nil)) * (key("extends") *    PreventIndent *       ensure(    Exp,      PopIndent) + C("")) ^-1 * (  ClassBlock + Ct("")),
    ClassBlock = SpaceBreak ^ 1 * m.V'Advance' * (m.V'ClassLine' * (SpaceBreak ^ 1 * m.V'ClassLine') ^ 0) * m.V'PopIndent',
  --ClassBlock = SpaceBreak ^ 1 *     Advance *Ct(    ClassLine *  (SpaceBreak ^ 1 *     ClassLine ) ^ 0) * PopIndent,
    ClassLine = m.V'CheckIndent' * ((m.V'KeyValueList' + m.V'Statement' + m.V'Exp') * util.sym(",") ^ -1),
    Export = m.V'EXPORT' * (             m.V'ClassDecl' + op("*") + op("^") + m.V'NameList' * (util.sym("=") * m.V'ExpListLow') ^ -1),
  --Export = key("export") * (Cc("class") *    ClassDecl  + op("*") + op("^") +  Ct(NameList) * (     sym("=") *  Ct(ExpListLow)) ^ -1),
  --KeyValue = (util.sym(":") * -SomeSpace * Name) + (KeyName + util.sym("[") * m.V'Exp' * util.sym("]") + Space * m.V'DoubleString' + Space * m.V'SingleString' * util.symx(":") * (m.V'Exp' + m.V'TableBlock' + SpaceBreak ^ 1 * m.V'Exp')),
    KeyValue = (util.sym(":") * -SomeSpace * m.V'Name') +               ((m.V'KeyName' + util.sym("[") * m.V'Exp' * util.sym("]") + Space * m.V'DoubleString' + Space * m.V'SingleString') * util.symx(":") * (m.V'Exp' + m.V'TableBlock' + SpaceBreak ^ 1 * m.V'Exp')),
  --KeyValue = (util.sym(":") * -SomeSpace * Name * m.Cp()) / util.self_assign + m.Ct( (KeyName + util.sym("[") * m.V'Exp' * util.sym("]") + Space * m.V'DoubleString' + Space * m.V'SingleString') * util.symx(":") * (m.V'Exp' + m.V'TableBlock' + SpaceBreak ^ 1 * m.V'Exp')),
    KeyValueList = m.V'KeyValue' * (util.sym(",") * m.V'KeyValue') ^ 0,
    KeyValueLine = m.V'CheckIndent' * m.V'KeyValueList' * util.sym(",") ^ -1,
    FnArgsDef = (util.sym("(") * White *   (m.V'FnArgDefList' ^ -1) * (m.V'USING' *   (m.V'NameList' + Space * "nil")) ^-1      * White * util.sym(")") )^-1,-- + m.Ct("") * m.Ct(""),
  --FnArgsDef =       sym("(") * White * Ct(    FnArgDefList ^ -1 ) * (key("using") * Ct(    NameList  + Space * "nil") + Ct("")) * White *      sym(")") + Ct("") * Ct(""),
    FnArgDefList = m.V'FnArgDef' * ((util.sym(",") + Break) * White * m.V'FnArgDef') ^ 0 * ((util.sym(",") + Break) * White * m.V'VarArg') ^ 0 + m.V'VarArg',
    FnArgDef = ((m.V'Name' + m.V'SelfName') * (util.sym("=") * m.V'Exp') ^ -1),
    FunLit = m.V'FnArgsDef' * (util.sym("->") + util.sym("=>")) * (m.V'Body' ^-1),
    NameList = m.V'Name' * (util.sym(",") * m.V'Name') ^ 0,
    NameOrDestructure = m.V'Name' + m.V'TableLit',
    AssignableNameList = m.V'NameOrDestructure' * (plain_space * util.sym(",") * plain_space * m.V'NameOrDestructure') ^ 0,
    ExpList = m.V'Exp' * (plain_space * util.sym(",") * plain_space * m.V'Exp') ^ 0,
    ExpListLow = m.V'Exp' * (plain_space * (util.sym(",") + util.sym(";") * plain_space) * m.V'Exp') ^ 0,
    InvokeArgs = -m.P("-") * (m.V'ExpList' * (util.sym(",") * (m.V'TableBlock' + SpaceBreak * m.V'Advance' * m.V'ArgBlock' * m.V'TableBlock' ^ -1) + m.V'TableBlock') ^ -1 + m.V'TableBlock'),
    ArgBlock = m.V'ArgLine' * (util.sym(",") * SpaceBreak * m.V'ArgLine') ^ 0 * m.V'PopIndent',
    ArgLine = m.V'CheckIndent' * m.V'ExpList',
    
    Name = Name2,
    SelfName = SelfName2,
    KeyName = KeyName2,
    VarArg = VarArg2,

    LOCAL = key('local'),       -- funkcia vrati: (Space * "local" * -AlphaNum)
    IMPORT = key('import'),
    BREAK = key('break'),
    FROM = key('from'),
    IF = key('if'),
    ELSE = key('else'),
    ELSEIF = key('elseif'),
    UNLESS = key('unless'),
    RETURN = key('return'),
    WITH = key('with'),
    SWITCH = key('switch'),
    DO = key('do'),
    WHEN = key('when'),
    THEN = key('then'),
    CONTINUE = key('continue'),
    WHILE = key('while'),
    FOR = key('for'),
    IN = key('in'),
    NOT = key('not'),
    CLASS = key('class') * plain_space,
    EXTENDS = key('extends'),
    EXPORT = key('export'),
    USING = key('using')
}


--[[
Checks if `input` is valid Moonscript source code.

**Parameters:**
* `input`: a string containing Moonscript source code.

**Returns:**
* `true`, if `input` is valid Moonscript source code, or `false` if the matching fails.
--]]
local function check(input)
  local builder = m.P(rules)
  local result = builder:match(input)
  
  if (type(result) == "number") then      -- ak je result cislo
    
    if (result == #input + 1) then        -- kontrola poctu sparsovanych znakov a celkoveho poctu znakov
      return true
    end

    return false
  end

  return false            -- ak result obsahuje nieco ine (tabulka, retazec)
end

----------
-- @name check_special
-- @author Matus Stefanik
-- @param input - [string] moonscript source code to parse
-- @return [boolean] [string] [string] State of parsable current input, parsed source code and unparsed source code.
local function check_special(input)
  local builder = m.P(rules)
  local result = builder:match(input)
  
  if (type(result) == "number") then
    if (result == #input + 1) then 
      return true, input:sub(1, result-1)
    end
    return false, input:sub(1, result-1), input:sub(result)
  end
  return false, nil, result
end


--[[
Uses [grammar.html#function_apply grammar.apply] to return a new grammar, with `captures` and extra rules. [#variable_rules rules] stays unmodified.

**Parameters:**
* `extraRules`: optional, the new and modified rules. See [grammar.html#function_apply grammar.apply] for the accepted format.
* `captures`: optional, the desired captures. See [grammar.html#function_apply grammar.apply] for the accepted format.

**Returns:**
* the extended grammar.
--]]
local function apply(extraRules, captures)
  return grammar.apply(rules, extraRules, captures)
end

return {
  rules = rules,
  apply = apply,
  check = check,
  check_special = check_special,
  Comment = Comment,
  Space = Space,
  Num = Num
}