local clientsharding=require 'clientsharding'
print("clientsharding version:" ..clientsharding.version)
local shards={
	{name="192.168.1.101",weight=1,location="192.168.1.101"},
	{name="192.168.1.102",weight=1,location="192.168.1.102"},
	{name="192.168.1.103",weight=1,location="192.168.1.103"}
	}

local shards1={"192.168.1.101:6379","192.168.1.102:6380"}

local shardClient1=clientsharding.init(shards)
local shardClient2=clientsharding.quick_init(shards1)

print("second init")
--clientsharding.init(shards)


for i=1,15 do
	local n=100000
	--print("id:".."J_"..(n+i).." location:" ..  shardClient1("J_"..(n+i)))
end

for i=1,15 do
	local n=100000
	print("id:".."J_"..(n+i).." location:" ..shardClient2("J_"..(n+i)))
end
--print(shardClient2("J_100012"))
print("invoke end")



