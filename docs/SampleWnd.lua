
function OnMouseMove(self, x, y)
	
end

function OnMouseEnter(self, x, y)
	if SpdLog ~= nil then
		SpdLog("OnMouseEnter: " .. tostring(self))
	end
end