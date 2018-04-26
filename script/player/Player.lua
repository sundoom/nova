--------------------------------------------------------------------------
-- Player.lua
-- author : sunjun 
-- email : 4421971@gmail.com
--------------------------------------------------------------------------

require("LoginMember")

require("PlayerData")

--------------------------------------------------------------------------

Player = Player or {}
Player.__index = Player

--------------------------------------------------------------------------

copy_table_function(Player, LoginMember)

--------------------------------------------------------------------------

function Player:new()
	local obj = {}
	setmetatable(obj, Player)
	obj:__init()
	return obj
end

function Player:__init()
	self.playerData = PlayerData:new()
end

function Player:enterGame()
	print('==========>>>>>>>>> enterGame')
	self:login()
	self.playerData:getName()
end

function Player:leaveGame()
	print('==========>>>>>>>>> leaveGame')
end

--------------------------------------------------------------------------