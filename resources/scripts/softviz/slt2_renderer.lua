-----------------------------------------------
-- SLT2 renderer - module for rendering slt2 template widgets
-- Michael Gloger
-----------------------------------------------
local slt2 = require('slt2')

local function render(template, models)
  local templateFile = assert(slt2.loadfile('../share/3dsoftviz/webview/'..template..'.slt2'))

  return slt2.render(templateFile, {models=models})
end

----------------------------------------
-- Public interface of module
return {render = render}
