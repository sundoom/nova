--------------------------------------------------------------------------
-- helpers.lua
-- author : sunjun 
-- email : 4421971@gmail.com
--------------------------------------------------------------------------

-- module ("helpers", package.seeall)
json = require('json')

--------------------------------------------------------------------------

function reload_module(modulename)
	if package.loaded[modulename] then
		print(string.format("reload module[%s]", modulename))
	end
	package.loaded[modulename] = nil
	require(modulename)
end

--------------------------------------------------------------------------

function copy_table_function(dst, src)
	for key, value in pairs(src) do
		if type(value) == "function" then
			dst[key] = value
		end
	end
end

function copy_table(dst, src)
	for key,value in pairs(src) do
		if type(value) ~= "table" then
			dst[key] = value
		else
			dst[key] = {}
			copy_table(dst[key], value)
		end
	end
end

--------------------------------------------------------------------------

function script_call(func_name, json_params)
	local func = _G[func_name]
	if not func then
		print('The global function ' .. func_name .. ' is not exist.')
		return
	end
	local result
	if not json_params then
		result = { pcall(func) }
	else
		local params = json.decode(json_params)
		result = { pcall(func, unpack(params)) }
	end
	if not result[1] then
		print('Fail to call the function:' .. func_name)
	end
	table.remove(result, 1)
	if #result > 0 then
		return json.encode(result)
	end
end

--------------------------------------------------------------------------