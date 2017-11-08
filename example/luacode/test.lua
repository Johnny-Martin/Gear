
function err(msg)
	MsgBox(msg)
end

function OnLoadLuaFile(a, b)
	if ObjectBase then
		ObjectBase:DispatchEvent("TestEvent", 1, 5, 6)
	else
		MsgBox("ObjectBase is not defined")
	end
	
	return a + b
end