--------------------------------------------------------------------------
-- host.lua
-- author : sunjun 
-- email : 4421971@gmail.com
--------------------------------------------------------------------------

require("sockets")
tcp = require('tcp')

--------------------------------------------------------------------------

Host = Host or {}
Host.__index = Host

--------------------------------------------------------------------------

function Host:new()
	local obj = {}
	setmetatable(obj, Host)
	return obj
end

function Host:open(port)
	self.socket = tcp.open(port)
	print("==========>>>>>>>>>>", "Host:open", self.socket)
	if self.socket ~= nil then
		register_socket(self.socket, self)
	end
	print("==========>>>>>>>>>>", "Host:open")
end

function Host:close()
	if self.socket ~= nil then
		tcp.close(self.socket)
		unregister_socket(self.socket)
	end
end

function Host:socket_event_error(socket)
end

function Host:socket_event_write(socket)
end

function Host:socket_event_read(socket)
	local new_socket
	local new_ip
	new_socket, new_ip = tcp.accept(socket)
	if new_socket ~= nil then
	end
	print("==========>>>>>>>>>>>>", "Host:socket_event_read", new_socket, new_ip)
end

--------------------------------------------------------------------------