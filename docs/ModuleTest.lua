

local LuaObjectXXX = {attr = "attr"}
function LuaObjectXXX:OnMouseIn(x, y)
	MsgBox("OnMouseIn" .. tostring(self) .. " x: " .. tostring(x) .. " y: " .. tostring(y) .. tostring(self and self.attr))
end

function LuaObjectXXX:OnMouseMove(x, y)
	MsgBox("OnMouseMove" .. tostring(self) .. " x: " .. tostring(x) .. " y: " .. tostring(y) .. tostring(self and self.attr))
end

function FuncA(x, y)
	MsgBox("Call FuncA")
	local var = 3
	var.a = 3
end

function FuncB(x, y)
	FuncA(x + 1, y + 1)
end

function FuncC(x, y)
	FuncB(x + 2, y + 2)
end

function FuncD(x, y)
	FuncC(x+3, y+3)
end

function FuncE(x, y)
	FuncD(x+4, y+4)
end

function OnMouseIn(x, y)
	-- MsgBox("OnMouseIn"  .. " x: " .. tostring(x) .. " y: " .. tostring(y))
	FuncE(x, y)
end

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

local pObj = nil
function TestLuaBridgeFuncA(self, x, y)
	if not self then
		return "self is nil"
	end
	pObj = self
	
	self:AttachListener("OnMouseIn", LuaObjectXXX.OnMouseIn, LuaObjectXXX)
	self:AttachListener("OnMouseIn", OnMouseIn)
	self:AttachListener("OnMouseMove", LuaObjectXXX.OnMouseMove, LuaObjectXXX)
	
	return self:AddNum(x, y)
end

function TestDelete()
	
end