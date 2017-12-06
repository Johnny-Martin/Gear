
local TestLuaObj_ObjA__ = nil
function TestLuaGlobalObj()
	if TestLuaObj_ObjA then
		TestLuaObj_ObjA__ = TestLuaObj_ObjA
	end
	MsgBox(tostring(TestLuaObj_ObjA))
	MsgBox(tostring(TestLuaObj_ObjA__ and TestLuaObj_ObjA__:Minus()))
	
	-- weakTable = {}
	-- weakTable[1] = function() MsgBox("i am the first element") end
	-- weakTable[2] = function() MsgBox("i am the second element") end
	-- weakTable[3] = {10, 20, 30}

	-- setmetatable(weakTable, {__mode = "v"})

	-- MsgBox(tostring(weakTable and #weakTable))

	-- local ele = weakTable[1]
	-- collectgarbage()
	-- MsgBox(tostring(weakTable and #weakTable))

	-- ele = nil
	-- collectgarbage()
	-- MsgBox(tostring(weakTable and #weakTable))

	return "OnMouseMove return"
end	


function TestLuaBridgeFuncA(self, x, y)
	if not self then
		return "self is nil"
	end
	return self:AddNum(x, y)
end