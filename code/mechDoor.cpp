
internal rect
getUseBoundA( MECH_DOOR Entity ) {
    rect result = Entity.Bound_SwitchA;
    result = addRadius( result, TILE_WIDTH );
    return result;
}

internal rect
getUseBoundB( MECH_DOOR Entity ) {
    rect result = Entity.Bound_SwitchB;
    result = addRadius( result, TILE_WIDTH );
    return result;
}

internal void
updateMechDoor( APP_STATE * AppState, flo32 dt ) {
    PLAYER_STATE * Player = &AppState->Player;
    vec2 PlayerP = GetPlayerUseP( *Player );
    
    MECH_DOOR_STATE * State  = &AppState->Mech;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        MECH_DOOR * Entity = State->Entity + iEntity;
        
        rect RA = getUseBoundA( *Entity );
        rect RB = getUseBoundB( *Entity );
        if( ( Player->Use_IsActive ) && ( ( IsInBound( PlayerP, RA ) ) || ( IsInBound( PlayerP, RB ) ) ) ) {
            Entity->doToggle = true;
        } else {
            Entity->doToggle = false;
            Entity->Toggle_Timer    = 0.0f;
        }
        
        if( Entity->doToggle ) {
            Entity->Toggle_Timer += dt;
            
            flo32 t = clamp01( Entity->Toggle_Timer / MECH_DOOR_TARGET_TIME );
            Player->Use_t = t;
        }
    }
}

internal void
DrawMechDoor( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    MECH_DOOR_STATE * State = Draw->Mech;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        MECH_DOOR Entity = State->Entity[ iEntity ];
        
        { // door
            rect R  = Entity.Bound_Door;
            rect hi = R;
            hi.Bottom = R.Top;
            hi.Top    = R.Top + ( TILE_HEIGHT * 0.5f );
            
            rect lo = R;
            lo.Top    = lo.Bottom;
            lo.Bottom = lo.Bottom - ( TILE_HEIGHT * 0.5f );
            
            {
                vec4 Color = COLOR_GRAY( 0.25f );
                DrawRectOutline( Pass, hi, COLOR_BLACK );
                DrawRect( Pass, hi, Color );
                DrawRect( Pass, lo, Color );
            }
            
            if( Entity.isClosed ) {
                rect A = Entity.Bound_Door;
                rect B = Entity.Bound_Door;
                
                flo32 marginA = 0.25f;
                flo32 marginB = 0.625f;
                
                A.Left += ( TILE_WIDTH * marginA );
                A.Right = A.Left + ( TILE_WIDTH * marginB );
                
                B.Right -= ( TILE_WIDTH * marginA );
                B.Left   = B.Right - ( TILE_WIDTH * marginB );
                
                DrawRect       ( Pass, A, COLOR_GRAY( 0.35f ) );
                DrawRectOutline( Pass, A, COLOR_BLACK );
                DrawRect       ( Pass, B, COLOR_GRAY( 0.35f ) );
                DrawRectOutline( Pass, B, COLOR_BLACK );
            }
        }
        
        { // switch
            vec4 ColorA = Vec4( 0.8f, 0.0f, 0.0f, 1.0f );
            vec4 ColorC = Vec4( 0.5f, 0.0f, 0.0f, 1.0f );
            vec4 ColorD = Vec4( 0.6f, 0.0f, 0.0f, 1.0f );
            
            vec4 ColorB = COLOR_GRAY( 0.25f );
            
            rect Bound[ 2 ] = { Entity.Bound_SwitchA, Entity.Bound_SwitchB };
            
            for( uint32 iBound = 0; iBound < 2; iBound++ ) {
                rect R = Bound[ iBound ];
                DrawRect( Pass, R, ColorB );
                DrawRectOutline( Pass, R, COLOR_BLACK );
                
                vec2 P = getCenter( R );
                
                flo32 radA = ( getWidth( R ) * 0.5f ) - ( TILE_WIDTH * 0.25f );
                DrawCircle( Pass, P, radA, ColorA );
                DrawCircleOutline( Pass, P, radA, ColorC );
                
                flo32 radB = radA - ( TILE_WIDTH * 0.35f );
                DrawCircle       ( Pass, P, radB, ColorB );
                DrawCircleOutline( Pass, P, radB, ColorC );
                
                vec2 dimA = Vec2( TILE_WIDTH * 0.25f, radB * 2.0f );
                flo32 radiansA = PI * 1.0f / 16.0f;
                flo32 radiansB = radiansA + ( PI * 0.5f );
                
                orect S = orectCD( P, dimA, radiansA );
                orect T = orectCD( P, dimA, radiansB );
                DrawORect( Pass, S, ColorD );
                DrawORect( Pass, T, ColorD );
                
                flo32 radC = ( TILE_WIDTH * 0.3f );
                DrawCircle       ( Pass, P, radC, ColorD );
                DrawCircleOutline( Pass, P, radC, ColorC );
                //rect debug = getUseBound( Entity );
                //DrawRectOutline( Pass, debug, COLOR_YELLOW );
            }
        }
    }
}

internal void
FinalizeMechDoor( APP_STATE * AppState ) {
    PLAYER_STATE    * Player = &AppState->Player;
    MECH_DOOR_STATE * State  = &AppState->Mech;
    
    COLLISION_STATE * Collision = &AppState->Collision;
    
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        MECH_DOOR * Entity = State->Entity + iEntity;
        
        if( Entity->doToggle ) {
            if( Entity->Toggle_Timer >= MECH_DOOR_TARGET_TIME ) {
                Entity->isClosed = !Entity->isClosed;
                
                Entity->doToggle     = false;
                Entity->Toggle_Timer = 0.0f;
                
                Player->Use_IsActive = false;
                Player->Use_t        = 0.0f;
                
                UINT32_PAIR iJumperPathLink = Entity->iJumperPathLink;
                Collision->JumperPath.PathLink_ActiveData[ iJumperPathLink.m ].IsActive = !Entity->isClosed;
                Collision->JumperPath.PathLink_ActiveData[ iJumperPathLink.n ].IsActive = !Entity->isClosed;
            }
        }
    }
}

internal EDITOR__MECH_DOOR
readMechDoor( uint32 Version, uint8 ** Ptr ) {
    EDITOR__MECH_DOOR Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            rect Bound_Door    = _read( ptr, rect );
            rect Bound_SwitchA = _read( ptr, rect );
            
            EDITOR__MECH_DOOR Entity = {};
            Entity.Bound_Door    = Bound_Door;
            Entity.Bound_SwitchA = Bound_SwitchA;
            
            Result = Entity;
        } break;
        
        case 2: {
            rect Bound_Door    = _read( ptr, rect );
            rect Bound_SwitchA = _read( ptr, rect );
            rect Bound_SwitchB = _read( ptr, rect );
            
            EDITOR__MECH_DOOR Entity = {};
            Entity.Bound_Door    = Bound_Door;
            Entity.Bound_SwitchA = Bound_SwitchA;
            Entity.Bound_SwitchB = Bound_SwitchB;
            
            Result = Entity;
        } break;
        
        case 3: {
            rect        Bound_Door      = _read( ptr, rect );
            rect        Bound_SwitchA   = _read( ptr, rect );
            rect        Bound_SwitchB   = _read( ptr, rect );
            boo32       isClosed        = _read( ptr, boo32 );
            UINT32_PAIR iJumperPathLink = _read( ptr, UINT32_PAIR );
            
            EDITOR__MECH_DOOR Entity = {};
            Entity.Bound_Door      = Bound_Door;
            Entity.Bound_SwitchA   = Bound_SwitchA;
            Entity.Bound_SwitchB   = Bound_SwitchB;
            Entity.isClosed        = isClosed;
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
ROOM_loadMechInit( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 iRoom ) {
    WORLD_STATE * World = &AppState->World;
    ROOM        * Room  = World->Room + iRoom;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        
        uint32 nEntity = _read( ptr, uint32 );
        Room->MechDoor = UInt32Pair( World->nDoor, nEntity );
        
        for( uint32 iEntity = 0; iEntity < nEntity; iEntity++ ) {
            EDITOR__MECH_DOOR Src = readMechDoor( Version, &ptr );
            
            WORLD__DOOR Dest = {};
            Dest.iJumperPathLink = Src.iJumperPathLink;
            Dest.isClosed        = ( uint8 )Src.isClosed;
            
            World->Door[ World->nDoor++ ] = Dest;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
GAME_loadMechDoor( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        
        MECH_DOOR_STATE * State = &AppState->Mech;
        
        State->nEntity = _read( ptr, uint32 );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            EDITOR__MECH_DOOR Src = readMechDoor( Version, &ptr );
            
            MECH_DOOR Entity = {};
            Entity.Bound_Door      = Src.Bound_Door;
            Entity.Bound_SwitchA   = Src.Bound_SwitchA;
            Entity.Bound_SwitchB   = Src.Bound_SwitchB;
            Entity.isClosed        = Src.isClosed;
            Entity.iJumperPathLink = Src.iJumperPathLink;
            State->Entity[ iEntity ] = Entity;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
EDITOR_loadMechDoor( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    EDITOR__MECH_DOOR_STATE * State = &AppState->Editor.Mech;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        State->nEntity = _read( ptr, uint32 );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            EDITOR__MECH_DOOR Entity = readMechDoor( Version, &ptr );
            State->Entity[ iEntity ] = Entity;
        }
        
        _popSize( TempMemory, file.size );
    }
}