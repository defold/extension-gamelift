// Extension lib defines
#define LIB_NAME "Gamelift"
#define MODULE_NAME "gamelift"

#define DLIB_LOG_DOMAIN "Gamelift"

#define GAMELIFT_USE_STD 1

#include <dmsdk/sdk.h>
#include <aws/gamelift/server/GameLiftServerAPI.h>
#include "gamelift.h"
#include "luautils.h"
#include <stdlib.h>
#include <string.h>


void GameLift_OnStartGameSession(Aws::GameLift::Server::Model::GameSession myGameSession) {
	dmLogInfo("GameLift_OnStartGameSession");
	lua_State* L = g_GameLift.m_OnStartGameSessionListener.m_L;
	int top = lua_gettop(L);

	lua_pushlistener(L, g_GameLift.m_OnStartGameSessionListener);

	lua_createtable(L, 0, 7);
	lua_pushtablestringstring(L, "name", myGameSession.GetName().c_str());
	lua_pushtablestringstring(L, "game_session_id", myGameSession.GetGameSessionId().c_str());
	lua_pushtablestringnumber(L, "maximum_player_session_count", myGameSession.GetMaximumPlayerSessionCount());
	lua_pushtablestringstring(L, "ip_address", myGameSession.GetIpAddress().c_str());
	lua_pushtablestringnumber(L, "port", myGameSession.GetPort());
	lua_pushtablestringstring(L, "fleet_id", myGameSession.GetFleetId().c_str());

	size_t size = myGameSession.GetGameProperties().size();
	lua_createtable(L, 0, size);
	for(int i=0; i < size; i++) {
		const Aws::GameLift::Server::Model::GameProperty property = myGameSession.GetGameProperties().at(i);
		lua_pushtablestringstring(L, property.GetKey().c_str(), property.GetValue().c_str());
	}
	lua_setfield(L, -2, "game_properties");

	int ret = lua_pcall(L, 2, 0, 0);
	if (ret != 0) {
		dmLogError("Error while invoking start game session callback: %s", lua_tostring(L, -1));
		lua_pop(L, 1); // pop error message
	}
	assert(top == lua_gettop(L));
}

void GameLift_OnProcessTerminate() {
	dmLogInfo("GameLift_OnProcessTerminate");
	lua_State* L = g_GameLift.m_OnProcessTerminateListener.m_L;
	int top = lua_gettop(L);

	lua_pushlistener(L, g_GameLift.m_OnProcessTerminateListener);
	int ret = lua_pcall(L, 1, 0, 0);
	if (ret != 0) {
		dmLogError("Error while invoking process terminate callback: %s", lua_tostring(L, -1));
		lua_pop(L, 1); // pop error message
	}
	assert(top == lua_gettop(L));
}

bool GameLift_OnHealthCheck() {
	dmLogInfo("GameLift_OnHealthCheck");
	lua_State* L = g_GameLift.m_OnHealthCheckListener.m_L;
	int top = lua_gettop(L);

	lua_pushlistener(L, g_GameLift.m_OnHealthCheckListener);

	int health = 0;
	int ret = lua_pcall(L, 1, 1, 0);
	if (ret != 0) {
		dmLogError("Error while invoking health check callback: %s", lua_tostring(L, -1));
		lua_pop(L, 1); // pop error message
	}
	else {
		health = lua_toboolean(L, 1);
	}
	lua_pop(L, 1); // pop health check boolean
	assert(top == lua_gettop(L));
	return health;
}


static int InitGamelift(lua_State* L) {
	dmLogInfo("InitGameLift");
	int top = lua_gettop(L);
	g_GameLift.m_Port = luaL_checknumber(L, 1);

	luaL_checklistener(L, 2, g_GameLift.m_OnStartGameSessionListener);
	luaL_checklistener(L, 3, g_GameLift.m_OnProcessTerminateListener);
	luaL_checklistener(L, 4, g_GameLift.m_OnHealthCheckListener);
	dmLogInfo("InitGameLift %d", g_GameLift.m_Port);

	auto initOutcome = Aws::GameLift::Server::InitSDK();
	if (!initOutcome.IsSuccess()) {
		dmLogError("Unable to initialize GameLift SDK %s", initOutcome.GetError().GetErrorMessage().c_str());
		lua_pushboolean(L, 0);
		lua_pushstring(L, initOutcome.GetError().GetErrorMessage().c_str());
		assert(top + 2 == lua_gettop(L));
		return false;
	}

	auto processReadyParameter = Aws::GameLift::Server::ProcessParameters(
		std::bind(&GameLift_OnStartGameSession, std::placeholders::_1),
		std::bind(&GameLift_OnProcessTerminate),
		std::bind(&GameLift_OnHealthCheck),
		g_GameLift.m_Port,
		Aws::GameLift::Server::LogParameters()
	);
	auto readyOutcome = Aws::GameLift::Server::ProcessReady(processReadyParameter);
	if (!readyOutcome.IsSuccess()) {
		dmLogError("Unable to initialize GameLift SDK %s", readyOutcome.GetError().GetErrorMessage().c_str());
		lua_pushboolean(L, 0);
		lua_pushstring(L, initOutcome.GetError().GetErrorMessage().c_str());
		assert(top + 2 == lua_gettop(L));
		return false;
	}

	lua_pushboolean(L, 1);
	dmLogInfo("InitGameLift - end");
	assert(top + 1 == lua_gettop(L));
	return 0;
}

static int ActivateGameSession(lua_State* L) {
	int top = lua_gettop(L);
	Aws::GameLift::Server::ActivateGameSession();
	assert(top == lua_gettop(L));
	return 0;
}

static int TerminateGameSession(lua_State* L) {
	int top = lua_gettop(L);
	Aws::GameLift::Server::TerminateGameSession();
	assert(top == lua_gettop(L));
	return 0;
}

static int ProcessEnding(lua_State* L) {
	int top = lua_gettop(L);
	Aws::GameLift::Server::ProcessEnding();
	assert(top == lua_gettop(L));
	return 0;
}

static int RemovePlayerSession(lua_State* L) {
	int top = lua_gettop(L);
	const char* playerSessionId = luaL_checkstring(L, 1);
	Aws::GameLift::Server::RemovePlayerSession(playerSessionId);
	assert(top == lua_gettop(L));
	return 0;
}

static int AcceptPlayerSession(lua_State* L) {
	int top = lua_gettop(L);
	const char* playerSessionId = luaL_checkstring(L, 1);
	auto outcome = Aws::GameLift::Server::AcceptPlayerSession(playerSessionId);
	if (!outcome.IsSuccess()) {
		lua_pushboolean(L, 0);
		lua_pushstring(L, outcome.GetError().GetErrorMessage().c_str());
		assert(top + 2 == lua_gettop(L));
		return 2;
	}
	lua_pushboolean(L, 1);
	assert(top + 1 == lua_gettop(L));
	return 1;
}


// Functions exposed to Lua
static const luaL_reg Module_methods[] = {
	{"init", InitGamelift},
	{"activate_game_session", ActivateGameSession},
	{"terminate_game_session", TerminateGameSession},
	{"process_ending", ProcessEnding},
	{"remove_player_session", RemovePlayerSession},
	{"accept_player_session", AcceptPlayerSession},
	{0, 0}
};

static void LuaInit(lua_State* L) {
	int top = lua_gettop(L);

	// Register lua names
	luaL_register(L, MODULE_NAME, Module_methods);

	lua_pop(L, 1);
	assert(top == lua_gettop(L));
}

dmExtension::Result AppInitializeGameliftExtension(dmExtension::AppParams* params) {
	return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeGameliftExtension(dmExtension::Params* params) {
	// Init Lua
	LuaInit(params->m_L);
	printf("Registered %s Extension\n", MODULE_NAME);
	return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeGameliftExtension(dmExtension::AppParams* params) {
	return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeGameliftExtension(dmExtension::Params* params) {
	return dmExtension::RESULT_OK;
}


// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

DM_DECLARE_EXTENSION(Gamelift, LIB_NAME, AppInitializeGameliftExtension, AppFinalizeGameliftExtension, InitializeGameliftExtension, 0, 0, FinalizeGameliftExtension)
