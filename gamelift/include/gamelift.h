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


class GameLiftWrapper {


	static GameLiftWrapper* s_Instance;

public:
    struct LuaListener OnStartGameSessionListener;
	struct LuaListener OnProcessTerminateListener;
	struct LuaListener OnHealthCheckListener;

	static GameLiftWrapper* GetInstance();
	void Init(int port);
	void OnStartGameSession(Aws::GameLift::Server::Model::GameSession myGameSession);
	void OnProcessTerminate();
	bool OnHealthCheck();


};
