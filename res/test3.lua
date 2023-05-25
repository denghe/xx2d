print("test3.lua begin")

-- init search path ( cpp )
xxSearchPathAdd("res", true)

-- sync search path to lua -> package.path
xxSearchPathSync()
print(package.path)

local fnt = xxLoadBMFont("res/font/3500+.fnt")
print(fnt)

local fpsViewer = xxFpsViewer()
print(fpsViewer)

local lbl = xxSimpleLabel()
print(lbl)
lbl:SetText(fnt, "hello")

function Update()
	lbl:Draw()
	fpsViewer:Draw(fnt)
end


print("test3.lua end")
