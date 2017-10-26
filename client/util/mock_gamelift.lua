local gamelift = require "aws-sdk.gamelift"

local M = {
	fleet_id = "fleet-id",
	alias_id = "alias-id",
	ip = "127.0.0.1",
	port = 5000,
}



local function generate_player_id(name)
	return hash_to_hex(hash(name))
end


local function generate_player()
	local first = {
		"Sven", "Mathias", "Andreas", "Ragnar", "Christian",
		"Mats", "Mikael", "Johan", "Oleg", "Bjorn",
		"Erik", "Benjamin", "Sara",
	}
	local last = {
		"Andersson", "Westerlund", "Tadic", "Dahlen", "Svensson", "Murray",
		"Gisselsson", "Saker", "Beck-Noren", "Pridiuk", "Ritzl",
		"Angelin", "Glaser",
	}	
	
	local name = first[math.random(#first)] .. " " .. last[math.random(#last)]
	local id = generate_player_id(name)
	return { name =  name, id = id }
end

local game_session_id_sequence = 0
local player_session_id_sequence = 0

local function create_game_session(args)
	game_session_id_sequence = game_session_id_sequence + 1
	return gamelift.GameSession({
				Status = args.Status or "ACTIVE",
				MaximumPlayerSessionCount = args.MaximumPlayerSessionCount or 10,
				Name = args.Name or "Foobar",
				CurrentPlayerSessionCount = args.CurrentPlayerSessionCount or math.random(1,10),
				TerminationTime = args.TerminationTime or nil,
				GameProperties = args.GameProperties or gamelift.GamePropertyList({}),
				CreationTime = args.CreationTime or gamelift.Timestamp(os.date(os.time() - math.random(2000, 4000))),
				PlayerSessionCreationPolicy = args.PlayerSessionCreationPolicy or nil,
				FleetId = args.FleetId or M.fleet_id,
				CreatorId = args.CreatorId or nil,
				GameSessionId = args.GameSessionId or "game_session_" .. tostring(game_session_id_sequence),
				IpAddress = args.IpAddress or M.ip,
				Port = args.Port or M.port
			}).all
end
		
		
local function create_player_session(args)
	assert(args.GameSessionId, "You must provide a GameSessionId")
	assert(args.PlayerId, "You must provide a PlayerId")
	player_session_id_sequence = player_session_id_sequence + 1
	return gamelift.PlayerSession({
		Status = args.Status or "RESERVED",
		PlayerId = args.PlayerId,
		TerminationTime = args.TerminationTime or nil,
		CreationTime = args.CreationTime or os.date(),
		PlayerData = args.PlayerData or nil,
		PlayerSessionId = args.PlayerSessionId or "player_session" .. tostring(player_session_id_sequence),
		GameSessionId = args.GameSessionId,
		FleetId = args.FleetId or M.fleet_id,
		IpAddress = args.IpAddress or M.ip,
		Port = args.Port or M.port
	}).all
end

function M.mock()
	local game_sessions = {}
	
	local alias_to_fleet = {
		[M.fleet_id] = M.alias_id,
	}
	
	for i=1,6 do
		local player = generate_player()
		local game_session = create_game_session({
			MaximumPlayerSessionCount = 10,
			Name = player.name .. "'s game",
			CreatorId = player.id,
		})
		table.insert(game_sessions, game_session)
	end


	local function find_game_session(id)
		for _,session in pairs(game_sessions) do
			if session.GameSessionId == id then
				return session
			end
		end
		return nil
	end

	function gamelift.SearchGameSessionsAsync(input, cb)
		local output = {
			GameSessions = game_sessions
		}
		timer.seconds(0.1, function()
			cb(output)
		end)
	end

	function gamelift.ListAliasesAsync(input, cb)
		local output = {
			Aliases = {
				gamelift.Alias({
						Name = "_Name",
						AliasArn = "_AliasArn",
						CreationTime = os.date(os.time() - 1000), -- created
						LastUpdatedTime = os.date(), -- updated
						RoutingStrategy = gamelift.RoutingStrategy({ FleetId = M.fleet_id, Message = "_Message", Type = "_Type" }).all,
						AliasId = M.alias_id,
						Description = "_Description"
					}).all
			}
		}
		timer.seconds(0.1, function()
			cb(output)
		end)
	end

	function gamelift.CreateGameSessionAsync(input, cb)
		local game_session = create_game_session({
			Status = "ACTIVATING",
			MaximumPlayerSessionCount = input.MaximumPlayerSessionCount,
			CurrentPlayerSessionCount = 0,
			GameProperties = input.GamePropertyList,
			CreationTime = os.date(),
			Name = input.Name,
			PlayerSessionCreationPolicy = "ACCEPT_ALL",
			FleetId = input.FleetId or alias_to_fleet[input.AliasId],
			input.CreatorId,
		})
		table.insert(game_sessions, game_session)
		local output = {
			GameSession = game_session
		}
		timer.seconds(0.1, function()
			cb(output)
		end)
	end

	function gamelift.DescribeGameSessionsAsync(input, cb)
		local game_session = find_game_session(input.GameSessionId)
		assert(game_session, "Unable to find game session")
		if game_session.Status == "ACTIVATING" then
			game_session.Status = "ACTIVE"
		end
		local output = {
			GameSessions = {
				game_session
			}
		}
		timer.seconds(0.1, function()
			cb(output)
		end)
	end
	
	
	function gamelift.CreatePlayerSessionAsync(input, cb)
		local game_session = find_game_session(input.all.GameSessionId)
		assert(game_session, "Unable to find game session")
		local output = {
			PlayerSession = create_player_session({
				GameSessionId = input.all.GameSessionId,
				FleetId = game_session.FleetId,
				PlayerId = input.all.PlayerId,
				PlayerData = input.all.PlayerData
			})
		}
		timer.seconds(0.1, function()
			cb(output)
		end)
	end
end




return M