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
-- @return [table] new luadb node
local function createNode(type, name, astNodeId, refToAstText)
	local node = luadb.node.new()
	node.meta = node.meta or {}
	node.meta.type = type
	node.data.name = name
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
--    { ["astNode"]=astNodeWithClass, ["name"]=astNode, ["extends"]=astNode, ["properties"]={astNode}, ["methods"]={["astNode"]=classLine, ["name"]=astNode, ["args"]={astNode} } }
local function getAllClasses(ast) 
	local out = {["astNode"]=nil, ["name"]=nil, ["extends"]=nil, ["properties"]=nil, ["methods"]=nil}

	if ast == nil then
		return out
	end

	-- ziska vsetky podstromy kde su definovane triedy (uzly ClassDecl)
	local classDeclTree = getChildNode(ast, "ClassDecl", "key", 14)

	for i=1, #classDeclTree do
		local classNameTree = {}
		local classLinesTree = {}
		local props = {}
		local methods = {}
		local extendedClass = {}
		local extendedClassTree = {}

		-- mnozina vsetkych properties v triede
		local setProps = {}

		-- v classNameTree bude meno triedy (malo by byt len jedno v danom bloku/podstromu ClassDecl)
		classNameTree = getChildNode(classDeclTree[i], "Name", "key", 3)

		-- v classLinesTree budu podstromy vsetkych metod a properties v danom bloku/podstromu ClassDecl
		-- classLinesTree = getChildNode(classDeclTree[i], "KeyValue", "key")
		classLinesTree = getChildNode(classDeclTree[i], "ClassLine", "key")


		-- ziskanie extends class
		for qq=1, #classDeclTree[i]["data"] do
			if classDeclTree[i]["data"][qq]["key"] == "Exp" then
				extendedClassTree = getChildNode(classDeclTree[i]["data"][qq], "Name", "key", 6)
			end
		end

		-- prevod tabulky extendedClass na string alebo nil. extendedClass by mal obsahovat iba jeden element
		if #extendedClassTree > 0 then
			extendedClass = extendedClassTree[1]["text"]
		else
			extendedClass = nil
		end

		-- ziskanie nazvu triedy. Nazov by mal byt iba jeden.
		if (#classNameTree > 0) then

			for j=1, #classLinesTree do
				
				local methodsArgsTrees2 = {}
				local methodsArgsTrees = {}
				local methodsArgs = {}

				methodsArgsTrees = getChildNode(classLinesTree[j], "FunLit", "key")

				-- ziskanie vsetkych metod
				if #methodsArgsTrees > 0 then
					methodsArgsTrees2 = getChildNode(methodsArgsTrees[1], "FnArgDef", "key")
					for k=1, #methodsArgsTrees2 do
						table.insert(methodsArgs, methodsArgsTrees2[k]["data"][1])
					end

					local methodd = {}
					methodd = getChildNode(classLinesTree[j], "KeyName", "key")
					if #methodd > 0 then
						table.insert(methods, {["name"] = methodd[1], ["astNode"] = classLinesTree[j], ["args"] = methodsArgs})

						--if methodd[1]["text"] == "new" then
						--	-- vsetky selfname na lavej strane od Assign v Statement, v konstruktore new()
						local stmsTree = {}
						stmsTree = getChildNode(classLinesTree[j], "Statement", "key")
						for k=1, #stmsTree do
							local selfNameTree ={}
							local expListTree ={}
							expListTree = getChildNode(stmsTree[k], "ExpList", "key")
							if #expListTree > 0 then
								selfNameTree = getChildNode(expListTree[1], "SelfName", "key")
								if #selfNameTree > 0 then
									local t = selfNameTree[1]["text"]:gsub('%W', '')
									if #t ~= 0 then
										if setProps[t] == nil or setProps[t] ~= true then
											setProps[t] = true
											table.insert(props, selfNameTree[1])
										end
									end
								end
							end
						end
						--end
					end
				else
					-- ziskanie vsetkych properties
					local propss = {}
					propss = getChildNode(classLinesTree[j], "KeyName", "key")
					if #propss ~= 0 then
						local t = propss[1]["text"]:gsub('%W', '')
						if #t ~= 0 then
							if setProps[t] == nil or setProps[t] ~= true then
								setProps[t] = true
								table.insert(props, propss[1])
							end
						end
					end
				end
			end

			table.insert(out, {["astNode"]=classDeclTree[i], ["name"]=classNameTree[1], ["extends"]=extendedClassTree[1], ["properties"]=props, ["methods"]=methods})
		end
		
	end

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

			nodeClass = createNode("class", className, nodeClassAstNodeId, refToAstText)

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
				
				nodeExtended = createNode("class", nodeExtendedName, nodeExtendedAstNodeId, refToAstText)
				table.insert(out.nodes, nodeExtended)
			else 
				nodeExtended = nodeExtended[1]		-- zoberiem len prvy vyskyt
			end

			local edge = createEdge("extends", nodeClass, nodeExtended, true)

			table.insert(out.edges, edge)
		end
		
		-- methods
		for j=1, #classes[i]["methods"] do

			local nodeMethodName = classes[i]["methods"][j]["name"]["text"]
			local nodeMethodAstNodeId = classes[i]["methods"][j]["astNode"]["nodeid"]
			local nodeMethod = createNode("method", nodeMethodName, nodeMethodAstNodeId, nil)
			
			-- arguments
			for k=1, #classes[i]["methods"][j]["args"] do

				local nodeArgName = classes[i]["methods"][j]["args"][k]["text"]
				local nodeArgAstNodeId = classes[i]["methods"][j]["args"][k]["nodeid"]
				local nodeArg = createNode("argument", nodeArgName, nodeArgAstNodeId, nil)

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

			local nodePropName = classes[i]["properties"][j]["text"]:gsub('%W', '')
			local nodePropAstNodeId = classes[i]["properties"][j]["nodeid"]
			local nodeProp = createNode("property", nodePropName, nodePropAstNodeId, nil)

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
