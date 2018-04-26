--------------------------------------------------------------------------
-- rpc.lua
-- author : sunjun 
-- email : 4421971@gmail.com
--------------------------------------------------------------------------

json = require('json')

--------------------------------------------------------------------------

function rpc_call(sid, method, ...)
	local call_cmd = {
		method = method,
		params = arg
	}
	local json_cmd = json.encode(call_cmd)
	rpc_send(sid, json_cmd)
end

--------------------------------------------------------------------------

function rpc_event(json_cmd)
	local call_cmd = json.decode(json_cmd)
	local method = _G[call_cmd.method]
	if not method then
		print('rpc method ' .. call_cmd.method .. ' is not exist at this server.')
		return
	end
	local result = pcall(method, unpack(call_cmd.params))
	if not result then
		print('Fail to call the rpc method: ' .. call_cmd.method)
	end
end

--------------------------------------------------------------------------