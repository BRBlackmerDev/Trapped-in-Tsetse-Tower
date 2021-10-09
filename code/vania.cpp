
internal void
APP_updateAndRender( APP_STATE * AppState, PLATFORM * Platform ) {
    RENDERER * Renderer = &Platform->Renderer;
    RENDER_PASS * Pass_Game = &Renderer->Pass_Game;
    RENDER_PASS * Pass_UI   = &Renderer->Pass_UI;
    
	MOUSE_STATE      * Mouse      = &Platform->Mouse;
	KEYBOARD_STATE   * Keyboard   = &Platform->Keyboard;
    CONTROLLER_STATE * controller = &Platform->controller;
	
	MEMORY * permMemory = &Platform->permMemory;
	MEMORY * TempMemory = &Platform->TempMemory;
	
    //AUDIO_SYSTEM * audioSystem = &Platform->audioSystem;
	//ASSET_SYSTEM * assetSystem = &Platform->assetSystem;
    
    flo32 dt = Platform->targetSec;
    AppState->dt = dt;
    
    switch( AppState->Mode ) {
        case appMode_game: {
            CAMERA_STATE * Camera = &AppState->Camera;
            
            vec2 WorldPos = getWorldPos( Camera, AppState->app_dim, Mouse->Position );
            DISPLAY_VALUE( vec2, WorldPos );
            
            if( !AppState->isInitialized ) {
                AppState->isInitialized    = true;
                
                initDebugPanel( AppState );
                startGame( Platform, AppState, TempMemory );
            }
            
            REPLAY_STATE * replay = &AppState->replay;
            PANEL        * panel  = &AppState->debug;
            
            if( !replay->show ) {
                if( AppState->ChangeRoom ) {
                    OutputRoomToWorld( AppState, TempMemory );
                    ChangeRoom( Platform, AppState, TempMemory );
                    InputWorldToRoom( AppState );
                }
                
                updateEvent( AppState );
                updateWorld( AppState, TempMemory, dt );
                
                if( AppState->stat.isTopDown ) {
                    updatePlayerInputT( AppState, controller, dt );
                    updatePlayerT     ( AppState, dt );
                    updatePlayerHitScan( AppState );
                    //UpdateBeast( AppState, TempMemory, dt );
                } else {
                    updatePlayerInput( AppState, controller, dt );
                    updatePlayer( AppState, dt );
                    updatePlayerHitScan( AppState );
                    
                }
                updateExit( AppState );
                updateGrenade( AppState, dt );
                updateJumper( AppState, TempMemory, dt );
                updateNest( AppState, dt );
                updateSecureDoor( AppState, dt );
                updateMechDoor( AppState, dt );
                updatePowerSwitch( AppState, dt );
                updateParticle( AppState, dt );
                UpdateCheckpoint( AppState, dt );
                
                { // update Camera
                    rect Bound = AppState->Camera_Bound;
                    
                    if( panel->show ) {
                        // TODO: check if Camera_dim is larger than terrain_dim
                        flo32 width = getWidth( panel->Bound ) / AppState->app_dim.x * Camera->dim.x;
                        
                        Bound.Right += width;
                    }
                    PLAYER_STATE * Player = &AppState->Player;
                    
                    vec2 PlayerP = GetPlayerCameraP( *Player );
                    vec2 CameraP = clampToBound( PlayerP, Bound );
                    
                    Camera->pos = CameraP;
                }
                
                captureReplay( AppState );
            }
            
            DRAW_STATE  _Draw = {};
            DRAW_STATE * Draw = &_Draw;
            Draw->app_dim       = AppState->app_dim;
            Draw->app_halfDim   = AppState->app_halfDim;
            Draw->App_Bound     = AppState->App_Bound;
            Draw->font          = AppState->font;
            Draw->Debug_iEntity = AppState->Debug_iEntity;
            Draw->AppState      = AppState;
            Draw->Collision     = &AppState->Collision;
            Draw->Exit          = &AppState->Exit;
            Draw->Checkpoint    = &AppState->Checkpoint;
            
            if( replay->show ) {
                if( !replay->show_holdFrame ) {
                    flo32 t = clamp01( Mouse->Position.x / AppState->app_dim.x );
                    replay->show_atFrame = ( ( uint32 )( t * ( flo32 )REPLAY_FRAME_MAX_COUNT ) + replay->atFrame ) % REPLAY_FRAME_MAX_COUNT;
                    DISPLAY_VALUE( uint32, replay->show_atFrame );
                }
                
                if( wasPressed( Mouse, MouseButton_Right ) ) {
                    replay->show_holdFrame = !replay->show_holdFrame;
                }
                
                REPLAY_FRAME * frame = replay->frame + replay->show_atFrame;
                Draw->Beast   = &frame->Beast;
                Draw->Player  = &frame->Player;
                Draw->Grenade = &frame->Grenade;
                Draw->Jumper  = &frame->Jumper;
                Draw->Nest    = &frame->Nest;
                Draw->Secure  = &frame->Secure;
                Draw->Mech    = &frame->Mech;
                Draw->power   = &frame->power;
                
                Draw->PanicPost = &frame->PanicPost;
            } else {
                Draw->Beast      = &AppState->Beast;
                Draw->Player     = &AppState->Player;
                Draw->Grenade    = &AppState->Grenade;
                Draw->Jumper     = &AppState->Jumper;
                Draw->Nest       = &AppState->Nest;
                Draw->Secure     = &AppState->Secure;
                Draw->Mech       = &AppState->Mech;
                Draw->power      = &AppState->power;
                
                Draw->PanicPost  = &AppState->PanicPost;
            }
            if( wasPressed( Keyboard, KeyCode_e, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                replay->show = !replay->show;
                replay->show_holdFrame = false;
            }
            if( wasPressed( controller, controllerButton_start ) ) {
                if( replay->show ) {
                    replay->show = false;
                    panel->show  = false;
                } else {
                    replay->show = true;
                    panel->show  = true;
                }
            }
            
            DrawTerrain    ( Pass_Game, AppState );
            DrawNest       ( Pass_Game, Pass_UI, Draw );
            DrawSecureDoor ( Pass_Game, Draw );
            DrawMechDoor   ( Pass_Game, Draw );
            DrawPowerSwitch( Pass_Game, Draw );
            DrawCheckpoint ( Pass_Game, Draw );
            DrawExit       ( Pass_Game, Draw );
            //DrawBeast      ( Pass_Game, Draw );
            
            if( AppState->stat.isTopDown ) {
                DrawPlayerT( Pass_Game, Pass_UI, Draw );
            } else {
                DrawPlayer    ( Pass_Game, Pass_UI, Draw );
            }
            DrawJumper    ( Pass_Game, Draw );
            DrawGrenade   ( Pass_Game, Draw );
            
            DrawParticle( Pass_Game, AppState );
            
            if( !replay->show ) {
                //FinalizeBeast( AppState );
                FinalizePlayer( AppState );
                FinalizeGrenade( AppState );
                FinalizeJumper( AppState, TempMemory );
                FinalizeNest  ( AppState );
                FinalizeSecureDoor( AppState );
                FinalizeMechDoor( AppState );
                FinalizePowerSwitch( AppState );
                FinalizeExit  ( AppState );
                FinalizeCheckpoint( AppState );
                FinalizeParticle( AppState );
                
                if( AppState->DoSaveGame ) {
                    AppState->DoSaveGame = false;
                    SaveGame( AppState, TempMemory );
                    
                    AppState->DoRecentSave = true;
                }
                if( AppState->DoRecentSave ) {
                    flo32 TargetTime = ( 1.0f / 60.0f );
                    if( AppState->RecentSave_Timer < TargetTime ) {
                        DrawRect( Pass_UI, AppState->App_Bound, COLOR_WHITE );
                    }
                    
                    AppState->RecentSave_Timer += dt;
                    if( AppState->RecentSave_Timer >= 3.0f ) {
                        AppState->DoRecentSave       = false;
                        AppState->RecentSave_Timer = 0.0f;
                    }
                    
                    vec2 P = getT( AppState->App_Bound, Vec2( 0.5f, 0.1f ) );
                    DrawString( Pass_UI, AppState->font, "Game Saved.", P, textAlign_Center, Vec2( 2.0f, 2.0f ), COLOR_WHITE );
                }
                
                { // LOAD FROM CHECKPOINT
                    PLAYER_STATE * Player = &AppState->Player;
                    if( ( Player->IsDead ) && ( Player->IsDead_Timer >= 1.0f ) ) {
                        vec2 P = getT( AppState->App_Bound, Vec2( 0.45f, 0.1f ) );
                        
                        vec2 Dim = Vec2( 36.0f, 36.0f );
                        rect R   = rectRCD( P + Vec2( -10.0f, 0.0f ), Dim );
                        DrawRect       ( Pass_UI, R, COLOR_GRAY( 0.02f ) );
                        DrawRectOutline( Pass_UI, R, COLOR_GRAY( 0.1f  ) );
                        
                        if( AppState->LoadCheckpoint_IsActive ) {
                            AppState->LoadCheckpoint_Timer += dt;
                            
                            flo32 t = clamp01( AppState->LoadCheckpoint_Timer / LOAD_CHECKPOINT_TARGET_TIME );
                            rect  S = R;
                            S.Top = lerp( S.Bottom, t, S.Top );
                            
                            DrawRect( Pass_UI, S, COLOR_YELLOW );
                        } else {
                            AppState->LoadCheckpoint_Timer = 0.0f;
                        }
                        
                        DrawRect( Pass_UI, TextureID_ControlIcon_Use, R, COLOR_WHITE );
                        
                        DrawString( Pass_UI, AppState->font, ": Load Checkpoint", P, textAlign_CenterLeft, Vec2( 2.0f, 2.0f ), COLOR_WHITE );
                        
                        if( AppState->LoadCheckpoint_Timer >= LOAD_CHECKPOINT_TARGET_TIME ) {
                            AppState->DoLoadCheckpoint = true;
                            
                            AppState->LoadCheckpoint_Timer    = 0.0f;
                            AppState->LoadCheckpoint_IsActive = false;
                        }
                    }
                }
                if( wasPressed( Keyboard, KeyCode_c, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                    AppState->DoLoadCheckpoint = true;
                }
                if( AppState->DoLoadCheckpoint ) {
                    AppState->DoLoadCheckpoint = false;
                    LoadCheckpoint( Platform, AppState, TempMemory );
                }
                if( wasPressed( Keyboard, KeyCode_r, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                    AppState->testRoom = false;
                    startGame( Platform, AppState, TempMemory );
                }
                if( wasPressed( Keyboard, KeyCode_t, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                    if( AppState->testRoom_name[ 0 ] != 0 ) {
                        AppState->testRoom = true;
                        startGame( Platform, AppState, TempMemory );
                    } 
                }
                if( wasPressed( controller, controllerButton_back ) ) {
                    startGame( Platform, AppState, TempMemory );
                }
            }
            if( wasPressed( Keyboard, KeyCode_l, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                AppState->Mode = appMode_Editor;
            }
            
            rect R = rectCD( AppState->lastJump, Vec2( 0.1f, 0.1f ) );
            DrawRect( Pass_Game, R, COLOR_YELLOW );
            
            UpdateAndDrawDebugPanel( AppState, Mouse, Pass_Game, Pass_UI, Draw );
            if( wasPressed( Keyboard, KeyCode_d, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                panel->show = !panel->show;
            }
        } break;
        
        case appMode_Editor: {
            EDITOR_STATE * Editor = &AppState->Editor;
            CAMERA_STATE * Camera = &Editor->Camera;
            FILE_SAVE_OPEN * SaveOpen = &Editor->SaveOpen;
            
            if( !Editor->isInitialized ) {
                Editor->isInitialized = true;
                
                SaveOpen->fileList  = StringBuffer( permMemory, 4096, _KB( 256 ) );
                Editor->Event_names = StringBuffer( permMemory, 1024, _KB(  32 ) );
                
                initEditorPanel( AppState );
                reset( Editor );
            }
            
            updateCamera( Camera, Keyboard, dt );
            vec2 MouseP = getWorldPos( Camera, AppState->app_dim, Mouse->Position );
            DISPLAY_VALUE( vec2, MouseP );
            
            { // Draw cursor cell
                UINT32_PAIR cell = getCell( Camera, AppState->app_dim, Mouse->Position );
                DISPLAY_VALUE( uint32, cell.x );
                DISPLAY_VALUE( uint32, cell.y );
                
                rect R = getBound( cell.x, cell.y );
                DrawRectOutline( Pass_Game, R, COLOR_YELLOW );
            }
            
            { // Draw grid Bound
                vec2 grid_dim = Vec2( ( flo32 )EDITOR__GRID_MAX_WIDTH, ( flo32 )EDITOR__GRID_MAX_HEIGHT ) * TILE_DIM;
                rect grid_Bound = rectBLD( Vec2( 0.0f, 0.0f ), grid_dim );
                DrawRectOutline( Pass_Game, grid_Bound, COLOR_RED );
            }
            
            rect view_Bound  = AppState->App_Bound;
            view_Bound.Right = Editor->panel.Bound.Left;
            
            DISPLAY_VALUE( rect, Editor->stat.Bound );
            
            if( Editor->DrawGrid ) {
                DrawEditorGrid( Pass_Game, AppState );
            }
            
            boo32 canSaveOpen = true;
            if( IsInBound( Mouse->Position, view_Bound ) ) {
                boo32 moUse_IsActive = isDown( Mouse, MouseButton_Left  );
                boo32 erase_IsActive = isDown( Mouse, MouseButton_Right );
                if( ( Editor->Mode == EditorMode_terrain ) && ( ( wasReleased( Mouse, MouseButton_Left ) ) || ( wasReleased( Mouse, MouseButton_Right ) ) ) ) {
                    updateRoomBound( AppState );
                }
                
                if( ( moUse_IsActive ) || ( erase_IsActive ) ) {
                    canSaveOpen = false;
                    SaveOpen->unsavedChanges = true;
                }
                
                switch( Editor->Mode ) {
                    case EditorMode_default: {
                        
                    } break;
                    
                    case EditorMode_terrain: {
                        boo32 DrawGuide = false;
                        if( Editor->terrain_doPlace ) {
                            DrawGuide = true;
                            if( wasReleased( Mouse, MouseButton_Left ) ) {
                                UINT32_PAIR iCell = getCell( MouseP ) ;
                                UINT32_QUAD Bound = getExtrema( iCell, Editor->Mouse_iCell );
                                
                                for( uint32 y = Bound.min.y; y <= Bound.max.y; y++ ) {
                                    for( uint32 x = Bound.min.x; x <= Bound.max.x; x++ ) {
                                        Editor->grid[ y ][ x ] = CellType_Terrain;
                                    }
                                }
                                Editor->terrain_doPlace = false;
                            }
                            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->terrain_doPlace = false;
                            }
                        } else if( Editor->terrain_doErase ) {
                            DrawGuide = true;
                            if( wasReleased( Mouse, MouseButton_Right ) ) {
                                UINT32_PAIR iCell = getCell( MouseP ) ;
                                UINT32_QUAD Bound = getExtrema( iCell, Editor->Mouse_iCell );
                                
                                for( uint32 y = Bound.min.y; y <= Bound.max.y; y++ ) {
                                    for( uint32 x = Bound.min.x; x <= Bound.max.x; x++ ) {
                                        Editor->grid[ y ][ x ] = CellType_Empty;
                                    }
                                }
                                Editor->terrain_doErase = false;
                            }
                            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->terrain_doErase = false;
                            }
                        } else {
                            if( wasPressed( Mouse, MouseButton_Left ) ) {
                                Editor->terrain_doPlace = true;
                                Editor->Mouse_iCell     = getCell( MouseP );
                            } else if( wasPressed( Mouse, MouseButton_Right ) ) {
                                Editor->terrain_doErase = true;
                                Editor->Mouse_iCell     = getCell( MouseP );
                            }
                        }
                        
                        if( DrawGuide ) {
                            UINT32_PAIR iCellA = Editor->Mouse_iCell;
                            UINT32_PAIR iCellB = getCell( MouseP );
                            uint32 x = abs( ( int32 )iCellA.x - ( int32 )iCellB.x ) + 1;
                            uint32 y = abs( ( int32 )iCellA.y - ( int32 )iCellB.y ) + 1;
                            
                            UINT32_QUAD Bound = getExtrema( iCellA, iCellB );
                            rect        R     = getBound( Bound );
                            DrawRectOutline( Pass_Game, R, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            vec2 P = getCellTL( iCellB );
                            char str[ 128 ] = {};
                            sprintf( str, "%u,%u", x, y );
                            DrawString( Pass_Game, AppState->font, str, P, textAlign_BottomLeft, WORLD_DEBUG_TEXT_SCALE * Camera->scale_ratio, COLOR_YELLOW );
                        }
                    } break;
                    
                    case EditorMode_PlayerSpawn: {
                        if( wasPressed( Mouse, MouseButton_Left ) ) {
                            Editor->Player_nSpawnPos     = 1;
                            Editor->Player_SpawnPos[ 0 ] = MouseP;
                            // TODO: error if placed in terrain, but this probably won't be an issue
                        }
                    } break;
                    
                    case EditorMode_Exit: {
                        // add new  : Left click + no hover, drag to set size
                        // move     : Left click + hover, drag to place
                        // Type     : Mouse wheel + hover
                        // delete   : delete + hover
                        
                        EDITOR__EXIT_STATE * State = &Editor->Exit;
                        EXIT_DIMS;
                        
                        int32 iHover = -1;
                        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                            if( IsInBound( MouseP, State->Entity[ iEntity ].Bound ) ) {
                                iHover = iEntity;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( State->nEntity < EXIT_MAX_COUNT ) {
                                EDITOR__EXIT Entity = {};
                                Entity.Type  = ( Editor->stat.isTopDown ) ? ExitTypeT_Walk_EnterLeft : ExitTypeS_Walk_EnterLeft;
                                Entity.Bound = rectBLD( getCellBL( MouseP ), ExitDim[ Entity.Type ] );
                                State->Entity[ State->nEntity++ ] = Entity;
                            } else {
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to add new SECURE_DOOR, but State was full! MaxCount = %u", SECURE_DOOR_MAX_COUNT );
                                CONSOLE_STRING( str );
                                globalVar_debugSystem.errorOccurred = true;
                            }
                        }
                        if( ( iHover > -1 ) && ( wasPressed( Keyboard, KeyCode_delete ) ) ) {
                            State->nEntity--;
                            for( uint32 iEntity = iHover; iEntity < State->nEntity; iEntity++ ) {
                                State->Entity[ iEntity ] = State->Entity[ iEntity + 1 ];
                            }
                            State->Entity[ State->nEntity ] = {};
                        }
                        if( ( iHover > -1 ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
                            Editor->Exit_doMove = true;
                            
                            Editor->Mouse_iCell   = getCell( MouseP );
                            Editor->Mouse_iEntity = iHover;
                        }
                        if( Editor->Exit_doMove ) {
                            UINT32_PAIR iCell = getCell( MouseP );
                            vec2 P = getWorldPos( Editor->Mouse_iCell );
                            vec2 Q = getWorldPos( iCell );
                            vec2 offset = Q - P;
                            
                            EDITOR__EXIT * Entity = State->Entity + Editor->Mouse_iEntity;
                            
                            rect Bound = addOffset( Entity->Bound, offset );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( wasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Exit_doMove = false;
                                
                                if( iCell != Editor->Mouse_iCell ) {
                                    Entity->Bound = Bound;
                                }
                            }
                            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Exit_doMove = false;
                            }
                        }
                        if( ( iHover > -1 ) && ( Mouse->wheelClicks != 0 ) ) {
                            EDITOR__EXIT * Entity = State->Entity + iHover;
                            
                            Entity->Type = ( EXIT_TYPE )( ( Entity->Type + ( ExitType_count * 2 ) + Mouse->wheelClicks ) % ExitType_count );
                            
                            Entity->Bound = rectBLD( getBL( Entity->Bound ), ExitDim[ Entity->Type ] );
                        }
                    } break;
                    
                    case EditorMode_Nest: {
                        EDITOR__NEST_STATE * State = &Editor->Nest;
                        // TODO: set Type : Mouse wheel
                        // add new  : Left click + no hover
                        // TODO: move     : Left click + hover
                        // TODO: isAwake  : ???
                        
                        int32 iHover = -1;
                        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                            EDITOR__NEST * Entity = State->Entity + iEntity;
                            
                            rect R = Entity->Bound;
                            if( IsInBound( MouseP, R ) ) {
                                iHover = iEntity;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
                            Editor->Mouse_doMouse = true;
                            Editor->Mouse_iCell   = getCell( MouseP );
                        }
                        if( ( iHover > -1 ) && ( wasPressed( Keyboard, KeyCode_delete ) ) ) {
                            State->nEntity--;
                            for( uint32 iEntity = iHover; iEntity < State->nEntity; iEntity++ ) {
                                State->Entity[ iEntity ] = State->Entity[ iEntity + 1 ];
                            }
                        }
                        if( Editor->Mouse_doMouse ) {
                            UINT32_PAIR iCell = getCell( MouseP );
                            
                            rect R = getBound( Editor->Mouse_iCell );
                            rect S = getBound( iCell );
                            rect Bound = rectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( wasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_doMouse = false;
                                
                                if( State->nEntity < NEST_MAX_COUNT ) {
                                    EDITOR__NEST Entity = {};
                                    Entity.Bound = Bound;
                                    
                                    State->Entity[ State->nEntity++ ] = Entity;
                                } else {
                                    char str[ 512 ] = {};
                                    sprintf( str, "ERROR! Attempted to add new NEST, but State was full! MaxCount = %u", NEST_MAX_COUNT );
                                    CONSOLE_STRING( str );
                                    globalVar_debugSystem.errorOccurred = true;
                                }
                            }
                            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_doMouse = false;
                            }
                        }
                    } break;
                    
                    case EditorMode_SecureDoor: {
                        // add new  : Left click + no hover
                        // TODO: move     : Left click + hover
                        
                        EDITOR__SECURE_DOOR_STATE * State = &Editor->Secure;
                        
                        int32 iHover = -1;
                        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                            if( IsInBound( MouseP, State->Entity[ iEntity ].Bound ) ) {
                                iHover = iEntity;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( State->nEntity < SECURE_DOOR_MAX_COUNT ) {
                                State->nEntity++;
                                
                                Editor->Mouse_doMouse = true;
                                Editor->Mouse_iCell   = getCell( MouseP );
                            } else {
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to add new SECURE_DOOR, but State was full! MaxCount = %u", SECURE_DOOR_MAX_COUNT );
                                CONSOLE_STRING( str );
                                globalVar_debugSystem.errorOccurred = true;
                            }
                        }
                        if( ( iHover > -1 ) && ( wasPressed( Keyboard, KeyCode_delete ) ) ) {
                            State->nEntity--;
                            for( uint32 iEntity = iHover; iEntity < State->nEntity; iEntity++ ) {
                                State->Entity[ iEntity ] = State->Entity[ iEntity + 1 ];
                            }
                            State->Entity[ State->nEntity ] = {};
                        }
                        if( ( iHover > -1 ) && ( Mouse->wheelClicks != 0 ) ) {
                            EDITOR__SECURE_DOOR * Entity = State->Entity + iHover;
                            Entity->isClosed = !Entity->isClosed;
                        }
                        if( Editor->Mouse_doMouse ) {
                            UINT32_PAIR iCell = getCell( MouseP );
                            
                            rect R = getBound( Editor->Mouse_iCell );
                            rect S = getBound( iCell );
                            rect Bound = rectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( wasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_doMouse = false;
                                State->Entity[ State->nEntity - 1 ].Bound = Bound;
                            }
                            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                                State->nEntity--;
                                Editor->Mouse_doMouse = false;
                            }
                        }
                    } break;
                    
                    case EditorMode_MechDoor: {
                        // add new  : Left click + no hover
                        // move     : Left click + hover
                        
                        EDITOR__MECH_DOOR_STATE * State = &Editor->Mech;
                        
                        int32  iHover = -1;
                        uint32 id     = 0;
                        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                            EDITOR__MECH_DOOR Entity = State->Entity[ iEntity ];
                            if( IsInBound( MouseP, Entity.Bound_Door ) ) {
                                iHover = iEntity;
                            }
                            if( IsInBound( MouseP, Entity.Bound_SwitchA ) ) {
                                iHover = iEntity;
                                id     = 1;
                            }
                            if( IsInBound( MouseP, Entity.Bound_SwitchB ) ) {
                                iHover = iEntity;
                                id     = 2;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( State->nEntity < MECH_DOOR_MAX_COUNT ) {
                                State->nEntity++;
                                
                                Editor->Mouse_doMouse = true;
                                Editor->Mouse_iCell   = getCell( MouseP );
                            } else {
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to add new MECH_DOOR, but State was full! MaxCount = %u", MECH_DOOR_MAX_COUNT );
                                CONSOLE_STRING( str );
                                globalVar_debugSystem.errorOccurred = true;
                            }
                        }
                        if( ( iHover > -1 ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
                            Editor->Mech_doMove   = true;
                            
                            Editor->Mouse_iCell   = getCell( MouseP );
                            Editor->Mouse_iEntity = iHover;
                            Editor->Mouse_iSubEntity = id;
                        }
                        if( ( iHover > -1 ) && ( wasPressed( Keyboard, KeyCode_delete ) ) ) {
                            State->nEntity--;
                            for( uint32 iEntity = iHover; iEntity < State->nEntity; iEntity++ ) {
                                State->Entity[ iEntity ] = State->Entity[ iEntity + 1 ];
                            }
                            State->Entity[ State->nEntity ] = {};
                        }
                        if( ( iHover > -1 ) && ( Mouse->wheelClicks != 0 ) ) {
                            EDITOR__MECH_DOOR * Entity = State->Entity + iHover;
                            if( id == 0 ) {
                                Entity->isClosed = !Entity->isClosed;
                            }
                            if( id == 1 ) {
                                Entity->Bound_SwitchA = rectBLD( Vec2( 0.0f, 0.0f ), MECH_DOOR_SWITCH_DIM );
                            }
                            if( id == 2 ) {
                                Entity->Bound_SwitchB = rectBLD( Vec2( 0.0f, 0.0f ), MECH_DOOR_SWITCH_DIM );
                            }
                        }
                        if( Editor->Mech_doMove ) {
                            UINT32_PAIR iCell = getCell( MouseP );
                            vec2 P = getWorldPos( Editor->Mouse_iCell );
                            vec2 Q = getWorldPos( iCell );
                            vec2 offset = Q - P;
                            
                            EDITOR__MECH_DOOR * Entity = State->Entity + Editor->Mouse_iEntity;
                            rect * R = &Entity->Bound_Door;
                            if( Editor->Mouse_iSubEntity == 1 ) {
                                R = &Entity->Bound_SwitchA;
                            }
                            if( Editor->Mouse_iSubEntity == 2 ) {
                                R = &Entity->Bound_SwitchB;
                            }
                            
                            rect Bound = addOffset( *R, offset );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( wasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mech_doMove = false;
                                
                                if( iCell != Editor->Mouse_iCell ) {
                                    *R = Bound;
                                }
                            }
                            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mech_doMove = false;
                            }
                        }
                        if( Editor->Mouse_doMouse ) {
                            UINT32_PAIR iCell = getCell( MouseP );
                            
                            rect R = getBound( Editor->Mouse_iCell );
                            rect S = getBound( iCell );
                            rect Bound = rectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( wasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_doMouse = false;
                                
                                vec2 P = getBL( Bound ) + Vec2( -TILE_WIDTH * 2.0f, TILE_HEIGHT * 3.0f );
                                vec2 Q = getBR( Bound ) + Vec2(  TILE_WIDTH * 2.0f, TILE_HEIGHT * 3.0f );
                                
                                EDITOR__MECH_DOOR * Entity = State->Entity + ( State->nEntity - 1 );
                                Entity->Bound_Door    = Bound;
                                Entity->Bound_SwitchA = rectBRD( P, MECH_DOOR_SWITCH_DIM );
                                Entity->Bound_SwitchB = rectBLD( Q, MECH_DOOR_SWITCH_DIM );
                            }
                            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                                State->nEntity--;
                                Editor->Mouse_doMouse = false;
                            }
                        }
                    } break;
                    
                    case EditorMode_interior: {
                        if( wasPressed( Mouse, MouseButton_Left ) ) {
                            ROOM_STATS * stat = &Editor->stat;
                            
                            UINT32_PAIR cell = getCell( MouseP );
                            if( Editor->grid[ cell.y ][ cell.x ] <= CellType_Interior ) {
                                stat->intCell = cell;
                            }
                        }
                    } break;
                    
                    case EditorMode_powerSwitch: {
                        // add new  : Left click + no hover
                        // move     : Left click + hover
                        EDITOR__POWER_SWITCH_STATE * State = &Editor->power;
                        
                        int32 iHover = -1;
                        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                            EDITOR__POWER_SWITCH Entity = State->Entity[ iEntity ];
                            if( IsInBound( MouseP, Entity.Bound ) ) {
                                iHover = iEntity;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( State->nEntity < POWER_SWITCH_MAX_COUNT ) {
                                State->nEntity++;
                                
                                Editor->Mouse_doMouse = true;
                                Editor->Mouse_iCell   = getCell( MouseP );
                            } else {
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to add new POWER_SWITCH, but State was full! MaxCount = %u", POWER_SWITCH_MAX_COUNT );
                                CONSOLE_STRING( str );
                                globalVar_debugSystem.errorOccurred = true;
                            }
                        }
                        if( ( iHover > -1 ) && ( wasPressed( Keyboard, KeyCode_delete ) ) ) {
                            State->nEntity--;
                            for( uint32 iEntity = iHover; iEntity < State->nEntity; iEntity++ ) {
                                State->Entity[ iEntity ] = State->Entity[ iEntity + 1 ];
                            }
                            State->Entity[ State->nEntity ] = {};
                        }
                        if( ( iHover > -1 ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
                            Editor->power_doMove  = true;
                            
                            Editor->Mouse_iCell   = getCell( MouseP );
                            Editor->Mouse_iEntity = iHover;
                        }
                        if( Editor->Mouse_doMouse ) {
                            UINT32_PAIR iCell = getCell( MouseP );
                            
                            rect R = getBound( Editor->Mouse_iCell );
                            rect S = getBound( iCell );
                            rect Bound = rectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( wasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_doMouse = false;
                                State->Entity[ State->nEntity - 1 ].Bound = Bound;
                            }
                            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                                State->nEntity--;
                                Editor->Mouse_doMouse = false;
                            }
                        }
                        if( Editor->power_doMove ) {
                            UINT32_PAIR iCell = getCell( MouseP );
                            
                            EDITOR__POWER_SWITCH * Entity = State->Entity + Editor->Mouse_iEntity;
                            vec2 dim = getDim( Entity->Bound );
                            vec2 P   = getCellBL( iCell );
                            rect R   = rectBLD( P, dim );
                            DrawRectOutline( Pass_Game, R, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( wasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->power_doMove = false;
                                Entity->Bound        = R;
                            }
                            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->power_doMove = false;
                            }
                        }
                    } break;
                    
                    case EditorMode_Checkpoint: {
                        // add new  : Left click + no hover
                        // move     : Left click + hover
                        EDITOR__CHECKPOINT_STATE * State  = &Editor->Checkpoint;
                        EDITOR__CHECKPOINT       * Entity = &State->Entity;
                        
                        int32 iHover = -1;
                        if( ( Entity->DoesExist ) && ( IsInBound( MouseP, Entity->Bound ) ) ) {
                            iHover = 0;
                        }
                        
                        if( ( iHover == -1 ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( !Entity->DoesExist ) {
                                Editor->Mouse_doMouse = true;
                                Editor->Mouse_iCell   = getCell( MouseP );
                            }
                        }
                        if( ( iHover > -1 ) && ( wasPressed( Keyboard, KeyCode_delete ) ) ) {
                            Entity->DoesExist = false;
                        }
                        if( ( iHover > -1 ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
                            Editor->Checkpoint_DoMove  = true;
                            
                            Editor->Mouse_iCell   = getCell( MouseP );
                            Editor->Mouse_iEntity = iHover;
                        }
                        if( ( iHover > -1 ) && ( Mouse->wheelClicks != 0 ) ) {
                            Entity->IsActive = !Entity->IsActive;
                        }
                        if( Editor->Mouse_doMouse ) {
                            UINT32_PAIR iCell = getCell( MouseP );
                            
                            rect R = getBound( Editor->Mouse_iCell );
                            rect S = getBound( iCell );
                            rect Bound = rectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( wasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_doMouse = false;
                                
                                Entity->DoesExist = true;
                                Entity->Bound     = Bound;
                            }
                            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_doMouse = false;
                            }
                        }
                        if( Editor->Checkpoint_DoMove ) {
                            UINT32_PAIR iCell = getCell( MouseP );
                            
                            vec2 dim = getDim( Entity->Bound );
                            vec2 P   = getCellBL( iCell );
                            rect R   = rectBLD( P, dim );
                            DrawRectOutline( Pass_Game, R, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( wasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Checkpoint_DoMove = false;
                                Entity->Bound             = R;
                            }
                            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Checkpoint_DoMove = false;
                            }
                        }
                    } break;
                    
#if 0
                    case EditorMode_NavMesh: {
                        // add new    : N -> New Tri mode
                        //            : Left click 3 Interior Vertices
                        // cancel new : escape
                        // delete     : hover Tri + delete
                        EDITOR__NAV_MESH_STATE * State     = &Editor->NavMesh;
                        COLLISION_STATE        * Collision = &AppState->Collision;
                        
                        if( Editor->NavMesh_DoNewTri ) {
                            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->NavMesh_DoNewTri = false;
                            }
                            
                            uint32 nElem = 0;
                            for( uint32 iElem = 0; iElem < 3; iElem++ ) {
                                if( Editor->NavMesh_NewTri.elem[ iElem ] > -1 ) {
                                    nElem++;
                                }
                            }
                            
                            int32 iHover = -1;
                            for( uint32 iVert = 0; iVert < Collision->nInteriorVertex; iVert++ ) {
                                vec2 P = Collision->InteriorVertex[ iVert ];
                                rect R = rectCD( P, TILE_DIM * 2.5f );
                                
                                vec4 Color = COLOR_RED;
                                if( IsInBound( MouseP, R ) ) {
                                    iHover = iVert;
                                    Color  = COLOR_YELLOW;
                                }
                                
                                DrawRectOutline( Pass_Game, R, Color );
                            }
                            if( ( iHover > -1 ) && ( nElem > 0 ) ) {
                                for( uint32 iElem = 0; iElem < 3; iElem++ ) {
                                    if( Editor->NavMesh_NewTri.elem[ iElem ] == iHover ) {
                                        iHover = -1;
                                    }
                                }
                            }
                            if( ( iHover > -1 ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
                                Editor->NavMesh_NewTri.elem[ nElem++ ] = iHover;
                                if( nElem >= 3 ) {
                                    UINT32_TRI Tri = ToUInt32Tri( Editor->NavMesh_NewTri );
                                    
                                    vec2 A = Collision->InteriorVertex[ Tri.x ];
                                    vec2 B = Collision->InteriorVertex[ Tri.y ];
                                    vec2 C = Collision->InteriorVertex[ Tri.z ];
                                    if( !IsCCW( A, B, C ) ) {
                                        _swap( uint32, Tri.y, Tri.z );
                                        _swap( vec2, B, C );
                                    }
                                    
                                    EDITOR__NAV_MESH_TRI T = {};
                                    T.iInteriorVert = Tri;
                                    T.iPathLink     = Int32Tri( -1, -1, -1 );
                                    T.A             = A;
                                    T.B             = B;
                                    T.C             = C;
                                    
                                    Assert( State->nEntity < NAV_MESH_TRI_MAX_COUNT );
                                    State->Entity[ State->nEntity++ ] = T;
                                    
                                    Editor->NavMesh_NewTri = Int32Tri( -1, -1, -1 );
                                    nElem = 0;
                                }
                            }
                            if( nElem > 0 ) {
                                for( uint32 iElem = 0; iElem < ( nElem - 1 ); iElem++ ) {
                                    uint32 iVertP = Editor->NavMesh_NewTri.elem[ iElem     ];
                                    uint32 iVertQ = Editor->NavMesh_NewTri.elem[ iElem + 1 ];
                                    
                                    vec2 P = Collision->InteriorVertex[ iVertP ];
                                    vec2 Q = Collision->InteriorVertex[ iVertQ ];
                                    DrawLine( Pass_Game, P, Q, COLOR_MAGENTA );
                                }
                                
                                uint32 iVert = Editor->NavMesh_NewTri.elem[ nElem - 1 ];
                                vec2    P    = Collision->InteriorVertex[ iVert ];
                                DrawLine( Pass_Game, P, MouseP, COLOR_RED );
                            }
                        } else {
                            int32 iHover = -1;
                            for( uint32 iTri = 0; iTri < State->nEntity; iTri++ ) {
                                EDITOR__NAV_MESH_TRI Tri = State->Entity[ iTri ];
                                
                                if( IsInBound( MouseP, Tri.A, Tri.B, Tri.C ) ) {
                                    iHover = iTri;
                                }
                            }
                            
                            if( ( iHover > -1 ) && ( wasPressed( Keyboard, KeyCode_delete ) ) ) {
                                State->nEntity--;
                                for( uint32 iTri = iHover; iTri < State->nEntity; iTri++ ) {
                                    State->Entity[ iTri ] = State->Entity[ iTri + 1 ];
                                }
                                
                                iHover = -1;
                            }
                            if( wasPressed( Keyboard, KeyCode_n ) ) {
                                Editor->NavMesh_DoNewTri = true;
                                Editor->NavMesh_NewTri   = Int32Tri( -1, -1, -1 );
                            }
                            
                            if( iHover > -1 ) {
                                EDITOR__NAV_MESH_TRI Tri = State->Entity[ iHover ];
                                
                                DrawLine( Pass_Game, Tri.A, Tri.B, COLOR_YELLOW );
                                DrawLine( Pass_Game, Tri.B, Tri.C, COLOR_YELLOW );
                                DrawLine( Pass_Game, Tri.C, Tri.A, COLOR_YELLOW );
                            }
                        }
                        
                        { // Draw Nav Mesh Tris
                            flo32 Offset = TILE_WIDTH * 0.25f;
                            for( uint32 iTri = 0; iTri < State->nEntity; iTri++ ) {
                                EDITOR__NAV_MESH_TRI Tri = State->Entity[ iTri ];
                                
                                vec2 A = Tri.A;
                                vec2 B = Tri.B;
                                vec2 C = Tri.C;
                                
                                vec2 AB = B - A;
                                vec2 BC = C - B;
                                vec2 CA = A - C;
                                
                                vec2 NA = GetNormal( GetPerp( AB ) ) * Offset;
                                vec2 NB = GetNormal( GetPerp( BC ) ) * Offset;
                                vec2 NC = GetNormal( GetPerp( CA ) ) * Offset;
                                
                                LINE2 LineA = Line2( A + NA, GetNormal( AB ) );
                                LINE2 LineB = Line2( B + NB, GetNormal( BC ) );
                                LINE2 LineC = Line2( C + NC, GetNormal( CA ) );
                                
                                LINE2_INTERSECT IntersectA = DoesIntersect( LineA, LineC );
                                LINE2_INTERSECT IntersectB = DoesIntersect( LineB, LineA );
                                LINE2_INTERSECT IntersectC = DoesIntersect( LineC, LineB );
                                
                                vec2 A0 = IntersectA.P;
                                vec2 B0 = IntersectB.P;
                                vec2 C0 = IntersectC.P;
                                
                                DrawLine( Pass_Game, A0, B0, COLOR_CYAN );
                                DrawLine( Pass_Game, B0, C0, COLOR_RED );
                                DrawLine( Pass_Game, C0, A0, COLOR_GREEN );
                            }
                        }
                        
                        { // Draw Nav Mesh Links
                            for( uint32 iTri = 0; iTri < State->nEntity; iTri++ ) {
                                EDITOR__NAV_MESH_TRI TriA = State->Entity[ iTri ];
                                for( uint32 iLink = 0; iLink < 3; iLink++ ) {
                                    int32 Link = TriA.iPathLink.elem[ iLink ];
                                    if( Link > -1 ) {
                                        EDITOR__NAV_MESH_TRI TriB = State->Entity[ Link ];
                                        
                                        vec2 P = ( TriA.A + TriA.B + TriA.C ) / 3.0f;
                                        vec2 Q = ( TriB.A + TriB.B + TriB.C ) / 3.0f;
                                        
                                        DrawLine( Pass_Game, P, Q, COLOR_CYAN );
                                    }
                                }
                            }
                        }
                    } break;
#endif
                    
                    case EditorMode_Event: {
                        switch( Editor->Event_Type ) {
                            case dataType_point: {
                                if( wasPressed( Mouse, MouseButton_Left ) ) {
                                    ( ( vec2 * )Editor->Event_data )[ 0 ] = MouseP;
                                }
                            } break;
                            
                            case dataType_rect: {
                                if( wasPressed( Mouse, MouseButton_Left ) ) {
                                    Editor->Event_doMouse = true;
                                    Editor->Event_iCell   = getCell( MouseP );
                                }
                                if( Editor->Event_doMouse ) {
                                    UINT32_PAIR iCell = getCell( MouseP );
                                    
                                    rect R = getBound( Editor->Event_iCell );
                                    rect S = getBound( iCell );
                                    rect Bound = rectExtrema( R, S );
                                    DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                                    
                                    if( wasReleased( Mouse, MouseButton_Left ) ) {
                                        Editor->Event_doMouse = false;
                                        
                                        ( ( rect * )Editor->Event_data )[ 0 ] = Bound;
                                    }
                                }
                            } break;
                        }
                    } break;
                }
                
                if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                    Editor->Mode = EditorMode_default;
                }
            }
            
            { // Draw
                for( uint32 iRow = 0; iRow < EDITOR__GRID_MAX_HEIGHT; iRow++ ) {
                    for( uint32 iCol = 0; iCol < EDITOR__GRID_MAX_WIDTH; iCol++ ) {
                        GRID_CELL_TYPE Type = ( GRID_CELL_TYPE )Editor->grid[ iRow ][ iCol ];
                        switch( Type ) {
                            case CellType_Empty: {
                                // do nothing
                            } break;
                            
                            case CellType_Interior: {
                                // TODO: group cells into larger rects for better rendering
                                //rect Bound = getBound( iCol, iRow );
                                //DrawRect( Pass_Game, Bound, Vec4( 0.2f, 0.0f, 0.0f, 1.0f ) );
                            } break;
                            
                            case CellType_Terrain: {
                                // TODO: group cells into larger rects for better rendering
                                rect Bound = getBound( iCol, iRow );
                                DrawRect       ( Pass_Game, Bound, TERRAIN_COLOR );
                                DrawRectOutline( Pass_Game, Bound, TERRAIN_OUTLINE_COLOR );
                            } break;
                        }
                    }
                }
                
                DrawPoint( Pass_Game, Editor->Player_SpawnPos[ 0 ], Vec2( 0.1f, 0.1f ), COLOR_GREEN );
                
                { // Draw Exit
                    EDITOR__EXIT_STATE * State = &Editor->Exit;
                    
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        EDITOR__EXIT Entity = State->Entity[ iEntity ];
                        
                        rect R = Entity.Bound;
                        DrawRectOutline( Pass_Game, R, COLOR_GREEN );
                        
                        vec2 P = {};
                        vec2 Q = {};
                        
                        flo32 margin = ( TILE_WIDTH * 0.5f );
                        switch( Entity.Type ) {
                            case ExitTypeS_Walk_EnterLeft:
                            case ExitTypeT_Walk_EnterLeft: {
                                P = getBL( R );
                                Q = getTL( R );
                            } break;
                            
                            case ExitTypeS_Walk_EnterRight:
                            case ExitTypeT_Walk_EnterRight: {
                                P = getTR( R );
                                Q = getBR( R );
                            } break;
                            
                            case ExitTypeT_Walk_EnterUp: {
                                P = getTL( R );
                                Q = getTR( R );
                            } break;
                            
                            case ExitTypeS_Grate_Drop:
                            case ExitTypeT_Walk_EnterDown: {
                                P = getBR( R );
                                Q = getBL( R );
                            } break;
                            
                            case ExitTypeS_Grate_Duck: {
                                P = getBL( R );
                                Q = getBR( R );
                            } break;
                            
                        }
                        
                        vec2 N = GetNormal( GetPerp( Q - P ) );
                        P += ( N * margin );
                        Q += ( N * margin );
                        vec2 U = ( ( P + Q ) * 0.5f ) + ( N * margin );
                        
                        DrawLine( Pass_Game, P, U, COLOR_YELLOW );
                        DrawLine( Pass_Game, Q, U, COLOR_YELLOW );
                    }
                }
                
                { // Draw Nest
                    EDITOR__NEST_STATE * State = &Editor->Nest;
                    ENTITY_TYPE_TAGS;
                    
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        EDITOR__NEST Entity = State->Entity[ iEntity ];
                        rect R = Entity.Bound;
                        DrawRect( Pass_Game, R, COLOR_GRAY( 0.3f ) );
                        
                        ENTITY_TYPE iType = Entity.Type;
                        vec2 P = getCenter( R );
                        DrawString( Pass_Game, AppState->font, EntityTypeTag[ iType ], P, textAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
                    }
                }
                
                { // DRAW SECURE DOOR
                    EDITOR__SECURE_DOOR_STATE * State = &Editor->Secure;
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        EDITOR__SECURE_DOOR Entity = State->Entity[ iEntity ];
                        
                        vec4 Color = Vec4( 0.0f, 0.5f, 0.0f, 1.0f );
                        if( Entity.isClosed ) {
                            Color = Vec4( 0.0f, 0.2f, 0.0f, 1.0f );
                        }
                        DrawRect( Pass_Game, Entity.Bound, Color );
                    }
                }
                
                { // Draw Mech door
                    EDITOR__MECH_DOOR_STATE * State = &Editor->Mech;
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        EDITOR__MECH_DOOR Entity = State->Entity[ iEntity ];
                        
                        vec4 Color = toColor( 60, 140, 120 );
                        if( Entity.isClosed ) {
                            Color = toColor( 30, 70, 60 );
                        }
                        DrawRect( Pass_Game, Entity.Bound_Door,    Color );
                        DrawRect( Pass_Game, Entity.Bound_SwitchA, toColor( 60, 140, 120 ) );
                        DrawRect( Pass_Game, Entity.Bound_SwitchB, toColor( 60, 140, 120 ) );
                        DrawRectOutline( Pass_Game, Entity.Bound_Door,    COLOR_BLACK );
                        DrawRectOutline( Pass_Game, Entity.Bound_SwitchA, COLOR_BLACK );
                        DrawRectOutline( Pass_Game, Entity.Bound_SwitchB, COLOR_BLACK );
                        
                        char str[ 16 ] = {};
                        sprintf( str, "%u", iEntity );
                        DrawString( Pass_Game, AppState->font, str, getCenter( Entity.Bound_Door ), textAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_BLACK );
                        DrawString( Pass_Game, AppState->font, str, getCenter( Entity.Bound_SwitchA ), textAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_BLACK );
                        DrawString( Pass_Game, AppState->font, str, getCenter( Entity.Bound_SwitchB ), textAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_BLACK );
                    }
                }
                
                { // Draw power switch
                    EDITOR__POWER_SWITCH_STATE * State = &Editor->power;
                    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                        EDITOR__POWER_SWITCH Entity = State->Entity[ iEntity ];
                        DrawRect( Pass_Game, Entity.Bound, toColor( 200, 220, 65 ) );
                        
                        char str[ 16 ] = {};
                        sprintf( str, "POWR: %u", Entity.iPower );
                        DrawString( Pass_Game, AppState->font, str, getCenter( Entity.Bound ), textAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_BLACK );
                    }
                }
                
                { // DRAW CHECKPOINT
                    EDITOR__CHECKPOINT_STATE * State  = &Editor->Checkpoint;
                    EDITOR__CHECKPOINT       * Entity = &State->Entity;
                    if( Entity->DoesExist ) {
                        vec4 Color = toColor( 110, 105, 0 ); 
                        if( Entity->IsActive ) {
                            Color = toColor( 220, 215, 0 );
                        }
                        
                        DrawRect( Pass_Game, Entity->Bound, Color );
                    }
                }
                
                { // Draw interior cell
                    ROOM_STATS stat = Editor->stat;
                    rect R = getBound( stat.intCell );
                    DrawRectOutline( Pass_Game, R, COLOR_RED );
                }
                
                if( Editor->DrawEntityCountOnEdge ) {
                    COLLISION_STATE * Collision = &AppState->Collision;
                    if( Collision->nJumperEdge > 0 ) {
                        EDITOR__JUMPER_STATE * State = &Editor->Jumper;
                        
#define JUMPER_EDGE_MAX_COUNT  ( 64 )
                        Assert( Collision->nJumperEdge < JUMPER_EDGE_MAX_COUNT );
                        uint32 nEntity[ JUMPER_EDGE_MAX_COUNT ] = {};
                        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                            EDITOR__JUMPER Entity = State->Entity[ iEntity ];
                            nEntity[ Entity.iJumperEdge ]++;
                        }
                        
                        for( uint32 iEdge = 0; iEdge < Collision->nJumperEdge; iEdge++ ) {
                            if( nEntity[ iEdge ] > 0 ) {
                                vec4 Edge = Collision->JumperEdge[ iEdge ];
                                
                                char str[ 16 ] = {};
                                sprintf( str, "nEnt: %u", nEntity[ iEdge ] );
                                DrawString( Pass_Game, AppState->font, str, Edge.P, textAlign_BottomLeft, WORLD_DEBUG_TEXT_SCALE, COLOR_YELLOW );
                            }
                        }
                    }
                }
            }
            
            UpdateAndDrawEditorPanel( Platform, AppState, TempMemory, Mouse, Pass_Game, Pass_UI );
            
            boo32 doOpen   = wasPressed( Keyboard, KeyCode_o, KEYBOARD_FLAGS__CONTROL );
            boo32 doSave   = wasPressed( Keyboard, KeyCode_s, KEYBOARD_FLAGS__CONTROL );
            boo32 doSaveAs = wasPressed( Keyboard, KeyCode_s, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT );
            boo32 doNew    = wasPressed( Keyboard, KeyCode_n, KEYBOARD_FLAGS__CONTROL );
            
            if( SaveOpen->current[ 0 ] != 0 ) {
                char str[ 128 ] = {};
                sprintf( str, "FILE: %s", SaveOpen->current );
                DISPLAY_STRING( str );
            } else {
                DISPLAY_STRING( "NO FILENAME!!", COLOR_RED );
            }
            if( SaveOpen->unsavedChanges ) {
                DISPLAY_STRING( "UNSAVED CHANGES!!", COLOR_RED );
            }
            if( SaveOpen->recentSave ) {
                char str[ 128 ] = {};
                sprintf( str, "File Saved!! %s", SaveOpen->current );
                DISPLAY_STRING( str, COLOR_YELLOW );
                
                SaveOpen->timer += dt;
                if( SaveOpen->timer >= 2.0f ) {
                    SaveOpen->timer      = 0.0f;
                    SaveOpen->recentSave = false;
                }
            }
            if( Editor->Player_nSpawnPos == 0 ) {
                DISPLAY_STRING( "Must set Player Spawn Pos!!", COLOR_YELLOW );
            }
            
            if( canSaveOpen ) {
                INPUT_STRING * i = &SaveOpen->input_string;
                if( doSave ) {
                    if( SaveOpen->current[ 0 ] != 0 ) {
                        saveRoom( Platform, AppState, TempMemory );
                    } else {
                        AppState->Mode = appMode_saveRoom;
                        AppState->Mode_isInitialized = false;
                        SaveOpen->overwrite = false;
                        init( i );
                    }
                } else if( doSaveAs ) {
                    AppState->Mode = appMode_saveRoom;
                    AppState->Mode_isInitialized = false;
                    SaveOpen->overwrite   = false;
                    init( i );
                } else if( doOpen ) {
                    AppState->Mode               = appMode_openRoom;
                    AppState->Mode_isInitialized = false;
                    init( i );
                }
            }
            
            if( doNew ) {
                if( !SaveOpen->unsavedChanges ) {
                    SaveOpen->confirmNew = true;
                }
                
                if( SaveOpen->confirmNew ) {
                    reset( Editor );
                } else if( SaveOpen->unsavedChanges ) {
                    SaveOpen->confirmNew = true;
                    
                }
            }
            if( SaveOpen->confirmNew ) {
                DISPLAY_STRING( "Careful! File contains unsaved changes!!" );
            }
            
            if( wasPressed( Keyboard, KeyCode_l, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                AppState->Mode = appMode_game;
            }
            if( wasPressed( Keyboard, KeyCode_t, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                if( ( SaveOpen->current[ 0 ] != 0 ) && ( Editor->Player_nSpawnPos > 0 ) ) {
                    AppState->testRoom = true;
                    strcpy( AppState->testRoom_name, SaveOpen->current );
                    startGame( Platform, AppState, TempMemory );
                }
            }
            if( wasPressed( Keyboard, KeyCode_m, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                AppState->Mode = appMode_layout;
            }
            
#if 0
            {
                EDITOR__NAV_MESH_STATE * State = &AppState->Editor.NavMesh;
                DISPLAY_VALUE( uint32, State->nEntity );
            }
#endif
        } break;
        
        case appMode_layout: {
            LAYOUT_STATE * layout = &AppState->layout;
            INPUT_STRING * filter = &layout->filter_string;
            CAMERA_STATE * Camera = &layout->Camera;
            FILE_SAVE_OPEN * SaveOpen = &layout->SaveOpen;
            
            if( !layout->isInitialized ) {
                layout->isInitialized = true;
                
                SaveOpen->fileList      = StringBuffer( permMemory, 32, 2048 );
                layout->filter_fileList = StringBuffer( permMemory, LAYOUT__FILE_MAX_COUNT, _KB( 256 ) );
                
                initLayoutPanel( AppState );
                
                init( filter, 32 );
                reset( layout );
                
                layout->doReload = true;
            }
            
            if( layout->doReload ) {
                // TODO: I should probably clear the State if the user saves a new Room in the Editor and then switches to the layout.
                // TODO: Then, I should probably add a warning if the user attempts to switch away from the layout if there are unsaved changes.
                // TODO: Then, I could probably add a check to verify that entries are still pointing to the correct file.
                layout->doReload = false;
                
                clear( filter );
                getFileList( AppState, &layout->filter_fileList, FILETAG__ROOM_STATS );
                setLayoutFilenames( AppState );
                loadLayoutBitmaps  ( Platform, AppState );
                loadLayoutRoomStats( Platform, AppState, TempMemory );
                loadLayoutExits    ( Platform, AppState );
            }
            
            UpdateKeyboardInputForLayoutFilter( filter, Keyboard );
            UpdateAndDrawLayoutPanel( AppState, Mouse, Pass_Game, Pass_UI );
            
            updateCamera( Camera, Keyboard, dt );
            vec2 MouseP = getWorldPos( Camera, AppState->app_dim, Mouse->Position );
            
            if( layout->doMove ) {
                vec2 dP = MouseP - layout->doMove_bPos;
                
                LAYOUT_ROOM * Room = layout->Room + layout->doMove_iRoom;
                Room->pos += dP;
                
                layout->doMove_bPos = MouseP;
                
                if( wasReleased( Mouse, MouseButton_Left ) ) {
                    layout->doMove = false;
                }
            }
            
            // init Exit Bounds for frame
            rect   * link_Bound = _pushArray_clear( TempMemory, rect,   layout->nExit );
            uint32 * link_iRoom = _pushArray_clear( TempMemory, uint32, layout->nExit );
            uint32 * link_iExit = _pushArray_clear( TempMemory, uint32, layout->nExit );
            for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
                LAYOUT_ROOM Room = layout->Room[ iRoom ];
                if( Room.show ) {
                    rect Bound = addRadius( Room.Bound, TILE_WIDTH * 4.0f );
                    vec2 dim   = getDim( Bound );
                    rect R = rectCD( Room.pos, dim );
                    
                    for( uint32 iExit = 0; iExit < Room.nExit; iExit++ ) {
                        rect S = Room.Exit[ iExit ].Bound;
                        vec2 A = ( getBL( S ) - getBL( Bound ) ) / dim;
                        vec2 B = ( getTR( S ) - getBL( Bound ) ) / dim;
                        S = rectMM( A, B );
                        
                        vec2 P = getT( R, getBL( S ) );
                        vec2 Q = getT( R, getTR( S ) );
                        
                        rect T = rectMM( P, Q );
                        
                        uint32 iLink = Room.bExit + iExit;
                        
                        link_Bound[ iLink ] = T;
                        link_iRoom[ iLink ] = iRoom;
                        link_iExit[ iLink ] = iExit;
                    }
                }
            }
            
            int32 iSet_iRoom = -1;
            int32 iSet_iExit = -1;
            rect  iSet_Bound = {};
            
            for( uint32 iLink = 0; iLink < layout->nExit; iLink++ ) {
                rect R = link_Bound[ iLink ];
                if( IsInBound( MouseP, R ) ) {
                    iSet_Bound = R;
                    iSet_iRoom = link_iRoom[ iLink ];
                    iSet_iExit = link_iExit[ iLink ];
                    
                    if( ( !layout->doSet ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
                        layout->doSet = true;
                        layout->doSet_iRoom = iSet_iRoom;
                        layout->doSet_iExit = iSet_iExit;
                        layout->doSet_Bound = R;
                    }
                }
            }
            
            if( layout->doSet ) {
                if( wasPressed( Mouse, MouseButton_Left ) ) {
                    boo32 isValid_entry = ( iSet_iRoom > -1 ) && ( iSet_iRoom != ( int32 )layout->doSet_iRoom );
                    boo32 isValid_Exit  = ( iSet_iExit  > -1 );
                    
                    if( ( isValid_entry ) && ( isValid_Exit ) ) {
                        layout->doSet = false;
                        
                        LAYOUT_ROOM * RoomA = layout->Room + layout->doSet_iRoom;
                        INT32_PAIR  * LinkA = RoomA->Link  + layout->doSet_iExit;
                        
                        LAYOUT_ROOM * RoomB = layout->Room + iSet_iRoom;
                        INT32_PAIR  * LinkB = RoomB->Link  + iSet_iExit;
                        
                        if( ( LinkA->m > -1 ) && ( LinkA->n > -1 ) ) {
                            LAYOUT_ROOM * RoomC = layout->Room + LinkA->m;
                            RoomC->Link          [ LinkA->n ] = Int32Pair( -1, -1 );
                            RoomC->Link_wasBroken[ LinkA->n ] = true;
                        }
                        if( ( LinkB->m > -1 ) && ( LinkB->n > -1 ) ) {
                            LAYOUT_ROOM * RoomC = layout->Room + LinkB->m;
                            RoomC->Link          [ LinkB->n ] = Int32Pair( -1, -1 );
                            RoomC->Link_wasBroken[ LinkB->n ] = true;
                        }
                        
                        *LinkA = Int32Pair( iSet_iRoom,          iSet_iExit          );
                        *LinkB = Int32Pair( layout->doSet_iRoom, layout->doSet_iExit );
                        
                        RoomA->Link_wasBroken[ layout->doSet_iExit ] = false;
                        RoomB->Link_wasBroken[ iSet_iExit          ] = false;
                    }
                }
                if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                    layout->doSet = false;
                }
            }
            
            boo32 canDoHover = ( !layout->doSet ) && ( iSet_iRoom == -1 );
            
            int32 iHover = -1;
            if( canDoHover ) {
                for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
                    LAYOUT_ROOM Room = layout->Room[ iRoom ];
                    if( Room.show ) {
                        rect R = rectCD( Room.pos, getDim( Room.Bound ) );
                        if( IsInBound( MouseP, R ) ) {
                            iHover = iRoom;
                        }
                    }
                }
            }
            
            if( iHover > -1 ) {
                if( wasPressed( Mouse, MouseButton_Left ) ) {
                    layout->doMove       = true;
                    layout->doMove_iRoom = iHover;
                    layout->doMove_bPos  = MouseP;
                }
                if( wasPressed( Keyboard, KeyCode_delete ) ) {
                    LAYOUT_ROOM * Room = layout->Room + iHover;
                    
                    Room->show = false;
                    Room->pos  = {};
                    
                    for( uint32 iExit = 0; iExit < Room->nExit; iExit++ ) {
                        INT32_PAIR * link = Room->Link + iExit;
                        
                        if( ( link->m > -1 ) && ( link->n > -1 ) ) {
                            LAYOUT_ROOM * RoomC = layout->Room + link->m;
                            RoomC->Link          [ link->n ] = Int32Pair( -1, -1 );
                            RoomC->Link_wasBroken[ link->n ] = true;
                        }
                        
                        *link = Int32Pair( -1, -1 );
                    }
                }
                if( wasPressed( Mouse, MouseButton_Right ) ) {
                    layout->iRoomToStartGame = iHover;
                }
            }
            
            { // Draw
                for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
                    LAYOUT_ROOM Room = layout->Room[ iRoom ];
                    if( Room.show ) {
                        rect R = rectCD( Room.pos, getDim( Room.Bound ) );
                        R = addRadius( R, TILE_WIDTH * 4.0f );
                        
                        TEXTURE_ID TextureID = ( TEXTURE_ID )( TextureID_Room_bitmap + iRoom );
                        DrawRect( Pass_Game, TextureID, R, COLOR_WHITE );
                        
                        vec4 Color = Vec4( 0.1f, 0.1f, 0.0f, 1.0f );
                        if( iRoom == layout->iRoomToStartGame ) {
                            Color = COLOR_GREEN;
                        } else if( ( int32 )iRoom == iHover ) {
                            Color = COLOR_YELLOW;
                        }
                        
                        DrawRectOutline( Pass_Game, R, Color );
                        
                        vec2 P = getTL( R );
                        DrawString( Pass_Game, AppState->font, Room.name, P, textAlign_TopLeft, WORLD_DEBUG_TEXT_SCALE * Camera->scale_ratio, COLOR_WHITE );
                    }
                }
                
                for( uint32 iLink = 0; iLink < layout->nExit; iLink++ ) {
                    rect R = link_Bound[ iLink ];
                    
                    vec4 Color = Vec4( 0.0f, 0.15f, 0.0f, 1.0f );
                    
                    int32 iRoom = link_iRoom[ iLink ];
                    int32 iExit = link_iExit[ iLink ];
                    
                    if( ( iSet_iRoom == iRoom ) && ( iSet_iExit == iExit ) ) {
                        Color = Vec4( 0.0f, 0.35f, 0.0f, 1.0f );
                    } else if( LinkWasBroken( layout, iRoom, iExit ) ) {
                        Color = COLOR_RED;
                    }
                    
                    DrawRectOutline( Pass_Game, R, Color );
                }
                
                if( layout->doSet ) {
                    DrawRect( Pass_Game, layout->doSet_Bound, COLOR_YELLOW );
                    
                    vec2 P = MouseP;
                    vec2 Q = getCenter( layout->doSet_Bound );
                    DrawLine( Pass_Game, P, Q, COLOR_YELLOW );
                }
                if( ( iSet_iRoom > -1 ) && ( iSet_iExit > -1 ) ) {
                    DrawRectOutline( Pass_Game, iSet_Bound, COLOR_YELLOW );
                }
                
                flo32 margin = 0.1f;
                
                for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
                    LAYOUT_ROOM RoomA = layout->Room[ iRoom ];
                    if( RoomA.show ) {
                        for( uint32 iExit = 0; iExit < RoomA.nExit; iExit++ ) {
                            INT32_PAIR LinkA = RoomA.Link[ iExit ];
                            if( ( LinkA.m > -1 ) && ( LinkA.n > -1 ) ) {
                                LAYOUT_ROOM RoomB = layout->Room[ LinkA.m ];
                                
                                rect BoundA = link_Bound[ RoomA.bExit + iExit   ];
                                rect BoundB = link_Bound[ RoomB.bExit + LinkA.n ];
                                
                                vec2 P = getCenter( BoundA );
                                vec2 Q = getCenter( BoundB );
                                
                                vec2 N = GetNormal( GetPerp( Q - P ) ) * margin;
                                
                                P += N;
                                Q += N;
                                
                                N *= 2.0f;
                                
                                DrawLine( Pass_Game, P, Q,     COLOR_MAGENTA );
                                DrawLine( Pass_Game, P, P + N, COLOR_MAGENTA );
                            }
                        }
                    }
                }
            }
            
            _popArray( TempMemory, uint32, layout->nExit );
            _popArray( TempMemory, uint32, layout->nExit );
            _popArray( TempMemory, rect,   layout->nExit );
            
            boo32 canSaveOpen = ( !layout->doMove ) && ( !layout->doSet );
            boo32 doOpen   = wasPressed( Keyboard, KeyCode_o, KEYBOARD_FLAGS__CONTROL );
            boo32 doSave   = wasPressed( Keyboard, KeyCode_s, KEYBOARD_FLAGS__CONTROL );
            boo32 doSaveAs = wasPressed( Keyboard, KeyCode_s, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT );
            boo32 doNew    = wasPressed( Keyboard, KeyCode_n, KEYBOARD_FLAGS__CONTROL );
            
            if( SaveOpen->current[ 0 ] != 0 ) {
                char str[ 128 ] = {};
                sprintf( str, "FILE: %s", SaveOpen->current );
                DISPLAY_STRING( str );
            } else {
                DISPLAY_STRING( "NO FILENAME!!", COLOR_RED );
            }
            if( SaveOpen->unsavedChanges ) {
                DISPLAY_STRING( "UNSAVED CHANGES!!", COLOR_RED );
            }
            if( SaveOpen->recentSave ) {
                char str[ 128 ] = {};
                sprintf( str, "File Saved!! %s", SaveOpen->current );
                DISPLAY_STRING( str, COLOR_YELLOW );
                
                SaveOpen->timer += dt;
                if( SaveOpen->timer >= 2.0f ) {
                    SaveOpen->timer      = 0.0f;
                    SaveOpen->recentSave = false;
                }
            }
            
            if( canSaveOpen ) {
                INPUT_STRING * i = &SaveOpen->input_string;
                if( doSave ) {
                    if( SaveOpen->current[ 0 ] != 0 ) {
                        saveLayout( Platform, AppState, TempMemory );
                    } else {
                        AppState->Mode = appMode_saveLayout;
                        AppState->Mode_isInitialized = false;
                        SaveOpen->overwrite   = false;
                        init( i );
                    }
                } else if( doSaveAs ) {
                    AppState->Mode = appMode_saveLayout;
                    AppState->Mode_isInitialized = false;
                    SaveOpen->overwrite   = false;
                    init( i );
                } else if( doOpen ) {
                    AppState->Mode               = appMode_openLayout;
                    AppState->Mode_isInitialized = false;
                    init( i );
                }
            }
            
            if( doNew ) {
                if( !SaveOpen->unsavedChanges ) {
                    SaveOpen->confirmNew = true;
                }
                
                if( SaveOpen->confirmNew ) {
                    reset( layout );
                } else if( SaveOpen->unsavedChanges ) {
                    SaveOpen->confirmNew = true;
                    
                }
            }
            if( SaveOpen->confirmNew ) {
                DISPLAY_STRING( "Careful! File contains unsaved changes!!" );
            }
            
            if( wasPressed( Keyboard, KeyCode_m, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                AppState->Mode = appMode_Editor;
            }
        } break;
        
        case appMode_saveRoom: {
            EDITOR_STATE * Editor = &AppState->Editor;
            FILE_SAVE_OPEN * SaveOpen = &Editor->SaveOpen;
            
            if( !AppState->Mode_isInitialized ) {
                AppState->Mode_isInitialized = true;
                getFileList( AppState, &SaveOpen->fileList, "dat" );
            }
            
            INPUT_STRING * i = &SaveOpen->input_string;
            UpdateKeyboardInputForFileSaveOpen( i, Keyboard, SaveOpen );
            
            if( ( wasPressed( Keyboard, KeyCode_Enter ) ) && ( i->nChar > 0 ) ) {
                if( SaveOpen->overwrite ) {
                    strcpy( SaveOpen->current, i->string );
                    saveRoom( Platform, AppState, TempMemory );
                } else {
                    boo32 fileExists = Platform->doesFileExist( ROOM_SAVE_DIRECTORY, i->string, "dat" );
                    if( fileExists ) {
                        SaveOpen->overwrite = true;
                    } else {
                        strcpy( SaveOpen->current, i->string );
                        saveRoom( Platform, AppState, TempMemory );
                    }
                }
            }
            
            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                AppState->Mode = appMode_Editor;
            }
            
            DrawFileListForSaveOpen( Pass_UI, AppState, SaveOpen );
        } break;
        
        case appMode_openRoom: {
            EDITOR_STATE * Editor = &AppState->Editor;
            FILE_SAVE_OPEN * SaveOpen = &Editor->SaveOpen;
            
            if( !AppState->Mode_isInitialized ) {
                AppState->Mode_isInitialized = true;
                getFileList( AppState, &SaveOpen->fileList, "dat" );
            }
            
            INPUT_STRING * i = &SaveOpen->input_string;
            UpdateKeyboardInputForFileSaveOpen( i, Keyboard, SaveOpen );
            
            if( ( wasPressed( Keyboard, KeyCode_Enter ) ) && ( i->nChar > 0 ) ) {
                boo32 fileExists = Platform->doesFileExist( ROOM_SAVE_DIRECTORY, i->string, "dat" );
                if( fileExists ) {
                    AppState->Mode = appMode_Editor;
                    EDITOR_loadRoom( Platform, AppState, TempMemory, ROOM_SAVE_DIRECTORY, i->string );
                }
            }
            
            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                AppState->Mode = appMode_Editor;
            }
            
            DrawFileListForSaveOpen( Pass_UI, AppState, SaveOpen );
        } break;
        
        case appMode_saveLayout: {
            LAYOUT_STATE * layout = &AppState->layout;
            FILE_SAVE_OPEN * SaveOpen = &layout->SaveOpen;
            char * filetag = FILETAG__LAYOUT_EDIT;
            
            if( !AppState->Mode_isInitialized ) {
                AppState->Mode_isInitialized = true;
                getFileList( AppState, &SaveOpen->fileList, filetag );
            }
            
            INPUT_STRING * i = &SaveOpen->input_string;
            UpdateKeyboardInputForFileSaveOpen( i, Keyboard, SaveOpen );
            
            if( ( wasPressed( Keyboard, KeyCode_Enter ) ) && ( i->nChar > 0 ) ) {
                if( SaveOpen->overwrite ) {
                    strcpy( SaveOpen->current, i->string );
                    saveLayout( Platform, AppState, TempMemory );
                } else {
                    boo32 fileExists = Platform->doesFileExist( ROOM_SAVE_DIRECTORY, i->string, filetag );
                    if( fileExists ) {
                        SaveOpen->overwrite = true;
                    } else {
                        strcpy( SaveOpen->current, i->string );
                        saveLayout( Platform, AppState, TempMemory );
                    }
                }
            }
            
            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                AppState->Mode = appMode_layout;
            }
            
            DrawFileListForSaveOpen( Pass_UI, AppState, SaveOpen );
        } break;
        
        case appMode_openLayout: {
            LAYOUT_STATE * layout = &AppState->layout;
            FILE_SAVE_OPEN * SaveOpen = &layout->SaveOpen;
            char * filetag = FILETAG__LAYOUT_EDIT;
            
            if( !AppState->Mode_isInitialized ) {
                AppState->Mode_isInitialized = true;
                getFileList( AppState, &SaveOpen->fileList, filetag );
            }
            
            INPUT_STRING * i = &SaveOpen->input_string;
            UpdateKeyboardInputForFileSaveOpen( i, Keyboard, SaveOpen );
            
            if( ( wasPressed( Keyboard, KeyCode_Enter ) ) && ( i->nChar > 0 ) ) {
                boo32 fileExists = Platform->doesFileExist( ROOM_SAVE_DIRECTORY, i->string, filetag );
                if( fileExists ) {
                    AppState->Mode = appMode_layout;
                    LoadLayoutForEdit( Platform, AppState, TempMemory, i->string );
                }
            }
            
            if( wasPressed( Keyboard, KeyCode_Escape ) ) {
                AppState->Mode = appMode_layout;
            }
            
            DrawFileListForSaveOpen( Pass_UI, AppState, SaveOpen );
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    DISPLAY_VALUE( uint64, TempMemory->used );
    Assert( TempMemory->used < TempMemory->size );
    
    WORLD_STATE * World = &AppState->World;
    DISPLAY_VALUE( uint32, World->nJumper );
    
    ROOM * Room = World->Room + AppState->iRoom;
    DISPLAY_VALUE( UINT32_PAIR, Room->Jumper );
    
    EVENT_STATE * Event = &AppState->Event;
    DISPLAY_VALUE( uint32, Event->nEvent );
}