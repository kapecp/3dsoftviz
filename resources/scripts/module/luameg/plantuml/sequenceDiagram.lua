
local function generateSequenceDiagramImage()
  os.execute("java -jar plantuml.jar  -quiet -tsvg _uml.txt")
  local svgFile = assert(io.open("_uml.svg", "r"))
  local svgContent = svgFile:read("*all")
  svgFile:close()

  os.remove("_uml.txt")
  os.remove("_uml.svg")
  return svgContent
end

-----------------------------------------------------

-- Returns part of luadb graph representing sequence diagram for given method - format = string
-- to save into file and then generate svg with use of plantUML
local function filterSequenceDiagram( graph, methodNodeID, outputStr, classCalledFrom )

  local outputString = outputStr or ''
  local methodNode = graph:findNodeByID( methodNodeID )
  
  if (methodNode == nil) then
    return nil
  end

  local classCalledWithin = classCalledFrom
  if (methodNode.meta.type == 'method') then
    classCalledWithin = graph:findEdgesByTarget( methodNodeID, 'contains' )[1].from[1].data.name
    outputString = outputString .. "activate " .. classCalledWithin .. "\n"
  end

  local methodExecutions = graph:findEdgesBySource( methodNodeID, 'executes' )

  for index, edge in pairs(methodExecutions) do
    local actualNode = edge.to[1]

    if (actualNode.meta.type == 'method') then
      
      local classCalledTo = graph:findEdgesByTarget( actualNode.id, 'contains' )[1].from[1].data.name

      outputString = outputString .. classCalledWithin .. " -> " .. classCalledTo .. " : " .. actualNode.data.name .. "\n"
      -- outputString = outputString .. "activate " .. classCalledTo .. "\n"

      -- recursive call
      outputString = filterSequenceDiagram( graph, actualNode.id, outputString, classCalledWithin )

      -- outputString = outputString .. "deactivate " .. classCalledTo .. "\n"

    elseif (actualNode.meta.type == 'condition') then

      local conditionBranches = graph:findEdgesBySource( actualNode.id, 'hasBranch' )

      for key, branch in pairs(conditionBranches) do

        local branchNode = branch.to[1]

        if (key == 1) then
          outputString = outputString .. "alt " .. branchNode.data.name .. "\n"
          outputString = filterSequenceDiagram( graph, branchNode.id, outputString, classCalledWithin )
        else
          outputString = outputString .. "else " .. branchNode.data.name .. "\n"
          outputString = filterSequenceDiagram( graph, branchNode.id, outputString, classCalledWithin )
        end

      end

      outputString = outputString .. "end\n"
    elseif (actualNode.meta.type == 'loop') then

      local loopHeaderNode = graph:findEdgesBySource( actualNode.id, 'hasHeader' )[1].to[1]
      outputString = outputString .. "loop " .. loopHeaderNode.data.name .. "\n"

      outputString = filterSequenceDiagram( graph, actualNode.id, outputString, classCalledWithin )
      outputString = outputString .. "end\n"
    elseif (actualNode.meta.type == 'return') then

      if (classCalledFrom) then
        outputString = outputString .. classCalledWithin .. " -> " .. classCalledFrom .. " : " .. actualNode.data.name .. "\n"
      end

    end
  end

  if (methodNode.meta.type == 'method') then
    outputString = outputString .. "deactivate " .. classCalledWithin .. "\n"
  end

  return outputString
end

-- Takes string and generates svg image for sequence diagram
local function generateSVG( sequenceDiagramString )

end

local function getImg(graph, methodID)
  local outputString = ''
  outputString = outputString .. "@startuml\n"

  outputString = filterSequenceDiagram( graph, methodID, outputString )

  outputString = outputString .. "@enduml"

  local file = io.open("_uml.txt", "w")
  file:write(outputString)
  file:close()

  return generateSequenceDiagramImage()
end

-----------------------------------------------------




return {
  getSVGDiagram = getImg
}