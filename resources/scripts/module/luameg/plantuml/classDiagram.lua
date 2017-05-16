--------------------------------
-- Submodule for generating plantuml template for uml class diagram and svg.
-- @release 08.04.2017 Matúš Štefánik
--------------------------------

local pairs = pairs

-----------------------
-- @name tableSize
-- @author Matus Stefanik
-- @param T - [table] table
-- @return [number] size of table T
local function tableSize(T)
	local count = 0
	for _ in pairs(T) do
		count = count + 1
	end
	return count
end

---------------------------------------
-- Pomocna funkcia na extrahovanie potrebnych 
-- hodnot ako nazov triedy, metody, clenske premenne, 
-- nazov rodica apod z uzlu typu trieda
--
-- Ukazka vystupnej tabulky dataOut:
--	data["Observer"]["nodeId"]
--	data["Observer"]["extends"]["name"]
--	data["Observer"]["extends"]["nodeId"]
--	data["Observer"]["properties"][i]["name"]
--	data["Observer"]["properties"][i]["visibility"]
--	data["Observer"]["properties"][i]["nodeId"]
--	data["Observer"]["methods"][i]["name"]
--	data["Observer"]["methods"][i]["visibility"]
--	data["Observer"]["methods"][i]["nodeId"]
--	data["Observer"]["methods"][i]["args"][i]
--	data["Observer"]["methods"][i]["args"][i]["name"]
--	data["Observer"]["methods"][i]["args"][i]["nodeId"]
--
-- @name getTableFromCLassNode
-- @author Matus Stefanik
-- @param graph - [table] luaDB graph with class graph
-- @param nodeId - [string] id of node for search
-- @param dataOut - [table] optional. Using in recursion. dataOut is returned.
-- @return [table] table with all collected info for class diagram from this nodeId
local function getTableFromClassNode(graph, nodeId, dataOut)
	local dataOut = dataOut or {}

	assert(graph ~= nil, "Graph is nil")
	assert(type(graph) == "table" and graph.nodes ~= nil and graph.edges ~= nil, "Problem with graph. Is it luadb graph?")
	assert(nodeId ~= nil and type(nodeId) == "string", "Problem with nodeId. Is it string?")

	local node = graph:findNodeByID(nodeId)
	assert(node ~= nil, "Node with id \"" .. nodeId .. "\" is nil. Is it correct id?")

	-- pozadovany uzol musi byt typu Class
	if node.meta.type:lower() == "class" and dataOut[node.data.name] == nil then

		-- vytvorenie polozky zatial s prazdnymi udajmi
		dataOut[node.data.name] = {["nodeId"]=node.id, ["extends"]=nil, ["properties"]={}, ["methods"]={}}

		-- najdenie vsetkych metod a clenskych premennych pre uzol class
		local edges_MethodsProperties = graph:findEdgesBySource(node.id, "contains")
		for i=1, #edges_MethodsProperties do
			local nodeChild = edges_MethodsProperties[i].to[1]
			if nodeChild.meta.type:lower() == "method" then
				-- method with arguments
				local argsList = {}

				local edges_argument = graph:findEdgesBySource(nodeChild.id, "has")
				for j=1, #edges_argument do
					local nodeArgument = edges_argument[j].to[1]
					if nodeArgument.meta.type:lower() == "argument" then
						table.insert(argsList, {["name"]=nodeArgument.data.name, ["nodeId"]=nodeArgument.id})
					end
				end
				table.insert(dataOut[node.data.name]["methods"], {["name"]=nodeChild.data.name, ["nodeId"]=nodeChild.id, ["args"]=argsList, ["visibility"]=nodeChild.data.visibility})
			elseif nodeChild.meta.type:lower() == "property" then
				-- property
				table.insert(dataOut[node.data.name]["properties"], {["name"]=nodeChild.data.name, ["nodeId"]=nodeChild.id, ["visibility"]=nodeChild.data.visibility})
			end
		end

		-- najdenie vsetkych rodicovskych tried
		local edges_Class = graph:findEdgesBySource(node.id, "extends")
		for i=1, #edges_Class do
			local nodeChild = edges_Class[i].to[1]
			if nodeChild.meta.type:lower() == "class" then
				-- extends
				dataOut[node.data.name]["extends"] = {["name"]=nodeChild.data.name, ["nodeId"]=nodeChild.id}
				dataOut = getTableFromClassNode(graph, nodeChild.id, dataOut)
			end
		end
	end

	return dataOut
end

-------------------------------------
-- @name getTableFromFileNode
-- @author Matus Stefanik
-- @param graph - [table] luaDB graph with class graph
-- @param nodeId - [string] id of node for search
-- @param dataOut - [table] optional. Using in recursion. dataOut is returned.
-- @return [table] table with all collected info for class diagram from this nodeId
local function getTableFromFileNode(graph, nodeId, outData)
	local outData = outData or {}

	local node = graph:findNodeByID(nodeId)
	assert(node ~= nil, "Node with id \"" .. nodeId .. "\" is nil. Is it correct id?")

	if node.meta.type:lower() == "file" then

		local edges_class = graph:findEdgesBySource(node.id, "contains")
		for i=1, #edges_class do
			local nodeChild = edges_class[i].to[1]
			if nodeChild.meta.type:lower() == "class" then
				outData = getTableFromClassNode(graph, nodeChild.id, outData)
			end
		end
	end

	return outData
end

-----------------------------
-- @name getTableFromDirectoryNode
-- @author Matus Stefanik
-- @param graph - [table] luaDB graph with class graph
-- @param nodeId - [string] id of node for search
-- @param dataOut - [table] optional. Using in recusive. dataOut is returned.
-- @return [table] table with all collected info for class diagram from this nodeId
local function getTableFromDirectoryNode(graph, nodeId, outData)
	local outData = outData or {}

	local node = graph:findNodeByID(nodeId)
	assert(node ~= nil, "Node with id \"" .. tostring(nodeId) .. "\" is nil. Is it correct id?")

	if node.meta.type:lower() == "directory" then

		local edges_subfile = graph:findEdgesBySource(node.id, "contains")
		for i=1, #edges_subfile do
			local nodeChild = edges_subfile[i].to[1]
			if nodeChild.meta.type:lower() == "file" then
				outData = getTableFromFileNode(graph, nodeChild.id, outData)
			elseif nodeChild.meta.type:lower() == "directory" then
				outData = getTableFromDirectoryNode(graph, nodeChild.id, outData)
			end
		end
	end

	return outData
end

--------------------------
-- Function return all needed data from project node. It means that all 
-- data from file, directory and class nodes are collected.
-- @name getTableFromProjectNode
-- @author Matus Stefanik
-- @param graph - [table] luaDB graph with class graph
-- @param nodeId - [string] id of node for search
-- @param dataOut - [table] optional. Using in recursion. dataOut is returned.
-- @return [table] table with all collected info for class diagram from this nodeId
local function getTableFromProjectNode(graph, nodeId, outData)
	local outData = outData or {}

	local node = graph:findNodeByID(nodeId)
	assert(node ~= nil, "Node with id \"" .. nodeId .. "\" is nil. Is it correct id?")

	if node.meta.type:lower() == "project" then

		local edges_fileDir = graph:findEdgesBySource(node.id, "contains")
		for i=1, #edges_fileDir do
			local nodeChild = edges_fileDir[i].to[1]
			if nodeChild.meta.type:lower() == "file" then
				outData = getTableFromFileNode(graph, nodeChild.id, outData)
			elseif nodeChild.meta.type:lower() == "directory" then
				outData = getTableFromDirectoryNode(graph, nodeChild.id, outData)
			end
		end
	end

	return outData
end



--------------------
-- Main functions to get plantuml template from node various type (class node, file node, directory node and project node)
-- @name getPlantUmlFromNode
-- @author Matus Stefanik
-- @param graph - [table] luaDB graph with class graph
-- @param nodeId - [string] id of node from which is needed image with class diagram
-- @return [string] Text with uml class diagram for plantUML
local function getPlantUmlFromNode(graph, nodeId)
	assert(graph ~= nil and type(graph) == "table" and graph.nodes ~= nil, "Problem with graph. Is it luadb graph?")
	local node = graph:findNodeByID(nodeId)
	assert(node ~= nil, 'Node with id "' .. nodeId .. '" is nil. Is it correct id?')

	if node == nil then
		return "@startuml\n \n@enduml"
	end

	local dataOut = {}
	table.insert(dataOut, "@startuml\n")

	local data = nil

	assert((node.meta ~= nil and node.meta.type ~= nil) or (node.data ~= nil and node.data.type ~= nil), 
		"Node has not defined type in meta.type or data.type. Is it correct luadb graph?")

	local nodeType = nil
	if node.meta ~= nil and node.meta.type ~= nil then
		nodeType = node.meta.type
	elseif node.data ~= nil and node.data.type ~= nil then
		nodeType = node.data.type
	end

	if nodeType:lower() == "project" then
		data = getTableFromProjectNode(graph, node.id)
	elseif nodeType:lower() == "directory" then
		data = getTableFromDirectoryNode(graph, node.id)
	elseif nodeType:lower() == "file" then
		data = getTableFromFileNode(graph, node.id)
	elseif nodeType:lower() == "class" then
		data = getTableFromClassNode(graph, node.id)
	else
		return "@startuml\n \n@enduml"
	end

	local strExtends = ""

	--[[
	plantuml template:

		@startuml
		class [name] {
			+[property]
			+[method]([args])
		}
		[extends] <|-- [name]
		@enduml

	]]

	-- z nazbieranych dat sa vytvori text pre plantuml
	for key, value in pairs(data) do
		-- trieda
		table.insert(dataOut, "class " .. key .. " {\n")
		
		-- properties
		for i=1, #value["properties"] do
			if value["properties"][i]["visibility"] == "private" then
				table.insert(dataOut, "\t- " .. value["properties"][i]["name"] .. "\n")
			else
				table.insert(dataOut, "\t+ " .. value["properties"][i]["name"] .. "\n")
			end
		end
		
		-- methods
		for i=1, #value["methods"] do
			if value["methods"][i]["visibility"] == "private" then
				table.insert(dataOut, "\t- " .. value["methods"][i]["name"] .. "(")
			else
				table.insert(dataOut, "\t+ " .. value["methods"][i]["name"] .. "(")
			end

			-- arguments
			for j=1, #value["methods"][i]["args"] do
				if j == #value["methods"][i]["args"] then
					table.insert(dataOut, value["methods"][i]["args"][j]["name"])
				else
					table.insert(dataOut, value["methods"][i]["args"][j]["name"] .. ", ")
				end
			end
			table.insert(dataOut, ")\n")
		end

		-- end of class block
		table.insert(dataOut, "}\n")

		-- extends
		if value["extends"] ~= nil then
			strExtends = strExtends .. value["extends"]["name"] .. " <|-- " .. key .. "\n"
		end
	end

	table.insert(dataOut, "\n\n" .. strExtends .. "\n")

	table.insert(dataOut, "@enduml\n")

	return table.concat(dataOut)
end

----------------
-- Main functions to get svg image from node various type (class node, file node, directory node and project node)
-- @name getImageFromNode
-- @author Matus Stefanik
-- @param graph - [table] luaDB graph with class graph
-- @param nodeId - [string] id of node from which is needed image with class diagram
-- @param pathToPlantuml - [string] (optional) path to executable plantuml.jar for generating svg.
-- @return [string] Image of Class diagram from node nodeId in SVG format as text
local function getImageFromNode(graph, nodeId, pathToPlantuml)
	local plant = getPlantUmlFromNode(graph, nodeId)
	local pathToPlantuml = pathToPlantuml or "plantuml.jar"

	-- vytvorenie docasneho suboru s plantUML textom
	local file = assert(io.open("_uml.txt", "w"))
	file:write(plant) 	-- zapis do txt suboru
	file:close()

	-- spustenie plantuml aplikacie s vytvorenym suborom na vstupe
	os.execute("java -jar " .. pathToPlantuml .. " -quiet -tsvg _uml.txt")

	-- plantUML vytvori novy subor s obrazkom. Precita sa a ulozi sa text do premennej
	file = assert(io.open("_uml.svg", "r"))
	local text = file:read("*all") 	-- precitanie svg
  	file:close()
  	
  	-- upratanie docasnych suborov
  	os.remove("_uml.txt")
  	os.remove("_uml.svg")
  	
  	-- vrati sa text SVG
  	return text
end

------------
-- @name getJsonDataFromNode
-- @author Matus Stefanik
-- @param graph - [table] luadb graph from luameg with class nodes
-- @param nodeId - [string] node id
-- @return [string] [string] json data with node data and link data
local function getJsonDataFromNode(graph, nodeId)
	assert(graph ~= nil and type(graph) == "table" and graph.nodes ~= nil, "Problem with graph. Is it luadb graph?")
	local node = graph:findNodeByID(nodeId)
	assert(node ~= nil, 'Node is nil. Is it correct nodeId? ("' .. nodeId .. '")')

	local outnode = {}
	local outlink = {}
	local templink = {}

	if node == nil then
		return "", ""
	end

	local data = nil

	assert((node.meta ~= nil and node.meta.type ~= nil) or (node.data ~= nil and node.data.type ~= nil), 
		"Node has not defined type in meta.type or data.type. Is it correct luadb graph?")

	local nodeType = nil
	if node.meta ~= nil and node.meta.type ~= nil then
		nodeType = node.meta.type
	elseif node.data ~= nil and node.data.type ~= nil then
		nodeType = node.data.type
	end

	if nodeType:lower() == "project" then
		data = getTableFromProjectNode(graph, node.id)
	elseif nodeType:lower() == "directory" then
		data = getTableFromDirectoryNode(graph, node.id)
	elseif nodeType:lower() == "file" then
		data = getTableFromFileNode(graph, node.id)
	elseif nodeType:lower() == "class" then
		data = getTableFromClassNode(graph, node.id)
	else
		return "", ""
	end

	--	data["Observer"]["nodeId"]
	--	data["Observer"]["extends"]["name"]
	--	data["Observer"]["extends"]["nodeId"]
	--	data["Observer"]["properties"][i]["name"]
	--	data["Observer"]["properties"][i]["visibility"]
	--	data["Observer"]["properties"][i]["nodeId"]
	--	data["Observer"]["methods"][i]["name"]
	--	data["Observer"]["methods"][i]["visibility"]
	--	data["Observer"]["methods"][i]["nodeId"]
	--	data["Observer"]["methods"][i]["args"][i]
	--	data["Observer"]["methods"][i]["args"][i]["name"]
	--	data["Observer"]["methods"][i]["args"][i]["nodeId"]

	local counter = 1
	local countClasses = tableSize(data)

	for className, value in pairs(data) do
		local nodeIdNum = string.match(value["nodeId"], "%d+")
		table.insert(outnode, '{\n\tkey: ' .. nodeIdNum .. ',\n')
		table.insert(outnode, '\tname: "' .. className .. '",\n')
		table.insert(outnode, '\tproperties: [\n')

		-- properties
		for i=1, #value["properties"] do
			local property = value["properties"][i]
			local visibilityStr = "public"
			if value["properties"][i]["visibility"] == "private" then
				visibilityStr = "private"
			end
			local propertyNodeId = string.match(property["nodeId"], "%d+")
			table.insert(outnode, '\t\t{ name: "' .. property["name"] .. '", visibility: "' .. visibilityStr .. '", id: ' .. propertyNodeId .. ' ' )

			if i == #value["properties"] then
				-- last
				table.insert(outnode, '}\n')
			else
				table.insert(outnode, '},\n')
			end
		end

		table.insert(outnode, '\t],\n')
		table.insert(outnode, '\tmethods: [\n')
		-- methods
		for i=1, #value["methods"] do
			local methodData = value["methods"][i]
			local methodName = methodData["name"]
			local methodVisibilityStr = "public"
			if value["methods"][i]["visibility"] == "private" then
				methodVisibilityStr = "private"
			end
			local methodNodeId = string.match(methodData["nodeId"], "%d+")

			table.insert(outnode, '\t\t{ name: "' .. methodName .. '", visibility: "' .. methodVisibilityStr .. '", parameters: [ ')

			-- arguments
			for j=1, #methodData["args"] do
				local argName = methodData["args"][j]
				local argNodeId = string.match(argName["nodeId"], "%d+")
				table.insert(outnode, ' { name: "' .. argName["name"] .. '", id: ' .. argNodeId .. ' }')

				if j ~= #methodData["args"] then
					-- not last
					table.insert(outnode, ', ')
				end
			end

			if i == #value["methods"] then
				-- last
				table.insert(outnode, '], id: ' .. methodNodeId .. ' }\n')
			else
				table.insert(outnode, '], id: ' .. methodNodeId .. ' },\n')
			end
		end
		table.insert(outnode, '\t]\n')

		if counter == countClasses then
			-- last
			table.insert(outnode, '}\n')
		else
			table.insert(outnode, '},\n')
		end

		-- extends
		if value["extends"] ~= nil then
			local childId = value["extends"]["nodeId"]
			local childIdNum = string.match(childId, "%d+")
			table.insert(templink, {nodeIdNum, childIdNum})
		end

		counter = counter + 1
	end

	-- relationships
	for i=1, #templink do
		local nodeIdNum = templink[i][1]
		local childIdNum = templink[i][2]

		table.insert(outlink, '{ from: ' .. nodeIdNum .. ', to: ' .. childIdNum .. ', relationship: "generalization" } ')
		if i == #templink then
			-- last
			table.insert(outlink, '\n')
		else
			table.insert(outlink, ',\n')
		end
	end

	--[[
	Ukazka:
	outnode = [=[
	      {
	        key: 1,
	        name: "Component",
	        properties: [
	          { name: "name", visibility: "public", id: 8 }
	        ],
	        methods: [
	          { name: "new", visibility: "public", parameters: [{ name: "title", id: 15}, { name: "value", id: 16 }], id: 9 }
	        ]
	      } ]=]

	outlink = [=[
	      { from: 11, to: 1, relationship: "generalization" },
	      { from: 12, to: 1, relationship: "generalization" }
	      ]=]
	]]
	return table.concat(outnode), table.concat(outlink)
end

return {
	getPlantUmlFromNode = getPlantUmlFromNode,
	getImageFromNode = getImageFromNode,
	getJsonDataFromNode = getJsonDataFromNode
}

