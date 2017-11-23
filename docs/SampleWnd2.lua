
function OnMouseMove(self, x, y)
	if MsgBox ~= nil then
		MsgBox(tostring(self))
	end
	return "OnMouseMove return"
end