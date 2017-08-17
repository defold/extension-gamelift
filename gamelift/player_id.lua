local uuid = require "gamelift.internal.uuid"
uuid.seed()

local function generate()
	local is_ios = sys.get_sys_info().system_name == "iPhone OS"
	for _, a in ipairs(sys.get_ifaddrs()) do
		if a.up and a.running and (is_ios and a.name == "en0" or true) then
			return uuid(a.mac)
		end
	end
	return uuid()
end

local player_id = generate()

local M = {}

function M.get()
	return player_id
end


return M
