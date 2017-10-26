#pragma once

#include <dmsdk/sdk.h>
#include <aws/gamelift/server/GameLiftServerAPI.h>

struct LuaListener {
    LuaListener() {
        m_L = 0;
        m_Callback = LUA_NOREF;
        m_Self = LUA_NOREF;
    }
    lua_State* m_L;
    int        m_Callback;
    int        m_Self;
};


struct GameLiftWrapper {
    int m_Port;
    struct LuaListener m_OnStartGameSessionListener;
	struct LuaListener m_OnProcessTerminateListener;
	struct LuaListener m_OnHealthCheckListener;
} g_GameLift;
