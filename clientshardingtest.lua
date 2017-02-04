local clientsharding=require 'clientsharding'
print("clientsharding version:" ..clientsharding.version)
local shards1={
	{name="192.168.1.101",weight=1,location="192.168.1.101"},
	{name="192.168.1.102",weight=1,location="192.168.1.102"},
	{name="192.168.1.103",weight=1,location="192.168.1.103"}
	}

local shards2={"192.168.1.101:6379","192.168.1.102:6380"}

local getposition1=clientsharding.init(shards1)
local getposition2=clientsharding.quick_init(shards2)

for i=1,15 do
	local n=100000
	print("id:".."J_"..(n+i).." location:" ..  getposition1("J_"..(n+i)))
end

for i=1,15 do
	local n=100000
	print("id:".."J_"..(n+i).." location:" ..getposition2("J_"..(n+i)))
end

print("test end")



