--------------------------------------------------------------------------
-- unit.lua
-- author : sunjun 
-- email : 4421971@gmail.com
--------------------------------------------------------------------------

Unit = Unit or {}
Unit.__index = Unit

--------------------------------------------------------------------------

function Unit:new()
	local obj = {}
	setmetatable(obj, Unit)
	obj:__init()
	return obj
end

function Unit:__init()	
	self.hp = 10
	self.mp = 20
end

function Unit:setName(name)
end

function Unit:getName()
	print("getName 1111111111111111111111111")	
end

function Unit:increment()
	print("increment +++++++++++++++++++++++++++++++++")
	return 0
end

--------------------------------------------------------------------------