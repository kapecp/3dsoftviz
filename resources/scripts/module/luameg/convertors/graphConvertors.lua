-------------------------------------------------------
-- Convertor from 'luadb' graph and 'hypergraph' to special import 
-- graph needed for Lua::LuaGraph.loadGraph()
-- @release 2017/03/29 Matúš Štefánik
-------------------------------------------------------

local id_ = 0

--------------------------
-- Generator for unique IDs
-- @name inc
-- @author Matus Stefanik
-- @return [number] unique id
local function inc()
	id_ = id_ + 1
	return id_
end

---------------------------------------
-- Convert luadb graph to special import graph for HyperLua Lua::LuaGraph
-- @name convertGraphToImportGraph
-- @author Matus Stefanik
-- @param graph - [table] luaDB graph from luameg
-- @return [table] graph in import format for HyperLua Lua::LuaGraph. Graph is in format
--   [{type,id,label,params}] = {[{type,id,label,direction}]={type,id,label,params}, [{type,id,label,direction}]={type,id,label,params}} 
--   or simple: [edge]={[incidence]=[node],[incidence]=[node]}
local function convertGraphToImportGraph(graph)
	local newGraph = {}

	local nodes = {}

	assert(graph ~= nil, "Input graph is nil.")
	assert(graph.nodes ~= nil, "Input graph does not contain key 'nodes'. Is it luadb graph?")
	if graph == nil or graph.nodes == nil then
		return newGraph
	end

	-- nodes
	for k, vNode in pairs(graph.nodes) do
		local newNode = {type="node", id=inc(), label=vNode.data.name, params={origid=vNode.id, name=vNode.data.name}}

		-- ak je to root node
		if newNode.id == 1 then 
			newNode.params.root = true 
		end

		assert((vNode.meta ~= nil and vNode.meta.type ~= nil) or vNode.data.type ~= nil, "Node without defined type in meta.type or data.type. Is it correct luadb graph?")
		-- doplni type uzla bud z meta.type, alebo z data.type
		if vNode.meta ~= nil and vNode.meta.type ~= nil then
			newNode.params.type = vNode.meta.type
		elseif vNode.data.type ~= nil then
			newNode.params.type = vNode.data.type
		end

		-- pre uzly file a directory sa zoberu ich cesty 
		if vNode.meta ~= nil and (vNode.meta.type:lower() == "file" or vNode.meta.type:lower() == "directory") then
			newNode.params.path = vNode.data.path
		end

		-- automaticke skopirovanie dat z atributu data do newNode.params. Vynechaju sa uz skopirovane atributy.
		for kKey, vValue in pairs(vNode.data) do
			if kKey ~= "name" and kKey ~= "type" and kKey ~= "path" then
			--	newNode.params[kKey] = vValue		-- ak bude treba preniest vsetky data, odkomentovat. Zatial netreba, k datam sa pristupuje cez id.
			end
		end

		-- ulozenie uzla do zoznamu
		nodes[vNode] = newNode		
	end

	assert(graph.edges ~= nil, "Input graph does not contain key 'edges'. Is it luadb graph?")
	-- edges
	for k, vEdge in pairs(graph.edges) do
		local edge = {type="edge", id=inc(), label=vEdge.label, params={origid=vEdge.id, type=vEdge.label}}

		-- incidencie k hrane
		local incid1 = {type="edge_part", id=inc(), label=''}
		local incid2 = {type="edge_part", id=inc(), label=''}

		-- ak je hrana orientovana, tak sa nastavi orientacia na incidenciach
		if vEdge.orientation:lower() == "oriented" then
			incid1.direction = "in"
			incid2.direction = "out"
		end

		-- ulozenie uzlov a incidencii do zoznamu s hranou
		newGraph[edge] = {[incid1]=nodes[vEdge.from[1]], [incid2]=nodes[vEdge.to[1]]}
	end

	return newGraph
end

--------------
-- TODO: lepsie otestovat, mozu byt vacsie nedostatky, chybaju niektore polozky!
--       'ast, fcgraph = hyperluametrics.getMetricsForFile(filename)'
--       skusane na grafe z ast.hypergraph
-- @deprecated
-- Convert hypergraph from module 'hypergraph' to special import graph needed for Lua::LuaGraph.loadGraph()
-- @name convertHypergraphToImportGraph
-- @author Matus Stefanik
-- @param graph - [table] graph from module 'hypergraph'
-- @return [table] graph in import format for HyperLua Lua::LuaGraph. Graph is in format
--   [{type,id,label,params}] = {[{type,id,label,direction}]={type,id,label,params}, [{type,id,label,direction}]={type,id,label,params}} 
--   or simple: [edge]={[incidence]=[node],[incidence]=[node]}
local function convertHypergraphToImportGraph(graph)
	local newGraph = {}

	assert(graph ~= nil, "Input graph is nil.")
	assert(graph.Nodes ~= nil, "Input graph does not contain key Nodes. Is it 'hypergraph' graph?")
	local hNodes = graph.Nodes
	assert(graph.Edges ~= nil, "Input graph does not contain key Edges. Is it 'hypergraph' graph?")
	local hEdges = graph.Edges

	for kEdge, vTableIncidence in pairs(hEdges) do

		if kEdge ~= nil or vTableIncidence ~= nil then
			local newEdge = {type="edge", id=inc(), label=kEdge.label, params={origid=kEdge.id, type=kEdge.label}}

			-- pravdepodobne vzdy 2 prvky obsahuje vTableIncidence (vyskytuje sa aj 1 prvok?)
			local incids = {}
			local nodes = {}

			if vTableIncidence ~= nil then

				isOk = true
				for kInc, vNode in pairs(vTableIncidence) do
					if kInc ~= nil or vNode ~= nil then

						local newIncid = {type="edge_part", id=inc(), label=kInc.label, origid=kInc.id}
						table.insert(incids, newIncid)

						local newNode = {type="node", id=inc(), label=vNode.label, params={origid=vNode.id, name=vNode.label}}
						table.insert(nodes, newNode)
					else
						isOk = false
						--print("03 table.insert " .. kEdge.id)
					end
				end

				-- vyskytuje sa hrana ktora ma len jednu incidenciu (chyba?)
				if isOk == true and (incids[2] ~= nil and nodes[2] ~= nil) then
					newGraph[newEdge] = {[incids[1]]=nodes[1], [incids[2]]=nodes[2]}
				else
					newGraph[newEdge] = {[incids[1]]=nodes[1]}
					--print("02 incids, nodes " .. kEdge.id .. ", " .. #incids .. ", " .. #nodes)
				end
			else
				--print("04 vTableIncidence is nil " .. kEdge.id)
			end
		else -- if kEdge ~= nil or vTableIncidence ~= nil then ... else
			--print("01 " .. kEdge.id)
		end

	end

	return newGraph
end

return {
	convertGraphToImportGraph = convertGraphToImportGraph,
	convertHypergraphToImportGraph = convertHypergraphToImportGraph
}