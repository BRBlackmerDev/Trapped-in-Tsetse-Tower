
//----------
// CELL TOOLS
//----------

internal UINT32_PAIR
getCell( CAMERA_STATE * Camera, vec2 app_dim, vec2 pos ) {
    vec2 t = clamp01( pos / app_dim ) - Vec2( 0.5f, 0.5f );
    vec2 P = Camera->pos + Camera->dim * t;
    
    int32 xCell = ( int32 )( P.x / TILE_WIDTH  );
    int32 yCell = ( int32 )( P.y / TILE_HEIGHT );
    xCell = clamp( xCell, 0, EDITOR__GRID_MAX_WIDTH  - 1 );
    yCell = clamp( yCell, 0, EDITOR__GRID_MAX_HEIGHT - 1 );
    
    UINT32_PAIR result = UInt32Pair( ( uint32 )xCell, ( uint32 )yCell );
    return result;
}

internal UINT32_PAIR
getCell( vec2 pos ) {
    int32 xCell = ( int32 )( pos.x / TILE_WIDTH  );
    int32 yCell = ( int32 )( pos.y / TILE_HEIGHT );
    xCell = clamp( xCell, 0, EDITOR__GRID_MAX_WIDTH  - 1 );
    yCell = clamp( yCell, 0, EDITOR__GRID_MAX_HEIGHT - 1 );
    
    UINT32_PAIR result = UInt32Pair( ( uint32 )xCell, ( uint32 )yCell );
    return result;
}

internal UINT32_PAIR
getCell( vec2 pos, uint32 xMin, uint32 yMin ) {
    int32 xCell = ( int32 )( pos.x / TILE_WIDTH  );
    int32 yCell = ( int32 )( pos.y / TILE_HEIGHT );
    xCell = clamp( xCell, 0, EDITOR__GRID_MAX_WIDTH  - 1 );
    yCell = clamp( yCell, 0, EDITOR__GRID_MAX_HEIGHT - 1 );
    
    UINT32_PAIR result = UInt32Pair( ( uint32 )xCell - xMin, ( uint32 )yCell - yMin );
    return result;
}

internal UINT32_PAIR
getCell( vec2 pos, UINT32_PAIR min ) {
    UINT32_PAIR result = getCell( pos, min.x, min.y );
    return result;
}

internal rect
getBound( uint32 xCell, uint32 yCell ) {
    vec2 P = Vec2( ( flo32 )xCell, ( flo32 )yCell ) * TILE_DIM;
    rect result = rectBLD( P, TILE_DIM );
    return result;
}

internal rect
getBound( UINT32_PAIR cell ) {
    rect result = getBound( cell.x, cell.y );
    return result;
}

internal rect
getBound( UINT32_QUAD Bound ) {
    rect A = getBound( Bound.min );
    rect B = getBound( Bound.max );
    rect result = rectMM( getBL( A ), getTR( B ) );
    return result;
}

internal vec2
getCellBL( UINT32_PAIR cell ) {
    rect R = getBound( cell );
    
    vec2 result = getBL( R );
    return result;
}

internal vec2
getCellTL( UINT32_PAIR cell ) {
    rect R = getBound( cell );
    
    vec2 result = getTL( R );
    return result;
}

internal vec2
getCellBL( vec2 pos ) {
    UINT32_PAIR cell   = getCell( pos );
    vec2        result = getCellBL( cell );
    return result;
}

internal vec2
getCellTL( vec2 pos ) {
    UINT32_PAIR cell   = getCell( pos );
    vec2        result = getCellTL( cell );
    return result;
}

internal vec2
getWorldPos( uint32 xCell, uint32 yCell ) {
    vec2 result = Vec2( ( flo32 )xCell, ( flo32 )yCell ) * TILE_DIM;
    return result;
}

internal vec2
getWorldPos( UINT32_PAIR cell ) {
    vec2 result = getWorldPos( cell.x, cell.y );
    return result;
}

internal vec2
getWorldPos( CAMERA_STATE * Camera, vec2 app_dim, vec2 pos ) {
    vec2 t = clamp01( pos / app_dim ) - Vec2( 0.5f, 0.5f );
    vec2 P = Camera->pos + Camera->dim * t;
    return P;
}

//----------
// EDITOR
//----------

internal void
reset( EDITOR_STATE * Editor ) {
    Editor->Mode = EditorMode_default;
    
    { // init Camera
        flo32 x = ( flo32 )EDITOR__GRID_MAX_WIDTH  * 0.5f * TILE_WIDTH;
        flo32 y = ( flo32 )EDITOR__GRID_MAX_HEIGHT * 0.5f * TILE_HEIGHT;
        
        flo32 aspectRatio = 1920.0f / 1080.0f;
        flo32 dimY = CAMERA_TILE_Y_COUNT * TILE_HEIGHT;
        flo32 dimX = dimY * aspectRatio;
        
        CAMERA_STATE * Camera = &Editor->Camera;
        Camera->pos = Vec2( x, y );
        Camera->dim = Vec2( dimX, dimY );
    }
    
    reset( &Editor->SaveOpen );
    
    uint8 * addrA = ( uint8 * )Editor;
    uint8 * addrB = ( uint8 * )&Editor->reset_reset;
    uint32 reset_size = sizeof( EDITOR_STATE ) - ( uint32 )( addrB - addrA );
    memset( addrB, 0, reset_size );
}

internal UINT32_QUAD
GetGridBound( EDITOR_STATE * Editor ) {
    uint32 xMin = EDITOR__GRID_MAX_WIDTH;
    uint32 xMax = 0;
    uint32 yMin = EDITOR__GRID_MAX_HEIGHT;
    uint32 yMax = 0;
    
    uint32 nCell = 0;
    
    for( uint32 y = 0; y < EDITOR__GRID_MAX_HEIGHT; y++ ) {
        for( uint32 x = 0; x < EDITOR__GRID_MAX_WIDTH; x++ ) {
            if( Editor->grid[ y ][ x ] > CellType_Empty ) {
                xMin = minValue( x, xMin );
                xMax = maxValue( x, xMax );
                yMin = minValue( y, yMin );
                yMax = maxValue( y, yMax );
                nCell++;
            }
        }
    }
    
    if( nCell == 0 ) {
        xMin = xMax = 0;
        yMin = yMax = 0;
    }
    
    UINT32_QUAD Result = {};
    Result.min.x = xMin;
    Result.min.y = yMin;
    Result.max.x = xMax;
    Result.max.y = yMax;
    return Result;
}

internal UINT32_PAIR
getDim( UINT32_QUAD Bound ) {
    UINT32_PAIR result = {};
    result.x = Bound.max.x - Bound.min.x;
    result.y = Bound.max.y - Bound.min.y;
    return result;
}

internal UINT32_QUAD
GetCellBound( rect R ) {
    UINT32_QUAD result = {};
    result.min = getCell( getBL( R ) );
    result.max = getCell( getTR( R ) );
    result.max.x -= 1;
    result.max.y -= 1;
    return result;
}

internal UINT32_PAIR
GetGridCell( APP_STATE * AppState, vec2 P ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Result = {};
    if( Collision->Grid_nCell > 0 ) {
        Result = getCell( P, Collision->Grid_Bound.min );
    }
    
    return Result;
}

internal UINT32_QUAD
getExtrema( UINT32_PAIR A, UINT32_PAIR B ) {
    UINT32_QUAD Result = {};
    Result.min.x = minValue( A.x, B.x );
    Result.max.x = maxValue( A.x, B.x );
    Result.min.y = minValue( A.y, B.y );
    Result.max.y = maxValue( A.y, B.y );
    return Result;
}

internal void
fillInterior( APP_STATE * AppState, MEMORY * TempMemory ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    
    UINT32_QUAD Bound = GetGridBound( Editor );
    uint32 xCell = ( Bound.max.x - Bound.min.x ) + 1;
    uint32 yCell = ( Bound.max.y - Bound.min.y ) + 1;
    uint32 nCell = xCell * yCell;
    
    if( nCell > 0 ) {
        for( uint32 y = Bound.min.y; y <= Bound.max.y; y++ ) {
            for( uint32 x = Bound.min.x; x <= Bound.max.x; x++ ) {
                if( Editor->grid[ y ][ x ] == CellType_Interior ) {
                    Editor->grid[ y ][ x ] = CellType_Empty;
                }
            }
        }
        
        { // mark Exits as interior to cap openings
            EDITOR__EXIT_STATE * State = &Editor->Exit;
            for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                EDITOR__EXIT Entity = State->Entity[ iEntity ];
                UINT32_QUAD A = GetCellBound( Entity.Bound );
                for( uint32 y = A.min.y; y <= A.max.y; y++ ) {
                    for( uint32 x = A.min.x; x <= A.max.x; x++ ) {
                        Editor->grid[ y ][ x ] = CellType_Interior;
                    }
                }
            }
        }
        
        ROOM_STATS stat = Editor->stat;
        
        uint32       nStack = 0;
        UINT32_PAIR * stack = _pushArray_clear( TempMemory, UINT32_PAIR, nCell );
        
        UINT32_PAIR cell = stat.intCell;
        cell.x = clamp( cell.x, Bound.min.x, Bound.max.x );
        cell.y = clamp( cell.y, Bound.min.y, Bound.max.y );
        
        boo32 isValid = ( Editor->grid[ cell.y ][ cell.x ] <= CellType_Interior );
        if( isValid ) {
            stack[ nStack++ ] = cell;
            
            while( nStack > 0 ) {
                UINT32_PAIR c = stack[ --nStack ];
                Editor->grid[ c.y ][ c.x ] = CellType_Interior;
                
                if( c.x > Bound.min.x ) {
                    UINT32_PAIR cA = UInt32Pair( c.x - 1, c.y );
                    if( Editor->grid[ cA.y ][ cA.x ] == CellType_Empty ) {
                        stack[ nStack++ ] = cA;
                    }
                }
                if( c.y > Bound.min.y ) {
                    UINT32_PAIR cB = UInt32Pair( c.x, c.y - 1 );
                    if( Editor->grid[ cB.y ][ cB.x ] == CellType_Empty ) {
                        stack[ nStack++ ] = cB;
                    }
                }
                if( c.x < Bound.max.x ) {
                    UINT32_PAIR cC = UInt32Pair( c.x + 1, c.y );
                    if( Editor->grid[ cC.y ][ cC.x ] == CellType_Empty ) {
                        stack[ nStack++ ] = cC;
                    }
                }
                if( c.y < Bound.max.y ) {
                    UINT32_PAIR cD = UInt32Pair( c.x, c.y + 1 );
                    if( Editor->grid[ cD.y ][ cD.x ] == CellType_Empty ) {
                        stack[ nStack++ ] = cD;
                    }
                }
            }
        } else {
            char str[ 512 ] = {};
            sprintf( str, "ERROR! Attempted to flood fill interior, but intCell is sitting on some geometry! ( %u, %u )", cell.x, cell.y );
            CONSOLE_STRING( str );
            globalVar_debugSystem.errorOccurred = true;
        }
        
        _popArray( TempMemory, UINT32_PAIR, nCell );
    }
}

internal boo32
DoesIntersectDoors( EDITOR_STATE * Editor, vec2 P, vec2 Q ) {
    boo32 result = false;
    
    RAY2 ray = Ray2( P, Q - P );
    
    { // MECH DOOR
        EDITOR__MECH_DOOR_STATE * State = &Editor->Mech;
        for( uint32 iEntity = 0; ( !result ) && ( iEntity < State->nEntity ); iEntity++ ) {
            EDITOR__MECH_DOOR Entity = State->Entity[ iEntity ];
            
            RAY2_INTERSECT intersect = DoesIntersect( ray, Entity.Bound_Door );
            if( ( intersect.isValid ) && ( intersect.t < 1.0f ) ) {
                result = true;
            }
        }
    }
    
    { // SECURE DOOR
        EDITOR__SECURE_DOOR_STATE * State = &Editor->Secure;
        for( uint32 iEntity = 0; ( !result ) && ( iEntity < State->nEntity ); iEntity++ ) {
            EDITOR__SECURE_DOOR Entity = State->Entity[ iEntity ];
            
            RAY2_INTERSECT Intersect = DoesIntersect( ray, Entity.Bound );
            if( ( Intersect.isValid ) && ( Intersect.t < 1.0f ) ) {
                result = true;
            }
        }
    }
    
    return result;
}

internal UINT32_PAIR
getPathLinkOfBound( PATH_STATE Path, rect R ) {
    uint32 nLink      = 0; 
    int32   link[ 2 ] = { -1, -1 };
    for( uint32 iLink = 0; iLink < Path.nPathLink; iLink++ ) {
        PATH_LINK__POSITION_DATA pos = Path.PathLink_PositionData[ iLink ];
        vec2 P = pos.toNodeP;
        vec2 Q = pos.ExitP;
        
        if( P.y == Q.y ) {
            if( P.x > Q.x ) { _swap( vec2, P, Q ); }
            if( ( P.y > R.Bottom ) && ( P.y < R.Top ) && ( P.x == R.Left ) && ( Q.x == R.Right ) ) {
                link[ nLink++ ] = iLink;
            }
        }
    }
    
    Assert( nLink == 2 );
    UINT32_PAIR result = UInt32Pair( link[ 0 ], link[ 1 ] );
    return result;
}

//----------
// CAMERA
//----------

internal void
updateCamera( CAMERA_STATE * Camera, KEYBOARD_STATE * Keyboard, flo32 dt ) {
    if( wasPressed ( Keyboard, KeyCode_num4 ) ) { Camera->move_Left  = true;  }
    if( wasReleased( Keyboard, KeyCode_num4 ) ) { Camera->move_Left  = false; }
    if( wasPressed ( Keyboard, KeyCode_num6 ) ) { Camera->move_Right = true;  }
    if( wasReleased( Keyboard, KeyCode_num6 ) ) { Camera->move_Right = false; }
    if( wasPressed ( Keyboard, KeyCode_num9 ) ) { Camera->move_in    = true;  }
    if( wasReleased( Keyboard, KeyCode_num9 ) ) { Camera->move_in    = false; }
    if( wasPressed ( Keyboard, KeyCode_num7 ) ) { Camera->move_out   = true;  }
    if( wasReleased( Keyboard, KeyCode_num7 ) ) { Camera->move_out   = false; }
    if( wasPressed ( Keyboard, KeyCode_num8 ) ) { Camera->move_up    = true;  }
    if( wasReleased( Keyboard, KeyCode_num8 ) ) { Camera->move_up    = false; }
    if( wasPressed ( Keyboard, KeyCode_num5 ) ) { Camera->move_down  = true;  }
    if( wasReleased( Keyboard, KeyCode_num5 ) ) { Camera->move_down  = false; }
    flo32 ratio = Camera->dim.y / ( CAMERA_TILE_Y_COUNT * TILE_HEIGHT );
    flo32 speed = 9.0f * ratio;
    Camera->scale_ratio = ratio;
    
    if( Camera->move_Left ) {
        Camera->pos.x -= ( speed * dt );
    }
    if( Camera->move_Right ) {
        Camera->pos.x += ( speed * dt );
    }
    if( Camera->move_down ) {
        Camera->pos.y -= ( speed * dt );
    }
    if( Camera->move_up ) {
        Camera->pos.y += ( speed * dt );
    }
    if( Camera->move_in ) {
        Camera->dim *= 0.99f;
    }
    if( Camera->move_out ) {
        Camera->dim *= 1.01f;
    }
}

//----------
// GRID TOOLS
//----------

internal void
updateRoomBound( APP_STATE * AppState ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    
    rect Bound = { FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX };
    
    uint32 nTerrain = 0;
    { // TERRAIN
        for( uint32 y = 0; y < EDITOR__GRID_MAX_HEIGHT; y++ ) {
            for( uint32 x = 0; x < EDITOR__GRID_MAX_WIDTH; x++ ) {
                if( Editor->grid[ y ][ x ] > CellType_Empty ) {
                    rect R = getBound( x, y );
                    Bound = rectExtrema( R, Bound );
                    
                    nTerrain++;
                }
            }
        }
    }
    
    if( nTerrain == 0 ) {
        Bound = {};
    }
    
    ROOM_STATS * stat = &Editor->stat;
    stat->Bound = Bound;
    
    // NOTE: I don't realistically see any entities being set outside of the terrain Bound, so I'm going to skip finding the Bounds of the other entities. Just putting this note here to remind me if there's an issue.
}

#define GRID_FLAG__EDGE_EXISTS_LEFT    ( 0x01 )
#define GRID_FLAG__EDGE_EXISTS_BOTTOM  ( 0x02 )
#define GRID_FLAG__EDGE_EXISTS_RIGHT   ( 0x04 )
#define GRID_FLAG__EDGE_EXISTS_TOP     ( 0x08 )

internal boo32
hasNEmptyCellsDown( uint8 * border, uint32 xCell, uint32 yCell, UINT32_PAIR cell, uint32 nMax ) {
    // NOTE: Count the number of empty cells below this cell, including this cell.
    
    boo32 result = true;
    if( cell.y < nMax ) {
        result = false;
    } else {
        uint32 yMin = cell.y - nMax;
        for( uint32 iY = cell.y; iY > yMin; iY-- ) {
            uint32 iCell = iY * xCell + cell.x;
            if( border[ iCell ] & GRID_FLAG__EDGE_EXISTS_TOP ) {
                result = false;
            }
        }
    }
    return result;
}

internal uint32
hasNEmptyCellsUp( uint8 * border, uint32 xCell, uint32 yCell, UINT32_PAIR cell, uint32 nMax ) {
    // NOTE: Count the number of empty cells above this cell, NOT including this cell.
    
    boo32 result = true;
    if( cell.y >= ( yCell - nMax ) ) {
        result = false;
    } else {
        uint32 yMax = cell.y + nMax;
        for( uint32 iY = cell.y + 1; iY <= yMax; iY++ ) {
            uint32 iCell = iY * xCell + cell.x;
            if( border[ iCell ] & GRID_FLAG__EDGE_EXISTS_BOTTOM ) {
                result = false;
            }
        }
    }
    return result;
}

//----------
// KEYBOARD INPUT
//----------

internal void
UpdateKeyboardInputForFileSaveOpen( INPUT_STRING * i, KEYBOARD_STATE * Keyboard, FILE_SAVE_OPEN * State ) {
    for( uint32 iKeyEvent = 0; iKeyEvent < Keyboard->nEvents; iKeyEvent++ ) {
        KEY_STATE Event = Keyboard->Event[ iKeyEvent ];
        if( Event.isDown ) {
            if( ( Event.KeyCode >= KeyCode_a ) && ( Event.KeyCode <= KeyCode_z ) ) {
                char code = ( char )( Event.KeyCode - KeyCode_a );
                if( Event.flags == KEYBOARD_FLAGS__SHIFT ) {
                    addChar( i, 'A' + code );
                } else if( Event.flags == 0 ) {
                    addChar( i, 'a' + code );
                }
                State->overwrite = false;
            } else if( ( Event.KeyCode >= KeyCode_0 ) && ( Event.KeyCode <= KeyCode_9 ) ) {
                char code = ( char )( Event.KeyCode - KeyCode_0 );
                addChar( i, '0' + code );
                State->overwrite = false;
            } else if( ( Event.KeyCode == KeyCode_dash ) && ( Event.flags == KEYBOARD_FLAGS__SHIFT ) ) {
                addChar( i, '_' );
                State->overwrite = false;
            } else if( Event.KeyCode == KeyCode_backspace ) {
                deleteChar_backspace( i );
                State->overwrite = false;
            }
        }
    }
}

internal void
UpdateKeyboardInputForLayoutFilter( INPUT_STRING * i, KEYBOARD_STATE * Keyboard ) {
    for( uint32 iKeyEvent = 0; iKeyEvent < Keyboard->nEvents; iKeyEvent++ ) {
        KEY_STATE Event = Keyboard->Event[ iKeyEvent ];
        if( Event.isDown ) {
            if( ( Event.KeyCode >= KeyCode_a ) && ( Event.KeyCode <= KeyCode_z ) ) {
                char code = ( char )( Event.KeyCode - KeyCode_a );
                if( Event.flags == KEYBOARD_FLAGS__SHIFT ) {
                    addChar( i, 'A' + code );
                } else if( Event.flags == 0 ) {
                    addChar( i, 'a' + code );
                }
            } else if( ( Event.KeyCode >= KeyCode_0 ) && ( Event.KeyCode <= KeyCode_9 ) ) {
                char code = ( char )( Event.KeyCode - KeyCode_0 );
                addChar( i, '0' + code );
            } else if( ( Event.KeyCode == KeyCode_dash ) && ( Event.flags == KEYBOARD_FLAGS__SHIFT ) ) {
                addChar( i, '_' );
            } else if( Event.KeyCode == KeyCode_backspace ) {
                deleteChar_backspace( i );
            }
        }
    }
}

//----------
// NAV MESH
//----------

#if 0
internal void
GenNavMeshLinks( EDITOR__NAV_MESH_STATE * State ) {
    for( uint32 iTriA = 0; iTriA < ( State->nEntity - 1 ); iTriA++ ) {
        EDITOR__NAV_MESH_TRI * TriA  = State->Entity + iTriA;
        UINT32_TRI            iVertA = TriA->iInteriorVert;
        
        for( uint32 iTriB = iTriA + 1; iTriB < State->nEntity; iTriB++ ) {
            EDITOR__NAV_MESH_TRI * TriB  = State->Entity + iTriB;
            UINT32_TRI            iVertB = TriB->iInteriorVert;
            
            for( uint32 iEdgeA = 0; iEdgeA < 3; iEdgeA++ ) {
                uint32 iA0 =   iEdgeA;
                uint32 iA1 = ( iEdgeA + 1 ) % 3;
                UINT32_PAIR EdgeA = UInt32Pair( iVertA.elem[ iA0 ], iVertA.elem[ iA1 ] );
                
                for( uint32 iEdgeB = 0; iEdgeB < 3; iEdgeB++ ) {
                    uint32 iB0 =   iEdgeB;
                    uint32 iB1 = ( iEdgeB + 1 ) % 3;
                    
                    // NOTE: Note that  iB0  and  iB1 are reversed to simplify the comparison.
                    UINT32_PAIR EdgeB = UInt32Pair( iVertB.elem[ iB1 ], iVertB.elem[ iB0 ] );
                    
                    if( EdgeA == EdgeB ) {
                        TriA->iPathLink.elem[ iEdgeA ] = iTriB;
                        TriB->iPathLink.elem[ iEdgeB ] = iTriA;
                    }
                }
            }
        }
    }
}
#endif