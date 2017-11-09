
local SomeObjClass = {}

function SomeObjClass:CallBack(eventName, param1, param2)
	MsgBox(tostring(eventName) .. tostring(param1) .. tostring(param2))
end

function AnOtherCallBack(_, eventName, param1, param2, param3, param4)
	MsgBox(tostring(eventName) .. tostring(param1) .. tostring(param2) .. tostring(param3) .. tostring(param4))
end

function OnLoadLuaFile(a, b)
	if BaseObject then
		BaseObject:AddListener("TestEvent", SomeObjClass.CallBack, SomeObjClass)
		BaseObject:AddListener("TestEvent", AnOtherCallBack)
	else
		MsgBox("BaseObject is not defined")
	end
	
	return a + b
end