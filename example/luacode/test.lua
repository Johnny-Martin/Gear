
function err(msg)
	MsgBox(msg)
end

function OnLoadLuaFile(a, b)
	if BaseObject then
		BaseObject:DispatchEvent("TestEvent", 1, 5, 6)
	else
		MsgBox("BaseObject is not defined")
	end
	
	return a + b
end