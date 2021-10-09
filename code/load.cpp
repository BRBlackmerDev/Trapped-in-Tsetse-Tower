
internal void
GAME_loadRoomStats( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1:
            case 2: 
            case 3:
            case 4:
            case 5:
            case 6:
            case 7: {
                ROOM_STATS stat = _read( ptr, ROOM_STATS );
                AppState->stat = stat;
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadPlayerSpawn( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                uint32 nSpawn = _read( ptr, uint32 );
                
                Assert( nSpawn == 1 );
                vec2 P = _read( ptr, vec2 );
                
                AppState->PlayerSpawn_Position = P;
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadTerrain( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                uint32       nTerrainPair = _read( ptr, uint32 );
                UINT32_PAIR * terrainPair = ( UINT32_PAIR * )ptr;
                
                for( uint32 iTerrain = 0; iTerrain < nTerrainPair; iTerrain++ ) {
                    UINT32_PAIR t = terrainPair[ iTerrain ];
                    addTerrain( AppState, t.x, t.y );
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadExit( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                EXIT_STATE * State = &AppState->Exit;
                State->nEntity = _read( ptr, uint32 );
                for( uint32 iExit = 0; iExit < State->nEntity; iExit++ ) {
                    EXIT Exit = {};
                    Exit.Bound = _read( ptr, rect );
                    Exit.Type  = _read( ptr, EXIT_TYPE );
                    
                    State->Entity[ iExit ] = Exit;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadNest( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1:
            case 2: {
                NEST_STATE * State = &AppState->Nest;
                State->nEntity = _read( ptr, uint32 );
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    NEST Entity = {};
                    Entity.isAwake = false;
                    Entity.Bound   = _read( ptr, rect        );
                    Entity.Type    = _read( ptr, ENTITY_TYPE );
                    Entity.health  = NEST_HEALTH;
                    
                    State->Entity[ iEntity ] = Entity;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadPowerSwitch( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                POWER_SWITCH_STATE * State = &AppState->power;
                State->nEntity = _read( ptr, uint32 );
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    POWER_SWITCH Entity = {};
                    Entity.Bound   = _read( ptr, rect   );
                    Entity.iPower  = _read( ptr, uint32 );
                    
                    Entity.isEnabled = true;
                    
                    State->Entity[ iEntity ] = Entity;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadCollisionTerrain( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                COLLISION_STATE * Collision = &AppState->Collision;
                for( uint32 iType = 0; iType < 4; iType++ ) {
                    Collision->Type[ iType ] = _read( ptr, UINT32_PAIR );
                }
                Collision->nEdge = _read( ptr, uint32 );
                Collision->Edge  = _copya( &AppState->Collision_memory, ptr, vec4, Collision->nEdge );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadCollisionJump( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                COLLISION_STATE * Collision = &AppState->Collision;
                Collision->nJumpBound = _read( ptr, uint32 );
                Collision->jumpBound  = _copya( &AppState->Collision_memory, ptr, rect, Collision->nJumpBound );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadWallSlide( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                COLLISION_STATE * Collision = &AppState->Collision;
                Collision->nWallSlideLeft  = _read( ptr, uint32 );
                Collision->nWallSlideRight = _read( ptr, uint32 );
                Collision->wallSlideLeft = _copya( &AppState->Collision_memory, ptr, rect, Collision->nWallSlideLeft );
                Collision->wallSlideRight = _copya( &AppState->Collision_memory, ptr, rect, Collision->nWallSlideRight );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadJumperBound( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                COLLISION_STATE * Collision = &AppState->Collision;
                Collision->nJumperBound = _read( ptr, uint32 );
                Collision->JumperBound  = _copya( &AppState->Collision_memory, ptr, rect, Collision->nJumperBound );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadJumperEdge( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                COLLISION_STATE * Collision = &AppState->Collision;
                Collision->nJumperEdge = _read( ptr, uint32 );
                Collision->JumperEdge  = _copya( &AppState->Collision_memory, ptr, vec4, Collision->nJumperEdge );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadJumperPathPositionData( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename ) {
    char filetag[ 16 ] = {};
    sprintf( filetag, "%s%s", FILETAG_PREFIX__JUMPER, FILETAG__PATHS__POSITION_DATA );
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                PATH_STATE * Path = &AppState->Collision.JumperPath;
                
                uint32 nPathLink = _read( ptr, uint32 );
                Assert( nPathLink == Path->nPathLink );
                
                Path->PathLink_PositionData = _copya( &AppState->Collision_memory, ptr, PATH_LINK__POSITION_DATA, nPathLink );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadJumperPathInData( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename ) {
    char filetag[ 16 ] = {};
    sprintf( filetag, "%s%s", FILETAG_PREFIX__JUMPER, FILETAG__PATHS__IN_DATA );
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                PATH_STATE * Path = &AppState->Collision.JumperPath;
                
                uint32 nNodeLink = _read( ptr, uint32 );
                Assert( nNodeLink == Path->nNodeLink );
                
                uint32 nPathLink = _read( ptr, uint32 );
                Assert( nPathLink == Path->nPathLink );
                
                Path->NodeLinkIn      = _copya( &AppState->Collision_memory, ptr, UINT32_PAIR, nNodeLink );
                Path->PathLink_InData = _copya( &AppState->Collision_memory, ptr, PATH_LINK__IN_DATA, nPathLink );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadRoom( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename ) {
    // TODO: How should I handle GAME_loading previous versions? Should I send an error message? Where should I send it?
    
    GAME_loadRoomStats( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__ROOM_STATS );
    GAME_loadPlayerSpawn( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__PLAYER_SPAWN );
    GAME_loadTerrain( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__TERRAIN );
    
    GAME_loadCollisionTerrain( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__COLLISION_TERRAIN );
    GAME_loadCollisionJump( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__COLLISION_JUMP );
    GAME_loadWallSlide( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__WALL_SLIDE );
    GAME_loadJumperBound( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__JUMPER_BOUND );
    GAME_loadJumperEdge( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__JUMPER_EDGE );
    
    // TODO: are we testing a Room  or  playing the game. Right now, we're assuming that we're playing the game
    
    WORLD_STATE * World = &AppState->World;
    ROOM Room = World->Room[ AppState->iRoom ];
    
    COLLISION_STATE * Collision = &AppState->Collision;
    MEMORY * memory = &AppState->Collision_memory;
    
    PATH_STATE * Path = &Collision->JumperPath;
    Path->nNodeLink = Room.NodeLink.n;
    Path->NodeLink  = _pushArray( memory, UINT32_PAIR, Path->nNodeLink );
    memcpy( Path->NodeLink, World->NodeLink + Room.NodeLink.m, sizeof( UINT32_PAIR ) * Path->nNodeLink );
    
    Path->nPathLink = Room.PathLink.n;
    Path->PathLink_ActiveData = _pushArray( memory, PATH_LINK__ACTIVE_DATA, Path->nPathLink );
    memcpy( Path->PathLink_ActiveData, World->PathLink + Room.PathLink.m, sizeof( PATH_LINK__ACTIVE_DATA ) * Path->nPathLink );
    
    GAME_loadJumperPathPositionData( Platform, AppState, TempMemory, SaveDir, filename );
    GAME_loadJumperPathInData      ( Platform, AppState, TempMemory, SaveDir, filename );
    
    ROOM_STATS Stat = AppState->stat;
    if( Stat.isTopDown ) {
        GAME_LoadCellGrid( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__CELL_GRID );
    }
    
    GAME_loadExit( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__EXIT_EDIT );
    GAME_loadNest( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__NEST );
    GAME_loadSecureDoor( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__SECURE_DOOR );
    GAME_loadMechDoor( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__MECH_DOOR );
    GAME_loadPowerSwitch( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__POWER_SWITCH );
    GAME_LoadCheckpoint( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__CHECKPOINT );
    
    
    
    { // set Camera Bound
        TERRAIN_STATE * terrain = &AppState->terrain;
        
        rect Bound = { FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX };
        for( uint32 iTerrain = 0; iTerrain < terrain->nTerrain; iTerrain++ ) {
            TERRAIN t = terrain->terrain[ iTerrain ];
            
            Bound.Left   = minValue( Bound.Left,   t.Bound.Left   );
            Bound.Bottom = minValue( Bound.Bottom, t.Bound.Bottom );
            Bound.Right  = maxValue( Bound.Right,  t.Bound.Right  );
            Bound.Top    = maxValue( Bound.Top,    t.Bound.Top    );
        }
        
        vec2 Center   = getCenter( Bound );
        vec2 dimA     = getDim( Bound );
        vec2 halfDimA = dimA * 0.5f;
        
        flo32 aspectRatio = 1920.0f / 1080.0f;
        flo32 y = CAMERA_TILE_Y_COUNT * TILE_HEIGHT;
        flo32 x = y * aspectRatio;
        
        vec2 dimB     = Vec2( x, y );
        vec2 halfDimB = dimB * 0.5f;
        
        if( dimB.x < dimA.x ) {
            Bound.Left  += halfDimB.x;
            Bound.Right -= halfDimB.x;
        } else {
            Bound.Left  = Center.x;
            Bound.Right = Center.x;
        }
        if( dimB.y < dimA.y ) {
            Bound.Bottom += halfDimB.y;
            Bound.Top    -= halfDimB.y;
        } else {
            Bound.Bottom = Center.y;
            Bound.Top    = Center.y;
        }
        
        AppState->Camera_Bound = Bound;
    }
}

internal int32
getIRoom( APP_STATE * AppState, char * CurrentRoom ) {
    WORLD_STATE * World = &AppState->World;
    
    int32 result = -1;
    for( uint32 iRoom = 0; iRoom < World->nRoom; iRoom++ ) {
        ROOM Room = World->Room[ iRoom ];
        if( matchString( Room.name, CurrentRoom ) ) {
            result = iRoom;
        }
    }
    
    return result;
}

internal void
GAME_loadRoomNames( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * filename ) {
    char * SaveDir = ROOM_SAVE_DIRECTORY;
    char * filetag = FILETAG__LAYOUT_ROOM_NAMES;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                WORLD_STATE * World = &AppState->World;
                
                World->nRoom = _read( ptr, uint32 );
                for( uint32 iRoom = 0; iRoom < World->nRoom; iRoom++ ) {
                    ROOM * Room = World->Room + iRoom;
                    
                    char * name = _addrStr( ptr );
                    strcpy( Room->name, name );
                }
                
                char * RoomToStartGame = _addrStr( ptr );
                int32 iRoomToStartGame = getIRoom( AppState, RoomToStartGame );
                if( iRoomToStartGame > -1 ) {
                    World->iRoomToStartGame = iRoomToStartGame;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
ROOM_loadEntityCount( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * filename, uint32 iRoom ) {
    WORLD_STATE * World = &AppState->World;
    ROOM        * Room  = World->Room + iRoom;
    
    char * SaveDir = ROOM_SAVE_DIRECTORY;
    char * filetag = FILETAG__ENTITY_COUNT;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                uint32 nType = _read( ptr, uint32 );
                if( nType != EntityType_count ) {
                    globalVar_debugSystem.errorOccurred = true;
                    char str[ 512 ] = {};
                    sprintf( str, "ERROR! When loading %s in the game, nType != EntityType_count: nType = %u, EntityType_count = %u", Room->name, nType, EntityType_count );
                    CONSOLE_STRING( str );
                }
                
                ENTITY_TYPE_TAGS;
                
                for( uint32 iType = 0; iType < nType; iType++ ) {
                    char * tag     = _addra( ptr, char, 4 );
                    uint32 nEntity = _read( ptr, uint32 );
                    
                    int32 i = -1;
                    for( uint32 iTag = 0; iTag < EntityType_count; iTag++ ) {
                        if( matchSegment( tag, EntityTypeTag[ iTag ], 4 ) ) {
                            i = iTag;
                        }
                    }
                    if( i > -1 ) {
                        Room->nEntity[ i ] = nEntity;
                    }
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadEntityCount( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    WORLD_STATE * World = &AppState->World;
    for( uint32 iRoom = 0; iRoom < World->nRoom; iRoom++ ) {
        ROOM * Room = World->Room + iRoom;
        ROOM_loadEntityCount( Platform, AppState, TempMemory, Room->name, iRoom );
    }
}

internal void
ROOM_loadEvents( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * filename, uint32 iRoom ) {
    WORLD_STATE * World = &AppState->World;
    ROOM        * Room  = World->Room + iRoom;
    
    EVENT_STATE * Event  = &AppState->Event;
    MEMORY      * memory = &AppState->Event_memory;
    
    char * SaveDir = ROOM_SAVE_DIRECTORY;
    char * filetag = FILETAG__EVENTS_GAME;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                // TODO: error checking and verification
                
                uint32 nEvent = _read( ptr, uint32 );
                
                Room->Event = UInt32Pair( Event->nEvent, nEvent );
                Event->nEvent += nEvent;
                
                for( uint32 iEvent = 0; iEvent < nEvent; iEvent++ ) {
                    EVENT * e = Event->Event + ( Room->Event.m + iEvent );
                    e->IsActive = _read( ptr, boo32 );
                    
                    uint32 data_size = _read( ptr, uint32 );
                    uint8 * src  = _addra( ptr, uint8, data_size );
                    uint8 * dest = ( uint8 * )_pushSize( memory, data_size );
                    memcpy( dest, src, data_size );
                    
                    e->data = dest;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
ROOM_loadJumperInit( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 iRoom ) {
    WORLD_STATE * World = &AppState->World;
    ROOM        * Room  = World->Room + iRoom;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                uint32 nEntity = _read( ptr, uint32 );
                Room->Jumper = UInt32Pair( World->nJumper, nEntity );
                
                for( uint32 iEntity = 0; iEntity < nEntity; iEntity++ ) {
                    EDITOR__JUMPER src  = _read( ptr, EDITOR__JUMPER );
                    Assert( src.iJumperEdge < 255 );
                    
                    WORLD__JUMPER  dest = {};
                    dest.health        = JUMPER_HEALTH;
                    dest.iJumperEdge   = src.iJumperEdge;
                    dest.hasPathToExit = src.hasPathToExit;
                    
                    World->Jumper[ World->nJumper++ ] = dest;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
ROOM_loadNestInit( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 iRoom ) {
    WORLD_STATE * World = &AppState->World;
    ROOM        * Room  = World->Room + iRoom;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                InvalidCodePath;
                
                uint32 nEntity = _read( ptr, uint32 );
                Room->Nest = UInt32Pair( World->nNest, nEntity );
                
                for( uint32 iEntity = 0; iEntity < nEntity; iEntity++ ) {
                    EDITOR__NEST Src = {};
                    Src.Bound       = _read( ptr, rect );
                    Src.Type        = _read( ptr, ENTITY_TYPE );
                    
                    WORLD__NEST Dest = {};
                    Dest.Health        = NEST_HEALTH;
                    Dest.isAwake       = ( uint8 )Src.isAwake;
                    Dest.iJumperEdge   = ( uint8 )Src.iJumperEdge;
                    // NOTE: This value is always 0, which breaks the system. There are no more Nests of Version_01, but I'm just keeping this code around for whatever.
                    
                    World->Nest[ World->nNest++ ] = Dest;
                }
            } break;
            
            case 2: {
                uint32 nEntity = _read( ptr, uint32 );
                Room->Nest = UInt32Pair( World->nNest, nEntity );
                
                for( uint32 iEntity = 0; iEntity < nEntity; iEntity++ ) {
                    EDITOR__NEST Src = {};
                    Src.Bound       = _read( ptr, rect );
                    Src.Type        = _read( ptr, ENTITY_TYPE );
                    Src.isAwake     = _read( ptr, boo32 );
                    Src.iJumperEdge = _read( ptr, uint32 );
                    
                    Assert( Src.isAwake     < 255 );
                    Assert( Src.iJumperEdge < 255 );
                    
                    WORLD__NEST Dest = {};
                    Dest.Health        = NEST_HEALTH;
                    Dest.isAwake       = ( uint8 )Src.isAwake;
                    Dest.iJumperEdge   = ( uint8 )Src.iJumperEdge;
                    
                    World->Nest[ World->nNest++ ] = Dest;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadEvents( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    WORLD_STATE * World = &AppState->World;
    for( uint32 iRoom = 0; iRoom < World->nRoom; iRoom++ ) {
        ROOM * Room = World->Room + iRoom;
        ROOM_loadEvents( Platform, AppState, TempMemory, Room->name, iRoom );
    }
}

internal void
GAME_loadStats( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    WORLD_STATE * World = &AppState->World;
    for( uint32 iRoom = 0; iRoom < World->nRoom; iRoom++ ) {
        ROOM * Room = World->Room + iRoom;
        GAME_loadRoomStats( Platform, AppState, TempMemory, ROOM_SAVE_DIRECTORY, Room->name, FILETAG__ROOM_STATS );
        
        ROOM_STATS stat = AppState->stat;
        
        Room->nExit = stat.nExit;
        memcpy( Room->Exit_iJumperEdge_Exit,  stat.Exit_iJumperEdge_Exit,  sizeof( int32 ) * Room->nExit );
        memcpy( Room->Exit_iJumperEdge_Enter, stat.Exit_iJumperEdge_Enter, sizeof( int32 ) * Room->nExit );
        
        Room->NodeLink    = UInt32Pair( World->nNodeLink, stat.nNodeLink );
        World->nNodeLink += stat.nNodeLink;
        Assert( World->nNodeLink < WORLD__NODE_LINK_MAX_COUNT );
        
        Room->PathLink    = UInt32Pair( World->nPathLink, stat.nPathLink );
        World->nPathLink += stat.nPathLink;
        Assert( World->nPathLink < WORLD__PATH_LINK_MAX_COUNT );
        
        ROOM_loadJumperInit( Platform, AppState, TempMemory, ROOM_SAVE_DIRECTORY, Room->name, FILETAG__JUMPER_INIT, iRoom );
        
        ROOM_loadNestInit( Platform, AppState, TempMemory, ROOM_SAVE_DIRECTORY, Room->name, FILETAG__NEST, iRoom );
        ROOM_loadMechInit( Platform, AppState, TempMemory, ROOM_SAVE_DIRECTORY, Room->name, FILETAG__MECH_DOOR, iRoom );
        ROOM_loadSecureInit( Platform, AppState, TempMemory, ROOM_SAVE_DIRECTORY, Room->name, FILETAG__SECURE_DOOR, iRoom );
        ROOM_LoadCheckpointInit( Platform, AppState, TempMemory, ROOM_SAVE_DIRECTORY, Room->name, FILETAG__CHECKPOINT, iRoom );
    }
}

internal void
GAME_loadWorldExits( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * filename ) {
    char * SaveDir = ROOM_SAVE_DIRECTORY;
    char * filetag = FILETAG__EXIT_GAME;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                WORLD_STATE * World = &AppState->World;
                
                uint32 nRoom = _read( ptr, uint32 );
                Assert( nRoom == World->nRoom );
                
                for( uint32 iRoom = 0; iRoom < nRoom; iRoom++ ) {
                    ROOM * Room = World->Room + iRoom;
                    
                    char * name = _addrStr( ptr );
                    Assert( matchString( Room->name, name ) );
                    
                    uint32 nExit = _read( ptr, uint32 );
                    Assert( nExit == Room->nExit );
                    
                    for( uint32 iExit = 0; iExit < nExit; iExit++ ) {
                        WORLD__EXIT * Exit = Room->Exit + iExit;
                        
                        INT32_PAIR link = _read( ptr, INT32_PAIR );
                        if( ( link.m > -1 ) && ( link.n > -1 ) ) {
                            char * toRoom_name = _addrStr( ptr );
                            strcpy( Exit->toRoom_name, toRoom_name );
                            
                            int32 toRoom_iRoom = getIRoom( AppState, Exit->toRoom_name );
                            Assert( toRoom_iRoom > -1 );
                            Exit->toRoom_iRoom = ( uint32 )toRoom_iRoom;
                            
                            Exit->toRoom_iExit       = _read( ptr, uint32 );
                            Exit->toRoom_iJumperEdge = _read( ptr, uint32 );
                            
                            Exit->isValid = true;
                        }
                    }
                }
                
                char * RoomToStartGame = _addrStr( ptr );
                int32 iRoomToStartGame = getIRoom( AppState, RoomToStartGame );
                if( iRoomToStartGame > -1 ) {
                    World->iRoomToStartGame = iRoomToStartGame;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadJumperPaths( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * filename ) {
    char * SaveDir = ROOM_SAVE_DIRECTORY;
    
    char filetag[ 16 ] = {};
    sprintf( filetag, "%s%s", FILETAG_PREFIX__JUMPER, FILETAG__PATHS__ACTIVE_DATA );
    
    WORLD_STATE * World = &AppState->World;
    for( uint32 iRoom = 0; iRoom < World->nRoom; iRoom++ ) {
        ROOM * Room = World->Room + iRoom;
        
        FILE_DATA file = Platform->readFile( TempMemory, SaveDir, Room->name, filetag );
        if( file.contents ) {
            uint8 * ptr = ( uint8 * )file.contents;
            
            uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
            switch( version ) {
                case 1: {
                    uint32 nNodeLink = _read( ptr, uint32 );
                    uint32 nPathLink = _read( ptr, uint32 );
                    Assert( nNodeLink == Room->NodeLink.n );
                    Assert( nPathLink == Room->PathLink.n );
                    
                    memcpy( World->NodeLink + Room->NodeLink.m, ptr, sizeof( UINT32_PAIR ) * nNodeLink );
                    ptr += ( sizeof( UINT32_PAIR ) * nNodeLink );
                    
                    memcpy( World->PathLink + Room->PathLink.m, ptr, sizeof( PATH_LINK__ACTIVE_DATA ) * nPathLink );
                    ptr += ( sizeof( PATH_LINK__ACTIVE_DATA ) * nPathLink );
                } break;
                
                default: {
                    InvalidCodePath;
                } break;
            }
            
            _popSize( TempMemory, file.size );
        }
    }
}

internal void
GAME_loadLayout( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * filename ) {
    GAME_loadRoomNames( Platform, AppState, TempMemory, filename );
    GAME_loadEntityCount( Platform, AppState, TempMemory );
    GAME_loadEvents( Platform, AppState, TempMemory );
    GAME_loadStats( Platform, AppState, TempMemory );
    GAME_loadWorldExits( Platform, AppState, TempMemory, filename );
    GAME_loadJumperPaths( Platform, AppState, TempMemory, filename );
}

//----------
// EDITOR LOAD ENTITY
//----------

internal void
EDITOR_loadPlayerSpawn( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        EDITOR_STATE * Editor = &AppState->Editor;
        
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                uint32 nSpawn = _read( ptr, uint32 );
                
                Assert( nSpawn == 1 );
                vec2 P = _read( ptr, vec2 );
                
                Editor->Player_nSpawnPos     = nSpawn;
                Editor->Player_SpawnPos[ 0 ] = P;
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
EDITOR_loadTerrain( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        EDITOR_STATE * Editor = &AppState->Editor;
        
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                uint32 nTerrainPair = _read( ptr, uint32 );
                for( uint32 iTerrainPair = 0; iTerrainPair < nTerrainPair; iTerrainPair++ ) {
                    UINT32_PAIR terrainPair = _read( ptr, UINT32_PAIR );
                    
                    Editor->grid[ terrainPair.y ][ terrainPair.x ] = CellType_Terrain;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
EDITOR_loadExit( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        EDITOR_STATE * Editor = &AppState->Editor;
        EDITOR__EXIT_STATE * State = &Editor->Exit;
        
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                State->nEntity = _read( ptr, uint32 );
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    EDITOR__EXIT Entity = {};
                    Entity.Bound = _read( ptr, rect );
                    Entity.Type  = _read( ptr, EXIT_TYPE );
                    State->Entity[ iEntity ] = Entity;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
EDITOR_loadNest( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        EDITOR_STATE       * Editor = &AppState->Editor;
        EDITOR__NEST_STATE * State  = &Editor->Nest;
        
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                State->nEntity = _read( ptr, uint32 );
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    EDITOR__NEST Entity = {};
                    Entity.Bound       = _read( ptr, rect );
                    Entity.Type        = _read( ptr, ENTITY_TYPE );
                    
                    State->Entity[ iEntity ] = Entity;
                }
            } break;
            
            case 2: {
                State->nEntity = _read( ptr, uint32 );
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    EDITOR__NEST Entity = {};
                    Entity.Bound       = _read( ptr, rect );
                    Entity.Type        = _read( ptr, ENTITY_TYPE );
                    Entity.isAwake     = _read( ptr, boo32 );
                    Entity.iJumperEdge = _read( ptr, uint32 );
                    
                    State->Entity[ iEntity ] = Entity;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
EDITOR_loadPowerSwitch( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    EDITOR__POWER_SWITCH_STATE * State = &AppState->Editor.power;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        EDITOR_STATE * Editor = &AppState->Editor;
        
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                State->nEntity = _read( ptr, uint32 );
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    State->Entity[ iEntity ] = _read( ptr, EDITOR__POWER_SWITCH );
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
EDITOR_loadRoomStats( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        EDITOR_STATE * Editor = &AppState->Editor;
        
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                ROOM_STATS stat = {};
                stat.isTopDown = _read( ptr, boo32 );
                stat.Bound     = _read( ptr, rect  );
                stat.intCell   = _read( ptr, UINT32_PAIR );
                Editor->stat = stat;
            } break;
            
            case 2: {
                ROOM_STATS stat = {};
                stat.isTopDown = _read( ptr, boo32 );
                stat.Bound     = _read( ptr, rect  );
                stat.intCell   = _read( ptr, UINT32_PAIR );
                
                stat.nExit     = _read( ptr, uint32 );
                for( uint32 iExit = 0; iExit < stat.nExit; iExit++ ) {
                    stat.Exit_iJumperEdge_Exit[ iExit ] = _read( ptr, uint32 );
                }
                
                stat.nPathLink   = _read( ptr, uint32 );
                stat.nMechDoor   = _read( ptr, uint32 );
                stat.nSecureDoor = _read( ptr, uint32 );
                
                Editor->stat = stat;
            } break;
            
            case 3: {
                ROOM_STATS stat = {};
                stat.isTopDown = _read( ptr, boo32 );
                stat.Bound     = _read( ptr, rect );
                stat.intCell   = _read( ptr, UINT32_PAIR );
                
                stat.nExit     = _read( ptr, uint32 );
                for( uint32 iExit = 0; iExit < stat.nExit; iExit++ ) {
                    stat.Exit_iJumperEdge_Exit[ iExit ] = _read( ptr, uint32 );
                }
                
                stat.nPathLink   = _read( ptr, uint32 );
                stat.nMechDoor   = _read( ptr, uint32 );
                stat.nSecureDoor = _read( ptr, uint32 );
                
                Editor->stat = stat;
                
                { // MECH DOOR
                    EDITOR__MECH_DOOR_STATE * State = &Editor->Mech;
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        _read( ptr, UINT32_PAIR );
                    }
                }
                
                { // SECURE DOOR
                    EDITOR__SECURE_DOOR_STATE * State = &Editor->Secure;
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        _read( ptr, UINT32_PAIR );
                    }
                }
            } break;
            
            case 4: {
                ROOM_STATS stat = {};
                stat.isTopDown = _read( ptr, boo32 );
                stat.Bound     = _read( ptr, rect );
                stat.intCell   = _read( ptr, UINT32_PAIR );
                
                stat.nExit     = _read( ptr, uint32 );
                for( uint32 iExit = 0; iExit < stat.nExit; iExit++ ) {
                    stat.Exit_iJumperEdge_Exit [ iExit ] = _read( ptr, uint32 );
                    stat.Exit_iJumperEdge_Enter[ iExit ] = stat.Exit_iJumperEdge_Exit[ iExit ];
                }
                
                stat.nNodeLink   = _read( ptr, uint32 );
                stat.nPathLink   = _read( ptr, uint32 );
                stat.nMechDoor   = _read( ptr, uint32 );
                stat.nSecureDoor = _read( ptr, uint32 );
                
                Editor->stat = stat;
                
                { // MECH DOOR
                    EDITOR__MECH_DOOR_STATE * State = &Editor->Mech;
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        _read( ptr, UINT32_PAIR );
                    }
                }
                
                { // SECURE DOOR
                    EDITOR__SECURE_DOOR_STATE * State = &Editor->Secure;
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        _read( ptr, UINT32_PAIR );
                    }
                }
            } break;
            
            case 5: {
                ROOM_STATS stat = {};
                stat.isTopDown = _read( ptr, boo32 );
                stat.Bound     = _read( ptr, rect );
                stat.intCell   = _read( ptr, UINT32_PAIR );
                
                stat.nExit     = _read( ptr, uint32 );
                for( uint32 iExit = 0; iExit < stat.nExit; iExit++ ) {
                    stat.Exit_iJumperEdge_Exit [ iExit ] = _read( ptr, int32 );
                    stat.Exit_iJumperEdge_Enter[ iExit ] = _read( ptr, int32 );
                }
                
                stat.nNodeLink   = _read( ptr, uint32 );
                stat.nPathLink   = _read( ptr, uint32 );
                stat.nMechDoor   = _read( ptr, uint32 );
                stat.nSecureDoor = _read( ptr, uint32 );
                
                Editor->stat = stat;
                
                { // MECH DOOR
                    EDITOR__MECH_DOOR_STATE * State = &Editor->Mech;
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        _read( ptr, UINT32_PAIR );
                    }
                }
                
                { // SECURE DOOR
                    EDITOR__SECURE_DOOR_STATE * State = &Editor->Secure;
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        _read( ptr, UINT32_PAIR );
                    }
                }
            } break;
            
            case 6: {
                ROOM_STATS stat = {};
                stat.isTopDown = _read( ptr, boo32 );
                stat.Bound     = _read( ptr, rect );
                stat.intCell   = _read( ptr, UINT32_PAIR );
                
                stat.nExit     = _read( ptr, uint32 );
                for( uint32 iExit = 0; iExit < stat.nExit; iExit++ ) {
                    stat.Exit_iJumperEdge_Exit [ iExit ] = _read( ptr, int32 );
                    stat.Exit_iJumperEdge_Enter[ iExit ] = _read( ptr, int32 );
                }
                
                stat.nNodeLink   = _read( ptr, uint32 );
                stat.nPathLink   = _read( ptr, uint32 );
                stat.nMechDoor   = _read( ptr, uint32 );
                stat.nSecureDoor = _read( ptr, uint32 );
                
                Editor->stat = stat;
                
                { // MECH DOOR
                    EDITOR__MECH_DOOR_STATE * State = &Editor->Mech;
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        _read( ptr, WORLD__DOOR );
                    }
                }
                
                { // SECURE DOOR
                    EDITOR__SECURE_DOOR_STATE * State = &Editor->Secure;
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        _read( ptr, WORLD__DOOR );
                    }
                }
            } break;
            
            case 7: {
                ROOM_STATS stat = {};
                stat.isTopDown = _read( ptr, boo32 );
                stat.Bound     = _read( ptr, rect );
                stat.intCell   = _read( ptr, UINT32_PAIR );
                
                stat.nExit     = _read( ptr, uint32 );
                for( uint32 iExit = 0; iExit < stat.nExit; iExit++ ) {
                    stat.Exit_iJumperEdge_Exit [ iExit ] = _read( ptr, int32 );
                    stat.Exit_iJumperEdge_Enter[ iExit ] = _read( ptr, int32 );
                }
                
                stat.nNodeLink   = _read( ptr, uint32 );
                stat.nPathLink   = _read( ptr, uint32 );
                stat.nMechDoor   = _read( ptr, uint32 );
                stat.nSecureDoor = _read( ptr, uint32 );
                
                Editor->stat = stat;
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
EDITOR_loadEntityCount( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        EDITOR_STATE * Editor = &AppState->Editor;
        
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                uint32 nType = _read( ptr, uint32 );
                if( nType != EntityType_count ) {
                    globalVar_debugSystem.errorOccurred = true;
                    char str[ 512 ] = {};
                    sprintf( str, "ERROR! When loading %s in the Editor, nType != EntityType_count: nType = %u, EntityType_count = %u", filename, nType, EntityType_count );
                    CONSOLE_STRING( str );
                }
                
                ENTITY_TYPE_TAGS;
                
                for( uint32 iType = 0; iType < nType; iType++ ) {
                    char * tag     = _addra( ptr, char, 4 );
                    uint32 nEntity = _read( ptr, uint32 );
                    
                    int32 i = -1;
                    for( uint32 iTag = 0; iTag < EntityType_count; iTag++ ) {
                        if( matchSegment( tag, EntityTypeTag[ iTag ], 4 ) ) {
                            i = iTag;
                        }
                    }
                    if( i > -1 ) {
                        Editor->nEntity[ i ] = nEntity;
                    }
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
EDITOR_loadEvents( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        EDITOR_STATE * Editor = &AppState->Editor;
        
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                STRING_BUFFER * Event_names = &Editor->Event_names;
                
                MEMORY  _memory = Memory( Editor->Event_memory, EDITOR__EVENT_MEMORY_SIZE );
                MEMORY * memory = &_memory;
                
                Editor->nEvent = _read( ptr, uint32 );
                
                DATA_TYPE_LABELS;
                DATA_TYPE_SIZES;
                
                for( uint32 iEvent = 0; iEvent < Editor->nEvent; iEvent++ ) {
                    EDITOR__EVENT * Event = Editor->Event + iEvent;
                    
                    char * name = _addrStr( ptr );
                    Event->name = add( Event_names, name );
                    Event->IsActive = _read( ptr, boo32 );
                    Event->nData = _read( ptr, uint32 );
                    Assert( Event->nData < EDITOR_EVENT__DATA_MAX_COUNT );
                    for( uint32 iData = 0; iData < Event->nData; iData++ ) {
                        char * label = _addra( ptr, char, 4 );
                        
                        int32 i = -1;
                        for( uint32 iLabel = 0; iLabel < dataType_count; iLabel++ ) {
                            if( matchSegment( label, dataTypeLabel[ iLabel ], 4 ) ) {
                                i = iLabel;
                            }
                        }
                        if( i > -1 ) {
                            uint32 size = dataTypeSize[ i ];
                            
                            char * var = _addrStr( ptr );
                            Event->data_name[ iData ] = add( Event_names, var );
                            Event->data_Type[ iData ] = ( EDITOR_EVENT__DATA_TYPE )i;
                            Event->data_ptr [ iData ] = ( uint8 * )_pushSize( memory, size );
                            
                            uint8 * src  = _addra( ptr, uint8, size );
                            memcpy( Event->data_ptr[ iData ], src, size );
                            
                            Event->data_size += size;
                        } else {
                            InvalidCodePath;
                        }
                    }
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
EDITOR_loadRoom( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename ) {
    // TODO: How should I handle EDITOR_loading previous versions? Should I send an error message? Where should I send it?
    
    EDITOR_STATE * Editor = &AppState->Editor;
    reset( Editor );
    
    FILE_SAVE_OPEN * open = &Editor->SaveOpen;
    strcpy( open->current, filename );
    
    EDITOR_loadPlayerSpawn( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__PLAYER_SPAWN );
    EDITOR_loadTerrain( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__TERRAIN );
    EDITOR_loadExit( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__EXIT_EDIT );
    EDITOR_loadNest( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__NEST );
    EDITOR_loadSecureDoor( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__SECURE_DOOR );
    EDITOR_loadMechDoor( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__MECH_DOOR );
    EDITOR_loadPowerSwitch( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__POWER_SWITCH );
    EDITOR_loadRoomStats( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__ROOM_STATS );
    EDITOR_loadEntityCount( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__ENTITY_COUNT );
    EDITOR_loadEvents( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__EVENTS_EDIT );
    EDITOR_LoadCheckpoint( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__CHECKPOINT );
    //EDITOR_LoadNavMesh( Platform, AppState, TempMemory, SaveDir, filename, FILETAG__NAV_MESH );
    
    updateRoomBound( AppState );
}

//----------
// LAYOUT LOAD ENTITY
//----------

internal void
setLayoutFilenames( APP_STATE * AppState ) {
    LAYOUT_STATE  * layout = &AppState->layout;
    STRING_BUFFER * list   = &layout->filter_fileList;
    
    layout->nRoom = list->nStr;
    for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
        LAYOUT_ROOM * Room = layout->Room + iRoom;
        Room->name = list->str[ iRoom ];
    }
}

internal void
LAYOUT_loadRoomStats( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * filename, uint32 iRoom ) {
    char * SaveDir = ROOM_SAVE_DIRECTORY;
    char * filetag = FILETAG__ROOM_STATS;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        LAYOUT_STATE * layout = &AppState->layout;
        
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7: {
                ROOM_STATS stat = _read( ptr, ROOM_STATS );
                
                LAYOUT_ROOM * Room = layout->Room + iRoom;
                Room->Bound = stat.Bound;
                Room->nExit = stat.nExit;
                memcpy( Room->Exit_iJumperEdge_Exit,  stat.Exit_iJumperEdge_Exit, sizeof( int32 ) * stat.nExit );
                memcpy( Room->Exit_iJumperEdge_Enter, stat.Exit_iJumperEdge_Enter, sizeof( int32 ) * stat.nExit );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
loadLayoutRoomStats( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    LAYOUT_STATE  * layout = &AppState->layout;
    
    for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
        LAYOUT_ROOM Room = layout->Room[ iRoom ];
        LAYOUT_loadRoomStats( Platform, AppState, TempMemory, Room.name, iRoom );
    }
}

internal void
loadLayoutBitmaps( PLATFORM * Platform, APP_STATE * AppState ) {
    LAYOUT_STATE * layout = &AppState->layout;
    
    RENDERER * Renderer = &Platform->Renderer;
    for( uint32 iTexture = TextureID_Room_bitmap; iTexture < TextureID_Room_bitmap_end; iTexture++ ) {
        Renderer->Texture_isLoaded[ iTexture ] = false;
    }
    for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
        TEXTURE_ID  TextureID = ( TEXTURE_ID )( ( uint32 )TextureID_Room_bitmap + iRoom );
        UploadBMPFromFile( Platform, TextureID, ROOM_SAVE_DIRECTORY, layout->Room[ iRoom ].name );
    }
}

internal void
LAYOUT_loadExit( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 iRoom ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        LAYOUT_STATE * layout = &AppState->layout;
        
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                LAYOUT_ROOM * Room = layout->Room + iRoom;
                
                Room->nExit = _read( ptr, uint32 );
                for( uint32 iEntity = 0; iEntity < Room->nExit; iEntity++ ) {
                    EDITOR__EXIT Exit = {};
                    Exit.Bound = _read( ptr, rect );
                    Exit.Type  = _read( ptr, EXIT_TYPE );
                    
                    Room->Exit[ iEntity ] = Exit;
                }
                
                for( uint32 iLink = 0; iLink < EXIT_MAX_COUNT; iLink++ ) {
                    Room->Link[ iLink ] = Int32Pair( -1, -1 );
                }
                
                Room->bExit    = layout->nExit;
                layout->nExit += Room->nExit;
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
loadLayoutExits( PLATFORM * Platform, APP_STATE * AppState ) {
    LAYOUT_STATE * layout = &AppState->layout;
    
    for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
        LAYOUT_loadExit( Platform, AppState, &Platform->TempMemory, ROOM_SAVE_DIRECTORY, layout->Room[ iRoom ].name, FILETAG__EXIT_EDIT, iRoom );
    }
}

internal void
LoadLayoutForEdit( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * filename ) {
    LAYOUT_STATE * layout = &AppState->layout;
    reset( layout );
    
    FILE_SAVE_OPEN * open = &layout->SaveOpen;
    strcpy( open->current, filename );
    
    char * SaveDir = ROOM_SAVE_DIRECTORY;
    char * filetag = FILETAG__LAYOUT_EDIT;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                uint32 nShow = _read( ptr, uint32 );
                for( uint32 iShow = 0; iShow < nShow; iShow++ ) {
                    char * name = _addrStr( ptr );
                    int32 iRoom = getIRoom( layout, name );
                    
                    if( iRoom > -1 ) {
                        LAYOUT_ROOM * Room = layout->Room + iRoom;
                        Room->show = true;
                        Room->pos  = _read( ptr, vec2 );
                        
                        uint32 nLink = _read( ptr, uint32 );
                        // TODO: verify that the number of links written to the layout file matches the number of Exits in the Room file
                        
                        for( uint32 iLink = 0; iLink < nLink; iLink++ ) {
                            INT32_PAIR link = Int32Pair( -1, -1 );
                            link.m = _read( ptr, int32 );
                            link.n = _read( ptr, int32 );
                            
                            // TODO: verify link names, but this must come after reading in all the link data, meaning that we have to redo the saveLayout to output the string and rect information at the end of the file
                            if( ( link.m > -1 ) && ( link.n > -1 ) ) {
                                char * n0 = _addrStr( ptr );
                                rect   r0 = _read( ptr, rect );
                                
                                int32 iR0 = getIRoom( layout, n0 );
                                if( iR0 > -1 ) {
                                    link.m = iR0;
                                }
                            }
                            
                            Room->Link[ iLink ] = link;
                        }
                    } else {
                        globalVar_debugSystem.errorOccurred = true;
                        char str[ 512 ] = {};
                        sprintf( str, "ERROR! ENTRY %u expected file %s, but the file was not found!", iShow, filename );
                        CONSOLE_STRING( str );
                        
                        // TODO: If the filename is not found, then this kinda destroys any additional data. We should probably have additional Room slots for error files, so we can at least see the data and potentially reassociate it with the correct Room.
                        
                        // throw away the rest of the data
                        vec2   pos   = _read( ptr, vec2 );
                        uint32 nLink = _read( ptr, uint32 );
                        for( uint32 iLink = 0; iLink < nLink; iLink++ ) {
                            int32 m = _read( ptr, int32 );
                            int32 n = _read( ptr, int32 );
                            if( ( m > -1 ) && ( n > -1 ) ) {
                                char * n0 = _addrStr( ptr );
                                rect   r0 = _read( ptr, rect );
                            }
                        }
                    }
                }
                
                char * RoomToStartGame = _addrStr( ptr );
                int32 iRoomToStartGame = getIRoom( layout, RoomToStartGame );
                if( iRoomToStartGame > -1 ) {
                    layout->iRoomToStartGame = iRoomToStartGame;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}