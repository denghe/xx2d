print("test3.lua begin")

-- init search path ( cpp )
xxSearchPathAdd("res", true)

-- sync search path to lua -> package.path
xxSearchPathSync()
print(package.path)


-- consts
M_PI = 3.14159265358979323846
frameDelaySeconds = 1 / 60


-- utils

-- for(kv : t) if (f(kv)) del(t)
function Foreach(t, f)
	local n = #t
	if n == 0 then return end
	for i = n, 1, -1 do
		if (f(t[i])) then
			table.remove(t, i)
		end
	end
end


-- Scene metatable
Scene = {}
Scene.__index = Scene
Scene.Make = function()
	local t = {}
	setmetatable(t, Scene)
	return t
end

Scene.Init = function(self)
	self.tex = xxLoadSharedTexture("res/mouse.pkm")
	self.ms = {}
	self.timePool = 0
	self.radians = 0
end

Scene.Update = function(self)
	local fds = frameDelaySeconds
	local tp = self.timePool + xxDelta()
	local ms = self.ms
	while tp >= fds do
		tp = tp - fds

		local r = self.radians
		for i = 1, 50 do
			r = r + 0.005

			local m = Mouse.Make()
			m:Init(self, 0, 0, r)
			table.insert(ms, m)

		end
		self.radians = r

		Foreach(ms, function(m)
			return m:Update()
		end)

	end
	self.timePool = tp

	Foreach(ms, function(m)
		m:Draw()
	end)
end


-- Mouse metatable
Mouse = {}
Mouse.__index = Mouse
Mouse.Make = function()
	local t = {}
	setmetatable(t, Mouse)
	return t
end

Mouse.Init = function(self, owner, posX, posY, radians, scale, colorR, colorG, colorB, colorA)
	scale = scale or 1
	colorR = colorR or 255
	colorG = colorG or 255
	colorB = colorB or 255
	colorA = colorA or 255
	local body = xxQuad()
		:SetTexture(owner.tex)
		:SetPosition(posX, posY)
		:SetScale(scale)
		:SetRotate(-radians + M_PI / 2)
		:SetColor(colorR, colorG, colorB, colorA)
	local r = body:GetRotate()
	self.body = body
	self.baseIncX = math.sin(r) * 2
	self.baseIncY = math.cos(r) * 2
end

Mouse.Update = function(self)
	local body = self.body
	body:AddPosition(self.baseIncX, self.baseIncY)
	local posX, posY = body:GetPosition()
	if (posX * posX > (1800 / 2) * (1800 / 2)
		or posY * posY > (1000 / 2) * (1000 / 2)) then
		return true
	end
end

Mouse.Draw = function(self)
	local body = self.body
	local r,g,b,a = body:GetColor()	-- bak
	body:AddPosition(3, 3):SetColor(255,127,127,127):Draw()	-- shadow
	body:AddPosition(-3, -3):SetColor(r,g,b,a):Draw()
end



-- logic
local fnt = xxLoadBMFont("res/font/3500+.fnt")
local fpsViewer = xxFpsViewer()
local scene = Scene.Make()
scene:Init()

function Update()
	scene:Update()
	fpsViewer:Draw(fnt)
end


print("test3.lua end")

--[[
local lbl = xxSimpleLabel()
print(lbl)
lbl:SetText(fnt, "hello")

lbl:Draw()
]]
