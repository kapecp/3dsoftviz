-----------------------------------------------
-- SLT2 renderer - module for rendering slt2 template widgets
-- Michael Gloger
-----------------------------------------------
local slt2 = require('slt2')

-- TODO just a test
local user = {
    name = 'world'
}

local function test()
  local templateFile = assert(slt2.loadfile('../share/3DSOFTVIZ/webview/templates/template1.slt2'))
  return slt2.render(templateFile, {user = user})
end

----------------------------------------
-- Public interface of module
return {test = test}
