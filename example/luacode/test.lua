local BaseObject = GetGlobal("BaseObject")

function err(msg)
	MsgBox(msg)
end

function OnLoadLuaFile(a, b)
	-- TestGlobalFunction()
	
	
	if BaseObject then
		BaseObject:DispatchEvent("TestEvent", 1, 5, 6)
	else
		MsgBox("BaseObject is not defined")
	end
	
	local ret = "_ENV: " .. tostring(_ENV) .. "\n"
	for key, value in pairs(_ENV) do
		ret = ret .. "key: " .. key .. " "
		ret = ret .. "value: " .. tostring(value)
		ret = ret .. "\n"
	end
	MsgBox(ret)
	
	ret =  "_G: " .. tostring(_G) .. "\n"
	for key, value in pairs(_G) do
		ret = ret .. "key: " .. key .. " "
		ret = ret .. "value: " .. tostring(value)
		ret = ret .. "\n"
	end
	MsgBox(ret)
	
	return a + b
end