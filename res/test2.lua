gCoros = {}
yield = coroutine.yield
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
			print(t[i].coroutine_name)
			print(debug.traceback(c))
			error(err)
		end
		if co_status(c) == "dead" then
			table.remove(t, i)
		end
	end
end

go = function(name, func, ...)
	local c = coroutine.create(func)
	local ok, err = co_resume(c, ...)
	if not ok then
		print(debug.traceback(c))
		error(err)
	end
	if co_status(c) == "dead" then
		return
	end
	local t = { coroutine = c, coroutine_name = name }
	table.insert(gCoros, t)
	return t
end


for i = 1, 100000 do
	go("c "..i, function()
		while true do
			yield()
		end
	end)
end

local bt = NowEpochSeconds()
gCorosRun()
print( NowEpochSeconds() - bt )

function Update(delta)
end
