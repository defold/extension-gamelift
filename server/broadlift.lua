local broadsock = require "broadsock.server.broadsock"
local stream = require "broadsock.util.stream"


local handle_client_connected = broadsock.handle_client_connected
local handle_client_disconnected = broadsock.handle_client_disconnected
local handle_client_message = broadsock.handle_client_message
local start = broadsock.start

local M = broadsock


local function log(...)
	print("[BROADLIFT]", ...)
end


local function tomessage(str)
	return stream.number_to_int32(#str) .. str
end


local function on_health_check(self)
	log("on_health_check")
	return true
end

local function on_start_game_session(self, session)
	log("on_start_game_session")
	pprint(session)
	gamelift.activate_game_session()
end

local function on_process_terminate(self)
	log("on_process_terminate")
	M.terminate_game_session(0xDEAD)
end


function M.terminate_game_session(code)
	log("terminate_game_session")
	gamelift.terminate_game_session()
	gamelift.process_ending()
	os.exit(code)
end

function M.handle_client_disconnected(client)
	log("handle_client_disconnected")
	gamelift.remove_player_session(client.data)
	handle_client_disconnected(client)
	if broadsock.client_count() == 0 then
		M.terminate_game_session(0xDEAD)
	end
end

function M.handle_client_message(client, message)
	log("handle_client_message")
	local reader = stream.reader(data, data_length)
	local from_uid = reader.number()
	local msg_id = reader.string()
	if msg_id == "GL_CLAIM_PLAYER_SESSION" then
		client.data = reader.string()
		local ok, err = gamelift.accept_player_session(client.data)
		if not ok then
			local writer = stream.writer()
			writer.number(client.uid)
			writer.string("GL_CLAIM_PLAYER_SESSION_FAILED")
			broadsock.send_message(client, tomessage(writer.tostring()))
		else
			local writer = stream.writer()
			writer.number(client.uid)
			writer.string("GL_CLAIM_PLAYER_SESSION_SUCCESS")
			broadsock.send_message(client, tomessage(writer.tostring()))
		end
	else
		handle_client_message(client, message)
	end
end

function M.start(port)
	log("start", port)
	start(port)
	gamelift.init(port, on_start_game_session, on_process_terminate, on_health_check)
end


return M
