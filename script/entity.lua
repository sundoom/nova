--------------------------------------------------------------------------
-- entity.lua
-- author : sunjun 
-- email : 4421971@gmail.com
--------------------------------------------------------------------------

module ("entity", package.seeall)

--------------------------------------------------------------------------

class 'Entity'
	
	function Entity:__init()
		self.DepthBits = 124
		self.age = 11
	end
	
	function Entity:__exit()
	end
	
	function Entity:tick()
		print("2222222222222222222222222")
	end
	
	function Entity:sethp(h)
		print(h)
	end

--------------------------------------------------------------------------