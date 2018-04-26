--------------------------------------------------------------------------
-- actor.lua
-- author : sunjun 
-- email : 4421971@gmail.com
--------------------------------------------------------------------------

require("unit")
require("timer")

--------------------------------------------------------------------------

Actor = Actor or Unit:new()
Actor.__index = Actor

--------------------------------------------------------------------------

function Actor:new(name)
	local obj = {}
	setmetatable(obj, Actor)
	obj:__init(name)
	return obj
end

function Actor:__init(name)
	Unit.__init(self)
	self.name = name
	Timer:create(self, "increment", 10)
end

function Actor:setName(name)
end

--------------------------------------------------------------------------