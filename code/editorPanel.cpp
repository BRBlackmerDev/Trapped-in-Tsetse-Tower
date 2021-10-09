
internal void
initEditorPanel( APP_STATE * AppState ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * panel  = &Editor->panel;
    
    { // set Bound
        rect R = AppState->App_Bound;
        R.Left = R.Right - 320.0f;
        
        panel->Bound = R;
    }
}

internal void
EDITOR_addToggleButton( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, char * label, boo32 * value ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * panel  = &Editor->panel;
    
    vec2 button_dim = Vec2( 44.0f, 28.0f );
    rect Bound = getBound( panel, button_dim );
    
    if( ( wasPressed( Mouse, MouseButton_Left ) ) && ( IsInBound( Mouse->Position, Bound ) ) ) {
        *value = !( *value );
    }
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( *value ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    }
    DrawRect( Pass_UI, Bound, Color );
    DrawRectOutline( Pass_UI, Bound, COLOR_BLACK );
    DrawString( Pass_UI, AppState->font, label, getCenter( Bound ), textAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
}

internal boo32
EDITOR_addPressButton( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, char * label, vec2 dim ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * panel  = &Editor->panel;
    
    rect  Bound   = getBound( panel, dim );
    boo32 isHover = IsInBound( Mouse->Position, Bound );
    
    boo32 result = false;
    if( ( wasPressed( Mouse, MouseButton_Left ) ) && ( isHover ) ) {
        result = true;
    }
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( result ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    } else if( isHover ) {
        Color = COLOR_GRAY( 0.5f );
    }
    
    DrawRect( Pass_UI, Bound, Color );
    DrawRectOutline( Pass_UI, Bound, COLOR_BLACK );
    DrawString( Pass_UI, AppState->font, label, getCenter( Bound ), textAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    return result;
}

internal boo32
EDITOR_addEnumButton( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, char ** label, uint32 nLabel, uint32 * value ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * panel  = &Editor->panel;
    
    boo32 result = false;
    
    vec2 button_dim = Vec2( 44.0f, 28.0f );
    rect Bound = getBound( panel, button_dim );
    
    boo32 isHover = IsInBound( Mouse->Position, Bound );
    uint32 m = *value;
    
    if( ( Mouse->wheelClicks != 0 ) && ( isHover ) ) {
        uint32 n = abs( Mouse->wheelClicks );
        if( ( n % nLabel ) != 0 ) {
            m = ( m + ( nLabel * 4 ) + Mouse->wheelClicks ) % nLabel;
            *value = m;
            
            result = true;
        }
    }
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( result ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    } else if( isHover ) {
        Color = COLOR_GRAY( 0.5f );
    }
    
    DrawRect( Pass_UI, Bound, Color );
    DrawRectOutline( Pass_UI, Bound, COLOR_BLACK );
    DrawString( Pass_UI, AppState->font, label[ m ], getCenter( Bound ), textAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    return result;
}

internal void
EDITOR_addModeButton( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, char * label, EDITOR_MODE Mode ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * panel  = &Editor->panel;
    
    vec2 button_dim = Vec2( 44.0f, 28.0f );
    rect Bound = getBound( panel, button_dim );
    
    if( ( wasPressed( Mouse, MouseButton_Left ) ) && ( IsInBound( Mouse->Position, Bound ) ) ) {
        Editor->Mode = Mode;
    }
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( Editor->Mode == Mode ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    }
    DrawRect( Pass_UI, Bound, Color );
    DrawRectOutline( Pass_UI, Bound, COLOR_BLACK );
    DrawString( Pass_UI, AppState->font, label, getCenter( Bound ), textAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
}

internal void
EDITOR_addU32Counter( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass, char * label, uint32 * value ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * panel  = &Editor->panel;
    
    vec2 counter_dim = Vec2( 72.0f, 28.0f );
    rect Bound = getBound( panel, counter_dim );
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( IsInBound( Mouse->Position, Bound ) ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
        if( Mouse->wheelClicks != 0 ) {
            if( Mouse->wheelClicks < 0 ) {
                uint32 sub = minValue( *value, ( uint32 )abs( Mouse->wheelClicks ) );
                *value -= sub;
            } else {
                *value += Mouse->wheelClicks;
            }
        }
    }
    
    DrawRect( Pass, Bound, Color );
    DrawRectOutline( Pass, Bound, COLOR_BLACK );
    
    char str[ 32 ] = {};
    sprintf( str, "%s: %u", label, *value );
    DrawString( Pass, AppState->font, str, getCenter( Bound ), textAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
}

internal void
EDITOR_addEvent( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, EDITOR__EVENT * Event, uint32 iEvent ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * panel  = &Editor->panel;
    
    flo32 name_Height = 18.0f;
    vec2 inner_margin = Vec2( 4.0f, 4.0f );
    vec2 data_dim = Vec2( getWidth( panel->inner ) - ( inner_margin.x * 2.0f ), 18.0f );
    
    flo32 Height = ( flo32 )Event->nData * ( data_dim.y + inner_margin.y ) + name_Height + inner_margin.y * 2.0f;
    
    vec2 bPos = panel->atPos;
    vec2 bDim = Vec2( getWidth( panel->inner ), Height );
    rect B    = rectTLD( bPos, bDim );
    vec4 bColor   = COLOR_GRAY( 0.9f );
    vec4 bOutline = COLOR_BLACK;
    if( ( Editor->Mode == EditorMode_Event ) && ( Editor->Event_iEvent == iEvent ) ) {
        bOutline = COLOR_RED;
    }
    if( Event->IsActive ) {
        bColor = Vec4( 0.0f, 0.4f, 0.0f, 1.0f );
    }
    DrawRect       ( Pass_UI, B, bColor );
    DrawRectOutline( Pass_UI, B, bOutline );
    
    vec2 atPos = bPos;
    
    DrawString( Pass_UI, AppState->font, Event->name, bPos + Vec2( inner_margin.x * 2.0f, -inner_margin.y ), textAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    if( Event->nData == 0 ) {
        DrawString( Pass_UI, AppState->font, "*NO DATA*", getTR( B ) + Vec2( -inner_margin.x, -inner_margin.y ), textAlign_TopRight, Vec2( 1.0f, 1.0f ), COLOR_GRAY( 0.6f ) );
    }
    
    atPos.y -= ( name_Height + ( inner_margin.y * 2.0f ) );
    atPos.x += inner_margin.x;
    
    panel->atPos.y -= ( Height + inner_margin.y );
    
    DATA_TYPE_LABELS;
    
    for( uint32 iData = 0; iData < Event->nData; iData++ ) {
        rect R = rectTLD( atPos, data_dim );
        
        boo32 doHover = IsInBound( Mouse->Position, R );
        if( ( doHover ) && ( wasPressed( Mouse, MouseButton_Left ) ) ) {
            Editor->Mode = EditorMode_Event;
            
            Editor->Event_Type = Event->data_Type[ iData ];
            Editor->Event_data = Event->data_ptr [ iData ];
            Editor->Event_iEvent = iEvent;
            Editor->Event_iData  = iData;
        }
        
        vec4 rColor = COLOR_GRAY( 0.7f );
        if( IsInBound( Mouse->Position, R ) ) {
            rColor = COLOR_GRAY( 0.5f );
        }
        if( ( Editor->Mode == EditorMode_Event ) && ( Editor->Event_iEvent == iEvent ) && ( Editor->Event_iData == iData ) ) {
            rColor = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
        }
        DrawRect       ( Pass_UI, R, rColor   );
        DrawRectOutline( Pass_UI, R, COLOR_BLACK );
        
        char str[ 64 ] = {};
        sprintf( str, "%s : %s", dataTypeLabel[ Event->data_Type[ iData ] ], Event->data_name[ iData ] );
        DrawString( Pass_UI, AppState->font, str, atPos + Vec2( inner_margin.x, -1.0f ), textAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
        
        atPos.y -= ( data_dim.y + inner_margin.y );
        
        switch( Event->data_Type[ iData ] ) {
            case dataType_point: {
                vec2 P = ( ( vec2 * )Event->data_ptr[ iData ] )[ 0 ];
                DrawPoint( Pass_Game, P, TILE_DIM * 0.5f, COLOR_YELLOW );
                
                if( doHover ) {
                    DISPLAY_VALUE( vec2, P );
                }
            } break;
            
            case dataType_rect: {
                rect S = ( ( rect * )Event->data_ptr[ iData ] )[ 0 ];
                DrawRectOutline( Pass_Game, S, COLOR_YELLOW );
                
                if( doHover ) {
                    DISPLAY_VALUE( rect, S );
                }
            } break;
        }
    }
}

internal void
DrawEditorGrid( RENDER_PASS * Pass, APP_STATE * AppState ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    CAMERA_STATE * Camera = &Editor->Camera;
    
    vec2 P = Camera->pos - Camera->dim * 0.5f;
    UINT32_PAIR cell = getCell( P );
    
    cell.x -= ( cell.x % 4 );
    cell.y -= ( cell.y % 4 );
    
    uint32 scale = 4;
    uint32 nX = ( ( uint32 )( Camera->dim.x / TILE_WIDTH  ) / 4 ) + 1;
    uint32 nY = ( ( uint32 )( Camera->dim.y / TILE_HEIGHT ) / 4 ) + 1;
    vec2 offset = Vec2( 4.0f, 4.0f ) * TILE_DIM;
    
    vec2 Q = getWorldPos( cell.x, cell.y );
    
    vec2 atPos = Q;
    for( uint32 iX = 0; iX < nX; iX++ ) {
        vec2 A = atPos;
        vec2 B = atPos + Vec2( 0.0f, ( flo32 )nY * offset.y );
        
        DrawLine( Pass, A, B, Vec4( 0.1f, 0.1f, 0.0f, 1.0f ) );
        
        atPos.x += offset.x;
    }
    
    atPos = Q;
    for( uint32 iX = 0; iX < nX; iX++ ) {
        vec2 A = atPos;
        vec2 B = atPos + Vec2( ( flo32 )nX * offset.x, 0.0f );
        
        DrawLine( Pass, A, B, Vec4( 0.1f, 0.1f, 0.0f, 1.0f ) );
        
        atPos.y += offset.y;
    }
}

internal void
GenEventsFromCpp( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    char * SaveDir = ROOM_SAVE_DIRECTORY;
    char * filetag = FILETAG__EVENTS_EDIT;
    uint32 version = VERSION__EVENTS_EDIT;
    
    EDITOR_STATE * Editor   = &AppState->Editor;
    FILE_SAVE_OPEN SaveOpen = Editor->SaveOpen;
    
    char * filename = SaveOpen.current;
    
    FILE_DATA file_cpp = Platform->readFile( TempMemory, CODE_DIRECTORY, filename, "cpp" );
    if( file_cpp.contents ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        uint32   nEvent  = 0;
        uint32 * nEvent0 = _pushType( output, uint32 );
        
        FILE_PARSER  _parser = FileParser( file_cpp.contents, file_cpp.size );
        FILE_PARSER * parser = &_parser;
        
        while( hasTextRemaining( parser ) ) {
            STRING token = parseToken( parser );
            
            if( matchString( token, "internal" ) ) {
                parseToken( parser ); // void
                
                uint32 Nest = 0;
                STRING tokenA = {};
                STRING tokenB = {};
                STRING tokenC = {};
                STRING tokenD = {};
                
                STRING func_name = parseToken( parser );
                Assert( func_name.string[ func_name.nChar - 1 ] == '(' );
                func_name.nChar--;
                
                nEvent++;
                writeString( output, func_name.string, func_name.nChar );
                
                boo32 * IsActive = _pushType( output, boo32 );
                
                uint32   nData  = 0;
                uint32 * nData0 = _pushType( output, uint32 );
                
                int32 iEvent0 = -1;
                for( uint32 iEvent = 0; iEvent < Editor->nEvent; iEvent++ ) {
                    EDITOR__EVENT e = Editor->Event[ iEvent ];
                    if( matchString( func_name, e.name ) ) {
                        iEvent0 = iEvent;
                    }
                }
                
                DATA_TYPE_LABELS;
                DATA_TYPE_TOKENS;
                DATA_TYPE_SIZES;
                
                boo32 doParse = true;
                while( doParse ) {
                    tokenA = parseToken( parser );
                    
                    if( matchString( tokenA, "_read(" ) ) {
                        int32 i = -1;
                        for( uint32 iToken = 0; iToken < dataType_count; iToken++ ) {
                            if( matchSegment( tokenD.string, dataTypeToken[ iToken ], tokenD.nChar ) ) {
                                i = iToken;
                            }
                        }
                        
                        writeSegment( output, dataTypeLabel[ i ] );
                        writeString ( output, tokenC.string, tokenC.nChar );
                        uint8 * data = ( uint8 * )_pushSize( output, dataTypeSize[ i ] );
                        
                        int32 iData0 = -1;
                        if( iEvent0 > -1 ) {
                            EDITOR__EVENT e = Editor->Event[ iEvent0 ];
                            
                            for( uint32 iData = 0; iData < e.nData; iData++ ) {
                                if( matchString( tokenC, e.data_name[ iData ] ) ) {
                                    iData0 = iData;
                                }
                            }
                            
                            if( iData0 > -1 ) {
                                memcpy( data, e.data_ptr[ iData0 ], dataTypeSize[ i ] );
                            }
                        }
                        
                        nData++;
                    } else if( matchString( tokenA, "//ACTIVE//" ) ) {
                        *IsActive = true;
                    } else if( matchString( tokenA, "{" ) ) {
                        Nest++;
                    } else if( matchString( tokenA, "}" ) ) {
                        Assert( Nest > 0 );
                        
                        Nest--;
                        if( Nest == 0 ) {
                            doParse = false;
                        }
                    }
                    
                    tokenD = tokenC;
                    tokenC = tokenB;
                    tokenB = tokenA;
                }
                
                *nData0 = nData;
            }
        }
        
        *nEvent0 = nEvent;
        
        outputFile( Platform, output, SaveDir, filename, filetag ); 
        _popSize( TempMemory, output->size );
        
        _popSize( TempMemory, file_cpp.size );
    }
    
}

internal void
UpdateAndDrawEditorPanel( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * panel  = &Editor->panel;
    
    vec2  button_dim = Vec2( 40.0f, 16.0f );
    flo32 labelY     = 14.0f;
    flo32 labelPreY  = 8.0f;
    
    panel->inner = addRadius( panel->Bound, -4.0f );
    panel->atPos = getTL( panel->inner );
    panel->advanceY = 0.0f;
    
    DrawRect( Pass_UI, panel->Bound, COLOR_WHITE );
    
    EDITOR_addToggleButton( AppState, Mouse, Pass_Game, Pass_UI, "TOP", &Editor->stat.isTopDown );
    EDITOR_addToggleButton( AppState, Mouse, Pass_Game, Pass_UI, "GRID", &Editor->DrawGrid );
    EDITOR_addModeButton  ( AppState, Mouse, Pass_Game, Pass_UI, "INTF", EditorMode_interior );
    boo32 doIntFill = EDITOR_addPressButton( AppState, Mouse, Pass_Game, Pass_UI, "FILL", button_dim );
    if( doIntFill ) {
        fillInterior( AppState, TempMemory );
    }
    
    wrapPanel( panel );
    EDITOR_addModeButton( AppState, Mouse, Pass_Game, Pass_UI, "TERR", EditorMode_terrain );
    EDITOR_addModeButton( AppState, Mouse, Pass_Game, Pass_UI, "PLYR", EditorMode_PlayerSpawn );
    EDITOR_addModeButton( AppState, Mouse, Pass_Game, Pass_UI, "EXIT", EditorMode_Exit );
    EDITOR_addModeButton( AppState, Mouse, Pass_Game, Pass_UI, "NEST", EditorMode_Nest );
    EDITOR_addModeButton( AppState, Mouse, Pass_Game, Pass_UI, "SECU", EditorMode_SecureDoor );
    EDITOR_addModeButton( AppState, Mouse, Pass_Game, Pass_UI, "MECH", EditorMode_MechDoor   );
    EDITOR_addModeButton( AppState, Mouse, Pass_Game, Pass_UI, "POWR", EditorMode_powerSwitch );
    EDITOR_addModeButton( AppState, Mouse, Pass_Game, Pass_UI, "CHCK", EditorMode_Checkpoint );
    EDITOR_addModeButton( AppState, Mouse, Pass_Game, Pass_UI, "NAVM", EditorMode_NavMesh );
    
    DRAW_STATE  _Draw = {};
    DRAW_STATE * Draw = &_Draw;
    Draw->app_dim     = AppState->app_dim;
    Draw->app_halfDim = AppState->app_halfDim;
    Draw->App_Bound   = AppState->App_Bound;
    Draw->font        = AppState->font;
    Draw->Collision   = &AppState->Collision;
    
    boo32 * IsActive = AppState->Debug_IsActive;
    { // Draw debug Collision
        wrapPanel( panel, labelPreY );
        addLabel( panel, Pass_UI, Draw->font, "COLLISION", labelY );
        
        wrapPanel( panel );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "COLL", DrawCollisionEdges, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "JUMP", DrawJumpBound,      IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "WLJP", DrawWallSlideBound, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "LDGE", DrawLedgeGrabBound, IsActive++ );
        
        wrapPanel( panel, labelPreY );
        addLabel( panel, Pass_UI, Draw->font, "JUMPER", labelY );
        wrapPanel( panel );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "BOND", DrawJumperBound,    IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "EDGE", DrawJumperEdge,     IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "PATH", DrawJumperPathLinks,      IsActive++ );
        
        //wrapPanel( panel, labelPreY );
        //addLabel( panel, Pass_UI, Draw->font, "NAV MESH", labelY );
        //EDITOR__NAV_MESH_STATE * NavMesh = &Editor->NavMesh;
        //if( NavMesh->nEntity > 0 ) {
        //boo32 isPressed = EDITOR_addPressButton( AppState, Mouse, Pass_Game, Pass_UI, "GEN", button_dim );
        //if( isPressed ) {
        //GenNavMeshLinks( NavMesh );
        //}
        //}
        //wrapPanel( panel );
        //addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "INTV", DrawInteriorVertex,    IsActive++ );
        //addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "CIRC", DrawCircumcircle,      IsActive++ );
        //addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "NAVM", DrawNavMesh,           IsActive++ );
        
        
        
        Assert( ( IsActive - AppState->Debug_IsActive ) <= DEBUG_MAX_COUNT );
    }
    
    ENTITY_TYPE_TAGS;
    
    wrapPanel( panel, labelPreY );
    addLabel( panel, Pass_UI, Draw->font, "ENTITY COUNT:", labelY );
    EDITOR_addToggleButton( AppState, Mouse, Pass_Game, Pass_UI, "nEnt", &Editor->DrawEntityCountOnEdge );
    wrapPanel( panel );
    
    for( uint32 iType = 0; iType < EntityType_count; iType++ ) {
        EDITOR_addU32Counter( AppState, Mouse, Pass_UI, EntityTypeTag[ iType ], Editor->nEntity + iType );
    }
    
    wrapPanel( panel, labelPreY );
    addLabel( panel, Pass_UI, Draw->font, "EVENTS:", labelY );
    
    FILE_SAVE_OPEN SaveOpen = Editor->SaveOpen;
    if( SaveOpen.current[ 0 ] != 0 ) {
        boo32 isPressed = EDITOR_addPressButton( AppState, Mouse, Pass_Game, Pass_UI, "GEN", button_dim );
        if( isPressed ) {
            GenEventsFromCpp( Platform, AppState, TempMemory );
            
            reset( &Editor->Event_names );
            memset( &Editor->reset_Event_start, 0, &Editor->reset_Event_end - &Editor->reset_Event_start );
            
            EDITOR_loadEvents( Platform, AppState, TempMemory, ROOM_SAVE_DIRECTORY, SaveOpen.current, FILETAG__EVENTS_EDIT );
        }
    }
    
    wrapPanel( panel );
    for( uint32 iEvent = 0; iEvent < Editor->nEvent; iEvent++ ) {
        EDITOR__EVENT * Event = Editor->Event + iEvent;
        EDITOR_addEvent( AppState, Mouse, Pass_Game, Pass_UI, Event, iEvent );
    }
}