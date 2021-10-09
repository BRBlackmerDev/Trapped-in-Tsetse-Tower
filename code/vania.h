
typedef DIRECTX_12_RENDERER    RENDERER;
typedef DIRECTX_12_RENDER_PASS RENDER_PASS;

struct PLATFORM {
    boo32 isRunning;
    
    flo32 targetSec;
    
    KEYBOARD_STATE   Keyboard;
    MOUSE_STATE      Mouse;
    CONTROLLER_STATE controller;
    
    MEMORY permMemory;
    MEMORY TempMemory;
    
    AUDIO_SYSTEM audioSystem;
    
    RENDERER Renderer;
    
    READ_FILE       * readFile;
    WRITE_FILE      * writeFile;
    DOES_FILE_EXIST * doesFileExist;
    DELETE_FILE     * deleteFile;
};

//----------
// CAMERA
//----------

struct CAMERA_SHAKE {
    vec2 P;
    vec2 Q;
    
    flo32 timer;
    flo32 upTime;
    flo32 downTime;
    
    flo32 t;
    flo32 targetT;
    
    flo32 strength;
    vec2  offset;
};

struct CAMERA_STATE {
    vec2  pos;
    vec2  dim;
    flo32 scale_ratio;
    
    boo32 move_Left;
    boo32 move_Right;
    boo32 move_down;
    boo32 move_up;
    boo32 move_out;
    boo32 move_in;
};

//----------
// FILE SAVE/OPEN
//----------

struct FILE_SAVE_OPEN {
    INPUT_STRING  input_string;
    STRING_BUFFER fileList;
    boo32 overwrite;
    boo32 unsavedChanges;
    boo32 recentSave;
    flo32 timer;
    boo32 confirmNew;
    
#define FILE_SAVE_OPEN__FILENAME_MAX_CHAR  ( EXIT__NAME_MAX_CHAR_COUNT )
    char  current[ FILE_SAVE_OPEN__FILENAME_MAX_CHAR + 1 ];
};

//----------
// LAYOUT PANEL
//----------

struct LAYOUT_PANEL {
    rect Bound;
    
    rect inner;
    vec2 atPos;
};

//----------
// LAYOUT STATE
//----------

struct LAYOUT_ROOM {
    boo32       show;
    char *      name;
    rect        Bound;
    vec2        pos;
    
    uint32      bExit; // NOTE: This is a helper to indicate the base link index for the link_Bounds calculated each frame
    uint32      nExit;
    EDITOR__EXIT Exit            [ EXIT_MAX_COUNT ];
    INT32_PAIR   Link            [ EXIT_MAX_COUNT ];
    boo32        Link_wasBroken  [ EXIT_MAX_COUNT ];
    int32        Exit_iJumperEdge_Exit [ EXIT_MAX_COUNT ];
    int32        Exit_iJumperEdge_Enter[ EXIT_MAX_COUNT ];
};

#define LAYOUT_Y_PIXELS_PER_UNIT  ( 8.0f )
struct LAYOUT_STATE {
    boo32 isInitialized;
    boo32 doReload;
    
    CAMERA_STATE Camera;
    LAYOUT_PANEL panel;
    
#define LAYOUT__FILE_MAX_COUNT  ( 4096 )
    INPUT_STRING  filter_string;
    STRING_BUFFER filter_fileList;
    
    FILE_SAVE_OPEN SaveOpen;
    
    uint32 nExit;
    uint32 iRoomToStartGame;
    
#define LAYOUT__ROOM_MAX_COUNT  ( 1024 )
    uint32     nRoom;
    LAYOUT_ROOM Room[ LAYOUT__ROOM_MAX_COUNT ];
    
    // NOTE: Any variable listed below this point will be cleared to zero when reset( layout ) is called.
    uint32 reset_reset;
    
    boo32  doMove;
    uint32 doMove_iRoom;
    vec2   doMove_bPos;
    
    boo32  doSet;
    uint32 doSet_iRoom;
    uint32 doSet_iExit;
    rect   doSet_Bound;
};

//----------
// EDITOR PANEL
//----------

struct EDITOR_PANEL {
    rect Bound;
    
    rect inner;
    vec2 atPos;
    
    flo32 advanceY;
};

//----------
// EDITOR STATE
//----------

enum EDITOR_MODE {
    EditorMode_default,
    
    EditorMode_interior,
    
    EditorMode_terrain,
    EditorMode_PlayerSpawn,
    EditorMode_Exit,
    EditorMode_Nest,
    EditorMode_SecureDoor,
    EditorMode_MechDoor,
    EditorMode_powerSwitch,
    EditorMode_Event,
    EditorMode_Checkpoint,
    EditorMode_NavMesh,
};

enum GRID_CELL_TYPE {
    CellType_Empty,
    CellType_Interior,
    
    CellType_Terrain,
    
    CellType_Interior_Blocked = 128,
    CellType_Interior_Goal,
};

enum EDITOR_EVENT__DATA_TYPE {
    dataType_point,
    dataType_rect,
    
    dataType_count,
};

#define DATA_TYPE_LABELS  char * dataTypeLabel[ dataType_count ] = { "PONT", "RECT", }
#define DATA_TYPE_TOKENS  char * dataTypeToken[ dataType_count ] = { "vec2", "rect", }
#define DATA_TYPE_SIZES   uint32 dataTypeSize [ dataType_count ] = { sizeof( vec2 ), sizeof( rect ), }

#define EDITOR_EVENT__DATA_MAX_COUNT  ( 16 )
struct EDITOR__EVENT {
    char * name;
    boo32  IsActive;
    
    uint32 nData;
    uint32  data_size;
    char *  data_name[ EDITOR_EVENT__DATA_MAX_COUNT ];
    uint32  data_Type[ EDITOR_EVENT__DATA_MAX_COUNT ];
    uint8 * data_ptr [ EDITOR_EVENT__DATA_MAX_COUNT ];
};

struct EDITOR_STATE {
    boo32 isInitialized;
    EDITOR_MODE Mode;
    
    CAMERA_STATE Camera;
    PANEL        panel;
    boo32        DrawGrid;
    boo32        DrawEntityCountOnEdge;
    
    STRING_BUFFER Event_names;
    
    FILE_SAVE_OPEN SaveOpen;
    
    // NOTE: Any variable listed below this point will be cleared to zero when reset( Editor ) is called.
    uint32 reset_reset;
    
    ROOM_STATS stat;
    
#define EDITOR__PLAYER_MAX_SPAWN_POS  ( 8 )
    uint32 Player_nSpawnPos;
    vec2   Player_SpawnPos[ EDITOR__PLAYER_MAX_SPAWN_POS ];
    
    EDITOR__NEST_STATE         Nest;
    EDITOR__EXIT_STATE         Exit;
    EDITOR__SECURE_DOOR_STATE  Secure;
    EDITOR__MECH_DOOR_STATE    Mech;
    EDITOR__POWER_SWITCH_STATE power;
    EDITOR__JUMPER_STATE       Jumper;
    EDITOR__CHECKPOINT_STATE   Checkpoint;
    //EDITOR__NAV_MESH_STATE     NavMesh;
    
    boo32       Mouse_doMouse;
    UINT32_PAIR Mouse_iCell;
    uint32      Mouse_iEntity;
    uint32      Mouse_iSubEntity;
    
    boo32 terrain_doPlace;
    boo32 terrain_doErase;
    boo32 Exit_doMove;
    boo32 power_doMove;
    boo32 Mech_doMove;
    boo32 Checkpoint_DoMove;
    
    boo32     NavMesh_DoNewTri;
    INT32_TRI NavMesh_NewTri;
    
    boo32       Event_doMouse;
    UINT32_PAIR Event_iCell;
    uint32      Event_Type;
    uint8 *     Event_data;
    uint32      Event_iEvent;
    uint32      Event_iData;
    
    uint32 nEntity[ EntityType_count ];
    
    uint8 reset_Event_start;
#define EDITOR__EVENT_MAX_COUNT          ( 32 )
#define EDITOR__EVENT_MEMORY_BLOCK_SIZE  ( sizeof( rect ) * EDITOR_EVENT__DATA_MAX_COUNT )
#define EDITOR__EVENT_MEMORY_SIZE        ( EDITOR__EVENT_MAX_COUNT * EDITOR__EVENT_MEMORY_BLOCK_SIZE )
    uint32       nEvent;
    EDITOR__EVENT Event       [ EDITOR__EVENT_MAX_COUNT ];
    uint8         Event_memory[ EDITOR__EVENT_MEMORY_SIZE ];
    uint8 reset_Event_end;
    
#define EDITOR__GRID_MAX_HEIGHT  ( 4096 )
#define EDITOR__GRID_MAX_WIDTH   ( 4096 )
    uint8 grid[ EDITOR__GRID_MAX_HEIGHT ][ EDITOR__GRID_MAX_WIDTH ];
};

//----------
// DRAW STATE
//----------

struct DRAW_STATE {
    vec2 app_dim;
    vec2 app_halfDim;
    rect App_Bound;
    
    FONT * font;
    
    int32 Debug_iEntity;
    
    APP_STATE       * AppState;
    COLLISION_STATE * Collision;
    EXIT_STATE      * Exit;
    
    BEAST_STATE        * Beast;
    PLAYER_STATE       * Player;
    GRENADE_STATE      * Grenade;
    JUMPER_STATE       * Jumper;
    NEST_STATE         * Nest;
    SECURE_DOOR_STATE  * Secure;
    MECH_DOOR_STATE    * Mech;
    POWER_SWITCH_STATE * power;
    CHECKPOINT_STATE   * Checkpoint;
    
    PANIC_POST_STATE   * PanicPost;
};

//----------
// DEBUG REPLAY
//----------

struct REPLAY_FRAME {
    BEAST_STATE        Beast;
    PLAYER_STATE       Player;
    GRENADE_STATE      Grenade;
    JUMPER_STATE       Jumper;
    NEST_STATE         Nest;
    SECURE_DOOR_STATE  Secure;
    MECH_DOOR_STATE    Mech;
    POWER_SWITCH_STATE power;
    PANIC_POST_STATE   PanicPost;
};

#define REPLAY__SECONDS_TO_CAPTURE  ( 10 )
#define REPLAY__FRAMES_PER_SECOND   ( 60 )
#define REPLAY_FRAME_MAX_COUNT  ( REPLAY__SECONDS_TO_CAPTURE * REPLAY__FRAMES_PER_SECOND )
struct REPLAY_STATE {
    boo32  show;
    uint32 show_atFrame;
    boo32  show_holdFrame;
    
    uint32     atFrame;
    REPLAY_FRAME frame[ REPLAY_FRAME_MAX_COUNT ];
};

//----------
// APP STATE
//----------

enum APP_MODE {
    appMode_game,
    appMode_Editor,
    appMode_layout,
    
    appMode_saveRoom,
    appMode_openRoom,
    appMode_saveLayout,
    appMode_openLayout,
};

struct APP_STATE {
	boo32 isRunning;
	boo32 isInitialized;
    boo32 isPaused;
    
    APP_MODE Mode;
    boo32    Mode_isInitialized;
	
    vec2 PlayerSpawn_Position;
    uint32 iRoom;
    char   CurrentRoom    [ FILE_SAVE_OPEN__FILENAME_MAX_CHAR + 1 ];
    char   ChangeRoom_Name[ FILE_SAVE_OPEN__FILENAME_MAX_CHAR + 1 ];
    boo32  ChangeRoom;
    uint32 ChangeRoom_iExit;
    uint32 ChangeRoom_iJumperEdge;
    
    boo32  DoSaveGame;
    boo32  DoRecentSave;
    boo32  DoLoadCheckpoint;
    flo32  RecentSave_Timer;
    uint32 LoadCheckpoint_iRoom;
    char   LoadCheckpoint_Name[ FILE_SAVE_OPEN__FILENAME_MAX_CHAR + 1 ];
    boo32  LoadCheckpoint_IsActive;
    flo32  LoadCheckpoint_Timer;
    
    flo32  GameOver_Timer;
    
	vec2 app_dim;
	vec2 app_halfDim;
	rect App_Bound;
    flo32 dt;
    
    FONT * font;
    
    EDITOR_STATE Editor;
    LAYOUT_STATE layout;
    
    ROOM_STATS       stat;
    rect             Camera_Bound;
    CAMERA_STATE     Camera;
    BEAST_STATE      Beast;
    PLAYER_STATE     Player;
    GRENADE_STATE    Grenade;
    TERRAIN_STATE    terrain;
    JUMPER_STATE     Jumper;
    NEST_STATE       Nest;
    EXIT_STATE       Exit;
    CHECKPOINT_STATE Checkpoint;
    //NAV_MESH_STATE   NavMesh;
    WORLD_STATE      World;
    WORLD_STATE      World_Save;
    EVENT_STATE      Event;
    MEMORY           Event_memory;
    
    PANIC_POST_STATE      PanicPost;
    
    SECURE_DOOR_STATE  Secure;
    MECH_DOOR_STATE    Mech;
    POWER_SWITCH_STATE power;
    
    boo32 testRoom;
    char  testRoom_name[ FILE_SAVE_OPEN__FILENAME_MAX_CHAR + 1 ];
    
    PARTICLE_STATE Particle;
    
    COLLISION_STATE Collision;
    MEMORY          Collision_memory;
    
    PANEL debug;
    int32 Debug_iEntity;
#define DEBUG_MAX_COUNT  ( 32 )
    boo32 Debug_IsActive[ DEBUG_MAX_COUNT ];
    
    vec2 lastJump;
    
    REPLAY_STATE  replay;
    PROFILE_STATE Profile;
};