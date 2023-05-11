-- 测试结果， 一万个 quad, 这样写就有一千多 fps, 协程写法只有 2 fps. 
-- 测试结果， 10 万个 quad, 33 fps, c++ 协程写法 122 fps

gTimePool = 0
gDelta = 0.0166666666666667

-- 被 cpp 每帧调用
function Update(delta)
	local tp = gTimePool
	tp = tp + delta
	while tp >= 0.0166666666666667 do
		tp = tp - 0.0166666666666667
		Logic()
	end
	gTimePool = tp

	if Draw then
		Draw()
	end
end


local gQuads = {}
local tex = xxLoadSharedTexture("res/tree.pkm")

for j = 1, 100000 do
	
	local ctx = {}
	ctx[1] = xxQuad():SetTexture( tex ):SetPosition( xxRndNext( -500, 500 ), xxRndNext( -300, 300 ) )
	ctx[2] = 0
	ctx[3] = xxRndNext( 1000, 2000 )
	gQuads[j] = ctx

end

Logic = function()

	local t = gQuads
	local n = #t
	if n == 0 then return end
	for i = n, 1, -1 do
		local c = t[i]
		c[1]:AddRotate(gDelta)
		c[2] = c[2] + 1
		if c[2] >= c[3] then
			table.remove(t, i)
		end
	end

end

Draw = function()
	for j, c in pairs(gQuads) do
		c[1]:Draw()
	end
end



--[[

-- 为了便于使用
yield = coroutine.yield																			

__G__TRACKBACK__ = function(msg)
    local msg = debug.traceback(msg, 3)
    print(msg)
    return msg
end

-- 全局协程池( 经测试发现，lua 协程 resume 开销非常巨大， 一万个就会降到 2 fps. 所以在需要性能的逻辑里就不要上协程了 )
local gCoros = {}																				

-- 压入一个协程函数. 有参数就跟在后面. 有延迟执行的效果. 报错时带 name 显示
-- 参数传入  func + args...  或  name + func + args...   或  ctx{ name, ... } + func + args...
go = function(...)
	local args = {...}
	local t1 = type(args[1])
	local t2 = type(args[2])
	assert( #args > 0 and ( t1 == 'function' or t2 == 'function') )

	local ctx
	local func
	if t1 ~= 'function' then
		if t1 == 'string' then
			ctx = { name = args[1] }
		else
			ctx = args[1]
		end
		func = args[2]
		table.remove(args, 1)
	else
		ctx = { name = "" }
		func = args[1]
	end
	table.remove(args, 1)

	ctx[1] = coroutine.create(function() xpcall(func, __G__TRACKBACK__, ctx) end)
	table.insert(gCoros, ctx)

	return ctx
end

--- 通过名称获取协程上下文
---@param name  string
---@return table
goContext = function(name)
	for k, v in pairs(gCoros) do
		if v.name == name then
			return v
		end
	end
end

-- 睡指定秒( 保底睡3帧 )( coro )
SleepSecs = function(secs)
	local timeout = NowEpochMS() + secs * 1000
	yield()
	yield()
	yield()
	while timeout > NowEpochMS() do 
		yield()
	end
end

-- 固定帧率被 Update 调用, 执行所有 coros
GlobalUpdate = function()
	local cr = coroutine.resume

	if gCoro then
		if not cr(gCoro) then
			gCoro = nil
		end
	end

	local t = gCoros
	local n = #t
	if n == 0 then return end
	for i = n, 1, -1 do
		if not cr(t[i][1]) then
			table.remove(t, i)
		end
	end
end

gTimePool = 0
gDelta = 0.0166666666666667

-- 被 cpp 每帧调用
function Update(delta)
	local tp = gTimePool
	tp = tp + delta
	while tp >= 0.0166666666666667 do
		tp = tp - 0.0166666666666667
		GlobalUpdate()
	end
	gTimePool = tp

	if Draw then
		Draw()
	end
end


local gQuads = {}
local tex = xxLoadSharedTexture("res/tree.pkm")

for j = 1, 10000 do
	
	go("coro "..j, function(c)
		--print( c.name .. " begin" )
		local q = xxQuad():SetTexture( tex ):SetPosition( xxRndNext( -500, 500 ), xxRndNext( -300, 300 ) )
		gQuads[q] = 1
		for i = 1, xxRndNext( 1000, 2000 ) do
			q:AddRotate(gDelta)
			yield()
		end
		gQuads[q] = nil
		--print( c.name .. " end" )
	end)

end

Draw = function()
	for o, _ in pairs(gQuads) do
		o:Draw()
	end
end

]]







--[[


local tex = xxLoadSharedTexture("res/tree.pkm")

local q = xxQuad()
q:SetTexture( tex )

function FixedUpdate(delta)
	q:AddRotate(delta)
end

function Update(delta)
	q:Draw()
end


-- test user value for userdata

function GetUserValue( ud )
	local t = debug.getuservalue( ud )
	if t == nil then
		t = {}
		debug.setuservalue( ud, t )
	end
	return t
end

local t = xxLoadSharedTexture("res/tree.pkm")
print(t)

local q = xxQuad()
q:SetTexture( t )
print(q)
print(t == q:GetTexture())

local t = GetUserValue(q)
t.abc = 123
local t2 = GetUserValue(q)
print(t2.abc)

]]

