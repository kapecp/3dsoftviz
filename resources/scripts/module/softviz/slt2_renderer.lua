-----------------------------------------------
-- SLT2 renderer - module for rendering slt2 template widgets
-- Michael Gloger
-----------------------------------------------
local slt2 = require('slt2')
local classDiagram = require "luameg.plantuml"

----------------------------------------------
-- Renderer
local function render(template, models)
  local templateFile = assert(slt2.loadfile('../share/3dsoftviz/webview/'..template..'.slt2'))

  return slt2.render(templateFile, {models=models})
end


----------------------------------------------
-- Return plantUML output data for specified node
-- luameg.plantuml.classDiagram functions
----------------------------------------------

----------------------------------------------
-- Return SVG text for specified node
-- @param nodeId id of a node in AST
-- @return text string of SVG content
local function getClassDiagramSvg(nodeId)
  local luadbGraph = graphManager:findLastGraph()
  local text = classDiagram.getClassDiagramSvg(luadbGraph, nodeId)
  return text
end

----------------------------------------------
-- Return plantuml template for specified node
-- @param nodeId id of a node in AST
-- @return text string of plantuml template
local function getClassPlantuml(nodeId)
  local luadbGraph = graphManager:findLastGraph()
  local text = classDiagram.getClassPlantuml(luadbGraph, nodeId)
  return text
end

----------------------------------------------
-- Return JSON for specified node
-- @param nodeId id of a node in AST
-- @return text JSON object
local function getClassJsonData(nodeId)
  local luadbGraph = graphManager:findLastGraph()
  local text = classDiagram.getClassJsonData(luadbGraph, nodeId)
  return text
end

----------------------------------------
-- Public interface of module
return {
  render = render,
  getClassPlantuml = getClassPlantuml,
  getClassDiagramSvg = getClassDiagramSvg,
  getClassJsonData = getClassJsonData
}
