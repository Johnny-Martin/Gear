function Func()
	print("selene")
	return 5
end

function add(a, b)
	local btn = Button.new();
	local ret = btn:SetEnable(false)
	print("selene SetEnable " .. ret)
	return a + b + my_multiply(a, b)
end