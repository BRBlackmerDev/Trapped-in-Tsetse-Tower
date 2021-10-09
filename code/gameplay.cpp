
internal void
SpawnRoomEntities( APP_STATE * AppState ) {
#if ( !DISABLE_ENEMY_SPAWN )
    WORLD_STATE * World = &AppState->World;
    ROOM        * Room  = World->Room + AppState->iRoom;
    
    COLLISION_STATE * Collision = &AppState->Collision;
    vec4  * Edge = Collision->JumperEdge;
    uint32 nEdge = Collision->nJumperEdge;
    
    for( uint32 iType = 0; iType < EntityType_count; iType++ ) {
        uint32 nEntity = Room->nEntity[ iType ];
        switch( iType ) {
            case EntityType_Jumper: {
                for( uint32 iEntity = 0; iEntity < nEntity; iEntity++ ) {
                    uint32 iEdge = RandomU32InRange( 0, nEdge - 1 );
                    vec4    e    = Edge[ iEdge ];
                    
                    vec2 P = lerp( e.P, RandomF32(), e.Q );
                    spawnDelayedJumper( AppState, P );
                }
            } break;
        }
    }
#endif
}

internal void
OutputRoomToWorld( APP_STATE * AppState, MEMORY * TempMemory ) {
    WORLD_STATE * World = &AppState->World;
    ROOM * Room = World->Room + AppState->iRoom;
    
    { // JUMPER PATH
        PATH_STATE Path = AppState->Collision.JumperPath;
        memcpy( World->PathLink + Room->PathLink.m, Path.PathLink_ActiveData, sizeof( PATH_LINK__ACTIVE_DATA ) * Room->PathLink.n );
    }
    
    { // JUMPER
        JUMPER_STATE * State = &AppState->Jumper;
        
        UINT32_PAIR Jumper = Room->Jumper;
        if( Jumper.n != State->nEntity ) {
            int32 d = ( int32 )State->nEntity - ( int32 )Jumper.n;
            
            if( AppState->iRoom < ( World->nRoom - 1 ) ) {
                ROOM r0 = World->Room[ AppState->iRoom + 1 ];
                WORLD__JUMPER * src  = World->Jumper + r0.Jumper.m;
                WORLD__JUMPER * dest = World->Jumper + ( Jumper.m + State->nEntity );
                
                uint32 nCopy = World->nJumper - r0.Jumper.m;
                for( uint32 iCopy = 0; iCopy < nCopy; iCopy++ ) {
                    dest[ iCopy ] = src[ iCopy ];
                    
                    Assert( src [ iCopy ].iJumperEdge < 255 );
                    Assert( dest[ iCopy ].iJumperEdge < 255 );
                }
                
                for( uint32 iRoom = AppState->iRoom + 1; iRoom < World->nRoom; iRoom++ ) {
                    World->Room[ iRoom ].Jumper.m += d;
                }
            }
            
            World->nJumper += d;
            Jumper.n = State->nEntity;
        }
        
        WORLD__JUMPER * output = World->Jumper + Jumper.m;
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            JUMPER Entity = State->Entity[ iEntity ];
            
            WORLD__JUMPER out = {};
            out.health        = Entity.health;
            out.iJumperEdge   = ( uint8 )Entity.CurrentPlatform;
            Assert( out.iJumperEdge < 255 );
            
            out.hasPathToExit = setJumperHasPathToExit( AppState, TempMemory, AppState->iRoom, Entity.CurrentPlatform );
            
            output[ iEntity ] = out;
        }
        
        Room->Jumper = Jumper;
    }
    
    { // NEST
        NEST_STATE * State = &AppState->Nest;
        Assert( Room->Nest.n == State->nEntity );
        
        WORLD__NEST * NestW = World->Nest + Room->Nest.m;
        NEST        * NestG = State->Entity;
        
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            WORLD__NEST * Dest = NestW + iEntity;
            NEST          Src  = NestG[ iEntity ];
            
            Dest->Health  = Src.health;
            Dest->Timer   = Src.spawn_timer;
            Dest->isAwake = ( uint8 )Src.isAwake;
        }
    }
    
    { // MECH DOOR
        MECH_DOOR_STATE * State = &AppState->Mech;
        
        WORLD__DOOR * Door = World->Door + Room->MechDoor.m;
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            MECH_DOOR   * Entity = State->Entity + iEntity;
            WORLD__DOOR * D      = Door + iEntity;
            D->iJumperPathLink = Entity->iJumperPathLink;
            D->isClosed        = ( uint8 )Entity->isClosed;
        }
    }
    
    { // SECURE DOOR
        SECURE_DOOR_STATE * State = &AppState->Secure;
        
        WORLD__DOOR * Door = World->Door + Room->SecureDoor.m;
        for( uint32 iDoor = 0; iDoor < State->nEntity; iDoor++ ) {
            SECURE_DOOR * S = State->Entity + iDoor;
            WORLD__DOOR * D = Door + iDoor;
            D->iJumperPathLink = S->iJumperPathLink;
            D->isClosed  = ( uint8 )S->isClosed;
        }
    }
    
    { // CHECKPOINT
        if( Room->iCheckpoint > -1 ) {
            CHECKPOINT_STATE * State = &AppState->Checkpoint;
            Assert( State->Entity.DoesExist );
            World->Checkpoint[ Room->iCheckpoint ].IsActive = State->Entity.IsActive;
        }
    }
}

internal void
InputWorldToRoom( APP_STATE * AppState ) {
    WORLD_STATE * World = &AppState->World;
    ROOM * Room = World->Room + AppState->iRoom;
    
    { // JUMPER
        UINT32_PAIR Jumper = Room->Jumper;
        
        COLLISION_STATE * Collision = &AppState->Collision;
        JUMPER_STATE    * State     = &AppState->Jumper;
        
        WORLD__JUMPER * src = World->Jumper + Jumper.m;
        
        for( uint32 iEntity = 0; iEntity < Jumper.n; iEntity++ ) {
            WORLD__JUMPER j = src[ iEntity ];
            
            Assert( j.iJumperEdge < Collision->nJumperEdge );
            vec4 e = Collision->JumperEdge[ j.iJumperEdge ];
            
            vec2 P = lerp( e.P, RandomF32(), e.Q );
            spawnDelayedJumper( AppState, P, j.health );
        }
    }
    
    { // NEST
        NEST_STATE * State = &AppState->Nest;
        Assert( Room->Nest.n == State->nEntity );
        
        WORLD__NEST * NestW = World->Nest + Room->Nest.m;
        NEST        * NestG = State->Entity;
        
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            WORLD__NEST Src  = NestW[ iEntity ];
            NEST      * Dest = NestG + iEntity;
            
            Dest->health      = Src.Health;
            Dest->spawn_timer = Src.Timer;
            Dest->isAwake     = Src.isAwake;
            if( Dest->health == 0 ) {
                Dest->IsDead = true;
            }
            
            if( !Dest->IsDead ) {
                if( Dest->health < ( NEST_HEALTH / 2 ) ) {
                    Dest->nSpawn   = Dest->maxSpawn;
                    Dest->maxSpawn = NEST_JUMPER_MAX_SPAWN;
                } else {
                    Dest->maxSpawn = NEST_JUMPER_INIT_SPAWN;
                }
            }
        }
    }
    
    { // MECH DOOR
        MECH_DOOR_STATE * State = &AppState->Mech;
        
        WORLD__DOOR * Door = World->Door + Room->MechDoor.m;
        for( uint32 iDoor = 0; iDoor < State->nEntity; iDoor++ ) {
            MECH_DOOR * Entity = State->Entity + iDoor;
            WORLD__DOOR D      = Door[ iDoor ];
            Entity->iJumperPathLink = D.iJumperPathLink;
            Entity->isClosed        = D.isClosed;
        }
    }
    
    { // SECURE DOOR
        SECURE_DOOR_STATE * State = &AppState->Secure;
        
        WORLD__DOOR * Door = World->Door + Room->SecureDoor.m;
        for( uint32 iDoor = 0; iDoor < State->nEntity; iDoor++ ) {
            SECURE_DOOR * S = State->Entity + iDoor;
            WORLD__DOOR D = Door[ iDoor ];
            S->iJumperPathLink = D.iJumperPathLink;
            S->isClosed        = D.isClosed;
        }
    }
    
    { // JUMPER PATH
        // TODO: NOTE: This data is being input in GAME_loadRoom(). Is this the best place for it?
    }
    
    { // CHECKPOINT
        if( Room->iCheckpoint > -1 ) {
            CHECKPOINT_STATE * State = &AppState->Checkpoint;
            Assert( State->Entity.DoesExist );
            State->Entity.IsActive = World->Checkpoint[ Room->iCheckpoint ].IsActive;
        }
    }
}

internal void
ResetWorldMemory( APP_STATE * AppState ) {
    memset( &AppState->World,      0, sizeof( WORLD_STATE        ) );
    memset( &AppState->Event,      0, sizeof( EVENT_STATE        ) );
}

internal void
ResetPlayerMemory( APP_STATE * AppState ) {
    memset( &AppState->Player,     0, sizeof( PLAYER_STATE       ) );
}

internal void
ResetRoomMemory( APP_STATE * AppState ) {
    reset( &AppState->Collision_memory );
    memset( &AppState->Collision,  0, sizeof( COLLISION_STATE    ) );
    memset( &AppState->Camera,     0, sizeof( CAMERA_STATE       ) );
    memset( &AppState->terrain,    0, sizeof( TERRAIN_STATE      ) );
    memset( &AppState->Jumper,     0, sizeof( JUMPER_STATE       ) );
    memset( &AppState->Grenade,    0, sizeof( GRENADE_STATE      ) );
    memset( &AppState->Exit,       0, sizeof( EXIT_STATE         ) );
    memset( &AppState->replay,     0, sizeof( REPLAY_STATE       ) );
    memset( &AppState->Nest,       0, sizeof( NEST_STATE         ) );
    memset( &AppState->Secure,     0, sizeof( SECURE_DOOR_STATE  ) );
    memset( &AppState->Mech,       0, sizeof( MECH_DOOR_STATE    ) );
    memset( &AppState->power,      0, sizeof( POWER_SWITCH_STATE ) );
    memset( &AppState->Checkpoint, 0, sizeof( CHECKPOINT_STATE   ) );
}

internal void
SaveGame( APP_STATE * AppState, MEMORY * TempMemory ) {
    OutputRoomToWorld( AppState, TempMemory );
    
    WORLD_STATE * World = &AppState->World;
    WORLD_STATE * Save  = &AppState->World_Save;
    memcpy( Save, World, sizeof( WORLD_STATE ) );
    
    // TODO: write world_save to file
    // TODO: save events
}

internal void
InitCameraForRoom( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    CAMERA_STATE * Camera = &AppState->Camera;
    
    flo32 aspectRatio = 1920.0f / 1080.0f;
    flo32 dimY = CAMERA_TILE_Y_COUNT * TILE_HEIGHT;
    flo32 dimX = dimY * aspectRatio;
    Camera->dim = Vec2( dimX, dimY );
    
    vec2 PlayerP = GetPlayerCameraP( *Player );
    Camera->pos  = clampToBound( PlayerP, AppState->Camera_Bound );
}

internal vec2
GetCheckpointPOfCurrentRoom( APP_STATE * AppState ) {
    CHECKPOINT_STATE * State  = &AppState->Checkpoint;
    CHECKPOINT         Entity = State->Entity;
    
    //Assert( ( Entity.DoesExist ) && ( Entity.IsActive ) );
    Assert( ( Entity.DoesExist ) );
    vec2 Result = getBC( Entity.Bound );
    return Result;
}

internal void
LoadCheckpoint( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    ResetPlayerMemory( AppState );
    ResetRoomMemory  ( AppState );
    // reset events? probably not, i think i'm just adding additional events to handle cases of death (is there a better way?)
    
    WORLD_STATE * World = &AppState->World;
    WORLD_STATE * Save  = &AppState->World_Save;
    memcpy( World, Save, sizeof( WORLD_STATE ) );
    
    strcpy( AppState->CurrentRoom, AppState->LoadCheckpoint_Name );
    AppState->iRoom = AppState->LoadCheckpoint_iRoom;
    
    GAME_loadRoom( Platform, AppState, TempMemory, ROOM_SAVE_DIRECTORY, AppState->CurrentRoom );
    InputWorldToRoom( AppState );
    
    PLAYER_STATE * Player = &AppState->Player;
    Player->Position = GetCheckpointPOfCurrentRoom( AppState );
    Player->Height   = PLAYER_HEIGHT_STANDING;
    Player->health   = PLAYER_HEALTH;
    Player->FaceDir = Vec2( 1.0f, 0.0f );
    
    int32 CurrentPlatform = getCurrentPlatform( AppState );
    Assert( CurrentPlatform > -1 );
    Player->ChangePlatform  = true;
    Player->CurrentPlatform = CurrentPlatform;
    
    InitCameraForRoom( AppState );
}

internal void
startGame( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    ResetWorldMemory ( AppState );
    ResetPlayerMemory( AppState );
    ResetRoomMemory  ( AppState );
    
    AppState->Mode = appMode_game;
    
#if START_IN_ROOM
    AppState->testRoom = true;
    strcpy( AppState->testRoom_name, START_IN_ROOM_NAME );
#endif
    
    if( AppState->testRoom ) {
        WORLD_STATE * World = &AppState->World;
        World->nRoom = 1;
        
        ROOM * Room = World->Room;
        strcpy( Room->name, AppState->testRoom_name );
        strcpy( AppState->CurrentRoom, AppState->testRoom_name );
        
        ROOM_loadEntityCount( Platform, AppState, TempMemory, Room->name, 0 );
        
#if 0
        { // power switch
            POWER_SWITCH_STATE * State = &AppState->power;
            
            POWER_SWITCH Entity = {};
            Entity.isEnabled = true;
            Entity.Bound     = Rect( 519.5f, 507.5f, 520.75f, 508.25f );
            State->Entity[ State->nEntity++ ] = Entity;
        }
#endif
        
        ROOM_loadEvents( Platform, AppState, TempMemory, Room->name, 0 );
        
        EVENT_STATE * Event = &AppState->Event;
        Event->Event[ 0 ].func = Nest01_Event01;
    } else {
        GAME_loadLayout( Platform, AppState, TempMemory, "layout01" );
        
        WORLD_STATE * World = &AppState->World;
        ROOM Room = World->Room[ World->iRoomToStartGame ];
        strcpy( AppState->CurrentRoom, Room.name );
        AppState->iRoom = getIRoom( AppState, AppState->CurrentRoom );
        
        uint32 nEvent = 0;
        EVENT_STATE * Event = &AppState->Event;
        Event->Event[ nEvent++ ].func = Nest01_Event01;
        Event->Event[ nEvent++ ].func = Nest02_Event01;
        Event->Event[ nEvent++ ].func = Nest03_Event01;
        Event->Event[ nEvent++ ].func = Nest04_Event01;
        Event->Event[ nEvent++ ].func = Nest05_Event01;
        Event->Event[ nEvent++ ].func = shaft01_Event01;
        Event->Event[ nEvent++ ].func = Tut08_Event01;
        Assert( nEvent < EVENT_MAX_COUNT );
        // TODO: Do these items just need to be in alphabetical order?
        
        World->power_isOn[ 0 ] = true;
    }
    
    GAME_loadRoom( Platform, AppState, TempMemory, ROOM_SAVE_DIRECTORY, AppState->CurrentRoom );
    //SpawnRoomEntities( AppState );
    InputWorldToRoom( AppState );
    
    PLAYER_STATE * Player = &AppState->Player;
    Player->Position = AppState->PlayerSpawn_Position;
    Player->Height   = PLAYER_HEIGHT_STANDING;
    Player->health   = PLAYER_HEALTH;
    Player->FaceDir = Vec2( 1.0f, 0.0f );
    
    if( !AppState->stat.isTopDown ) {
        int32 CurrentPlatform = getCurrentPlatform( AppState );
        Assert( CurrentPlatform > -1 );
        Player->ChangePlatform  = true;
        Player->CurrentPlatform = CurrentPlatform;
    }
    
    BEAST_STATE * Beast = &AppState->Beast;
    Beast->Entity.Position = Vec2( 503.0f, 512.0f );
    
    PANIC_POST_STATE * PanicPost = &AppState->PanicPost;
    PANIC_POST Post = {};
    Post.Position = Vec2( 506.25f, 513.5f );
    PanicPost->PanicPost[ PanicPost->nPanicPost++ ] = Post;
    
    InitCameraForRoom( AppState );
}

internal void
ChangeRoom( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    ResetRoomMemory( AppState );
    
    AppState->ChangeRoom = false;
    
    strcpy( AppState->CurrentRoom, AppState->ChangeRoom_Name );
    AppState->iRoom = getIRoom( AppState, AppState->CurrentRoom );
    
    GAME_loadRoom( Platform, AppState, TempMemory, ROOM_SAVE_DIRECTORY, AppState->CurrentRoom );
    
    //SpawnRoomEntities( AppState );
    
    EXIT_STATE * Exit = &AppState->Exit;
    EXIT e = Exit->Entity[ AppState->ChangeRoom_iExit ];
    
    PLAYER_STATE * Player = &AppState->Player;
    Player->Position = getEnterP( e, PLAYER_HEIGHT_STANDING );
    Player->Velocity = {};
    Player->ChangePlatform  = true;
    Player->CurrentPlatform = AppState->ChangeRoom_iJumperEdge;
    
    InitCameraForRoom( AppState );
}