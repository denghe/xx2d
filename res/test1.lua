--[[

-- 测试结果， 一万个 quad, 这样写就有 13xx fps, 协程写法有 3xx fps. 
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

]]






gCoros = {}
yield = coroutine.yield
co_create = coroutine.create
co_resume = coroutine.resume
co_status = coroutine.status

function gCorosRun()
	local t = gCoros
	local n = #t
	if n == 0 then return end
	for i = n, 1, -1 do
		local c = t[i].coroutine
		local ok, err = co_resume(c)
		if not ok then
			print("resume coroutine_name = " .. t[i].coroutine_name)
			print(debug.traceback(c))
			error(err)
		end
		if co_status(c) == "dead" then
			table.remove(t, i)
		end
	end
end

go = function(name, func, ...)
	local c = co_create(func)
	local ctx = { coroutine = c, coroutine_name = name }
	local ok, err = co_resume(c, ctx, ...)
	if not ok then
		print("create coroutine_name = " .. name)
		print(debug.traceback(c))
		error(err)
	end
	if co_status(c) == "dead" then
		return
	end
	table.insert(gCoros, ctx)
	return ctx
end

gTimePool = 0
gDelta = 0.0166666666666667

-- 被 cpp 每帧调用
function Update(delta)
	local tp = gTimePool
	tp = tp + delta
	while tp >= 0.0166666666666667 do
		tp = tp - 0.0166666666666667
		gCorosRun()
	end
	gTimePool = tp

	Draw()
end

local gQuads = {}
local tex = xxLoadSharedTexture("res/tree.pkm")

for j = 1, 10000 do
	
	go("coro "..j, function(ctx)
		local q = xxQuad():SetTexture( tex ):SetPosition( xxRndNext( -500, 500 ), xxRndNext( -300, 300 ) )
		gQuads[q] = 1
		for i = 1, xxRndNext( 1000, 2000 ) do
			q:AddRotate(gDelta)
			yield()
		end
		gQuads[q] = nil
	end)

end

Draw = function()
	for o, _ in pairs(gQuads) do
		o:Draw()
	end
end









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

