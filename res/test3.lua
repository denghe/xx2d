function Update(delta) end

print( package )

--[[
for k, v in pairs(package) do
	print (k, v)
end
]]

for k, v in pairs(package.searchers) do
	print (k, v)
end

