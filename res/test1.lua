
function GetUserValue( ud )
	local t = debug.getuservalue( ud )
	if t == nil then
		t = {}
		debug.setuservalue( ud, t )
	end
	return t
end

local t = xxLoadSharedTexture("res/sword.png")
print(t)

local q = xxQuad()
q:SetTexture( t ):SetScale( 0.5 )
print(q)
print(t == q:GetTexture())

local t = GetUserValue(q)
t.abc = 123
local t2 = GetUserValue(q)
print(t2.abc)


function FixedUpdate(delta)
	q:AddRotate(delta)
end

function Update(delta)
	q:Draw()
end
