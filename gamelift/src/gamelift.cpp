// Extension lib defines
#define LIB_NAME "Gamelift"
#define MODULE_NAME "gamelift"

#define DLIB_LOG_DOMAIN "Gamelift"

#include <dmsdk/sdk.h>
#include <aws/gamelift/server/GameLiftServerAPI.h>
#include "gamelift.h"
#include <stdlib.h>
#include <string.h>


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

	// get the function callback from the registry to the top of the stack
	lua_rawgeti(L, LUA_REGISTRYINDEX, listener.m_Callback);
	// get self from registry to top of the stack
	lua_rawgeti(L, LUA_REGISTRYINDEX, listener.m_Self);
	// push copy of self to top of the stack
	lua_pushvalue(L, -1);
	// set current script instance from top of the stack (and pop it)
	dmScript::SetInstance(L);

	assert(top + 1 == lua_gettop(L));
}



void GameLiftWrapper::OnStartGameSession(Aws::GameLift::Server::Model::GameSession myGameSession) {
	GameLiftWrapper *instance = GameLiftWrapper::GetInstance();
	LuaListener listener = instance->OnHealthCheckListener;
	lua_State* L = listener.m_L;
	int top = lua_gettop(L);

	PrepareForListenerInvocation(L, listener);
	int ret = lua_pcall(L, 1, 0, 0);
	if (ret != 0) {
		dmLogError("Error while invoking start game session callback: %s", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	assert(top == lua_gettop(L));
}

void GameLiftWrapper::OnProcessTerminate() {
	GameLiftWrapper *instance = GameLiftWrapper::GetInstance();
	LuaListener listener = instance->OnHealthCheckListener;
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

bool GameLiftWrapper::OnHealthCheck() {
	GameLiftWrapper *instance = GameLiftWrapper::GetInstance();
	LuaListener listener = instance->OnHealthCheckListener;
	lua_State* L = listener.m_L;
	int top = lua_gettop(L);

	PrepareForListenerInvocation(L, listener);
	int ret = lua_pcall(L, 1, 1, 0);
	if (ret != 0) {
		dmLogError("Error while invoking health check callback: %s", lua_tostring(L, -1));
		lua_pop(L, 1);
		return 0;
	}
	int health = lua_toboolean(L, 1);
	assert(top == lua_gettop(L));
	return health;
}


void GameLiftWrapper::Init(int port) {
	auto processReadyParameter = Aws::GameLift::Server::ProcessParameters(
		std::bind(&GameLiftWrapper::OnStartGameSession, this, std::placeholders::_1),
		std::bind(&GameLiftWrapper::OnProcessTerminate, this),
		std::bind(&GameLiftWrapper::OnHealthCheck, this),
		port,
		Aws::GameLift::Server::LogParameters()
	);

	Aws::GameLift::Server::ProcessReady(processReadyParameter);
}

GameLiftWrapper* GameLiftWrapper::GetInstance() {
    if (!s_Instance) {
        s_Instance = new GameLiftWrapper();
    }
    return s_Instance;
}


static int InitGamelift(lua_State* L) {
    int top = lua_gettop(L);
    int port = luaL_checknumber(L, 1);


    GameLiftWrapper *instance = GameLiftWrapper::GetInstance();
	SetListener(instance->OnStartGameSessionListener, L, 2);
	SetListener(instance->OnProcessTerminateListener, L, 3);
	SetListener(instance->OnHealthCheckListener, L, 4);
    instance->Init(port);

    assert(top == lua_gettop(L));
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
    }
    else {
        lua_pushboolean(L, 1);
        lua_pushnil(L);
    }
    assert(top + 2 == lua_gettop(L));
    return 2;
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
