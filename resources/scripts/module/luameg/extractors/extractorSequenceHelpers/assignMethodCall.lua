
-- helper function which returns true if actual node represents assign statement
local function isAssignStatement (node)
	return (#node.data == 2) and (node.key == "Statement") and (node.data[1].key == "ExpList") and (node.data[2].key == "Assign")
end

-- 
local function isMethodWithArguments (node)
	return (#node.data == 2) and (node.data[1].key == "Callable") and (node.data[2].key == "InvokeArgs")
end

-- 
local function isMethodWithoutArguments (node)
	return (#node.data == 1) and (node.data[1].key == "Chain") and (node.data[1].data[1].key == "Callable") and (node.data[1].data[2].key == "ChainItems")
end

-- 
local function isObjectMethodWithArguments (node)
	return (#node.data == 2) and (#node.data[1].data == 2) and (node.data[1].key == "Chain") and (node.data[2].key == "InvokeArgs") and (node.data[1].data[1].key == "Callable") and (node.data[1].data[2].key == "ChainItems")
end

local function isSystemCall (methodName)
	local systemMethods = {
		print = 0,
		require = 0
	}
	local index = systemMethods[methodName] or -1

	return (index ~= -1)
end

--
local function isParenthesArgs (argumentsNode)
	if (argumentsNode ~= nil) then
		return false, nil
	end

	if ( argumentsNode.key == "FnArgsExpList") then
		return true, argumentsNode
	else
		return isParenthesArgs(argumentsNode.data[1])
	end
end

local function getArguments (argumentsNode, arguments)

	if ((argumentsNode.key ~= "InvokeArgs") and (argumentsNode.key ~= "ChainItems")) then
		return arguments
	end

	local isNestedWithParentheses, nestedWithParenthesesNode = isParenthesArgs(argumentsNode)
	if (isNestedWithParentheses) then
		for key, value in pairs(nestedWithParenthesesNode.data) do
			local argName = value.text
			print(argName)
			argName = argName:gsub(" ", "")
			argName = argName:gsub("@", "")
			argName = argName:gsub("%(", "")
			argName = argName:gsub("%)", "")

			table.insert(arguments, argName)
		end
	else
		-- use ExpList node instead of parent InvokeArgs
		for key, value in pairs(argumentsNode.data[1].data) do
			local argName = value.text
			print(argName)
			argName = argName:gsub(" ", "")
			argName = argName:gsub("@", "")
			argName = argName:gsub("%(", "")
			argName = argName:gsub("%)", "")

			table.insert(arguments, argName)
		end
	end

	return arguments
end

-- 
local function isFunctionCall (node, arguments)

	if (node.key ~= 'Exp') then
		return false
	else
		node = node.data[1].data[1]

		local hasArguments = isMethodWithArguments(node)
		local withoutArguments = isMethodWithoutArguments(node)
		local objectMethodWithArguments = isObjectMethodWithArguments(node)

		if (hasArguments or objectMethodWithArguments) then
			arguments = getArguments(node.data[2], arguments)
		elseif (withoutArguments) then
			arguments = getArguments(node.data[1].data[2], arguments)
		end

		local returnedBoolValue = hasArguments or withoutArguments or objectMethodWithArguments
		return returnedBoolValue, arguments
	end
end

-- 
local function findMethodCall(statement)
  local methodCall = nil
  local methodArguments = {}

  for i, node in pairs(statement.data) do
  	local isFunction
  	isFunction, methodArguments = isFunctionCall(node, methodArguments)
    if (isFunction) then
      methodCall = node
      break
    else
      methodCall, methodArguments = findMethodCall(node)
    end
  end

  return methodCall, methodArguments
end

-- 
function getName(node)

  if (node.key == "Name") or (node.key == "SelfName") then
  	local nodeText = node.text
  	return nodeText:gsub("@", "")
  else
  	for index, nextNode in pairs(node.data) do 
    	return getName(nextNode)
    end
  end
end

-- 
local function constructMethodNode (node)

	local methodName = ""
	local varName = ""

	-- pure method call with arguments
	if (isMethodWithArguments(node.data[1].data[1])) then

		node = node.data[1].data[1]
		methodName = getName(node.data[1])

	-- method call without arguments - this case has conflicts between method call on object and
	-- pure method call
	elseif (isMethodWithoutArguments(node.data[1].data[1])) then

		node = node.data[1].data[1]
		-- newMethodNode.name = getName(node.data[1].data[1])
		local helpName = node.data[1].data[2].data[1].text

		if (helpName:sub(1,1) == '.') then
			varName = getName(node.data[1].data[1])
			methodName = helpName:gsub("^.", "")
		elseif (helpName:sub(1,1) == '\\') then
			varName = getName(node.data[1].data[1])
			helpName = node.data[1].data[2].data[1].data[1].text
			methodName = helpName:gsub("^\\", "")
		else
			methodName = getName(node.data[1].data[1])
		end

	-- object method call with arguments
	elseif (isObjectMethodWithArguments(node.data[1].data[1])) then
		node = node.data[1].data[1]
		varName = getName(node.data[1].data[1])
		methodName = node.data[1].data[2].text:gsub("^.", "")
		methodName = methodName:gsub("^\\", "")
	end

	return varName, methodName
end

-- 
local function checkArgumentsAgainstScope (scope, arguments)

	local matchedArguments = {}
	for i=1, #arguments, 1 do
		local actualArg = arguments[i]

		matchedArguments[i] = scope[actualArg]
	end

	return matchedArguments
end


return {
	isAssignStatement = isAssignStatement,
	isFunctionCall = isFunctionCall,
	findMethodCall = findMethodCall,
	constructMethodNode = constructMethodNode,
	getName = getName,
	isSystemCall = isSystemCall,
	checkArgumentsAgainstScope = checkArgumentsAgainstScope
}