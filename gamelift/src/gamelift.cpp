// Extension lib defines
#define LIB_NAME "Gamelift"
#define MODULE_NAME "gamelift"

#define DLIB_LOG_DOMAIN "Gamelift"

#define GAMELIFT_USE_STD 1

#include <dmsdk/sdk.h>
#include <aws/gamelift/server/GameLiftServerAPI.h>
#include "gamelift.h"
#include <stdlib.h>
#include <string.h>


static void printStack(lua_State* L) {
	int n = lua_gettop(L);
	for (int i = 1; i <= n; i++)  {
		dmLogInfo("STACK %d %s %s", i, lua_tostring(L, i), luaL_typename(L, i));
	}
}


static void PushTableStringString(lua_State* L, const char* key, const char* value) {
	lua_pushstring(L, value);
	lua_setfield(L, -2, key);
}

static void PushTableStringNumber(lua_State* L, const char* key, int value) {
	lua_pushnumber(L, value);
	lua_setfield(L, -2, key);
}

static void SetListener(struct LuaListener& listener, lua_State* L, int index) {
	luaL_checktype(L, index, LUA_TFUNCTION);
	lua_pushvalue(L, index);
	int cb = dmScript::Ref(L, LUA_REGISTRYINDEX);

	if (listener.m_Callback != LUA_NOREF) {
		dmScript::Unref(listener.m_L, LUA_REGISTRYINDEX, listener.m_Callback);
		dmScript::Unref(listener.m_L, LUA_REGISTRYINDEX, listener.m_Self);
	}

	listener.m_L = dmScript::GetMainThread(L);
	listener.m_Callback = cb;
	dmScript::GetInstance(L);
	listener.m_Self = dmScript::Ref(L, LUA_REGISTRYINDEX);
}

static void PrepareForListenerInvocation(lua_State* L, struct LuaListener& listener) {
	int top = lua_gettop(L);

	// get the function callback from the registry and push it to the top of the stack
	lua_rawgeti(L, LUA_REGISTRYINDEX, listener.m_Callback);
	// get self from registry and push it to the top of the stack
	lua_rawgeti(L, LUA_REGISTRYINDEX, listener.m_Self);
	// push copy of self to top of the stack
	lua_pushvalue(L, -1);
	// set current script instance from top of the stack (and pop it)
	dmScript::SetInstance(L);

	assert(top + 2 == lua_gettop(L));
}

void GameLift_OnStartGameSession(Aws::GameLift::Server::Model::GameSession myGameSession) {
	dmLogInfo("GameLift_OnStartGameSession");
	LuaListener listener = g_GameLift.m_OnStartGameSessionListener;
	lua_State* L = listener.m_L;
	int top = lua_gettop(L);

	PrepareForListenerInvocation(L, listener);

	lua_createtable(L, 0, 7);
	PushTableStringString(L, "name", myGameSession.GetName().c_str());
	PushTableStringString(L, "game_session_id", myGameSession.GetGameSessionId().c_str());
	PushTableStringNumber(L, "maximum_player_session_count", myGameSession.GetMaximumPlayerSessionCount());
	PushTableStringString(L, "ip_address", myGameSession.GetIpAddress().c_str());
	PushTableStringNumber(L, "port", myGameSession.GetPort());
	PushTableStringString(L, "fleet_id", myGameSession.GetFleetId().c_str());

	size_t size = myGameSession.GetGameProperties().size();
	lua_createtable(L, 0, size);
	for(int i=0; i < size; i++) {
		const Aws::GameLift::Server::Model::GameProperty property = myGameSession.GetGameProperties().at(i);
		PushTableStringString(L, property.GetKey().c_str(), property.GetValue().c_str());
	}
	lua_setfield(L, -2, "game_properties");

	int ret = lua_pcall(L, 2, 0, 0);
	if (ret != 0) {
		dmLogError("Error while invoking start game session callback: %s", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	assert(top == lua_gettop(L));
}

void GameLift_OnProcessTerminate() {
	dmLogInfo("GameLift_OnProcessTerminate");
	LuaListener listener = g_GameLift.m_OnProcessTerminateListener;
	lua_State* L = listener.m_L;
	int top = lua_gettop(L);

	PrepareForListenerInvocation(L, listener);
	int ret = lua_pcall(L, 1, 0, 0);
	if (ret != 0) {
		dmLogError("Error while invoking process terminate callback: %s", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	assert(top == lua_gettop(L));
}

bool GameLift_OnHealthCheck() {
	dmLogInfo("GameLift_OnHealthCheck");
	LuaListener listener = g_GameLift.m_OnHealthCheckListener;
	lua_State* L = listener.m_L;
	int top = lua_gettop(L);

	PrepareForListenerInvocation(L, listener);
	int ret = lua_pcall(L, 1, 1, 0);
	if (ret != 0) {
		dmLogError("Error while invoking health check callback: %s", lua_tostring(L, -1));
		lua_pop(L, 1);
		assert(top == lua_gettop(L));
		return 0;
	}
	assert(top + 1 == lua_gettop(L));
	int health = lua_toboolean(L, 1);
	return health;
}


static int InitGamelift(lua_State* L) {
	int top = lua_gettop(L);
	g_GameLift.m_Port = luaL_checknumber(L, 1);
	SetListener(g_GameLift.m_OnStartGameSessionListener, L, 2);
	SetListener(g_GameLift.m_OnProcessTerminateListener, L, 3);
	SetListener(g_GameLift.m_OnHealthCheckListener, L, 4);
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
