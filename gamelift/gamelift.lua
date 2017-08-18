local gamelift = require "aws-sdk.gamelift"
local aws_config = require "aws-sdk.core.config"

function aws_config.http_request(uri, method, headers, post_data, callback)
	http.request(uri, method, function(_, _, response)
		callback(response)
	end, headers, post_data)
end


local M = {}


function M.list_aliases(args)
	assert(args, "You must provide a table of arguments (can be empty)")
	local input = gamelift.ListAliasesInput(
		args.NextToken,
		args.Limit,
		args.Name,
		args.RoutingStrategyType)
	return gamelift.ListAliasesSync(input)
end

function M.search_game_sessions(args)
	assert(args, "You must provide a table of arguments (can be empty)")
	assert(args.FleetId or args.AliasId, "You must provide either a FleetId or an AliasId")
	local input = gamelift.SearchGameSessionsInput(
		args.FilterExpression,
		args.SortExpression,
		args.FleetId,
		args.Limit,
		args.NextToken,
		args.AliasId)
	return gamelift.SearchGameSessionsSync(input)
end

function M.describe_game_sessions(args)
	assert(args, "You must provide a table of arguments (can be empty)")
	local input = gamelift.DescribeGameSessionsInput(
		args.Limit,
		args.GameSessionId,
		args.StatusFilter,
		args.FleetId,
		args.NextToken,
		args.AliasId)
	return gamelift.DescribeGameSessionsSync(input)
end

function M.create_game_session(args)
	assert(args, "You must provide a table of arguments (can be empty)")
	assert(args.FleetId or args.AliasId, "You must provide either a FleetId or an AliasId")
	local input = gamelift.CreateGameSessionInput(
		args.MaximumPlayerSessionCount,
		args.Name,
		args.GameProperties,
		args.IdempotencyToken,
		args.FleetId,
		args.CreatorId,
		args.GameSessionId,
		args.AliasId)
	return gamelift.CreateGameSessionSync(input)
end


function M.join_game_session(args)
	assert(args, "You must provide a table of arguments (can be empty)")
	local input = gamelift.CreatePlayerSessionInput(
		args.PlayerId,
		args.GameSessionId,
		args.PlayerData)
	return gamelift.CreatePlayerSessionSync(input)
end


return M
