

-- test package searchers
--[[
function Update(delta) end

print( package )

--for k, v in pairs(package) do
--	print (k, v)
--end

for k, v in pairs(package.searchers) do
	print (k, v)
end

]]


-- test unique userdata
----[[
local tex = xxLoadSharedTexture("res/tree.pkm")
print(tex)
local q = xxQuad():SetTexture( tex )
print(q)
local co = coroutine.create(function()
	local tex2 = q:GetTexture()
	print(tex2)
	print(tex == tex2)
end)
coroutine.resume(co)
--]]
