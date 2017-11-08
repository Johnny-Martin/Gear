
-- local SomeObjClass = {}

-- function SomeObjClass:CallBack(eventName, param1, param2)
	-- MsgBox(tostring(eventName) .. tostring(param1) .. tostring(param2))
-- end

-- function AnOtherCallBack(_, eventName, param1, param2, param3, param4)
	-- MsgBox(tostring(eventName) .. tostring(param1) .. tostring(param2) .. tostring(param3) .. tostring(param4))
-- end

function OnLoadLuaFile(a, b)
	-- if ObjectBase then
		-- ObjectBase:AddListener("TestEvent", SomeObjClass.CallBack, SomeObjClass)
		-- ObjectBase:AddListener("TestEvent", AnOtherCallBack)
	-- else
		-- MsgBox("ObjectBase is not defined")
	-- end
	
	return a + b
end