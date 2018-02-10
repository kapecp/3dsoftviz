-----------------
-- Submodule for extracting class graph from AST
-- @release 29.04.2017 Matúš Štefánik
-----------------

local luadb = require 'luadb.hypergraph'

-----------
-- @name createNode
-- @author Matus Stefanik
-- @param type - [string] type of node. Write to meta.type
-- @param name - [string] name of node. Write to data.name
-- @param astNodeId - [number] nodeid from AST tree. Write to data.astNodeID
-- @param refToAstText - [table] This table contains ids to ast nodes where is same text as in this node. 
--           It is used in changing text of this node. Then is replace text on all nodes from this table.
-- @param visibility - [string] visibility of element in node (for methods and properties)
-- @return [table] new luadb node
local function createNode(type, name, astNodeId, refToAstText, visibility)
	local node = luadb.node.new()
	node.meta = node.meta or {}
	node.meta.type = type
	node.data.name = name
	node.data.visibility = visibility
	node.data.refToAstText = refToAstText
	--node.data.astID = astId  -- nech sa astID doplni mimo tohto submodulu
	node.data.astNodeID = astNodeId
	return node
end

-----------
-- @name createNode
-- @author Matus Stefanik
-- @param label - [string] label of edge. Write to label
-- @param from - [table] luadb node as source of this edge
-- @param to - [table]  luadb node as target of this edge
-- @param isOriented - [boolean] if this edge is oriented or not
-- @return [table] new luadb edge
local function createEdge(label, from, to, isOriented)
	assert(type(from)=="table" and type(to)=="table", 'Node "from" or "to" is not table. Is it correct luadb node?')
	local edge = luadb.edge.new()
	edge.label = label
	edge:setSource(from)
	edge:setTarget(to)
	if isOriented ~= nil and isOriented == true then edge:setAsOriented() end
	return edge
end

---------------------
-- Vrati zoznam podstromov, pre ktore boli splnene neededValue a inKey.
-- @name getChildNode
-- @author Matus Stefanik
-- @param ast - [table] ast from luameg (moonscript)
-- @param neededValue - [string] required value from table
-- @param inKey - [string] which key in table must have value 'neededValue'
-- @param maxDepth - [numeric] max depth to search for neededValue
-- @param dataOut - [table] returned data. Contains all subtrees/subtables containing required value in key. Using for recursion.
-- @return [table] dataOut
local function getChildNode(ast, neededValue, inKey, maxDepth, dataOut)
	local dataOut = dataOut or {}
	local maxDepth = maxDepth or nil

	if maxDepth ~= nil and maxDepth <= 0 then
		return dataOut
	end

	if (ast == nil) then
		return dataOut
	end

	if (ast[inKey] == neededValue) then
		table.insert(dataOut, ast)
	end

	for i=1, #ast["data"] do
		local newDepth = maxDepth
		if maxDepth ~= nil then
			newDepth = maxDepth-1
		end
		getChildNode(ast["data"][i], neededValue, inKey, newDepth, dataOut)
	end

	return dataOut
end


--------
-- Vrati vsetky ast uzly ktore splnuju zadanu cestu 'path'. Specialny symbol '*' znamena lubovolny nazov uzlu. 
-- Priklad path:
--    { "*", "ClassLine", "KeyValueList", "KeyValue", "KeyName" }
--    { "*", "ClassDecl", "Statement", "*", "Value", "*", "Name" }
--    { "*", "ClassDecl", "*" ,"Name"}   -> zoberie napr. ClassDecl->Statement->Exp->ExpList->Name ale aj ClassDecl->ClassLine->Value->Name
--    { "*" } -> vrati vsetky uzly z AST
--    { "*", "ClassDecl" } -> vrati vsetky ClassDecl uzly
--    { "*", "ClassDecl", "*"} -> vrati vsetky uzly pod ClassDecl -> cele podstromy
-- @name getAstNodesByPath
-- @author Matus Stefanik
-- @param ast - [table] ast tree or subtree from luameg. Nodes must have ["data"] as list of childrens.
-- @param path - [table] path of needed types (or 'inKey'). Path support special character '*' for all
--         types. Examples: {"*", "ClassDecl"} return all ClassDecl nodes.
-- @param inKey - [string] (optional) default is "key". What parameter is comparing with path.
-- @param maxDepth - [number] (optional) max depth to search. Default is nil as infinite.
-- @return [table] All ast nodes whose are in path.
local function getAstNodesByPath(ast, path, inKey, maxDepth)
	local dataOut = {}
	local index = 1
	local maxDepth = maxDepth or nil
	local inKey = inKey or "key"

	if maxDepth ~= nil and maxDepth <= 0 then
		return dataOut
	end

	if ast == nil then
		return dataOut
	end

	if path == nil or #path == 0 then
		return dataOut
	end

	local function getNode(ast, path, inKey, maxDepth, index, dataOut, flagAny)
		local dataOut = dataOut or {}
		local index = index or 1
		local lastIndex = #path  
		local flagAny = flagAny or false
		assert(type(flagAny) == "boolean")

		if path[index] == "*" then
			flagAny = true
		end

		if maxDepth ~= nil and maxDepth <= 0 then
			return dataOut
		end
		if ast == nil then
			return dataOut
		end

		-- last element
		if index >= lastIndex and (tostring(ast[inKey]) == path[index] or flagAny) then
			table.insert(dataOut, ast)
			
			if flagAny == "*" then
				for i=1, #ast["data"] do
					local val = tostring(ast["data"][i][inKey])
					local newDepth = maxDepth
					if maxDepth ~= nil then
						newDepth = maxDepth-1
					end
					getNode(ast["data"][i], path, inKey, newDepth, index+1, dataOut, flagAny)
				end
			end
			return dataOut
		end


		if tostring(ast[inKey]) == path[index] or path[index] == "*" then

			-- prehladavanie potomkov
			for i=1, #ast["data"] do
				local val = tostring(ast["data"][i][inKey])
				local newDepth = maxDepth
				if maxDepth ~= nil then
					newDepth = maxDepth-1
				end

				if flagAny and path[index+1]==ast["data"][i][inKey] then
					getNode(ast["data"][i], path, inKey, newDepth, index+1, dataOut, false)
				end

				if path[index] == "*" then
					getNode(ast["data"][i], path, inKey, newDepth, index, dataOut, true)
				else
					getNode(ast["data"][i], path, inKey, newDepth, index+1, dataOut, false)
				end

			end

		end

		return dataOut
	end

	dataOut = getNode(ast, path, inKey, maxDepth, 1)

	return dataOut
end

----------
-- Check if ast contain node folowed by path. Same as function getAstNodesByPath
-- Example path: { "*", "ClassDecl", "Statement", "*", "Value", "*", "Name" }
-- @name isAstNodesByPath
-- @author Matus Stefanik
-- @param ast - [table] ast tree or subtree from luameg. Nodes must have ["data"] as list of childrens.
-- @param path - [table] path of needed types (or 'inKey'). Path support special character '*' for all
--         types. Examples: {"*", "ClassDecl"} return all ClassDecl nodes.
-- @param inKey - [string] (optional) default is "key". What parameter is comparing with path.
-- @param maxDepth - [number] (optional) max depth to search. Default is nil as infinite.
-- @return [boolean] true if node exist described by path and in maxDepth
local function isAstNodesByPath(ast, path, inKey, maxDepth)
	local dataOut = {}
	local index = 1
	local maxDepth = maxDepth or nil
	local inKey = inKey or "key"

	local result = getAstNodesByPath(ast, path, inKey, maxDepth)
	if #result > 0 then
		return true
	end

	return false
end

---------------------------
-- @name isValueInTree
-- @author Matus Stefanik
-- @param ast - [table] AST from luameg (moonscript)
-- @param key - [string] which key in table must have value 'value'
-- @param value - [string] required value from key
-- @return [boolean][number] true or false, and count of matches
local function isValueInTree(ast, key, value)
	local outResult = false
	local outCount = 0

	if (ast == nil) then
		return outResult, outCount
	end

	if (ast[key] == value) then
		outResult = true
		outCount = outCount + 1
	end

	for i=1, #ast["data"] do
		local result, count = isValueInTree(ast["data"][i], key, value)
		
		if result == true then
			outResult = (outResult or result)
			outCount = outCount + count
		end
	end

	return outResult, outCount
end

---------------------
-- @name getAllClasses
-- @author Matus Stefanik
-- @param ast - [table] ast from luameg (moonscript)
-- @return [table] list of nodes of all classes from AST with all methods and properties
--    { ["astNode"]=astNodeWithClass, ["name"]=astNode, ["extends"]=astNode, ["properties"]={["astNode"]=astNode, ["visibility"]=text}, ["methods"]={["astNode"]=classLine, ["name"]=astNode, ["visibility"]=text, ["args"]={astNode} } }
local function getAllClasses(ast)
	local out = {["astNode"]=nil, ["name"]=nil, ["extends"]=nil, ["properties"]=nil, ["methods"]=nil}

	if ast == nil then
		return out
	end

	-- vsetky ast uzly classDecl
	local classDeclTree = getAstNodesByPath(ast, {"*", "ClassDecl"}, "key", 14)

	for k, astClassDecl in pairs(classDeclTree) do
		local outClassData = {}
		outClassData["methods"] = {}
		outClassData["properties"] = {}

		-- mnozina vsetkych properties v triede - pomocna mnozina kvoli duplicitam
		local setProps = {}

		outClassData["astNode"] = astClassDecl

		-- class name
		local astClassName = getAstNodesByPath(astClassDecl, {"ClassDecl", "Assignable", "Name"}, "key", 3)
		outClassData["name"] = astClassName[1]

		-- extended class name
		local isAstExtendedClassName = isAstNodesByPath(astClassDecl, {"ClassDecl", "EXTENDS"}, "key", 2)
		if isAstExtendedClassName == true then
			local astExtendedClassName = getAstNodesByPath(astClassDecl, {"ClassDecl", "Exp", "Value", "*", "Name"}, "key", 6)
			outClassData["extends"] = astExtendedClassName[1]
		end

		-- jednotlive class line uzly
		local astClassLines = getAstNodesByPath(astClassDecl, {"ClassDecl", "ClassBlock", "ClassLine"})
		for _, astClassLine in pairs(astClassLines) do

			-- nazov hodnoty v class line
			local astKeyName = getAstNodesByPath(astClassLine, {"ClassLine", "KeyValueList", "KeyValue", "KeyName"})
			local astStatementName = getAstNodesByPath(astClassLine, {"ClassLine", "Statement", "ExpList", "*", "Callable", "Name"})
			local astStatementSelfName = getAstNodesByPath(astClassLine, {"ClassLine", "Statement", "ExpList", "*", "Callable", "SelfName"})
			local isAssignInStatement = isAstNodesByPath(astClassLine, {"ClassLine", "Statement", "Assign"}, "key", 4)

			-- metody obsahuju FunLit (uzol obsahujuci argumenty), clenske premenne ho neobsahuju
			local astFunLit_arguments = getAstNodesByPath(astClassLine, {"ClassLine", "*", "FunLit"}, "key", 9)
			if astFunLit_arguments ~= nil and #astFunLit_arguments > 0 then
				-- classLine obsahuje metodu (bud s argumentami alebo bez)

				local outMethods = {}
				outMethods["astNode"] = astClassLine
				outMethods["args"] = {}

				-- name and visibility. Visibility sa urcuje podla toho co je pod ClassLine: KeyValueList alebo Statement,
				--  a ak Statement, tak musi obsahovat Assign
				if astKeyName[1] ~= nil and #astKeyName > 0 then
					outMethods["visibility"] = "public"
					outMethods["name"] = astKeyName[1]
				elseif isAssignInStatement == true and astStatementName[1] ~= nil and #astStatementName > 0 then
					outMethods["visibility"] = "private"
					outMethods["name"] = astStatementName[1]
				elseif isAssignInStatement == true and astStatementSelfName[1] ~= nil and #astStatementSelfName > 0 then
					outMethods["visibility"] = "public"
					outMethods["name"] = astStatementSelfName[1]
				else
					-- error
					print("Unknow method name in extractorClass !")
					outMethods["name"] = "unknow"
					outMethods["visibility"] = "public"
				end

				local astFnArgDefs = getAstNodesByPath(astFunLit_arguments[1], {"FunLit", "*", "FnArgDef"}, "key", 4)
					
				for _, astFnArgDef in pairs(astFnArgDefs) do

					-- potomok uzla obsahujuci nazov argumentu
					local childNode = astFnArgDef["data"][1]
					
					table.insert(outMethods["args"], childNode)

					if childNode.key == "SelfName" then
						local childNodeName = childNode.text:gsub("@", "")
						if setProps[childNodeName] == nil and #childNodeName > 0 then
							setProps[childNodeName] = true
							table.insert(outClassData.properties, {["astNode"]=childNode, ["visibility"]="public"})
						end
					end
				end

				table.insert(outClassData["methods"], outMethods)


				local astStatements = getAstNodesByPath(astClassLine, {"ClassLine", "*", "Statement"})
				for _, astStatement in pairs(astStatements) do 
					local isAssign = isAstNodesByPath(astStatement, {"Statement", "Assign"}, "key", 2)
					if isAssign == true then
						local astSelfName = getAstNodesByPath(astStatement, {"Statement", "ExpList", "Exp", "*", "Callable", "SelfName"})
						if #astSelfName > 0 then
							local name = astSelfName[1].text:gsub('@', '')
							if setProps[name] == nil and #name > 0 then
								setProps[name] = true
								table.insert(outClassData["properties"], {["astNode"]=astSelfName[1], ["visibility"]="public"})
							end
						end
					end
				end

			else
				-- classLine obsahuje clensku premennu
				if astKeyName[1] ~= nil and #astKeyName > 0 then
					local propName = astKeyName[1].text:gsub("@", "")
					if setProps[propName] == nil and #propName > 0 then
						setProps[propName] = true
						table.insert(outClassData["properties"], {["astNode"]=astKeyName[1], ["visibility"]="public"})
					end
				elseif isAssignInStatement == true and astStatementName[1] ~= nil and #astStatementName > 0 then
					local propName = astStatementName[1].text:gsub("@", "")
					if setProps[propName] == nil and #propName > 0 then
						setProps[propName] = true
						table.insert(outClassData["properties"], {["astNode"]=astStatementName[1], ["visibility"]="private"})
					end
				end

			end


		end  -- koniec classLine

		table.insert(out, outClassData)
	end  -- koniec classDecl

	return out
end

--------
-- @name getUsingClassName
-- @author Matus Stefanik
-- @param ast - [table] ast tree from luameg module
-- @param className - [string] name of class to find using in ast
-- @return [table] table with ast node where it is used className. Finding assign and extends in ast.
local function getUsingClassName(ast, className)
	-- nazvy tried pri priradeni a pri extend
	local out = {}

	-- // nazov triedy -> Line, Statement, Assign, ExpListLow, Exp, Value, ChainValue, Chain, Callable, Name 
	-- // argumenty do konstruktora -> Line, Statement, Assign, ExpListLow, Exp, Value, ChainValue, Chain, ChainItems, ... argumenty
	-- // ClassDecl, ak obsahuje EXTENDS tak potom v Exp, Value, ChainValue, Callable, Name

	local path = {"*", "Callable", "Name"}
	local usingClassName = getAstNodesByPath(ast, path, "key", nil)
	for k, v in pairs(usingClassName) do
		if v.text == className then
			table.insert(out, v)
		end
	end

	return out
end

--------------------------
-- Podobne ako luadb findNodesByName, vyhlada vsetky uzly zo zoznamov 'nodes' a 'oldNodes' s menom 'name'
-- @name findNodesByName
-- @author Matus Stefanik
-- @param nodes - [table] list of nodes
-- @param name - [string] name of needed node
-- @param oldNodes - [table] (optional) list of nodes
-- @return [table] all nodes with needed 'name' from 'nodes' and 'oldNodes'
local function findNodesByName(nodes, name, oldNodes)
	local out = {}

	if oldNodes ~= nil then
		for k, node in pairs(oldNodes) do
			if node.data ~= nil and node.data.name == name then
				table.insert(out, node)
			end
		end
	end
	for k, node in pairs(nodes) do
		if node.data ~= nil and node.data.name == name then
			table.insert(out, node)
		end
	end

	return out
end

--------------------
-- Return all nodes and edges as graph from this AST
-- @name getGraph
-- @author Matus Stefanik
-- @param ast - [table] AST from luameg
-- @param nodes - [table] (optional) existing nodes
-- @return [table] list of all luadb nodes and luadb edges for luadb graph
local function getGraph(ast, nodes)
	assert(ast ~= nil, 'AST is nil.')

	local nodes = nodes or {}
	local out = {["nodes"]={}, ["edges"]={}}

	-- pomocna tabulka s potrebnymi udajmi pre vytvorenie grafu tried
	local classes = getAllClasses(ast)

	for i=1, #classes do
		local className = classes[i]["name"]["text"]

		-- vytvori sa novy uzol s triedou alebo ak uz existuje, tak sa k nemu pripoja nove hrany
		local nodeClass = findNodesByName(out.nodes, className, nodes)
		if nodeClass == nil or #nodeClass == 0 then
			local nodeClassAstNodeId = classes[i]["astNode"]["nodeid"]
			
			-- ulozenie informacii o pouzivani tejto triedy -> ulozenie id k ast uzlom
			local using = getUsingClassName(ast, className)
			local refToAstText = {}
			table.insert(refToAstText, classes[i]["name"]["nodeid"])
			for k,v in pairs(using) do
				table.insert(refToAstText, v.nodeid)
			end

			nodeClass = createNode("class", className, nodeClassAstNodeId, refToAstText, nil)

			table.insert(out.nodes, nodeClass)
		else
			nodeClass = nodeClass[1]	-- zoberiem len prvy vyskyt (nemalo by byt viacej tried s rovnakym nazvom)
		end
		
		-- extends
		if classes[i]["extends"] ~= nil then
			local nodeExtended = findNodesByName(out.nodes, classes[i]["extends"]["text"], nodes)
			if #nodeExtended == 0 then

				local nodeExtendedName = classes[i]["extends"]["text"]
				local nodeExtendedAstNodeId = classes[i]["extends"]["nodeid"] -- FIX: nodeid of ast node contain Name node and not ClassDecl node

				-- ulozenie informacii o pouzivani tejto triedy -> ulozenie id k ast uzlom
				local using = getUsingClassName(ast, className)
				local refToAstText = {}
				table.insert(refToAstText, classes[i]["extends"]["nodeid"])
				for k,v in pairs(using) do
					table.insert(refToAstText, v.nodeid)
				end
				
				nodeExtended = createNode("class", nodeExtendedName, nodeExtendedAstNodeId, refToAstText, nil)
				table.insert(out.nodes, nodeExtended)
			else 
				nodeExtended = nodeExtended[1]		-- zoberiem len prvy vyskyt
			end

			local edge = createEdge("extends", nodeClass, nodeExtended, true)

			table.insert(out.edges, edge)
		end
		
		-- methods
		for j=1, #classes[i]["methods"] do

			local nodeMethodName = classes[i]["methods"][j]["name"]["text"]:gsub("@", "")
			local nodeMethodAstNodeId = classes[i]["methods"][j]["astNode"]["nodeid"]
			local nodeMethodVisibility = classes[i]["methods"][j]["visibility"]
			local nodeMethod = createNode("method", nodeMethodName, nodeMethodAstNodeId, nil, nodeMethodVisibility)
			
			-- arguments
			for k=1, #classes[i]["methods"][j]["args"] do

				local nodeArgName = classes[i]["methods"][j]["args"][k]["text"]:gsub('@', '')
				local nodeArgAstNodeId = classes[i]["methods"][j]["args"][k]["nodeid"]
				local nodeArg = createNode("argument", nodeArgName, nodeArgAstNodeId, nil, nil)

				local edgeArg = createEdge("has", nodeMethod, nodeArg, true)

				table.insert(out.nodes, nodeArg)
				table.insert(out.edges, edgeArg)
			end

			local edge = createEdge("contains", nodeClass, nodeMethod, true)

			table.insert(out.edges, edge)
			table.insert(out.nodes, nodeMethod)
		end

		-- properties
		for j=1, #classes[i]["properties"] do

			local nodePropName = classes[i]["properties"][j]["astNode"]["text"]:gsub('@', '')
			local nodePropAstNodeId = classes[i]["properties"][j]["astNode"]["nodeid"]
			local nodePropVisibility = classes[i]["properties"][j]["visibility"]
			local nodeProp = createNode("property", nodePropName, nodePropAstNodeId, nil, nodePropVisibility)

			local edge = createEdge("contains", nodeClass, nodeProp, true)

			table.insert(out.edges, edge)
			table.insert(out.nodes, nodeProp)
		end

		
	end

	return out
end

return {
	getGraph = getGraph
}
