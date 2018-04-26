--------------------------------------------------------------------------
-- main.lua
-- author : sunjun 
-- email : 4421971@gmail.com
--------------------------------------------------------------------------

-- package.path = package.path..';script/?.lua'
package.path = package.path..';?.lua;data/?.lua;lib/?.lua;lib/json/?.lua;'..
'player/?.lua;player/data/?.lua;player/member/?.lua;'
-- ?.lua 根目录下的lua文件
-- data/?.lua 根目录下data目录里的lua文件
-- 这里把所有的文件目录都包含进来

--------------------------------------------------------------------------

require("helpers")
require("item_data")
require("timer")

json = require('json')

require('rpc')

--------------------------------------------------------------------------

--[[
	package.cpath = package.cpath..';c:/lang/lua/?.dll'
	collectgarbage()
--]]

--------------------------------------------------------------------------

--------------------------------------------------------------------------

--[[
-- 测试reload
function reload_module(modulename)
  if package.loaded[modulename] then
    print(string.format("reload module[%s]", modulename))
  end
  package.loaded[modulename] = nil
  require(modulename)
end

require("player")
ent = Player()
ent:sethp(412)
ent:tick()
player_reset()
reload_module("player")
ent:tick()
ent = nil
--]]

--------------------------------------------------------------------------


--[[
require("unit")

u1 = Unit:new()
u1.hp = 10
u1:set_name("aa")
os.execute("pause")
reload_module("unit")
u1:set_name("aa")
u2 = Unit:new()
print(u2.hp)
--]]

--------------------------------------------------------------------------
--[[
-- 测试类派生、热更新、tick热更新
require("actor")

a1 = Actor:new("sun")
a2 = Actor:new("jun")
a2:getName()
os.execute("pause")
reload_module("unit")
-- func = getmetatable(a2)["getName"]
-- func()

--------------------------------------------------------------------------



--------------------------------------------------------------------------

os.execute("pause")

-- 打印全局表
print("打印全局表:", _G)

print(item_data)

-- 测试json
jsonTest = json.encode(item_data)
print(jsonTest)
--]]


--------------------------------------------------------------------------
--[[
-- 测试rpc

function test_func1(a, b, c, d)
	print("-----------------------", "test_func1", a, b, c, d)
end

rpc_call(0, "test_func1", 10, 20, 30, 40)
--]]
--------------------------------------------------------------------------

--[[
-- 测试socket
require("host")
function init_socket()
	h = Host:new()
	h:open(6600)
	h:close()
end

init_socket()
--]]

--------------------------------------------------------------------------

-- 测试
require("Player")
player1 = Player:new()
player1:enterGame()
player1:leaveGame()

--------------------------------------------------------------------------

function tick()
	print("-----------------------", "tick")
end

--------------------------------------------------------------------------
