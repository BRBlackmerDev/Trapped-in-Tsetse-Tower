
internal void
addTerrain( APP_STATE * AppState, uint32 x, uint32 y ) {
    TERRAIN_STATE * State = &AppState->terrain;
    
    TERRAIN terrain = {};
    
    vec2 P = Vec2( ( flo32 )x, ( flo32 )y ) * TILE_DIM;
    terrain.Bound = rectBLD( P, TILE_DIM );
    
    Assert( State->nTerrain < TERRAIN_MAX_COUNT );
    State->terrain[ State->nTerrain++ ] = terrain;
}

internal PATH_CONNECTION
doesConnect( COLLISION_STATE * Collision, uint32 iNode_upper, uint32 iNode_lower, vec2 pos_upper, vec2 pos_lower ) {
    boo32 isValid_up   = true;
    boo32 isValid_down = true;
    
    flo32 maxXDist      = JUMPER_JUMP_MAX_X_DIST;
    flo32 maxYDist_up   = JUMPER_JUMP_MAX_Y_DIST_UP;
    flo32 maxYDist_down = JUMPER_JUMP_MAX_Y_DIST_DOWN;
    
    flo32 xDist = fabsf( pos_upper.x - pos_lower.x );
    flo32 yDist = pos_upper.y - pos_lower.y;
    
    if( xDist > maxXDist ) {
        isValid_up   = false;
        isValid_down = false;
    }
    if( yDist > maxYDist_up ) {
        isValid_up = false;
    } else {
        // TODO: redo this test. look at the geometry of all possible valid jumps. this allows some big possible jumps down
        flo32 distSq    = GetLengthSq( pos_upper - pos_lower );
        flo32 maxDistSq = maxXDist * maxXDist;
        if( distSq >= maxDistSq ) {
            isValid_up = false;
        }
    }
    if( yDist > maxYDist_down ) {
        isValid_down = false;
    }
    
    if( ( isValid_up ) || ( isValid_down ) ) {
        vec4 *  coll = Collision->Edge;
        uint32 nColl = Collision->nEdge;
        
        for( uint32 iColl = 0; iColl < nColl; iColl++ ) {
            vec4 c = coll[ iColl ];
            
            boo32 doInt = DoIntersectEE( pos_upper, pos_lower, c.P, c.Q );
            if( doInt ) {
                isValid_up   = false;
                isValid_down = false;
            }
        }
    }
    
    PATH_CONNECTION result = {};
    if( ( isValid_up ) || ( isValid_down ) ) {
        result.isValid_up   = isValid_up;
        result.isValid_down = isValid_down;
        result.iNode_upper  = iNode_upper;
        result.iNode_lower  = iNode_lower;
        result.pos_upper    = pos_upper;
        result.pos_lower    = pos_lower;
    }
    return result;
}

internal void
genTerrainCollision( APP_STATE * AppState, UINT32_PAIR * terrain, uint32 nTerrain, MEMORY * TempMemory ) {
    reset( &AppState->Collision_memory );
    memset( &AppState->Collision, 0, sizeof( COLLISION_STATE ) );
    
    // get grid Bound
    uint32 xMin = EDITOR__GRID_MAX_WIDTH;
    uint32 xMax = 0;
    uint32 yMin = EDITOR__GRID_MAX_HEIGHT;
    uint32 yMax = 0;
    for( uint32 iTerrain = 0; iTerrain < nTerrain; iTerrain++ ) {
        UINT32_PAIR t = terrain[ iTerrain ];
        xMin = minValue( xMin, t.x );
        xMax = maxValue( xMax, t.x );
        yMin = minValue( yMin, t.y );
        yMax = maxValue( yMax, t.y );
    }
    
    uint32 nEdge_Left   = 0;
    uint32 nEdge_Bottom = 0;
    uint32 nEdge_Right  = 0;
    uint32 nEdge_Top    = 0;
    uint32 maxEdge      = nTerrain * 4;
    
    vec4 * Edge_Left   = _pushArray_clear( TempMemory, vec4, maxEdge );
    vec4 * Edge_Bottom = _pushArray_clear( TempMemory, vec4, maxEdge );
    vec4 * Edge_Right  = _pushArray_clear( TempMemory, vec4, maxEdge );
    vec4 * Edge_Top    = _pushArray_clear( TempMemory, vec4, maxEdge );
    
    // init grid
    uint32 xCell = ( xMax - xMin + 1 );
    uint32 yCell = ( yMax - yMin + 1 );
    uint32 nCell = xCell * yCell;
    uint8 * border = _pushArray_clear( TempMemory, uint8, nCell );
    for( uint32 iTerrain = 0; iTerrain < nTerrain; iTerrain++ ) {
        UINT32_PAIR t = terrain[ iTerrain ];
        
        uint32 x = ( t.x - xMin );
        uint32 y = ( t.y - yMin );
        uint32 cell = y * xCell + x;
        border[ cell ] = 0xFF;
    }
    
    uint8 wasNotVisited_Left   = 0x10;
    uint8 wasNotVisited_Bottom = 0x20;
    uint8 wasNotVisited_Right  = 0x40;
    uint8 wasNotVisited_Top    = 0x80;
    uint8 EdgeExists_Left      = 0x01;
    uint8 EdgeExists_Bottom    = 0x02;
    uint8 EdgeExists_Right     = 0x04;
    uint8 EdgeExists_Top       = 0x08;
    
    uint8 flag_Left   = ~( wasNotVisited_Left   | EdgeExists_Left   );
    uint8 flag_Bottom = ~( wasNotVisited_Bottom | EdgeExists_Bottom );
    uint8 flag_Right  = ~( wasNotVisited_Right  | EdgeExists_Right  );
    uint8 flag_Top    = ~( wasNotVisited_Top    | EdgeExists_Top    );
    
    // clear invalid Edges
    for( uint32 iTerrain = 0; iTerrain < nTerrain; iTerrain++ ) {
        UINT32_PAIR t = terrain[ iTerrain ];
        
        uint32 x = ( t.x - xMin );
        uint32 y = ( t.y - yMin );
        uint32 cell = y * xCell + x;
        
        uint8 * bA = border + cell;
        
        if( x > 0 ) {
            uint8 * bB = border + cell - 1;
            if( *bB ) {
                bA[ 0 ] &= flag_Left;
                bB[ 0 ] &= flag_Right;
            }
        }
        if( x < ( xCell - 2 ) ) {
            uint8 * bB = border + cell + 1;
            if( *bB ) {
                bA[ 0 ] &= flag_Right;
                bB[ 0 ] &= flag_Left;
            }
        }
        if( y > 0 ) {
            uint8 * bB = border + cell - xCell;
            if( *bB ) {
                bA[ 0 ] &= flag_Bottom;
                bB[ 0 ] &= flag_Top;
            }
        }
        if( y < ( yCell - 2 ) ) {
            uint8 * bB = border + cell + xCell;
            if( *bB ) {
                bA[ 0 ] &= flag_Top;
                bB[ 0 ] &= flag_Bottom;
            }
        }
    }
    
    // gen Collision geometry
    for( uint32 iCell = 0; iCell < nCell; iCell++ ) {
        uint8 * cell = border + iCell;
        if( cell[ 0 ] & 0xF0 ) {
            uint32 atCell = iCell;
            uint8 * cellA = cell;
            
            Assert( cellA[ 0 ] & wasNotVisited_Left   );
            Assert( cellA[ 0 ] & wasNotVisited_Bottom );
            
            int32 x = ( atCell % xCell ) + xMin;
            int32 y = ( atCell / xCell ) + yMin;
            
            uint32 EdgeID = 0;
            vec2 P = Vec2( ( flo32 )x, ( flo32 )y ) * TILE_DIM;
            vec2 R = P;
            R.y += TILE_HEIGHT;
            cellA[ 0 ] &= ( ~wasNotVisited_Left );
            
            
            boo32 findIsland = true;
            while( findIsland ) {
                switch( EdgeID ) {
                    case 0: { // Left
                        boo32 foundNext = false;
                        if( cellA[ 0 ] & wasNotVisited_Top ) {
                            foundNext = true;
                            
                            Edge_Left[ nEdge_Left++ ] = Vec4( P, R );
                            
                            P    = R;
                            R.x += TILE_WIDTH;
                            
                            cellA[ 0 ] &= ( ~wasNotVisited_Top );
                            
                            EdgeID = 3;
                        }
                        if( !foundNext ) {
                            cellA += xCell;
                            if( cellA[ 0 ] & wasNotVisited_Left ) {
                                foundNext = true;
                                
                                R.y += TILE_HEIGHT;
                                
                                cellA[ 0 ] &= ( ~wasNotVisited_Left );
                            }
                        }
                        if( !foundNext ) {
                            cellA -= 1;
                            if( cellA[ 0 ] & wasNotVisited_Bottom ) {
                                foundNext = true;
                                
                                Edge_Left[ nEdge_Left++ ] = Vec4( P, R );
                                
                                P    = R;
                                R.x -= TILE_WIDTH;
                                
                                cellA[ 0 ] &= ( ~wasNotVisited_Bottom );
                                
                                EdgeID = 1;
                            }
                        }
                        if( !foundNext ) {
                            InvalidCodePath;
                        }
                    } break;
                    
                    case 1: { // Bottom
                        boo32 foundNext = false;
                        if( cellA[ 0 ] & wasNotVisited_Left ) {
                            foundNext = true;
                            
                            Edge_Bottom[ nEdge_Bottom++ ] = Vec4( P, R );
                            
                            P    = R;
                            R.y += TILE_HEIGHT;
                            
                            cellA[ 0 ] &= ( ~wasNotVisited_Left );
                            
                            EdgeID = 0;
                        }
                        if( !foundNext ) {
                            cellA -= 1;
                            if( cellA[ 0 ] & wasNotVisited_Bottom ) {
                                foundNext = true;
                                
                                R.x -= TILE_WIDTH;
                                
                                cellA[ 0 ] &= ( ~wasNotVisited_Bottom );
                            }
                        }
                        if( !foundNext ) {
                            cellA -= xCell;
                            if( cellA[ 0 ] & wasNotVisited_Right ) {
                                foundNext = true;
                                
                                Edge_Bottom[ nEdge_Bottom++ ] = Vec4( P, R );
                                
                                P    = R;
                                R.y -= TILE_HEIGHT;
                                
                                cellA[ 0 ] &= ( ~wasNotVisited_Right );
                                
                                EdgeID = 2;
                            }
                        }
                        if( !foundNext ) {
                            Edge_Bottom[ nEdge_Bottom++ ] = Vec4( P, R );
                            findIsland = false;
                        }
                    } break;
                    
                    case 2: { // Right
                        boo32 foundNext = false;
                        if( cellA[ 0 ] & wasNotVisited_Bottom ) {
                            foundNext = true;
                            
                            Edge_Right[ nEdge_Right++ ] = Vec4( P, R );
                            
                            P    = R;
                            R.x -= TILE_WIDTH;
                            
                            cellA[ 0 ] &= ( ~wasNotVisited_Bottom );
                            
                            EdgeID = 1;
                        }
                        if( !foundNext ) {
                            cellA -= xCell;
                            if( cellA[ 0 ] & wasNotVisited_Right ) {
                                foundNext = true;
                                
                                R.y -= TILE_HEIGHT;
                                
                                cellA[ 0 ] &= ( ~wasNotVisited_Right );
                            }
                        }
                        if( !foundNext ) {
                            cellA += 1;
                            if( cellA[ 0 ] & wasNotVisited_Top ) {
                                foundNext = true;
                                
                                Edge_Right[ nEdge_Right++ ] = Vec4( P, R );
                                
                                P    = R;
                                R.x += TILE_WIDTH;
                                
                                cellA[ 0 ] &= ( ~wasNotVisited_Top );
                                
                                EdgeID = 3;
                            }
                        }
                        if( !foundNext ) {
                            InvalidCodePath;
                        }
                    } break;
                    
                    case 3: { // Top
                        boo32 foundNext = false;
                        if( cellA[ 0 ] & wasNotVisited_Right ) {
                            foundNext = true;
                            
                            Edge_Top[ nEdge_Top++ ] = Vec4( P, R );
                            
                            P    = R;
                            R.y -= TILE_HEIGHT;
                            
                            cellA[ 0 ] &= ( ~wasNotVisited_Right );
                            
                            EdgeID = 2;
                        }
                        if( !foundNext ) {
                            cellA += 1;
                            if( cellA[ 0 ] & wasNotVisited_Top ) {
                                foundNext = true;
                                
                                R.x += TILE_WIDTH;
                                
                                cellA[ 0 ] &= ( ~wasNotVisited_Top );
                            }
                        }
                        if( !foundNext ) {
                            cellA += xCell;
                            if( cellA[ 0 ] & wasNotVisited_Left ) {
                                foundNext = true;
                                
                                Edge_Top[ nEdge_Top++ ] = Vec4( P, R );
                                
                                P    = R;
                                R.y += TILE_HEIGHT;
                                
                                cellA[ 0 ] &= ( ~wasNotVisited_Left );
                                
                                EdgeID = 0;
                            }
                        }
                        if( !foundNext ) {
                            InvalidCodePath;
                        }
                    } break;
                }
            }
        }
    }
    
    uint32 at = 0;
    
    COLLISION_STATE * Collision = &AppState->Collision;
    Collision->Type[ 0 ] = UInt32Pair( at, nEdge_Left   );  at += nEdge_Left;
    Collision->Type[ 1 ] = UInt32Pair( at, nEdge_Bottom );  at += nEdge_Bottom;
    Collision->Type[ 2 ] = UInt32Pair( at, nEdge_Right  );  at += nEdge_Right;
    Collision->Type[ 3 ] = UInt32Pair( at, nEdge_Top    );  at += nEdge_Top;
    
    Collision->Edge  = _pushArray_clear( &AppState->Collision_memory, vec4, at );
    Collision->nEdge = at;
    
    at = 0;
    memcpy( Collision->Edge + at, Edge_Left,   sizeof( vec4 ) * nEdge_Left   ); at += nEdge_Left;
    memcpy( Collision->Edge + at, Edge_Bottom, sizeof( vec4 ) * nEdge_Bottom ); at += nEdge_Bottom;
    memcpy( Collision->Edge + at, Edge_Right,  sizeof( vec4 ) * nEdge_Right  ); at += nEdge_Right;
    memcpy( Collision->Edge + at, Edge_Top,    sizeof( vec4 ) * nEdge_Top    ); at += nEdge_Top;
    
    ROOM_STATS stat = AppState->Editor.stat;
    if( stat.isTopDown ) {
#if 0
        EDITOR_STATE * Editor = &AppState->Editor;
        { // NAV MESH
            uint32 MaxInteriorVertex  = 128;
            uint32 nInteriorVertex    = 0;
            Collision->InteriorVertex = _pushArray( &AppState->Collision_memory, vec2, MaxInteriorVertex );
            
            //uint32 MaxInteriorEdge    = 128;
            //uint32 nInteriorEdge      = 0;
            //Collision->InteriorEdge   = _pushArray( &AppState->Collision_memory, UINT32_PAIR, MaxInteriorEdge );
            
            INT32_PAIR OffsetP[ 4 ] = { { -1,  0 }, { -1, -1 }, { 0, -1 }, { 0, 0 } };
            for( uint32 iType = 0; iType < 4; iType++ ) {
                UINT32_PAIR Type = Collision->Type[ iType ];
                
                vec4 * Edge = Collision->Edge + Type.m;
                for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                    vec4 E = Edge[ iEdge ];
                    
                    UINT32_PAIR Cell = getCell( E.P );
                    Assert( Cell.x > 0 );
                    Assert( Cell.y > 0 );
                    
                    Cell.x += OffsetP[ iType ].x;
                    Cell.y += OffsetP[ iType ].y;
                    
                    if( Editor->grid[ Cell.y ][ Cell.x ] == CellType_Interior ) {
                        int32 iVertP = -1;
                        int32 iVertQ = -1;
                        for( uint32 iV = 0; iV < nInteriorVertex; iV++ ) {
                            if( E.P == Collision->InteriorVertex[ iV ] ) {
                                iVertP = iV;
                            }
                            if( E.Q == Collision->InteriorVertex[ iV ] ) {
                                iVertQ = iV;
                            }
                        }
                        
                        if( iVertP == -1 ) {
                            //iVertP = nInteriorVertex;
                            
                            Assert( nInteriorVertex < MaxInteriorVertex );
                            Collision->InteriorVertex[ nInteriorVertex++ ] = E.P;
                        }
                        if( iVertQ == -1 ) {
                            //iVertQ = nInteriorVertex;
                            
                            Assert( nInteriorVertex < MaxInteriorVertex );
                            Collision->InteriorVertex[ nInteriorVertex++ ] = E.Q;
                        }
                        
                        //UINT32_PAIR InteriorEdge = UInt32Pair( iVertP, iVertQ );
                        
                        //Assert( nInteriorEdge < MaxInteriorEdge );
                        //Collision->InteriorEdge[ nInteriorEdge++ ] = InteriorEdge;
                        
#if 0                        
                        flo32 MaxLength = TILE_WIDTH * 8.0f;
                        flo32    Length = GetLength( E.Q - E.P );
                        if( Length > MaxLength ) {
                            uint32 nSegment = ( uint32 )( Length / MaxLength );
                            vec2   N        = GetNormal( E.Q - E.P );
                            for( uint32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
                                vec2 P = E.P + N * ( MaxLength * ( flo32 )( iSegment + 1 ) );
                                
                                boo32 wasFound = false;
                                for( uint32 iV = 0; iV < nInteriorVertex; iV++ ) {
                                    if( P == Collision->InteriorVertex[ iV ] ) {
                                        wasFound = true;
                                    }
                                }
                                
                                if( !wasFound ) {
                                    Assert( nInteriorVertex < MaxInteriorVertex );
                                    Collision->InteriorVertex[ nInteriorVertex++ ] = P;
                                }
                            }
                        }
#endif
                    }
                }
            }
            
            Collision->nInteriorVertex = nInteriorVertex;
            //Collision->nInteriorEdge   = nInteriorEdge;
            
#if 0
            if( nInteriorVertex > 0 ) {
                uint32 MaxCircumcircle  = 128;
                uint32 nCircumcircle    = 0;
                Collision->Circumcircle = _pushArray( &AppState->Collision_memory, circ, MaxCircumcircle );
                
                uint32 MaxTri = MaxCircumcircle;
                uint32 nTri   = 0;
                Collision->Tri = _pushArray( &AppState->Collision_memory, UINT32_TRI, MaxTri );
                
                for( uint32 iA = 0; iA < ( nInteriorVertex - 2 ); iA++ ) {
                    for( uint32 iB = iA + 1; iB < ( nInteriorVertex - 1 ); iB++ ) {
                        for( uint32 iC = iB + 1; iC < ( nInteriorVertex ); iC++ ) {
                            vec2 A = Collision->InteriorVertex[ iA ];
                            vec2 B = Collision->InteriorVertex[ iB ];
                            vec2 C = Collision->InteriorVertex[ iC ];
                            
                            LINE2 BiAB = Line2( ( A + B ) * 0.5f, GetNormal( GetPerp( B - A ) ) );
                            LINE2 BiAC = Line2( ( A + C ) * 0.5f, GetNormal( GetPerp( C - A ) ) );
                            
                            LINE2_INTERSECT Intersect = DoesIntersect( BiAB, BiAC );
                            if( Intersect.IsValid ) {
                                circ Circumcircle = {};
                                Circumcircle.Center = Intersect.P;
                                Circumcircle.Radius = GetLength( A - Intersect.P );
                                
                                uint32 MaxAdditionalVert       = 16;
                                uint32   nAdditionalVert       = 0;
                                uint32    AdditionalVert[ 16 ] = {};
                                
                                boo32 isValid = true;
                                for( uint32 iVert = 0; iVert < nInteriorVertex; iVert++ ) {
                                    if( ( iVert != iA ) && ( iVert != iB ) && ( iVert != iC ) ) {
                                        vec2 P = Collision->InteriorVertex[ iVert ];
                                        if( IsInBound( P, Circumcircle ) ) {
                                            flo32 Length = GetLength( P - Circumcircle.Center );
                                            if( Length == Circumcircle.Radius ) {
                                                Assert( nAdditionalVert < MaxAdditionalVert );
                                                AdditionalVert[ nAdditionalVert++ ] = iVert;
                                            } else {
                                                isValid = false;
                                            }
                                        }
                                    }
                                }
                                
                                //isValid = false;
                                //if( ( iA == 2 ) && ( iB == 5 ) && ( iC == 6 ) ) {
                                //isValid = true;
                                //nAdditionalVert = 0;
                                //}
                                
                                if( isValid ) {
                                    Assert( nCircumcircle < MaxCircumcircle );
                                    Collision->Circumcircle[ nCircumcircle++ ] = Circumcircle;
                                    
                                    if( nAdditionalVert > 0 ) {
                                        // TODO: Triangulate polygon!
                                    } else {
                                        Assert( nTri < MaxTri );
                                        
                                        UINT32_TRI Tri = UInt32Tri( iA, iB, iC );
                                        if( !IsCCW( A, B, C ) ) {
                                            _swap( uint32, Tri.y, Tri.z );
                                        }
                                        
                                        Collision->Tri[ nTri++ ] = Tri;
                                    }
                                }
                            }
                        }
                    }
                }
                
                Collision->nCircumcircle = nCircumcircle;
                Collision->nTri          = nTri;
            }
#endif
        }
#endif
    } else {
        { // jump Bounds
            UINT32_PAIR Type = Collision->Type[ 3 ];
            
            if( Type.n > 0 ) {
                Collision->nJumpBound = Type.n;
                Collision->jumpBound  = _pushArray( &AppState->Collision_memory, rect, Type.n );
                
                vec4 * Edge = Collision->Edge + Type.m;
                for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                    vec4 e = Edge[ iEdge ];
                    
                    vec2 P = e.xy + Vec2( -( PLAYER_HALF_WIDTH + 0.125f ), -0.1f );
                    vec2 Q = e.zw + Vec2(  ( PLAYER_HALF_WIDTH + 0.125f ),  0.1f );
                    rect R = rectMM( P, Q );
                    
                    Collision->jumpBound[ iEdge ] = R;
                }
            }
        }
        
        { // wall slide Bounds
            // TODO: don't extend the wall jump down if there is a floor below it
            flo32 margin = TILE_WIDTH;
            { // Left
                UINT32_PAIR Type = Collision->Type[ 0 ];
                if( Type.n > 0 ) {
                    Collision->nWallSlideLeft = Type.n;
                    Collision->wallSlideLeft  = _pushArray( &AppState->Collision_memory, rect, Type.n );
                    
                    vec4 * Edge = Collision->Edge + Type.m;
                    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                        vec4 e = Edge[ iEdge ];
                        
                        flo32 length = GetLength( e.zw - e.xy );
                        if( length >= WALL_JUMP_MIN_LENGTH ) {
                            flo32 extension = 0.0f;
                            if( length >= WALL_JUMP_MIN_LENGTH_FOR_EXTENSION ) {
                                extension = WALL_JUMP_NEG_Y_EXTENSION;
                            }
                            vec2 P = e.xy + Vec2( -( PLAYER_HALF_WIDTH + margin ), extension );
                            vec2 Q = e.zw;
                            rect R = rectMM( P, Q );
                            
                            Collision->wallSlideLeft[ iEdge ] = R;
                        }
                    }
                }
            }
            
            { // Right
                UINT32_PAIR Type = Collision->Type[ 2 ];
                if( Type.n > 0 ) {
                    Collision->nWallSlideRight = Type.n;
                    Collision->wallSlideRight  = _pushArray( &AppState->Collision_memory, rect, Type.n );
                    
                    vec4 * Edge = Collision->Edge + Type.m;
                    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                        vec4 e = Edge[ iEdge ];
                        
                        flo32 length = GetLength( e.zw - e.xy );
                        if( length >= WALL_JUMP_MIN_LENGTH ) {
                            flo32 extension = 0.0f;
                            if( length >= WALL_JUMP_MIN_LENGTH_FOR_EXTENSION ) {
                                extension = WALL_JUMP_NEG_Y_EXTENSION;
                            }
                            vec2 P = e.zw + Vec2( 0.0f, extension );
                            vec2 Q = e.xy + Vec2( ( PLAYER_HALF_WIDTH + margin ), 0.0f );
                            rect R = rectMM( P, Q );
                            
                            Collision->wallSlideRight[ iEdge ] = R;
                        }
                    }
                }
            }
        }
        
#if 0
        { // lEdge grab Bounds
            vec2 margin = Vec2( TILE_WIDTH * 1.0f, TILE_HEIGHT * 1.0f );
            { // Left
                UINT32_PAIR Type = Collision->Type[ 0 ];
                if( Type.n > 0 ) {
                    Collision->nLedgeGrabLeft = Type.n;
                    Collision->lEdgeGrabLeft  = _pushArray( &AppState->Collision_memory, rect, Type.n );
                    
                    vec4 * Edge = Collision->Edge + Type.m;
                    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                        vec4 e = Edge[ iEdge ];
                        
                        UINT32_PAIR cell = getCell( e.Q, xMin, yMin );
                        if( ( cell.x > 0 ) && ( cell.y > 0 ) ) {
                            cell.x--;
                            cell.y--;
                            
                            uint32 minUp = PLATFORM_MIN_TILE_Y_FOR_CEILING_ABOVE_VALID_LEDGE_GRAB;
                            boo32  up_isValid = hasNEmptyCellsUp( border, xCell, yCell, cell, minUp );
                            
                            uint32 minDown = PLATFORM_TILE_Y_FOR_FULL_JUMP_NO_LEDGE_GRAB;
                            uint32 down_isValid = hasNEmptyCellsDown( border, xCell, yCell, cell, minDown + 1 );
                            
                            if( ( up_isValid ) && ( down_isValid ) ) {
                                vec2 P = e.zw + Vec2( -( margin.x + PLAYER_HALF_WIDTH ), -margin.y );
                                vec2 Q = e.zw;
                                rect R = rectMM( P, Q );
                                
                                Collision->lEdgeGrabLeft[ iEdge ] = R;
                            }
                        }
                    }
                }
            }
            
            { // Right
                UINT32_PAIR Type = Collision->Type[ 2 ];
                if( Type.n > 0 ) {
                    Collision->nLedgeGrabRight = Type.n;
                    Collision->lEdgeGrabRight  = _pushArray( &AppState->Collision_memory, rect, Type.n );
                    
                    vec4 * Edge = Collision->Edge + Type.m;
                    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                        vec4 e = Edge[ iEdge ];
                        
                        UINT32_PAIR cell = getCell( e.P, xMin, yMin );
                        if( ( cell.x < xCell ) && ( cell.y > 0 ) ) {
                            cell.y--;
                            
                            uint32 minUp = PLATFORM_MIN_TILE_Y_FOR_CEILING_ABOVE_VALID_LEDGE_GRAB;
                            boo32  up_isValid = hasNEmptyCellsUp( border, xCell, yCell, cell, minUp );
                            
                            uint32 minDown = PLATFORM_TILE_Y_FOR_FULL_JUMP_NO_LEDGE_GRAB;
                            uint32 down_isValid = hasNEmptyCellsDown( border, xCell, yCell, cell, minDown + 1 );
                            
                            if( ( up_isValid ) && ( down_isValid ) ) {
                                vec2 P = e.xy + Vec2( 0.0f, -margin.y );
                                vec2 Q = e.xy + Vec2( margin.x + PLAYER_HALF_WIDTH, 0.0f );
                                rect R = rectMM( P, Q );
                                
                                Collision->lEdgeGrabRight[ iEdge ] = R;
                            }
                        }
                    }
                }
            }
        }
#endif
        
        { // Jumper Edges
            EDITOR_STATE * Editor = &AppState->Editor;
            
            flo32 margin = TILE_WIDTH * 2.0f;
            
            UINT32_PAIR Type = Collision->Type[ 3 ];
            if( Type.n > 0 ) {
                uint32 maxJumperEdge = Type.n;
                uint32   nJumperEdge = 0;
                
                Collision->JumperEdge  = _pushArray( &AppState->Collision_memory, vec4, maxJumperEdge );
                
                vec4 * Edge = Collision->Edge + Type.m;
                for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                    vec4 e = Edge[ iEdge ];
                    
                    vec2 P = e.xy;
                    vec2 Q = e.zw;
                    
                    P.y += 0.1f;
                    Q.y += 0.1f;
                    
                    UINT32_PAIR cellA = getCell( P );
                    UINT32_PAIR cellB = getCell( Q );
                    cellB.x -= 1;
                    
                    boo32 isIntA = ( Editor->grid[ cellA.y ][ cellA.x ] == CellType_Interior );
                    boo32 isIntB = ( Editor->grid[ cellB.y ][ cellB.x ] == CellType_Interior );
                    if( ( isIntA ) && ( isIntB ) ) {
                        Collision->JumperEdge[ nJumperEdge++ ] = Vec4( P, Q );
                    }
                }
                
                Collision->nJumperEdge = nJumperEdge;
                
                _popArray( &AppState->Collision_memory, vec4, maxJumperEdge - nJumperEdge );
            }
            
            uint32 maxAddEdge = 128;
            vec4 *    addEdge = _pushArray( &AppState->Collision_memory, vec4, maxAddEdge );
            uint32   nAddEdge = 0;
            
            { // SECURE DOORS
                EDITOR__SECURE_DOOR_STATE * State = &Editor->Secure;
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    EDITOR__SECURE_DOOR Entity = State->Entity[ iEntity ];
                    rect R = Entity.Bound;
                    
                    for( uint32 iEdge = 0; iEdge < Collision->nJumperEdge; iEdge++ ) {
                        vec4 * e = Collision->JumperEdge + iEdge;
                        vec2 P = e->P;
                        vec2 Q = e->Q;
                        
                        if( ( P.y > R.Bottom ) && ( P.y < R.Top ) ) {
                            if( ( P.x <= R.Left ) && ( Q.x >= R.Right ) ) {
                                if( P.x == R.Left ) {
                                    e->P.x = R.Right;
                                } else if( Q.x == R.Right ) {
                                    e->Q.x = R.Left;
                                } else {
                                    vec2 A = P;
                                    vec2 B = Q;
                                    A.x = R.Right;
                                    
                                    e->Q.x = R.Left;
                                    
                                    Assert( nAddEdge < maxAddEdge );
                                    addEdge[ nAddEdge++ ] = Vec4( A, B );
                                    // TODO: Is this safe!?
                                    Collision->nJumperEdge++;
                                    // TODO: Is this safe!?
                                }
                            }
                        }
                    }
                }
            }
            
            { // MECH DOORS
                EDITOR__MECH_DOOR_STATE * State = &Editor->Mech;
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    EDITOR__MECH_DOOR Entity = State->Entity[ iEntity ];
                    rect R = Entity.Bound_Door;
                    
                    for( uint32 iEdge = 0; iEdge < Collision->nJumperEdge; iEdge++ ) {
                        vec4 * e = Collision->JumperEdge + iEdge;
                        vec2 P = e->P;
                        vec2 Q = e->Q;
                        
                        if( ( P.y > R.Bottom ) && ( P.y < R.Top ) ) {
                            if( ( P.x <= R.Left ) && ( Q.x >= R.Right ) ) {
                                if( P.x == R.Left ) {
                                    e->P.x = R.Right;
                                } else if( Q.x == R.Right ) {
                                    e->Q.x = R.Left;
                                } else {
                                    vec2 A = P;
                                    vec2 B = Q;
                                    A.x = R.Right;
                                    
                                    e->Q.x = R.Left;
                                    
                                    Assert( nAddEdge < maxAddEdge );
                                    addEdge[ nAddEdge++ ] = Vec4( A, B );
                                    // TODO: Is this safe!?
                                    Collision->nJumperEdge++;
                                    // TODO: Is this safe!?
                                }
                            }
                        }
                    }
                }
            }
            
            _popArray( &AppState->Collision_memory, vec4, maxAddEdge - nAddEdge );
        }
        
        { // Jumper Bounds
            if( Collision->nJumperEdge > 0 ) {
                Collision->nJumperBound = Collision->nJumperEdge;
                Collision->JumperBound  = _pushArray( &AppState->Collision_memory, rect, Collision->nJumperBound );
                
                vec4 * Edge = Collision->JumperEdge;
                for( uint32 iEdge = 0; iEdge < Collision->nJumperEdge; iEdge++ ) {
                    vec4 e = Edge[ iEdge ];
                    
                    vec2 P = e.xy + Vec2( 0.0f, -0.15f );
                    vec2 Q = e.zw + Vec2( 0.0f, -0.05f );
                    rect R = rectMM( P, Q );
                    
                    Collision->JumperBound[ iEdge ] = R;
                }
            }
        }
        
        { // gen Jumper Pathways
            if( Collision->nJumperEdge > 0 ) {
                vec4 * Edge  = Collision->JumperEdge;
                uint32 nEdge = Collision->nJumperEdge;
                
                uint32         maxConnect = 1024;
                uint32           nConnect = 0;
                PATH_CONNECTION * connect = _pushArray_clear( TempMemory, PATH_CONNECTION, maxConnect );
                
                flo32 margin = TILE_WIDTH * 3.0f;
                
                for( uint32 iEdgeA = 0; iEdgeA < ( nEdge - 1 ); iEdgeA++ ) {
                    vec4 eA = Edge[ iEdgeA ];
                    
                    for( uint32 iEdgeB = iEdgeA + 1; iEdgeB < nEdge; iEdgeB++ ) {
                        vec4 eB = Edge[ iEdgeB ];
                        
                        if( ( iEdgeA == 0 ) && ( iEdgeB == 19 ) ) {
                            uint32 breakHere = 1;
                        }
                        
                        uint32 iNode_up   = iEdgeA;
                        uint32 iNode_down = iEdgeB;
                        vec4 upper = eA;
                        vec4 lower = eB;
                        
                        if( lower.P.y > upper.P.y ) {
                            _swap( uint32, iNode_up, iNode_down );
                            _swap( vec4, upper, lower );
                        }
                        
                        boo32 doTest = true;
                        if( upper.P.x > lower.Q.x ) {
                            PATH_CONNECTION conn = doesConnect( Collision, iNode_up, iNode_down, upper.P, lower.Q );
                            if( ( conn.isValid_up ) || ( conn.isValid_down ) ) {
                                Assert( nConnect < maxConnect );
                                connect[ nConnect++ ] = conn;
                            }
                            doTest = false;
                        }
                        if( upper.Q.x < lower.P.x ) {
                            PATH_CONNECTION conn = doesConnect( Collision, iNode_up, iNode_down, upper.Q, lower.P );
                            if( ( conn.isValid_up ) || ( conn.isValid_down ) ) {
                                Assert( nConnect < maxConnect );
                                connect[ nConnect++ ] = conn;
                            }
                            doTest = false;
                        }
                        
                        if( doTest ) {
                            boo32 isValidP = true;
                            boo32 isValidQ = true;
                            
                            if( upper.P.x <= lower.P.x ) {
                                isValidP = false;
                            }
                            if( upper.Q.x >= lower.Q.x ) {
                                isValidQ = false;
                            }
                            
                            if( isValidP ) {
                                vec2 ExitP = upper.P;
                                vec2 NodeP = closestPE( ExitP, lower.P, lower.Q );
                                NodeP.x -= margin;
                                
                                PATH_CONNECTION conn = doesConnect( Collision, iNode_up, iNode_down, ExitP, NodeP );
                                if( ( conn.isValid_up ) || ( conn.isValid_down ) ) {
                                    Assert( nConnect < maxConnect );
                                    connect[ nConnect++ ] = conn;
                                }
                            }
                            if( isValidQ ) {
                                vec2 ExitP = upper.Q;
                                vec2 NodeP = closestPE( ExitP, lower.P, lower.Q );
                                NodeP.x += margin;
                                
                                PATH_CONNECTION conn = doesConnect( Collision, iNode_up, iNode_down, ExitP, NodeP );
                                if( ( conn.isValid_up ) || ( conn.isValid_down ) ) {
                                    Assert( nConnect < maxConnect );
                                    connect[ nConnect++ ] = conn;
                                }
                            }
                        }
                    }
                }
                
                MEMORY * memory = &AppState->Collision_memory;
                
                PATH_STATE Path = {};
                Path.nNodeLink = nEdge;
                Path.NodeLink  = _pushArray( memory, UINT32_PAIR, Path.nNodeLink );
                
                Path.nPathLink = nConnect * 2;
                Path.PathLink_ActiveData   = _pushArray( memory, PATH_LINK__ACTIVE_DATA,   Path.nPathLink );
                Path.PathLink_PositionData = _pushArray( memory, PATH_LINK__POSITION_DATA, Path.nPathLink );
                
                Path.NodeLinkIn      = _pushArray( memory, UINT32_PAIR,        Path.nNodeLink );
                Path.PathLink_InData = _pushArray( memory, PATH_LINK__IN_DATA, Path.nPathLink );
                
                uint32 atLink   = 0;
                EDITOR_STATE * Editor = &AppState->Editor;
                for( uint32 iEdge = 0; iEdge < nEdge; iEdge++ ) {
                    UINT32_PAIR NodeLink = {};
                    NodeLink.m = atLink;
                    
                    for( uint32 iConnect = 0; iConnect < nConnect; iConnect++ ) {
                        PATH_CONNECTION conn = connect[ iConnect ];
                        
                        if( ( conn.iNode_lower == iEdge ) && ( conn.isValid_up ) ) {
                            PATH_LINK__ACTIVE_DATA   A = {};
                            PATH_LINK__POSITION_DATA P = {};
                            A.iNode   = conn.iNode_upper;
                            P.toNodeP = conn.pos_upper;
                            P.ExitP   = conn.pos_lower;
                            
                            boo32 isValid = ( P.toNodeP.y == P.ExitP.y ) || ( !DoesIntersectDoors( Editor, P.toNodeP, P.ExitP ) );
                            if( isValid ) {
                                Path.PathLink_ActiveData  [ atLink ] = A;
                                Path.PathLink_PositionData[ atLink ] = P;
                                atLink++;
                                NodeLink.n++;
                            }
                        }
                        if( ( conn.iNode_upper == iEdge ) && ( conn.isValid_down ) ) {
                            PATH_LINK__ACTIVE_DATA   A = {};
                            PATH_LINK__POSITION_DATA P = {};
                            A.iNode   = conn.iNode_lower;
                            P.toNodeP = conn.pos_lower;
                            P.ExitP   = conn.pos_upper;
                            
                            boo32 isValid = ( P.toNodeP.y == P.ExitP.y ) || ( !DoesIntersectDoors( Editor, P.toNodeP, P.ExitP ) );
                            if( isValid ) {
                                Path.PathLink_ActiveData  [ atLink ] = A;
                                Path.PathLink_PositionData[ atLink ] = P;
                                atLink++;
                                NodeLink.n++;
                            }
                        }
                    }
                    
                    Path.NodeLink[ iEdge ] = NodeLink;
                }
                
                _popArray( TempMemory, PATH_CONNECTION, maxConnect );
                
                uint32 atLinkIn = 0;
                for( uint32 iNodeLinkA = 0; iNodeLinkA < Path.nNodeLink; iNodeLinkA++ ) {
                    UINT32_PAIR NodeLinkIn = {};
                    NodeLinkIn.m = atLinkIn;
                    
                    for( uint32 iNodeLinkB = 0; iNodeLinkB < Path.nNodeLink; iNodeLinkB++ ) {
                        UINT32_PAIR NodeLink = Path.NodeLink[ iNodeLinkB ];
                        
                        for( uint32 iPathLink = 0; iPathLink < NodeLink.n; iPathLink++ ) {
                            PATH_LINK__ACTIVE_DATA Active = Path.PathLink_ActiveData[ NodeLink.m + iPathLink ];
                            if( Active.iNode == iNodeLinkA ) {
                                PATH_LINK__IN_DATA InData = {};
                                InData.iNode     = iNodeLinkB;
                                InData.iPathLink = iPathLink;
                                
                                Path.PathLink_InData[ atLinkIn++ ] = InData;
                            }
                        }
                    }
                    
                    NodeLinkIn.n = atLinkIn - NodeLinkIn.m;
                    Path.NodeLinkIn[ iNodeLinkA ] = NodeLinkIn;
                }
                Assert( atLinkIn == atLink );
                
                for( uint32 iPathLink = 0; iPathLink < Path.nPathLink; iPathLink++ ) {
                    Path.PathLink_ActiveData[ iPathLink ].IsActive = true;
                }
                
                { // match doors with Path links
                    { // MECH DOOR
                        EDITOR__MECH_DOOR_STATE * State = &Editor->Mech;
                        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                            EDITOR__MECH_DOOR * Entity = State->Entity + iEntity;
                            Entity->iJumperPathLink = getPathLinkOfBound( Path, Entity->Bound_Door );
                            
                            if( Entity->isClosed ) {
                                Path.PathLink_ActiveData[ Entity->iJumperPathLink.m ].IsActive = false;
                                Path.PathLink_ActiveData[ Entity->iJumperPathLink.n ].IsActive = false;
                            }
                        }
                    }
                    
                    { // SECURE DOOR
                        EDITOR__SECURE_DOOR_STATE * State = &Editor->Secure;
                        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                            EDITOR__SECURE_DOOR * Entity = State->Entity + iEntity;
                            Entity->iJumperPathLink = getPathLinkOfBound( Path, Entity->Bound );
                            
                            if( Entity->isClosed ) {
                                Path.PathLink_ActiveData[ Entity->iJumperPathLink.m ].IsActive = false;
                                Path.PathLink_ActiveData[ Entity->iJumperPathLink.n ].IsActive = false;
                            }
                        }
                    }
                }
                Collision->JumperPath = Path;
            }
        }
    }
    
    _popArray( TempMemory, vec4, maxEdge * 4 );
    _popArray( TempMemory, uint8, nCell );
}

internal void
DrawTerrain( RENDER_PASS * Pass, APP_STATE * AppState ) {
    TERRAIN_STATE * State = &AppState->terrain;
    
    for( uint32 iTerrain = 0; iTerrain < State->nTerrain; iTerrain++ ) {
        TERRAIN terrain = State->terrain[ iTerrain ];
        
        DrawRect       ( Pass, terrain.Bound, TERRAIN_COLOR );
        DrawRectOutline( Pass, terrain.Bound, TERRAIN_OUTLINE_COLOR );
    }
}

#define VERSION__CELL_GRID  ( 1 )
#define FILETAG__CELL_GRID  ( "CELLGRID" )
internal void
SaveCellGrid( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    UINT32_QUAD Bound = GetGridBound( Editor );
    uint32 xCell = ( Bound.max.x - Bound.min.x ) + 1;
    uint32 yCell = ( Bound.max.y - Bound.min.y ) + 1;
    uint32 nCell = xCell * yCell;
    if( nCell > 1 ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, nCell );
        _writem( output, UINT32_QUAD, Bound );
        for( uint32 iY = Bound.min.y; iY <= Bound.max.y; iY++ ) {
            for( uint32 iX = Bound.min.x; iX <= Bound.max.x; iX++ ) {
                uint8 Cell = Editor->grid[ iY ][ iX ];
                _writem( output, uint8, Cell );
            }
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

internal void
GAME_LoadCellGrid( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        switch( version ) {
            case 1: {
                COLLISION_STATE * Collision = &AppState->Collision;
                
                Collision->Grid_nCell = _read( ptr, uint32 );
                Collision->Grid_Bound = _read( ptr, UINT32_QUAD );
                Collision->Grid       = _copya( &AppState->Collision_memory, ptr, uint8, Collision->Grid_nCell );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, file.size );
    }
}