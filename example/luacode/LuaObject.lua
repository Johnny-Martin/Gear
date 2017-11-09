--用lua实现面向对象、简单的消息分发, 不依赖任何C工程
BaseClass = {}

function BaseClass:New(obj)
	obj = obj or {}
	setmetatable(obj, self)
	
	if not obj.listener then
		obj.listener = {}
	end
	
	self.__index = self
	return obj
end

function BaseClass:AddListener(event, callBack, ownerObj, bAddToEnd)
	assert("string" == type(event), "param1 event must be string")
	if not self.listener[event] then
		self.listener[event] = {}
	end
	if nil == bAddToEnd then
		bAddToEnd = true
	end
	--这里不检查同一个callBack是否重复订阅同一个event
	if bAddToEnd then--添加到队列末尾
		table.insert(self.listener[event], {["callBack"] = callBack, ["ownerObj"] = ownerObj})
	else
		table.insert(self.listener[event], 1, {["callBack"] = callBack, ["ownerObj"] = ownerObj})
	end
	
	return callBack
end

function BaseClass:DispatchEvent(event, ...)
	if not self.listener[event] then
		return
	end
	
	for i=1, #self.listener[event] do
		self.listener[event][i]["callBack"](self.listener[event][i]["ownerObj"], event, ...)
	end
end

function BaseClass:RemoveListener(event, callBack, ownerObj)
	if not self.listener[event] then
		return
	end
	
	for i=#self.listener[event], 1, -1 do
		if self.listener[event][i]["callBack"] == callBack and 
			self.listener[event][i]["ownerObj"] == ownerObj then
			
			table.remove(self.listener[event], index)
			--重复订阅的也remove掉
		end
	end
end

function BaseClass:RemoveAllListener()
	self.listener = {}
end

function CreateReadOnlyTable(tbl)
	local fakeTable = {}
	local mt = {}
	mt.__index = tbl
	mt.__newindex = function(t, k, v) 
						MsgBox("attempt to update a read-only table!!!") 
					end
	
	setmetatable(fakeTable, mt)
	return fakeTable
end

BaseObject = BaseClass:New()

--E是一个只读的空表，供safe navigation时使用。例如:
--local ret = root and root.subTable1 and root.subTable1.subTable2 and root.subTable1.subTable2.subTable3
--可以优化成这样:
--local ret = (((root or E).subTable1 or E).subTable2 or E).subTable3
--第一种safe navigation中，root、subTable1被重复写了多次，第二种形式的safe navigation可以避免这种情况
E = CreateReadOnlyTable()