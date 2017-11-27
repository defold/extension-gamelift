#pragma once

#include <dmsdk/sdk.h>
#include <aws/gamelift/server/GameLiftServerAPI.h>
#include "luautils.h"

struct GameLiftWrapper {
    int m_Port;
    struct lua_Listener m_OnStartGameSessionListener;
	struct lua_Listener m_OnProcessTerminateListener;
	struct lua_Listener m_OnHealthCheckListener;
} g_GameLift;
