
internal rect
GetUseBound( CHECKPOINT Entity ) {
    flo32 OffsetX = TILE_WIDTH  * 7.0f;
    flo32 OffsetY = TILE_HEIGHT * 8.0f;
    
    rect Result = Entity.Bound;
    Result.Left  += OffsetX;
    Result.Right -= OffsetX;
    Result.Top    = Result.Bottom + OffsetY;
    return Result;
}

internal void
UpdateCheckpoint( APP_STATE * AppState, flo32 dT ) {
    CHECKPOINT_STATE * State  = &AppState->Checkpoint;
    CHECKPOINT       * Entity = &State->Entity;
    PLAYER_STATE     * Player = &AppState->Player;
    
    if( Entity->DoesExist ) {
        Entity->Shield_Timer += dT;
        Entity->Text_Timer   += dT;
        if( Entity->IsActive ) {
            Entity->Signal_Timer += dT;
        }
        
        { // USE BOUND
            rect R       = GetUseBound( *Entity );
            vec2 PlayerP = GetPlayerUseP( *Player );
            
            if( IsInBound( PlayerP, R ) ) {
                Player->Draw_SaveIndicator = true;
                if( Player->Use_WasPressed ) {
                    Entity->Save_IsActive = true;
                }
                if( Player->Use_WasReleased ) {
                    Entity->Save_IsActive = false;
                    Entity->Save_Timer    = 0.0f;
                }
            } else {
                Entity->Save_IsActive = false;
                Entity->Save_Timer    = 0.0f;
            }
        }
        
        if( Entity->Save_IsActive ) {
            Entity->Save_Timer += dT;
            
            flo32 TargetTime = CHECKPOINT_ACTIVATE_TARGET_TIME;
            if( Entity->IsActive ) {
                TargetTime = CHECKPOINT_SAVE_TARGET_TIME;
            }
            Player->Use_t = clamp01( Entity->Save_Timer / TargetTime );
        }
    }
}

internal void
DrawCheckpoint( RENDER_PASS * Pass_Game, DRAW_STATE * Draw ) {
    CHECKPOINT_STATE * State  = Draw->Checkpoint;
    CHECKPOINT         Entity = State->Entity;
    
    if( Entity.DoesExist ) {
        { // BACKGROUND
            flo32 OffsetX = TILE_WIDTH  * 2.0f;
            flo32 OffsetY = TILE_HEIGHT * 12.0f;
            
            rect R = Entity.Bound;
            R.Left  += OffsetX;
            R.Right -= OffsetX;
            R.Top    = R.Bottom + OffsetY;
            
            rect S = addRadius( R, -TILE_WIDTH * 0.25f );
            
            DrawRect( Pass_Game, R, COLOR_GRAY( 0.1f ) );
            DrawRect( Pass_Game, S, COLOR_GRAY( 0.05f ) );
        }
        
        { // PANEL
            vec2 P = getBC( Entity.Bound ) + Vec2( 0.0f, TILE_HEIGHT * 3.0f );
            
            rect R = rectBCD( P, CHECKPOINT_PANEL_DIM );
            rect S = addRadius( R, -TILE_WIDTH * 0.25f );
            
            DrawRect( Pass_Game, R, COLOR_GRAY( 0.1f ) );
            DrawRect( Pass_Game, S, COLOR_GRAY( 0.05f ) );
            
            { // TEXT
                uint32 nLines = 6;
                
                vec2 A[ 12 ] = {};
                A[  0 ] = getT( S, Vec2( 0.1f,  0.0f ) );
                A[  1 ] = getT( S, Vec2( 0.3f,  0.0f ) );
                A[  2 ] = getT( S, Vec2( 0.12f, 0.0f ) );
                A[  3 ] = getT( S, Vec2( 0.41f, 0.0f ) );
                A[  4 ] = getT( S, Vec2( 0.1f,  0.0f ) );
                A[  5 ] = getT( S, Vec2( 0.35f, 0.0f ) );
                A[  6 ] = getT( S, Vec2( 0.09f, 0.0f ) );
                A[  7 ] = getT( S, Vec2( 0.40f, 0.0f ) );
                A[  8 ] = getT( S, Vec2( 0.13f, 0.0f ) );
                A[  9 ] = getT( S, Vec2( 0.30f, 0.0f ) );
                A[ 10 ] = getT( S, Vec2( 0.11f, 0.0f ) );
                A[ 11 ] = getT( S, Vec2( 0.38f, 0.0f ) );
                
                rect C = rectMM( getT( S, Vec2( 0.06f, 0.17f ) ), getT( S, Vec2( 0.48f, 0.78f ) ) );
                DrawRect( Pass_Game, C, COLOR_GRAY( 0.02f ) );
                
                vec2 U = getBL( S );
                vec2 V = getTL( S );
                
                flo32  TargetTimeA = ( CHECKPOINT_TEXT_CYCLE_TARGET_TIME / ( flo32 )nLines );
                uint32 bIndex = clamp( ( uint32 )( Entity.Text_Timer / TargetTimeA ), ( uint32 )0, nLines - 1 ) * 2;
                
                for( uint32 iA = 0; iA < nLines; iA++ ) {
                    flo32 t = clamp01( ( flo32 )iA / ( flo32 )( nLines - 1 ) );
                    flo32 Y = lerp( U.y, lerp( 0.25f, t, 0.7f ), V.y );
                    
                    uint32 IndexA = ( bIndex + iA * 2     ) % ( nLines * 2 );
                    uint32 IndexB = ( bIndex + iA * 2 + 1 ) % ( nLines * 2 );
                    A[ IndexA ].y = Y;
                    A[ IndexB ].y = Y;
                }
                
                vec4 ColorA = Vec4( 0.5f, 0.0f, 0.0f, 1.0f );
                if( Entity.IsActive ) {
                    ColorA = Vec4( 0.0f, 0.35f, 0.0f, 1.0f );
                }
                for( uint32 iA = 0; iA < nLines; iA++ ) {
                    DrawLine( Pass_Game, A[ iA * 2 ], A[ iA * 2 + 1 ], ColorA );
                }
            }
            
            { // SIGNAL
                uint32 nLines = 5;
                
                flo32 HiY = S.Top;
                flo32 LoY = S.Bottom;
                
                flo32 X = lerp( S.Left, 0.76f, S.Right );
                
                flo32 TargetTimeA = ( 84.0f / 60.0f );
                flo32 tA = clamp01( Entity.Signal_Timer / TargetTimeA );
                uint32 tLine = 0;
                if( Entity.IsActive ) {
                    tLine = ( uint32 )( ( flo32 )nLines * tA );
                }
                
                flo32 Height = TILE_HEIGHT * 0.15f;
                
                for( uint32 iLine = 0; iLine < nLines; iLine++ ) {
                    flo32 tB = clamp01( ( flo32 )iLine / ( flo32 )( nLines - 1 ) );
                    vec2 B   = Vec2( X, lerp( LoY, lerp( 0.24f, tB, 0.74f ), HiY ) );
                    vec2 Dim = Vec2( Height * ( ( flo32 )iLine * 2.5f + 1.0f ), Height );
                    
                    rect C = rectCD( B, Dim );
                    
                    vec4 Color = COLOR_GRAY( 0.1f );
                    if( iLine <= tLine ) {
                        Color = COLOR_GRAY( 0.6f );
                    }
                    DrawRect( Pass_Game, C, Color );
                }
            }
        }
        
        { // USE BOUND
            //rect R = GetUseBound( Entity );
            //DrawRectOutline( Pass_Game, R, COLOR_YELLOW );
        }
        
        { // SHIELD
            flo32 t = cosf( ( Entity.Shield_Timer / CHECKPOINT_SHIELD_CYCLE_TARGET_TIME ) * 2.0f * PI );
            flo32 AddX = lerp( 0.0f, t, 0.05f ) * TILE_WIDTH;
            
            vec2 Dim = Vec2( ( TILE_WIDTH * 0.25f ) + AddX, getHeight( Entity.Bound ) );
            vec2 P   = getBL( Entity.Bound );
            vec2 Q   = getBR( Entity.Bound );
            P.x -= ( AddX * 0.5f );
            Q.x += ( AddX * 0.5f );
            
            rect BoundA = rectBLD( P, Dim );
            rect BoundB = rectBRD( Q, Dim );
            
            vec4 ColorA_lo = toColor( 160, 155, 0 );
            vec4 ColorA_hi = toColor( 220, 210, 0 );
            vec4 ColorA    = lerp( ColorA_lo, t, ColorA_hi );
            
            flo32 a = lerp( 0.33f, t, 0.35f );
            vec4 ColorB = toColor( 150, 145, 0 ) * a;
            
            DrawRect( Pass_Game, Entity.Bound, ColorB );
            DrawRect( Pass_Game, BoundA, ColorA );
            DrawRect( Pass_Game, BoundB, ColorA );
        }
    }
}

internal void
FinalizeCheckpoint( APP_STATE * AppState ) {
    CHECKPOINT_STATE * State  = &AppState->Checkpoint;
    CHECKPOINT       * Entity = &State->Entity;
    
    if( Entity->DoesExist ) {
        if( Entity->Signal_Timer >= CHECKPOINT_SIGNAL_CYCLE_TARGET_TIME ) {
            Entity->Signal_Timer = 0.0f;
        }
        if( Entity->Text_Timer >= CHECKPOINT_TEXT_CYCLE_TARGET_TIME ) {
            Entity->Text_Timer = 0.0f;
        }
        
        { // SAVE PROGRESS
            flo32 TargetTime = CHECKPOINT_ACTIVATE_TARGET_TIME;
            if( Entity->IsActive ) {
                TargetTime = CHECKPOINT_SAVE_TARGET_TIME;
            }
            if( Entity->Save_Timer >= TargetTime ) {
                Entity->IsActive = true;
                Entity->Save_IsActive = false;
                Entity->Save_Timer    = 0.0f;
                
                AppState->DoSaveGame = true;
                AppState->LoadCheckpoint_iRoom = AppState->iRoom;
                strcpy( AppState->LoadCheckpoint_Name, AppState->CurrentRoom );
            }
        }
    }
}

#define VERSION__CHECKPOINT  ( 1 )
#define FILETAG__CHECKPOINT  ( "CHECKPNT" )
internal void
SaveCheckpoint( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    EDITOR__CHECKPOINT_STATE * State  = &Editor->Checkpoint;
    EDITOR__CHECKPOINT       * Entity = &State->Entity;
    
    if( Entity->DoesExist ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, EDITOR__CHECKPOINT, *Entity );
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

internal EDITOR__CHECKPOINT
ReadCheckpoint( uint32 Version, uint8 ** Ptr ) {
    EDITOR__CHECKPOINT Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            boo32 DoesExist = _read( ptr, boo32 );
            boo32 IsActive  = _read( ptr, boo32 );
            rect  Bound     = _read( ptr, rect  );
            
            EDITOR__CHECKPOINT Entity = {};
            Entity.DoesExist = DoesExist;
            Entity.IsActive  = IsActive;
            Entity.Bound     = Bound;
            
            Result = Entity;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    return Result;
}

internal void
ROOM_LoadCheckpointInit( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 iRoom ) {
    WORLD_STATE * World = &AppState->World;
    ROOM        * Room  = World->Room + iRoom;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        Room->iCheckpoint = World->nCheckpoint;
        
        Assert( World->nCheckpoint < WORLD__CHECKPOINT_MAX_COUNT );
        World->nCheckpoint++;
        
        EDITOR__CHECKPOINT Src    = ReadCheckpoint( Version, &ptr );
        WORLD__CHECKPOINT  Entity = {};
        Entity.IsActive = Src.IsActive;
        
        World->Checkpoint[ Room->iCheckpoint ] = Entity;
        
        _popSize( TempMemory, file.size );
    } else {
        Room->iCheckpoint = -1;
    }
}

internal void
GAME_LoadCheckpoint( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        
        CHECKPOINT_STATE * State = &AppState->Checkpoint;
        EDITOR__CHECKPOINT Src   = ReadCheckpoint( Version, &ptr );
        
        CHECKPOINT Entity = {};
        Entity.DoesExist = Src.DoesExist;
        Entity.IsActive  = Src.IsActive;
        Entity.Bound     = Src.Bound;
        
        State->Entity = Entity;
        
        _popSize( TempMemory, file.size );
    }
}

internal void
EDITOR_LoadCheckpoint( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    EDITOR_STATE             * Editor = &AppState->Editor;
    EDITOR__CHECKPOINT_STATE * State  = &Editor->Checkpoint;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        
        EDITOR__CHECKPOINT Src = ReadCheckpoint( Version, &ptr );
        State->Entity = Src;
        
        _popSize( TempMemory, file.size );
    }
}