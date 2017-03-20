-----------------------------------------------
-- AST Manager
-- @release 2017/03/15, Denis Illes
-----------------------------------------------

local utils = require "luadb.utils"

local n = 0
local function inc()
  n = n + 1
  return n
end

-----------------------------------------------
-- AST Manager
-----------------------------------------------
local pASTManager = {
    _type = 'AST_Manager'
}

local function ASTManager(arguments)
  local arguments = arguments or {}
  
  local o = {
    label = nil,
    AST = {}
  }

  setmetatable(o,
    {
      __index = pASTManager,
      __tostring = function(t)
        if t.label then
          return "AST Manager: "..t.label
        else
          return "AST Manager"
        end
      end
    }
  )
  
  -- passing arguments
  for key,value in pairs(arguments) do
    o[key] = value
  end

  return o
end

function pASTManager:addLabel(label)
  self.label = label  
end

function pASTManager:addAST(AST, path)
  local ASTNode = {
    id = 'AST'..inc(),
    root = AST,
    path = path
  }
  table.insert(self.AST, ASTNode)
  if self.EdgeMapper then
    self.EdgeMapper:save(ASTNode)
  end
  return ASTNode.id
end

-----------------------------------------------
-- Print functions
-----------------------------------------------

function pASTManager:printASTs()
  for index,ast in pairs(self.AST) do
    if ast.path~=nil then
      print("AST with ID: "..ast['id'].." PATH: "..ast['path'])
    else
      print("AST with ID: "..ast['id'])
    end
  end
end

-----------------------------------------------
-- Find functions
-----------------------------------------------

-- get AST with selected id
function pASTManager:findASTByID(id)  
  for _,ast in pairs(self.AST) do
    if ast.id == id then
      return ast.root
    end
  end
  return nil
end

-- get AST with selected path
function pASTManager:findASTByPath(path)  
  for _,ast in pairs(self.AST) do
    if ast.path == path then
      utils.logger:debug('AST found, no need to extract')
      return ast.root, ast.id
    end
  end
  return nil
end

-----------------------------------------------
-- Return
-----------------------------------------------

return
{
  new = ASTManager
}