
internal uint32
getEntityCount( APP_STATE * AppState ) {
    uint32 result = 0;
    result += AppState->Jumper.nEntity;
    
    { // Nest
        NEST_STATE * State = &AppState->Nest;
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            NEST Entity = State->Entity[ iEntity ];
            if( ( !Entity.IsDead ) && ( Entity.isAwake ) ) {
                result++;
            }
        }
    }
    
    return result;
}

internal void
updateSecureDoor( APP_STATE * AppState, flo32 dT ) {
    uint32 nEntity = getEntityCount( AppState );
    
    WORLD_STATE       * World = &AppState->World;
    SECURE_DOOR_STATE * State = &AppState->Secure;
    
    COLLISION_STATE * Collision = &AppState->Collision;
    
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        SECURE_DOOR * Entity = State->Entity + iEntity;
        
        Entity->isPowered = World->power_isOn[ Entity->iPower ];
        
        if( Entity->isPowered ) {
            if( nEntity > 0 ) {
                Entity->isClosed = true;
                Entity->Timer    = 0.0f;
                
                UINT32_PAIR iJumperPathLink = Entity->iJumperPathLink;
                Collision->JumperPath.PathLink_ActiveData[ iJumperPathLink.m ].IsActive = !Entity->isClosed;
                Collision->JumperPath.PathLink_ActiveData[ iJumperPathLink.n ].IsActive = !Entity->isClosed;
            } else {
                Entity->Timer += dT;
            }
        }
    }
}

internal void
DrawSecureDoor( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    SECURE_DOOR_STATE * State = Draw->Secure;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        SECURE_DOOR Entity = State->Entity[ iEntity ];
        
        rect R  = Entity.Bound;
        rect Hi = R;
        Hi.Bottom = R.Top;
        Hi.Top    = R.Top + ( TILE_HEIGHT * 1.0f );
        
        rect Lo = R;
        Lo.Top    = Lo.Bottom;
        Lo.Bottom = Lo.Bottom - ( TILE_HEIGHT * 0.5f );
        
        {
            vec4 Color = COLOR_GRAY( 0.25f );
            DrawRectOutline( Pass, Hi, COLOR_BLACK );
            DrawRect( Pass, Hi, Color );
            DrawRect( Pass, Lo, Color );
        }
        
        if( Entity.isClosed ) {
            DrawRect       ( Pass, R, COLOR_GRAY( 0.35f ) );
            DrawRectOutline( Pass, R, COLOR_BLACK );
        }
        
        { // lights
            flo32 RadiusA = TILE_WIDTH * 0.325f;
            flo32 RadiusB = TILE_WIDTH * 0.25f;
            
            vec4 ColorA = Vec4( 0.0f, 0.2f, 0.0f, 1.0f );
            vec4 ColorB = Vec4( 0.2f, 0.0f, 0.0f, 1.0f );
            
            if( Entity.isPowered ) {
                if( Entity.isClosed ) {
                    if( Entity.Timer > SECURE_DOOR_VERIFY_OPEN_DELAY ) {
                        flo32 Numer = ( Entity.Timer - SECURE_DOOR_VERIFY_OPEN_DELAY );
                        flo32 Denom = ( SECURE_DOOR_VERIFY_OPEN_CYCLE_TIME );
                        uint32 Cycle = ( uint32 )( Numer / Denom );
                        if( ( Cycle % 2 ) == 0 ) {
                            ColorA = COLOR_GREEN;
                        }
                    }
                    ColorB = COLOR_RED;
                } else {
                    ColorA = COLOR_GREEN;
                }
            }
            
            vec2 P = getBL( Hi ) + ( TILE_DIM * 0.5f );
            vec2 Q = getTR( Hi ) - ( TILE_DIM * 0.5f );
            
            DrawCircle( Pass, P, RadiusA, COLOR_BLACK );
            DrawCircle( Pass, P, RadiusB, ColorA );
            
            DrawCircle( Pass, Q, RadiusA, COLOR_BLACK );
            DrawCircle( Pass, Q, RadiusB, ColorB );
        }
    }
}

internal void
FinalizeSecureDoor( APP_STATE * AppState ) {
    SECURE_DOOR_STATE * State = &AppState->Secure;
    
    COLLISION_STATE * Collision = &AppState->Collision;
    
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        SECURE_DOOR * Entity = State->Entity + iEntity;
        
        if( Entity->Timer >= SECURE_DOOR_VERIFY_OPEN_TARGET_TIME ) {
            Entity->isClosed = false;
            
            UINT32_PAIR iJumperPathLink = Entity->iJumperPathLink;
            Collision->JumperPath.PathLink_ActiveData[ iJumperPathLink.m ].IsActive = !Entity->isClosed;
            Collision->JumperPath.PathLink_ActiveData[ iJumperPathLink.n ].IsActive = !Entity->isClosed;
        }
    }
}

internal EDITOR__SECURE_DOOR
readSecureDoor( uint32 Version, uint8 ** Ptr ) {
    EDITOR__SECURE_DOOR Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            rect        Bound           = _read( ptr, rect );
            uint32      iPower          = _read( ptr, uint32 );
            uint32      iSensor         = _read( ptr, uint32 );
            UINT32_PAIR iJumperPathLink = _read( ptr, UINT32_PAIR );
            
            EDITOR__SECURE_DOOR Entity = {};
            Entity.Bound           = Bound;
            Entity.iPower          = iPower;
            Entity.iSensor         = iSensor;
            Entity.iJumperPathLink = iJumperPathLink;
            
            Result = Entity;
        } break;
        
        case 2: {
            rect        Bound           = _read( ptr, rect );
            boo32       isClosed        = _read( ptr, boo32 );
            uint32      iPower          = _read( ptr, uint32 );
            uint32      iSensor         = _read( ptr, uint32 );
            UINT32_PAIR iJumperPathLink = _read( ptr, UINT32_PAIR );
            
            EDITOR__SECURE_DOOR Entity = {};
            Entity.Bound           = Bound;
            Entity.isClosed        = isClosed;
            Entity.iPower          = iPower;
            Entity.iSensor         = iSensor;
            Entity.iJumperPathLink = iJumperPathLink;
            
            Result = Entity;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
ROOM_loadSecureInit( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 iRoom ) {
    WORLD_STATE * World = &AppState->World;
    ROOM        * Room  = World->Room + iRoom;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        
        uint32 nEntity = _read( ptr, uint32 );
        Room->SecureDoor = UInt32Pair( World->nDoor, nEntity );
        
        for( uint32 iEntity = 0; iEntity < nEntity; iEntity++ ) {
            EDITOR__SECURE_DOOR Src = readSecureDoor( Version, &ptr );
            
            WORLD__DOOR Dest = {};
            Dest.iJumperPathLink = Src.iJumperPathLink;
            Dest.isClosed        = ( uint8 )Src.isClosed;
            
            World->Door[ World->nDoor++ ] = Dest;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadSecureDoor( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        
        SECURE_DOOR_STATE * State = &AppState->Secure;
        
        State->nEntity = _read( ptr, uint32 );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            EDITOR__SECURE_DOOR Src = readSecureDoor( Version, &ptr );
            
            SECURE_DOOR Entity = {};
            Entity.Bound           = Src.Bound;
            Entity.isClosed        = Src.isClosed;
            Entity.iPower          = Src.iPower;
            Entity.iSensor         = Src.iSensor;
            Entity.iJumperPathLink = Src.iJumperPathLink;
            State->Entity[ iEntity ] = Entity;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
EDITOR_loadSecureDoor( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    EDITOR__SECURE_DOOR_STATE * State = &AppState->Editor.Secure;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        EDITOR_STATE * Editor = &AppState->Editor;
        
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        State->nEntity = _read( ptr, uint32 );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            EDITOR__SECURE_DOOR Entity = readSecureDoor( Version, &ptr );
            State->Entity[ iEntity ] = Entity;
        }
        
        _popSize( TempMemory, file.size );
    }
}