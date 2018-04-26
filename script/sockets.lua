--------------------------------------------------------------------------
-- sockets.lua
-- author : sunjun 
-- email : 4421971@gmail.com
--------------------------------------------------------------------------

sockets = sockets or {}

--------------------------------------------------------------------------

function register_socket(socket, obj)
	sockets[socket] = obj
end

function unregister_socket(socket)
	sockets[socket] = nil
end

--------------------------------------------------------------------------

function socket_event_error(socket)
	obj = sockets[socket]
	if obj ~= nil then
		func = getmetatable(obj)['socket_event_error']
		if func ~= nil then
			func(socket)
		end
	end
end

--------------------------------------------------------------------------

function socket_event_write(socket)
	obj = sockets[socket]
	if obj ~= nil then
		func = getmetatable(obj)['socket_event_write']
		if func ~= nil then
			func(socket)
		end
	end
end

--------------------------------------------------------------------------

function socket_event_read(socket)
	obj = sockets[socket]
	if obj ~= nil then
		func = getmetatable(obj)['socket_event_read']
		if func ~= nil then
			func(socket)
		end
	end
end

--------------------------------------------------------------------------