print("test3.lua begin")

-- init search path ( cpp )
xxSearchPathAdd("res", true)

-- sync search path to lua -> package.path
xxSearchPathSync()
print(package.path)

-- frame delay const declare
local frameDelaySeconds = 1 / 60

-- Scene metatable
local Scene = {}
Scene.__index = Scene
Scene.Make = function()
	local t = {}
	setmetatable(t, Scene)
	return t
end

Scene.Init = function()
	self.tex = xxLoadSharedTexture("res/mouse.pkm")
	self.ms = {}
	self.timePool = 0
	self.radians = 0
end

Scene.Update = function()
	local fds = frameDelaySeconds
	local tp = self.timePool + xxDelta()
	local ms = self.ms
	while tp >= fds do
		tp = tp - fds

		local r = self.radians
		for i = 1, 100 do
			r = r + 0.005
			local m = Mouse.Make()
			m.Init(self, 0, 0, r, 1)
			table.insert(ms, m)
		end
		self.radians = r

	end
	self.timePool = tp
end


-- Mouse metatable
local Mouse = {}
Mouse.__index = Mouse
Mouse.Make = function()
	local t = {}
	setmetatable(t, Mouse)
	return t
end

Mouse.Init = function(............)
end

Mouse.Update = function()
end


local fnt = xxLoadBMFont("res/font/3500+.fnt")
print(fnt)

local fpsViewer = xxFpsViewer()
print(fpsViewer)

local lbl = xxSimpleLabel()
print(lbl)
lbl:SetText(fnt, "hello")



local Mouse = {}
Mouse.Init = function()
	
end

function Update()
	lbl:Draw()
	fpsViewer:Draw(fnt)
end


print("test3.lua end")
