# Defold GameLift
Amazon GameLift extension for the Defold game engine. Run Defold on the server with GameLift SDK support!

## Installation
You can use the GameLift extension in your own project by adding this project as a [Defold library dependency](http://www.defold.com/manuals/libraries/). Open your game.project file and in the dependencies field under project add:

	https://github.com/defold/extension-gamelift/archive/master.zip

Or point to the ZIP file of a [specific release](https://github.com/defold/extension-gamelift/releases).

## Usage
Once you have the extension added to your project you can proceed to write your server and interface with the Amazon GameLift server SDK. The following functions are exposed to Lua via the extension:

#### gamelift.init(port, on_start_game_session_listener, on_process_terminate_listener, on_health_check_listener)
Initialize the extension and the GameLift Server SDK. This results in GameLift Server SDK calls to InitSDK().

**PARAMETERS**
* ```port``` (number) The port that the server is listening for connections on
* ```on_start_game_session_listener``` (function) Function to call when a game session is starting
* ```on_process_terminate_listener``` (function) Function to call when GameLift is terminating the server process
* ```on_health_check_listener``` (function) Function to call when GameLift is requesting a health check for the server. The function must return a boolean to indicate server health (true=healthy, false=unhealthy, shutdown)

**RETURN**
* ```success``` (boolean) True if successful, otherwise false, followed by an error message
* ```error``` (string) Error message if the operation failed, otherwise nil

#### gamelift.process_ready()
Notify the GameLift Server SDK that the game server is ready. This results in a GameLift Server SDK call to ```ProcessReady()```.

**RETURN**
* ```success``` (boolean) True if successful, otherwise false, followed by an error message
* ```error``` (string) Error message if the operation failed, otherwise nil

#### gamelift.activate_game_session()
Notify the GameLift Server SDK that the game session is ready to be activated. This function should be called as a result of callback to the ```on_start_game_session_listener``` function passed to a call to ```gamelift.init()```. This results in a GameLift Server SDK call to ActivateGameSession().

#### gamelift.accept_player_session(session_id)
Notify the GameLift Server SDK that the game server has accepted a connected player session. This results in a GameLift Server SDK call to ```AcceptPlayerSession().```

**PARAMETERS**
* ```session_id``` (string) The player session id to accept.

**RETURN**
* ```success``` (boolean) True if successful, otherwise false, followed by an error message
* ```error``` (string) Error message if the operation failed, otherwise nil

#### gamelift.remove_player_session(session_id)
Notify the GameLift Server SDK that the game server has removed a connected player session. This results in a GameLift Server SDK call to ```RemovePlayerSession()```.

**PARAMETERS**
* ```session_id``` (string) The player session id to remove.

**RETURN**
* ```success``` (boolean) True if successful, otherwise false, followed by an error message
* ```error``` (string) Error message if the operation failed, otherwise nil

#### gamelift.terminate_game_session()
Notify the GameLift Server SDK that the game session has finished and can be terminated. This results in a GameLift Server SDK call to ```TerminateGameSession()```.

#### gamelift.process_ending()
Notify the GameLift Server SDK that the game server is shutting down. This results in a GameLift Server SDK call to ```ProcessEnding()```.


## Updating the extension
The extension is built from the Amazon Gamelift Server SDK. Download the latest version from the [official Amazon Gamelift page](https://aws.amazon.com/gamelift/getting-started/)

Build it using the provided instructions and with default settings. When the build has completed you need to copy the includes from `out/prefix/include` and the built libraries from `out/prefix/lib` to the `gamelift/include` and `gamelift/lib` folders in this extension.
