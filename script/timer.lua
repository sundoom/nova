--------------------------------------------------------------------------
-- timer.lua
-- author : sunjun 
-- email : 4421971@gmail.com
--------------------------------------------------------------------------

Timer = Timer or { id = 0 }

--------------------------------------------------------------------------

function Timer:create(object, funcname, interval)
	local t = {}
	t.interval = interval
	t.object = object
	t.funcname = funcname
	t.time = NSeconds()
	self[self.id] = t
	-- rawset(self, self.id, t);
	self.id = self.id + 1
	return id
end

function Timer:destroy(id)
	self[id] = nil
end

function Timer:clean()
	for id, t in pairs(self) do
		if type(t) == "table" then
			self[id] = nil
		end
	end
end

function Timer:tick()
	for id, t in pairs(self) do
		if type(t) == "table" and t.object ~= nil and t.funcname ~= nil then
			func = getmetatable(t.object)[t.funcname]
			if func ~= nil then
				ret = func()
				if ret == 0 then
					self[id] = nil
				end
			end
		end
	end
end

--------------------------------------------------------------------------