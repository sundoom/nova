--------------------------------------------------------------------------
-- PlayerData.lua
-- author : sunjun 
-- email : 4421971@gmail.com
--------------------------------------------------------------------------

PlayerData = PlayerData or {}
PlayerData.__index = PlayerData

--------------------------------------------------------------------------

function PlayerData:new()
	local obj = {}
	setmetatable(obj, PlayerData)
	obj:__init()
	return obj
end

function PlayerData:__init()
	self.hp = 10
	self.mp = 20
end

function PlayerData:setName(name)
end

function PlayerData:getName()
	print("getName 1111111111111111111111111")	
end

--------------------------------------------------------------------------