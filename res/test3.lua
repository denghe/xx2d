-- test unique userdata



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


local tex = xxLoadSharedTexture("res/tree.pkm")
print(tex)
local q = xxQuad():SetTexture( tex )
print(q)
local tex2 = q:GetTexture()
print(tex2)
print(tex == tex2)
