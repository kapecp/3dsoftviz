-----------------------------------------------
-- SLT2 renderer - module for rendering slt2 template widgets
-- Michael Gloger
-----------------------------------------------
local slt2 = require('slt2')

local function render(template, data)
  local templateFile = assert(slt2.loadfile('../share/3DSOFTVIZ/webview/templates/'..template..'.slt2'))
  return slt2.render(templateFile, {data=data})
end

----------------------------------------
-- Public interface of module
return {render = render}
