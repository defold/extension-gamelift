local gamelift = require "aws-sdk.gamelift"


local M = {}


function M.list_aliases(cb, args)
	gamelift.ListAliasesAsync(ListAliasesInput,cb)
end

function M.search_game_sessions(cb, args)
	assert(args, "You must provide a table of arguments (can be empty)")
	assert(args.fleet_id or args.alias_id, "You must provide either a fleet_id or an alias_id")

	local input = gamelift.SearchGameSessionsInput(args.filter, args.sort, args.fleet_id, args.limit, args.next_token, args.alias_id)

	gamelift.SearchGameSessionsAsync(input, function(response, error_message)
		local SearchGameSessionsOutput = response
		local GameSessionList = SearchGameSessionsOutput.GameSessions
		cb(GameSessionList, error_message)
	end)
end


return M