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
-- luameg.plantuml.classDiagram functions
local function getClassDiagramSvg(nodeId)
  local luadbGraph = graphManager:findLastGraph()
  local text = classDiagram.getClassDiagramSvg(luadbGraph, nodeId)
  return text
end

local function getClassPlantuml(nodeId)
  local luadbGraph = graphManager:findLastGraph()
  local text = classDiagram.getClassPlantuml(luadbGraph, nodeId)
  return text
end

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
