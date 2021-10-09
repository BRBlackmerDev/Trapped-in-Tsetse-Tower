
//----------
// SHARED/TOOLS
//----------

enum ENTITY_MODE {
    EntityMode_Move,
    EntityMode_Jump,
    EntityMode_Stunned,
    EntityMode_Delay,
};

enum ENTITY_TYPE {
    EntityType_Jumper,
    
    EntityType_count,
};

#define ENTITY_TYPE_TAGS  char * EntityTypeTag[ EntityType_count ] = { "JUMP", }

enum HIT_TYPE {
    HitType_Wall,
    
    HitType_Jumper,
    HitType_Nest,
    
    HitType_count,
};

struct BULLET_HIT {
    HIT_TYPE       Type;
    int32          iHit;
    RAY2_INTERSECT bHit;
};

internal BULLET_HIT
BulletHit( HIT_TYPE Type, int32 iHit, RAY2_INTERSECT bHit ) {
    BULLET_HIT Result = {};
    Result.Type = Type;
    Result.iHit = iHit;
    Result.bHit = bHit;
    return Result;
}

#define WORLD_GRAVITY_ACCEL  ( -40.0f )

//----------
// DEBUG_PANEL
//----------

#define PANEL_LABEL_Y  ( 14.0f )
struct PANEL {
    boo32 show;
    rect  Bound;
    
    rect  inner;
    vec2  atPos;
    flo32 advanceY;
};

//----------
// STRING
//----------

struct STRING {
    char * string;
    uint32 nChar;
};

internal boo32
matchString( char * string0, char * string1 ) {
    boo32 doMatch = true;
    while( ( *string0 ) && ( *string1 ) ) {
        if( *string0 != *string1 ) { doMatch = false; }
        string0++;
        string1++;
    }
    if( ( *string0 ) || ( *string1 ) ) { doMatch = false; }
    return doMatch;
}

internal boo32
matchSegment( char * string0, char * string1, uint32 nChar ) {
    boo32 doMatch = true;
    for( uint32 iChar = 0; ( doMatch ) && ( iChar < nChar ); iChar++ ) {
        if( string0[ iChar ] != string1[ iChar ] ) { doMatch = false; }
    }
    return doMatch;
}

internal void
writeSegment( MEMORY * memory, char * str ) {
    uint32 length = ( uint32 )strlen( str );
    uint8 * dest = ( uint8 * )_pushSize( memory, length );
    memcpy( dest, str, length );
}

internal void
writeString( MEMORY * memory, char * str, uint32 nChar ) {
    uint8 * dest = ( uint8 * )_pushSize( memory, nChar );
    memcpy( dest, str, nChar );
    
    _writem( memory, uint8, 0 );
}

internal void
writeString( MEMORY * memory, char * str ) {
    uint32 nChar = ( uint32 )strlen( str );
    writeString( memory, str, nChar );
}

internal boo32
matchString( STRING strA, STRING strB ) {
    boo32 result = ( strA.nChar == strB.nChar );
    if( result ) {
        result = matchSegment( strA.string, strB.string, strA.nChar );
    }
    return result;
}

internal boo32
matchString( STRING strA, char * str ) {
    uint32 nChar = ( uint32 )strlen( str );
    boo32 result = ( strA.nChar == nChar );
    if( result ) {
        result = matchSegment( strA.string, str, strA.nChar );
    }
    return result;
}

//----------
// STRING BUFFER
//----------

struct STRING_BUFFER {
    uint32 nStr;
    uint32 nStrMax;
    char ** str;
    MEMORY  memory;
};

internal void
reset( STRING_BUFFER * buffer ) {
    buffer->nStr        = 0;
    buffer->memory.used = 0;
}

internal STRING_BUFFER
StringBuffer( MEMORY * parent_memory, uint32 maxString, uint32 maxSize ) {
    STRING_BUFFER result = {};
    result.str     = _pushArray_clear( parent_memory, char *, maxString );
    result.memory  = subArena( parent_memory, maxSize );
    result.nStrMax = maxString;
    return result;
}

internal char *
add( STRING_BUFFER * s, char * string ) {
    char * dest = 0;
    if( s->nStr < s->nStrMax ) {
        uint32 length = ( uint32 )strlen( string );
        dest = ( char * )_pushSize( &s->memory, length + 1 );
        strcpy( dest, string );
        
        s->str[ s->nStr++ ] = dest;
    }
    
    return dest;
}

//----------
// COLLISION
//----------

// NOTE: PATH_CONNECTION is a temp struct used only for PATH_LINK generation. Terrain/Collision generation makes a list of valid PATH_CONNECTIONs between Platforms. The list is analyzed and grouped into PATH_LINKs (ie. valid Exit points) for a single Platform.
struct PATH_CONNECTION {
    boo32  isValid_up;
    boo32  isValid_down;
    
    uint32 iNode_upper;
    vec2   pos_upper;
    
    uint32 iNode_lower;
    vec2   pos_lower;
};

struct PATH_LINK {
    uint32 iNode;
    vec2   NodeP;
    vec2   ExitP;
};

struct PATH_LINK__POSITION_DATA {
    vec2 ExitP;
    vec2 toNodeP;
};

struct PATH_LINK__ACTIVE_DATA {
    boo32  IsActive;
    uint32 iNode;
};

struct PATH_LINK__IN_DATA {
    uint32 iNode;
    uint32 iPathLink;
};

struct PATH_STATE {
    uint32 nNodeLink;
    uint32 nPathLink;
    UINT32_PAIR              * NodeLink;
    PATH_LINK__ACTIVE_DATA   * PathLink_ActiveData;
    PATH_LINK__POSITION_DATA * PathLink_PositionData;
    
    UINT32_PAIR              * NodeLinkIn;
    PATH_LINK__IN_DATA       * PathLink_InData;
};

struct COLLISION_STATE {
    UINT32_PAIR Type[ 4 ];
    uint32 nEdge;
    vec4  * Edge;
    
    uint32 nJumpBound;
    rect *  jumpBound;
    
    uint32 nWallSlideLeft;
    rect *  wallSlideLeft;
    
    uint32 nWallSlideRight;
    rect *  wallSlideRight;
    
    uint32 nLedgeGrabLeft;
    rect *  lEdgeGrabLeft;
    
    uint32 nLedgeGrabRight;
    rect *  lEdgeGrabRight;
    
    uint32 nJumperBound;
    rect *  JumperBound;
    
    uint32 nJumperEdge;
    vec4 *  JumperEdge;
    
    PATH_STATE JumperPath;
    
    uint32      Grid_nCell;
    UINT32_QUAD Grid_Bound;
    uint8 *     Grid;
    
    //uint32 nInteriorVertex;
    //vec2 *  InteriorVertex;
    
    //uint32       nInteriorEdge;
    //UINT32_PAIR * InteriorEdge;
    
    //uint32 nCircumcircle;
    //circ *  Circumcircle;
    
    //uint32      nTri;
    //UINT32_TRI * Tri;
};

//----------
// PARTICLES
//----------

struct PARTICLE {
    vec2 Position;
    vec2 Velocity;
    vec2 accel;
    
    vec2  dim;
    flo32 radians;
    
    flo32 timer;
    flo32 targetTime;
    vec4  Color;
};

#define PARTICLE_MAX_COUNT  ( 2048 )
struct PARTICLE_STATE {
    uint32    nParticle;
    PARTICLE * Particle;
};

//----------
// WORLD
//----------

struct WORLD__DOOR {
    UINT32_PAIR iJumperPathLink;
    uint8       isClosed;
};

//----------
// EXIT
//----------

#define  EXIT_DIMS  vec2 ExitDim[ ExitType_count ] = { \
Vec2( TILE_WIDTH * 2.0f, TILE_HEIGHT * 10.0f ), \
Vec2( TILE_WIDTH * 2.0f, TILE_HEIGHT * 10.0f ), \
\
Vec2( TILE_WIDTH * 2.0f, TILE_HEIGHT *  4.0f ), \
Vec2( TILE_WIDTH * 4.0f, TILE_HEIGHT *  2.0f ), \
Vec2( TILE_WIDTH * 2.0f, TILE_HEIGHT *  4.0f ), \
Vec2( TILE_WIDTH * 4.0f, TILE_HEIGHT *  2.0f ), \
\
Vec2( TILE_WIDTH * 4.0f, TILE_HEIGHT *  4.0f ), \
Vec2( TILE_WIDTH * 4.0f, TILE_HEIGHT *  2.0f ), \
};

// S : Side View
// T : Top Down View
enum EXIT_TYPE {
    ExitTypeS_Walk_EnterLeft,
    ExitTypeS_Walk_EnterRight,
    
    ExitTypeT_Walk_EnterLeft,
    ExitTypeT_Walk_EnterUp,
    ExitTypeT_Walk_EnterRight,
    ExitTypeT_Walk_EnterDown,
    
    ExitTypeS_Grate_Duck,
    ExitTypeS_Grate_Drop, // can only enter Room through this Exit, cannot leave
    
    ExitType_count,
};

struct EXIT {
    rect      Bound;
    EXIT_TYPE Type;
};

#define EXIT_MAX_COUNT  ( 8 )
struct EXIT_STATE {
    uint32 nEntity;
    EXIT    Entity[ EXIT_MAX_COUNT ];
};

struct EDITOR__EXIT {
    rect      Bound;
    EXIT_TYPE Type;
};

struct EDITOR__EXIT_STATE {
    uint32      nEntity;
    EDITOR__EXIT Entity[ EXIT_MAX_COUNT ];
};

//----------
// POWER SWITCH
//----------

#define POWER_SWITCH_TARGET_TIME  ( 2.5f )
struct POWER_SWITCH {
    boo32 isEnabled;
    boo32 isOn;
    rect  Bound;
    
    uint32 iPower;
    
    boo32 doPower;
    flo32 power_timer;
};

#define POWER_SWITCH_MAX_COUNT  ( 4 )
struct POWER_SWITCH_STATE {
    uint32      nEntity;
    POWER_SWITCH Entity[ POWER_SWITCH_MAX_COUNT ];
};

struct EDITOR__POWER_SWITCH {
    rect   Bound;
    uint32 iPower;
};

struct EDITOR__POWER_SWITCH_STATE {
    uint32              nEntity;
    EDITOR__POWER_SWITCH Entity[ POWER_SWITCH_MAX_COUNT ];
};

//----------
// SECURE DOOR
//----------

#define SECURE_DOOR_VERIFY_OPEN_DELAY        ( 32.0f / 60.0f )
#define SECURE_DOOR_VERIFY_OPEN_CYCLE_TIME   ( ( SECURE_DOOR_VERIFY_OPEN_TARGET_TIME - SECURE_DOOR_VERIFY_OPEN_DELAY ) * 0.25f )
#define SECURE_DOOR_VERIFY_OPEN_TARGET_TIME  ( 1.75f )
struct SECURE_DOOR {
    uint32 iPower;
    uint32 iSensor;
    
    boo32  isPowered;
    boo32  isClosed;
    
    rect   Bound;
    flo32  Timer;
    
    UINT32_PAIR iJumperPathLink;
};

#define SECURE_DOOR_MAX_COUNT  ( 8 )
struct SECURE_DOOR_STATE {
    uint32      nEntity;
    SECURE_DOOR  Entity[ SECURE_DOOR_MAX_COUNT ];
    //UINT32_PAIR iPathLink[ SECURE_DOOR_MAX_COUNT ];
};

struct EDITOR__SECURE_DOOR {
    rect        Bound;
    boo32       isClosed;
    
    uint32      iPower;
    uint32      iSensor;
    
    UINT32_PAIR iJumperPathLink;
};

struct EDITOR__SECURE_DOOR_STATE {
    uint32             nEntity;
    EDITOR__SECURE_DOOR Entity[ SECURE_DOOR_MAX_COUNT ];
};

//----------
// MECH DOOR
//----------

#define MECH_DOOR_TARGET_TIME  ( 1.15f )
#define MECH_DOOR_SWITCH_DIM  ( Vec2( TILE_WIDTH * 3.0f, TILE_HEIGHT * 3.0f ) )

#define VERSION__MECH_DOOR  ( 3 )
#define FILETAG__MECH_DOOR  ( "MECHDOOR" )
struct MECH_DOOR {
    rect Bound_Door;
    rect Bound_SwitchA;
    rect Bound_SwitchB;
    
    boo32 isClosed;
    
    UINT32_PAIR iJumperPathLink;
    
    boo32 doToggle;
    flo32 Toggle_Timer;
};

#define MECH_DOOR_MAX_COUNT  ( 16 )
struct MECH_DOOR_STATE {
    uint32      nEntity;
    MECH_DOOR    Entity[ MECH_DOOR_MAX_COUNT ];
};

struct EDITOR__MECH_DOOR {
    rect Bound_Door;
    rect Bound_SwitchA;
    rect Bound_SwitchB;
    
    boo32 isClosed;
    
    UINT32_PAIR iJumperPathLink;
};

struct EDITOR__MECH_DOOR_STATE {
    uint32           nEntity;
    EDITOR__MECH_DOOR Entity[ MECH_DOOR_MAX_COUNT ];
};

//----------
// ROOM STATS
//----------

#define VERSION__ROOM_STATS  ( 7 )
#define FILETAG__ROOM_STATS  ( "ROOMSTAT" )
struct ROOM_STATS {
    boo32       isTopDown;
    rect        Bound;
    UINT32_PAIR intCell;
    
    uint32 nExit;
    int32   Exit_iJumperEdge_Exit [ EXIT_MAX_COUNT ];
    int32   Exit_iJumperEdge_Enter[ EXIT_MAX_COUNT ];
    
    uint32 nNodeLink;
    uint32 nPathLink;
    uint32 nMechDoor;
    uint32 nSecureDoor;
};

//----------
// EVENT
//----------

struct APP_STATE;
typedef void ( EVENT_FUNC )( APP_STATE * AppState, uint8 * ptr, uint32 iEvent );

struct EVENT {
    boo32        IsActive;
    EVENT_FUNC * func;
    uint8      * data;
    // TODO: this timer variable should be a pointer to a data block in Event_State memory. Event_State will Pass out empty blocks of memory to EVENT_FUNCs that need it (block allocator)
    flo32        timer;
};

#define EVENT_MAX_COUNT  ( 128 )
struct EVENT_STATE {
    uint32 nEvent;
    EVENT   Event[ EVENT_MAX_COUNT ];
    
    // TODO: WORLD_STATE needs to keep track of event.IsActive and save it to file. Event data and function pointers are loaded at Game Init.
};

//----------
// TERRAIN
//----------

#define WALL_JUMP_NEG_Y_EXTENSION           ( -TILE_HEIGHT * 2.0f )
#define WALL_JUMP_MIN_LENGTH                (  TILE_HEIGHT * 2.0f )
#define WALL_JUMP_MIN_LENGTH_FOR_EXTENSION  (  TILE_HEIGHT * 3.0f )

#define TILE_WIDTH  ( 0.25f )
#define TILE_HEIGHT ( 0.25f )
#define TILE_DIM    ( Vec2( TILE_WIDTH, TILE_HEIGHT ) )

#define CAMERA_TILE_Y_COUNT  ( 72.8f )

#define TERRAIN_COLOR_U32     ( 0xFF7A5A02 )
#define TERRAIN_COLOR         ( toColor( 122, 90, 2 ) )
#define TERRAIN_OUTLINE_COLOR ( toColor( 92,  68, 3 ) )

struct TERRAIN {
    rect Bound;
};

#define TERRAIN_MAX_COUNT  ( 2048 )
struct TERRAIN_STATE {
    uint32 nTerrain;
    TERRAIN terrain[ TERRAIN_MAX_COUNT ];
};

//----------
// NEST
//----------

#define NEST_SPAWN_TARGET_TIME        ( 60.0f )
#define NEST_AWAKE_SPAWN_TARGET_TIME  (  7.5f )
#define NEST_HEALTH  ( 500 )

#define NEST_AWAKE_TARGET_TIME  ( 1.0f )
#define NEST_JUMPER_INIT_SPAWN  ( 10 )
#define NEST_JUMPER_MAX_SPAWN   ( 20 )
#define NEST_ADDED_SPAWN_TARGET_TIME  ( 0.25f )

#define NEST_KILL_TARGET_TIME  ( 0.4f )

struct NEST {
    boo32       IsDead;
    boo32       isAwake;
    rect        Bound;
    
    ENTITY_TYPE Type;
    int32       health;
    flo32       timer;
    flo32       awake_timer;
    
    uint32      nSpawn;
    uint32      maxSpawn;
    flo32       spawn_timer;
    
    boo32       doKill;
    uint32      nKill;
    flo32       kill_timer;
    uint32      nPost;
    
    // TODO: different variations of Nests? larger Bound? multiple spawns points? different health?
    // TODO: use random area for spawn Position?
    // TODO: damage Player if touches Bound?
    // TODO: is spawning count Bound by Room? is it calculated by World State including adjacent Rooms?
    // TODO: spawn faster if under attack?
};

#define NEST_MAX_COUNT  ( 4 )
struct NEST_STATE {
    uint32 nEntity;
    NEST    Entity[ NEST_MAX_COUNT ];
};

struct EDITOR__NEST {
    rect        Bound;
    ENTITY_TYPE Type;
    boo32       isAwake;
    uint32      iJumperEdge;
};

struct EDITOR__NEST_STATE {
    uint32      nEntity;
    EDITOR__NEST Entity[ NEST_MAX_COUNT ];
};

//----------
// JUMPER
//----------

#define JUMPER_HIT_BY_BULLET_MOVE_SPEED    ( 1.5f )
#define JUMPER_HIT_BY_BULLET_AIR_SPEED_X   ( 2.5f )
#define JUMPER_HIT_BY_BULLET_AIR_SPEED_Y   ( 1.0f )
//#define JUMPER_MOVE_SPEED           ( 6.0f )
//#define JUMPER_MOVE_SPEED     ( 2.0f )
#define JUMPER_MOVE_FRICTION  ( 20.0f )

#define JUMPER_MOVE_MAX_SPEED_HI  ( 6.0f )
#define JUMPER_MOVE_MAX_SPEED_LO  ( 5.0f )

#define JUMPER_ATTACK_MAX_X_DIST      (  18.5f * TILE_WIDTH )
#define JUMPER_ATTACK_POS_MAX_Y_DIST  (  12.5f * TILE_HEIGHT )
#define JUMPER_ATTACK_NEG_MAX_Y_DIST  ( -24.5f * TILE_HEIGHT )

#define JUMPER_HOP_MIN_VALID_DIST  ( 7.5f  * TILE_WIDTH )
#define JUMPER_HOP_MAX_VALID_DIST  ( 14.5f * TILE_WIDTH )

#define JUMPER_JUMP_MAX_X_DIST       ( 18.5f * TILE_WIDTH )
#define JUMPER_JUMP_MAX_Y_DIST_UP    ( 12.5f * TILE_HEIGHT )
#define JUMPER_JUMP_MAX_Y_DIST_DOWN  ( 22.5f * TILE_HEIGHT )
#define JUMPER_GRAVITY               ( 26.0f )

#define JUMPER_WIDTH   ( 0.6f )
#define JUMPER_HEIGHT  ( 0.6f )
#define JUMPER_DIM  ( Vec2( JUMPER_WIDTH, JUMPER_HEIGHT ) )

#define JUMPER_HEALTH  ( 35 )

#define JUMPER_SPAWN_DELAY_TARGET_TIME_LO  ( 36.0f / 60.0f )
#define JUMPER_SPAWN_DELAY_TARGET_TIME_HI  ( 72.0f / 60.0f )

#define JUMPER_STUNNED_TARGET_TIME  ( 50.0f / 60.0f )

struct JUMPER__PLATFORM_DATA {
    boo32  isJump;
    vec2   P;
    uint32 score;
    boo32  doLeft;
    uint32 iPlatform;
};

struct JUMPER {
    ENTITY_MODE Mode;
    
    vec2 Position;
    vec2 Velocity;
    vec2 dPos;
    
    flo32 timer;
    flo32 targetTime;
    
    flo32 stunned_timer;
    
    vec4  Color;
    
    int32 health;
    
    uint32 TargetPlatform;
    uint32 CurrentPlatform;
    
    boo32  HitByBullet;
    
    flo32 move_xDist;
    flo32 move_xDir;
    
    flo32 jump_xMaxSpeed;
    flo32 jump_xDir;
    
    vec2  jump_destP;
    vec2  jump_HeightP;
    
    boo32 canAttack;
    boo32 attack_isEnabled;
    
    boo32  DoFlank;
    uint32 DoFlank_iJumperEdge;
    
    flo32 Delay_Timer;
    flo32 Delay_TargetTime;
    
    flo32 Hop_Timer;
    flo32 Hop_TargetTime;
    flo32 Move_MaxSpeed;
    
#define PATH_MAX_COUNT  ( 32 )
    boo32   noPath;
    uint32   nPath;
    PATH_LINK Path[ PATH_MAX_COUNT ];
};

#define JUMPER_MAX_COUNT  ( 128 )
struct JUMPER_STATE {
    uint32 nEntity;
    JUMPER  Entity[ JUMPER_MAX_COUNT ];
};

struct EDITOR__JUMPER {
    uint8 iJumperEdge;
    uint8 hasPathToExit;
};

struct EDITOR__JUMPER_STATE {
    uint32 nEntity;
    EDITOR__JUMPER Entity[ JUMPER_MAX_COUNT ];
};

//----------
// GRENADE
//----------

#define GRENADE_RADIUS           ( TILE_WIDTH * 0.65f )
#define GRENADE_COLOR            ( toColor( 90, 105, 5 ) )

#define GRENADE_DAMAGE_RADIUS     ( TILE_WIDTH * 8.0f )
#define GRENADE_DAMAGE_TO_PLAYER  ( 20 )
#define GRENADE_DAMAGE_TO_JUMPER  ( 20 )
#define GRENADE_DAMAGE_TO_NEST    ( 60 )

#define GRENADE_THROW_SPEED_HI   ( 20.0f )
#define GRENADE_THROW_SPEED_LO   ( 10.0f )
#define GRENADE_AIR_FRICTION     ( 0.1f )
#define GRENADE_GROUND_FRICTION  ( 6.0f )
#define GRENADE_GRAVITY          ( 32.0f )

#define GRENADE_DETONATE_TARGET_TIME  ( 80.0f / 60.0f )

struct GRENADE {
    vec2  Position;
    vec2  Velocity;
    flo32 Timer;
};

#define GRENADE_MAX_COUNT  ( 8 )
struct GRENADE_STATE {
    uint32 nEntity;
    GRENADE Entity[ GRENADE_MAX_COUNT ];
};

//----------
// CHECKPOINT
//----------

#define CHECKPOINT_SHIELD_CYCLE_TARGET_TIME  (  72.0f / 60.0f )
#define CHECKPOINT_SIGNAL_CYCLE_TARGET_TIME  ( 120.0f / 60.0f )
#define CHECKPOINT_TEXT_CYCLE_TARGET_TIME    ( 240.0f / 60.0f )

#define CHECKPOINT_ACTIVATE_TARGET_TIME      (  72.0f / 60.0f )
#define CHECKPOINT_SAVE_TARGET_TIME          (  48.0f / 60.0f )

#define CHECKPOINT_PANEL_DIM  ( Vec2( TILE_WIDTH * 6.0f, TILE_HEIGHT * 4.0f ) )

struct CHECKPOINT {
    boo32 DoesExist;
    boo32 IsActive;
    rect  Bound;
    
    flo32 Shield_Timer;
    flo32 Signal_Timer;
    flo32 Text_Timer;
    
    boo32 Save_IsActive;
    flo32 Save_Timer;
};

struct CHECKPOINT_STATE {
    CHECKPOINT Entity;
};

struct EDITOR__CHECKPOINT {
    boo32 DoesExist;
    boo32 IsActive;
    rect  Bound;
};

struct WORLD__CHECKPOINT {
    boo32 IsActive;
};

struct EDITOR__CHECKPOINT_STATE {
    EDITOR__CHECKPOINT Entity;
};

//----------
// NAV MESH
//----------

#if 0
struct NAV_MESH_TRI {
    INT32_TRI iPathLink;
    vec2      A;
    vec2      B;
    vec2      C;
};

#define NAV_MESH_TRI_MAX_COUNT  ( 128 )
struct NAV_MESH_STATE {
    uint32      nEntity;
    NAV_MESH_TRI Entity[ NAV_MESH_TRI_MAX_COUNT ];
};

struct EDITOR__NAV_MESH_TRI {
    UINT32_TRI iInteriorVert;
    INT32_TRI  iPathLink;
    vec2       A;
    vec2       B;
    vec2       C;
};

struct EDITOR__NAV_MESH_STATE {
    uint32              nEntity;
    EDITOR__NAV_MESH_TRI Entity[ NAV_MESH_TRI_MAX_COUNT ];
};
#endif

//----------
// BEAST
//----------

#define BEAST_WIDTH       ( TILE_WIDTH  * 8.8f )
#define BEAST_HALF_WIDTH  ( BEAST_WIDTH * 0.5f )
#define BEAST_RADIUS      ( BEAST_HALF_WIDTH )

#define BEAST_WALKABLE_RADIUS      ( TILE_WIDTH * 4.0f )

#define BEAST_WALK_SPEED       ( 4.0f )
#define BEAST_WALK_FRICTION    ( 20.0f )

#define BEAST_RUN_SPEED       ( 11.0f )
#define BEAST_RUN_FRICTION    ( 6.0f )

#define BEAST_FOV_LOOKAHEAD_TILE_MAX_COUNT  ( 12 )

#define BEAST_FOV_ANGLE        ( PI / 6.0f )
#define BEAST_FOV_HALF_ANGLE   ( BEAST_FOV_ANGLE * 0.5f )
#define BEAST_FOV_OFFSET_DIST  ( TILE_WIDTH * 2.0f )

#define BEAST_COLOR
struct BEAST {
    vec2 Position;
    vec2 dPos;
    vec2 Velocity;
    
    flo32 FacingRadians;
    
    boo32 CanSeePlayer;
    boo32 DoesKnowPlayerP;
    vec2  LastKnownPlayerP;
    
#define BEAST_PATH_MAX_COUNT  ( 1024 )
    boo32   PathIsValid;
    uint32 nPath;
    vec2    Path[ BEAST_PATH_MAX_COUNT ];
};

struct BEAST_STATE {
    BEAST Entity;
};

//----------
// PATROL POST
//----------

struct PATROL_POST {
    vec2  Position;
    flo32 TimeSinceLastSearch;
    // TODO: Improve scoring function!
};

#define PATROL_POST_MAX_COUNT  ( 16 )
struct PATROL_POST_STATE {
    uint32     nPatrolPost;
    PATROL_POST PatrolPost[ PATROL_POST_MAX_COUNT ];
};

//----------
// PANIC POST
//----------

#define PANIC_POST_RADIUS  ( TILE_WIDTH * 6.0f )

struct PANIC_POST {
    vec2  Position;
    flo32 TimeSinceLastSearch;
    // TODO: Improve scoring function!
};

#define PANIC_POST_MAX_COUNT  ( 16 )
struct PANIC_POST_STATE {
    uint32    nPanicPost;
    PANIC_POST PanicPost[ PANIC_POST_MAX_COUNT ];
};

//----------
// PLAYER
//----------

// COMMON STATS
// PLATFORM HEIGHT FOR SHORT JUMP : 2
// PLATFORM HEIGHT FOR FULL JUMP, NO LEDGE GRAB : 11
// PLATFORM X-DIST FOR FULL JUMP, NO LEDGE GRAB : 22

#define LOAD_CHECKPOINT_TARGET_TIME  ( 72.0f / 60.0f )

#define PLATFORM_TILE_X_FOR_FULL_JUMP_NO_LEDGE_GRAB  ( 22 )
#define PLATFORM_TILE_Y_FOR_FULL_JUMP_NO_LEDGE_GRAB  ( 11 )
#define PLATFORM_MIN_TILE_Y_FOR_CEILING_ABOVE_VALID_LEDGE_GRAB  ( 2 )

#define JUMPER_ATTACK_DAMAGE  ( 3 )

#define PLAYER_HEALTH                   ( 50 )
#define PLAYER_HEAL_TARGET_TIME         ( 0.5f )
#define PLAYER_HEAL_WARMUP_TARGET_TIME  ( 1.0f )
#define PLAYER_HEAL_DISABLE_CYCLE_TARGET_TIME  ( 1.0f )
#define PLAYER_SHOW_DAMAGE_CYCLE_TARGET_TIME   ( 0.1f )

#define PLAYER_WIDTH       ( 0.8f )
#define PLAYER_HALF_WIDTH  ( PLAYER_WIDTH * 0.5f )
#define PLAYER_HEIGHT_STANDING   ( 1.8f )
#define PLAYER_HEIGHT_CROUCHING  ( 0.9f )
#define PLAYER_RADIUS      ( PLAYER_HALF_WIDTH )

#define PLAYER_DEAD_TARGET_TIME  ( 0.3f )
#define PLAYER_DEAD_DIM  ( Vec2( PLAYER_WIDTH, PLAYER_HEIGHT_CROUCHING ) )
#define PLAYER_DEAD_FRICTION  ( 6.0f )

#define PLAYER_GRAVITY  ( 40.0f )
#define PLAYER_LEDGE_GRAB_JUMP_SPEED  ( 13.0f )
#define PLAYER_JUMP_SPEED        ( 9.75f )
#define PLAYER_JUMP_TARGET_TIME  ( 12.0f / 60.0f )

#define PLAYER_WALL_JUMP_CHARGE_TARGET_TIME  ( 12.0f / 60.0f )
#define PLAYER_WALL_JUMP_DISPEL_TARGET_TIME  ( 10.0f / 60.0f )
#define PLAYER_WALL_JUMP_RUN_SPEED   ( Vec2( 13.0f, 12.0f ) )
#define PLAYER_WALL_JUMP_WALK_SPEED  ( Vec2(  9.0f, 9.75f ) )
#define PLAYER_WALL_JUMP_SPEED  ( 9.0f )

#define PLAYER_WALK_SPEED     (  4.0f )
#define PLAYER_WALK_FRICTION  ( 20.0f )
#define PLAYER_RUN_SPEED     (  8.0f )
#define PLAYER_RUN_FRICTION  ( 20.0f )
#define PLAYER_AIR_WALK_FRICTION  ( 6.0f )
#define PLAYER_AIR_RUN_FRICTION   ( 4.0f )

#define PLAYER_CROUCH_WALK_SPEED     (  2.0f )
#define PLAYER_CROUCH_WALK_FRICTION  ( 20.0f )
#define PLAYER_CROUCH_RUN_SPEED      (  4.0f )
#define PLAYER_CROUCH_RUN_FRICTION   ( 20.0f )

#define PLAYER_ROLL_TARGET_TIME   ( 24.0f / 60.0f )
#define PLAYER_ROLL_INIT_SPEED    ( 12.0f )
#define PLAYER_ROLL_SPEED         ( 4.0f )
#define PLAYER_ROLL_FRICTION      ( 6.0f )
#define PLAYER_ROLL_RUN_SPEED     ( 6.0f )
#define PLAYER_ROLL_RUN_FRICTION  ( 6.0f )
#define PLAYER_ROLL_RADIUS        ( PLAYER_WIDTH * 1.25f * 0.5f )

#define PLAYER_WALL_SLIDE_PARTICLE_CYCLE_TARGET_TIME  ( 0.1f )
#define PLAYER_WALL_SLIDE_SPEED     (  3.75f )
#define PLAYER_WALL_SLIDE_FRICTION  ( 20.0f )
#define PLAYER_FALL_MAX_SPEED     ( -28.0f )

#define PLAYER_GRENADE_COOLDOWN_TARGET_TIME  ( 120.0f / 60.0f )
#define PLAYER_BULLET_TARGET_TIME            ( 2.0f   / 60.0f )

enum PLAYER_DRAW_ID {
    DrawID_default,
    DrawID_wallSlide,
    DrawID_roll,
};

struct PLAYER_STATE {
    vec2  Position;
    vec2  dPos;
    flo32 Height;
    vec2  Velocity;
    flo32 control_xDir;
    
    uint32 health;
    flo32  heal_timer;
    flo32  heal_warmupTimer;
    boo32  heal_isDisabled;
    flo32  heal_disableTimer;
    flo32  showDamage_timer;
    boo32  IsDead;
    flo32  IsDead_Timer;
    
    PLAYER_DRAW_ID Draw_id;
    boo32          Draw_faceLeft;
    boo32          Draw_wallJump;
    boo32          Draw_EnterIndicator;
    boo32          Draw_SaveIndicator;
    vec2  FaceDir;
    
    boo32 jump_IsActive;
    flo32 jump_timer;
    boo32 jump_doDampen;
    
    boo32 run_isEnabled;
    boo32 run_IsActive;
    boo32 lEdgeGrabLeft_IsActive;
    boo32 lEdgeGrabRight_IsActive;
    flo32 wallJump_chargeTimer;
    flo32 wallJump_dispelTimer;
    flo32 wallSlide_ParticleTimer;
    boo32 crouch_IsActive;
    
    boo32 roll_IsActive;
    flo32 roll_timer;
    
    boo32 Throw_WasPressed;
    
    flo32 GrenadeThrow_CooldownTimer;
    
    boo32 Use_WasPressed;
    boo32 Use_WasReleased;
    boo32 Use_IsActive;
    flo32 Use_t;
    
    boo32  ChangePlatform;
    uint32 CurrentPlatform;
    
    vec2  aim_dir;
    boo32 fire_IsActive;
    flo32 fire_timer;
    vec2  fire_pos;
    vec2  fire_dir;
    flo32 fire_lo;
    flo32 fire_hi;
    boo32 fire_doHitScan;
};

//----------
// WORLD
//----------

#define EXIT__NAME_MAX_CHAR_COUNT  ( 128 )
struct WORLD__EXIT {
    boo32  isValid;
    char   toRoom_name[ EXIT__NAME_MAX_CHAR_COUNT + 1 ];
    uint32 toRoom_iRoom;
    uint32 toRoom_iExit;
    uint32 toRoom_iJumperEdge;
};

struct WORLD__ROOM_TRANSFER {
    uint32 nJumperToTransfer[ EXIT_MAX_COUNT ];
};

#define ROOM_NAME_CHAR_COUNT  ( 16 )
struct ROOM {
    char   name[ ROOM_NAME_CHAR_COUNT ];
    uint32 nEntity[ EntityType_count ];
    
    UINT32_PAIR Event;
    
    UINT32_PAIR NodeLink;
    UINT32_PAIR PathLink;
    
    UINT32_PAIR MechDoor;
    UINT32_PAIR SecureDoor;
    UINT32_PAIR Jumper;
    UINT32_PAIR Nest;
    
    int32 iCheckpoint;
    
    uint32      nExit;
    int32       Exit_iJumperEdge_Exit [ EXIT_MAX_COUNT ];
    int32       Exit_iJumperEdge_Enter[ EXIT_MAX_COUNT ];
    WORLD__EXIT Exit                  [ EXIT_MAX_COUNT ];
};

struct WORLD__JUMPER {
    int32 health;
    uint8 iJumperEdge;
    uint8 hasPathToExit;
};

struct WORLD__NEST {
    int32 Health;
    flo32 Timer;
    uint8 isAwake;
    uint8 iJumperEdge;
};

struct WORLD_STATE {
#define WORLD__ROOM_MAX_COUNT  ( 128 )
    uint32 iRoomToStartGame;
    
    uint32 nExit;
    uint32 nRoom;
    ROOM    Room[ WORLD__ROOM_MAX_COUNT ];
    
#define WORLD__POWER_MAX_COUNT  ( 128 )
    uint32 nPower;
    boo32   power_isOn[ WORLD__POWER_MAX_COUNT ];
    
#define WORLD__JUMPER_MAX_COUNT  ( 512 )
    uint32       nJumper;
    WORLD__JUMPER Jumper[ WORLD__JUMPER_MAX_COUNT ];
    
#define WORLD__NEST_MAX_COUNT  ( 128 )
    uint32     nNest;
    WORLD__NEST Nest[ WORLD__NEST_MAX_COUNT ];
    
#define WORLD__NODE_LINK_MAX_COUNT  ( 512 )
#define WORLD__PATH_LINK_MAX_COUNT  ( 2048 )
    uint32                 nNodeLink;
    uint32                 nPathLink;
    UINT32_PAIR             NodeLink[ WORLD__NODE_LINK_MAX_COUNT ];
    PATH_LINK__ACTIVE_DATA  PathLink[ WORLD__PATH_LINK_MAX_COUNT ];
    
#define WORLD__DOOR_MAX_COUNT  ( 512 )
    uint32     nDoor;
    WORLD__DOOR Door[ WORLD__DOOR_MAX_COUNT ];
    
#define WORLD__CHECKPOINT_MAX_COUNT  ( 128 )
    uint32           nCheckpoint;
    WORLD__CHECKPOINT Checkpoint[ WORLD__CHECKPOINT_MAX_COUNT ];
};

//----------
// PROFILE
//----------

struct PROFILE_BLOCK {
    char   Label[ 32 ];
    uint64 CycleCounter_Start;
    uint64 CycleCounter_End;
};

#define PROFILE_BLOCK_MAX_COUNT  ( 16 )
struct PROFILE_STATE {
    boo32         ProfilingIsActive;
    uint32       nProfileBlock;
    PROFILE_BLOCK ProfileBlock[ PROFILE_BLOCK_MAX_COUNT ];
};