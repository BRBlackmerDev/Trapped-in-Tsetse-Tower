
// #ifdef	STD_INCLUDE_DECL
// #endif	// STD_INCLUDE_DECL
// #ifdef	STD_INCLUDE_FUNC
// #endif	// STD_INCLUDE_FUNC

//#pragma optimize( "", off )

#include <math.h> // sqrtf, cosf, sinf
#include <stdint.h> // Type definitions
#include <stdlib.h> // rand
#include <time.h> // time
#include <cstdio> // sprintf
#include <shlwapi.h>
#include <windows.h>
#include <xinput.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_trueType.h"

#include "TypeDef.h"
#include "math.cpp"

#define 	STD_INCLUDE_DECL
#include "vector.cpp"
#include "memory.cpp"
#include "font.cpp"
#include "userInput.cpp"
#undef	STD_INCLUDE_DECL

#define	STD_INCLUDE_FUNC
#include "vector.cpp"
#include "memory.cpp"
#include "font.cpp"
#include "userInput.cpp"
#undef	STD_INCLUDE_FUNC

#define DEBUG_SYSTEM__SAFETY_OBJECT_COUNT  ( 4 )
#define DEBUG_SYSTEM__SAFETY_VERTEX_COUNT  ( 512 )
// NOTE: These variables add additional memory to the vertex buffer and text object list as a graceful failsafe to display the 'ERROR OCCURRED! Check console' message without crashing the program. This is just the cleaNest way to have a graceful failsafe doomsday error message without interfering with a bunch of systems. This system was added to prEvent Asserts from crashing our program and risk losing work.

#include "vania_config.h"
#include "d3d12.cpp"
#include "intersect.cpp"
#include "inputString.cpp"

#include "win32_Platform.h"

#define APP_targetFPS  ( 60.0f )

#include <dsound.h>
#include "audioSystem.cpp"

#include "Entity.h"
#include "vania.h"
#include "Debug_win32.cpp"

#include "bmp.cpp"
#include "wav.cpp"

#include "Draw.cpp"
#include "file.cpp"
#include "tools.cpp"
#include "Editor_tools.cpp"
#include "layout_tools.cpp"

#include "Collision.cpp"
#include "Particle.cpp"
#include "terrain.cpp"
#include "Player.cpp"
#include "Playert.cpp"

#include "Beast.cpp"

#include "Jumper.cpp"
#include "Exit.cpp"
#include "World.cpp"
#include "Event.cpp"
#include "Nest.cpp"
#include "Grenade.cpp"
#include "SecureDoor.cpp"
#include "MechDoor.cpp"
#include "powerSwitch.cpp"
#include "checkpoint.cpp"
//#include "navmesh.cpp"

#include "hit.cpp"

#include "parser.cpp"
#include "save.cpp"
#include "load.cpp"

#include "tut03.cpp"
#include "tut08.cpp"
#include "Nest01.cpp"
#include "Nest02.cpp"
#include "Nest03.cpp"
#include "Nest04.cpp"
#include "Nest05.cpp"
#include "shaft01.cpp"

#include "gameplay.cpp"

#include "replay.cpp"
#include "panel.cpp"
#include "debugPanel.cpp"
#include "EditorPanel.cpp"
#include "layoutPanel.cpp"

#include "vania.cpp"
#include "win32_Platform.cpp"
