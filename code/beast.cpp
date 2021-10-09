
internal flo32
DistFromPointToSegment( vec2 P, vec2 SegmentA, vec2 SegmentB ) {
    vec2 AP = P - SegmentA;
    vec2 AB = SegmentB - SegmentA;
    
    flo32 t = clamp01( dot( AP, AB ) / dot( AB, AB ) );
    vec2  Q = SegmentA + t * ( AB );
    
    // TODO: This function can probably be improved.
    flo32 Result = GetLength( Q - P );
    return Result;
}

internal vec2
GetWorldP( uint32 iCell, UINT32_QUAD Grid_Bound ) {
    UINT32_QUAD Bound = Grid_Bound;
    
    uint32 xCell = ( Bound.max.x - Bound.min.x ) + 1;
    uint32 yCell = ( Bound.max.y - Bound.min.y ) + 1;
    uint32 nCell = xCell * yCell;
    Assert( nCell > 1 );
    Assert( iCell < nCell );
    
    flo32 x = ( flo32 )( ( iCell % xCell ) + Bound.min.x ) * TILE_WIDTH  + ( TILE_WIDTH  * 0.5f );
    flo32 y = ( flo32 )( ( iCell / xCell ) + Bound.min.y ) * TILE_HEIGHT + ( TILE_HEIGHT * 0.5f );
    vec2 Result = Vec2( x, y );
    return Result;
}

//#pragma optimize( "", on )
internal void
InitBeastWalkableGrid( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    COLLISION_STATE * Collision = &AppState->Collision;
    uint8 *     Grid       = Collision->Grid;
    UINT32_QUAD Grid_Bound = Collision->Grid_Bound;
    
    for( uint32 iCell = 0; iCell < Collision->Grid_nCell; iCell++ ) {
        if( ( Grid[ iCell ] == CellType_Interior_Blocked ) || ( Grid[ iCell ] == CellType_Interior_Goal ) ) {
            Grid[ iCell ] = CellType_Interior;
        }
    }
    
    uint32 xCell = ( Grid_Bound.max.x - Grid_Bound.min.x ) + 1;
    uint32 yCell = ( Grid_Bound.max.y - Grid_Bound.min.y ) + 1;
    uint32 nCell = xCell * yCell;
    Assert( nCell > 1 );
    
    // TODO: This function can still be optimized! For example, we know that all cells in the rect created by the collision edge do not need to be checked for distance. They can automatically be set to CellType_Interior_Blocked.
    
    { // COLLISION EDGES
        for( uint32 iEdge = 0; iEdge < Collision->nEdge; iEdge++ ) {
            vec4 Edge = Collision->Edge[ iEdge ];
            
            vec2 U = Edge.Q - Edge.P;
            vec2 V = GetNormal( GetPerp( U ) ) * BEAST_WALKABLE_RADIUS;
            rect R = rectExtrema( Edge.P, Edge.P + U + V + GetNormal( U ) * BEAST_WALKABLE_RADIUS );
            
            UINT32_QUAD Bound = GetCellBound( R );
            if( ( Bound.min.x > Grid_Bound.max.x ) || ( Bound.min.y > Grid_Bound.max.y ) || ( Bound.max.x < Grid_Bound.min.x ) || ( Bound.max.y < Grid_Bound.min.y ) ) {
                // DO NOTHING!
            } else {
                UINT32_QUAD B = {};
                B.min.x = maxValue( Bound.min.x, Grid_Bound.min.x ) - Grid_Bound.min.x;
                B.min.y = maxValue( Bound.min.y, Grid_Bound.min.y ) - Grid_Bound.min.y;
                B.max.x = minValue( Bound.max.x, Grid_Bound.max.x ) - Grid_Bound.min.x;
                B.max.y = minValue( Bound.max.y, Grid_Bound.max.y ) - Grid_Bound.min.y;
                
                for( uint32 Y = B.min.y; Y <= B.max.y; Y++ ) {
                    for( uint32 X = B.min.x; X <= B.max.x; X++ ) {
                        uint32 iCell = Y * xCell + X;
                        vec2   P     = GetWorldP( iCell, Grid_Bound );
                        flo32  Dist  = DistFromPointToSegment( P, Edge.P, Edge.Q );
                        if( Dist < BEAST_WALKABLE_RADIUS ) {
                            Grid[ iCell ] = CellType_Interior_Blocked;
                        }
                    }
                }
            }
        }
    }
    
#if 0    
    for( uint32 iCell = 0; iCell < Collision->Grid_nCell; iCell++ ) {
        vec2 P = GetWorldP( iCell, Collision->Grid_Bound );
        if( ( Grid[ iCell ] == CellType_Interior_Blocked ) || ( Grid[ iCell ] == CellType_Interior_Goal ) ) {
            Grid[ iCell ] = CellType_Interior;
        }
        
        if( Grid[ iCell ] == CellType_Interior ) {
            for( uint32 iEdge = 0; iEdge < Collision->nEdge; iEdge++ ) {
                // TODO: This can be improved a lot! For example, we know that all segments are either vertical or horizontal. We don't need a DistFromPointToSegment() check.
                
                vec4  Edge = Collision->Edge[ iEdge ];
                flo32 Dist = DistFromPointToSegment( P, Edge.P, Edge.Q );
                if( Dist <= BEAST_WALKABLE_RADIUS ) {
                    Grid[ iCell ] = CellType_Interior_Blocked;
                }
            }
        }
        
        if( Grid[ iCell ] == CellType_Interior ) {
            flo32 DistToPlayer = GetLength( P - Player->Position );
            if( DistToPlayer <= BEAST_WALKABLE_RADIUS ) {
                Grid[ iCell ] = CellType_Interior_Goal;
            }
        }
    }
#endif
    
}
//#pragma optimize( "", off )

internal void
AddPlayerPositionToBeastWalkableGrid( APP_STATE * AppState, vec2 Position ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    uint8 *     Grid       = Collision->Grid;
    UINT32_QUAD Grid_Bound = Collision->Grid_Bound;
    
    uint32 xCell = ( Grid_Bound.max.x - Grid_Bound.min.x ) + 1;
    uint32 yCell = ( Grid_Bound.max.y - Grid_Bound.min.y ) + 1;
    uint32 nCell = xCell * yCell;
    Assert( nCell > 1 );
    
    rect R = rectCHD( Position, Vec2( BEAST_WALKABLE_RADIUS, BEAST_WALKABLE_RADIUS ) );
    
    UINT32_QUAD Bound = GetCellBound( R );
    if( ( Bound.min.x > Grid_Bound.max.x ) || ( Bound.min.y > Grid_Bound.max.y ) || ( Bound.max.x < Grid_Bound.min.x ) || ( Bound.max.y < Grid_Bound.min.y ) ) {
        // DO NOTHING!
    } else {
        UINT32_QUAD B = {};
        B.min.x = maxValue( Bound.min.x, Grid_Bound.min.x ) - Grid_Bound.min.x;
        B.min.y = maxValue( Bound.min.y, Grid_Bound.min.y ) - Grid_Bound.min.y;
        B.max.x = minValue( Bound.max.x, Grid_Bound.max.x ) - Grid_Bound.min.x;
        B.max.y = minValue( Bound.max.y, Grid_Bound.max.y ) - Grid_Bound.min.y;
        
        for( uint32 Y = B.min.y; Y <= B.max.y; Y++ ) {
            for( uint32 X = B.min.x; X <= B.max.x; X++ ) {
                uint32 iCell = Y * xCell + X;
                if( Grid[ iCell ] == CellType_Interior ) {
                    vec2   P     = GetWorldP( iCell, Grid_Bound );
                    flo32  Dist  = GetLength( P - Position );
                    if( Dist < BEAST_WALKABLE_RADIUS ) {
                        Grid[ iCell ] = CellType_Interior_Goal;
                    }
                }
            }
        }
    }
}

internal void
FindGridPath( APP_STATE * AppState, MEMORY * TempMemory, BEAST * Beast ) {
    UINT32_PAIR Beast_Cell  = GetGridCell( AppState, Beast->Position  );
    DISPLAY_VALUE( UINT32_PAIR, Beast_Cell );
    
    COLLISION_STATE * Collision = &AppState->Collision;
    UINT32_QUAD Bound = Collision->Grid_Bound;
    uint32 xCell = ( Bound.max.x - Bound.min.x ) + 1;
    uint32 yCell = ( Bound.max.y - Bound.min.y ) + 1;
    uint32 nCell = xCell * yCell;
    Assert( nCell > 1 );
    
    DISPLAY_VALUE( uint32, xCell );
    DISPLAY_VALUE( uint32, yCell );
    
    flo32  * Score      = _pushArray_clear( TempMemory, flo32,  nCell );
    boo32  * WasVisited = _pushArray_clear( TempMemory, boo32,  nCell );
    boo32  * IsInQueue  = _pushArray_clear( TempMemory, boo32,  nCell );
    uint32 * FromCell   = _pushArray_clear( TempMemory, uint32, nCell );
    
    { // Init Grid
        uint8 * Grid = Collision->Grid;
        for( uint32 iCell = 0; iCell < nCell; iCell++ ) {
            Score[ iCell ] = FLT_MAX;
            if( ( Grid[ iCell ] != CellType_Interior ) && ( Grid[ iCell ] != CellType_Interior_Goal ) ) {
                WasVisited[ iCell ] = true;
            }
        }
    }
    
    uint8 * Grid = Collision->Grid;
    
    uint32 * BufferA = _pushArray_clear( TempMemory, uint32, nCell );
    uint32 * BufferB = _pushArray_clear( TempMemory, uint32, nCell );
    uint32 * Read  = BufferA;
    uint32 * Write = BufferB;
    uint32  nRead  = 0;
    uint32  nWrite = 0;
    
    boo32  WasFound = false;
    uint32 InitCell = Beast_Cell.y  * xCell + Beast_Cell.x;
    uint32 DestCell = 0;
    
    Score    [ InitCell ] = 0.0f;
    IsInQueue[ InitCell ] = true;
    Read[ nRead++ ] = InitCell;
    
    INT32_PAIR Offset[ 8 ] = {
        { -1,  1 },  {  0,  1 },  {  1,  1 },  {  1,  0 },
        {  1, -1 },  {  0, -1 },  { -1, -1 },  { -1,  0 },
    };
    
    flo32 c  = 1.0f;
    flo32 cA = sqrtf( 2.0f );
    flo32 Cost[ 8 ] = { cA, c, cA, c, cA, c, cA, c };
    
    boo32 DoSearch = true;
    while( DoSearch ) {
        nWrite = 0;
        
        for( uint32 iRead = 0; iRead < nRead; iRead++ ) {
            uint32 iCell = Read[ iRead ];
            
            IsInQueue [ iCell ] = false;
            WasVisited[ iCell ] = true;
            
            flo32 iScore = Score[ iCell ];
            
            if( Grid[ iCell ] == CellType_Interior_Goal ) {
                WasFound = true;
                DestCell = iCell;
            }
            
            int32 x = ( int32 )( iCell % xCell );
            int32 y = ( int32 )( iCell / xCell );
            
            for( uint32 iNeighbor = 0; iNeighbor < 8; iNeighbor++ ) {
                INT32_PAIR Offset0 = Offset[ iNeighbor ];
                int32 x0 = x + Offset0.x;
                int32 y0 = y + Offset0.y;
                if( ( x0 > -1 ) && ( y0 > -1 ) && ( x0 < ( int32 )xCell ) && ( y0 < ( int32 )yCell ) ) {
                    uint32 Cell = y0 * xCell + x0;
                    if( !WasVisited[ Cell ] ) {
                        flo32 iScore0 = iScore + Cost[ iNeighbor ];
                        if( iScore0 < Score[ Cell ] ) {
                            Score[ Cell ]    = iScore0;
                            FromCell[ Cell ] = iCell;
                        }
                        if( !IsInQueue[ Cell ] ) {
                            Write[ nWrite++ ] = Cell;
                            IsInQueue[ Cell ] = true;
                        }
                    }
                }
            }
        }
        
        if( nWrite > 0 ) {
            _swap( uint32 *, Read, Write );
            nRead = nWrite;
        } else {
            DoSearch = false;
        }
        
        if( WasFound ) {
            DoSearch = false;
        }
    }
    
    if( WasFound ) {
        Beast->PathIsValid = true;
        Beast->nPath = 0;
        
        uint32 atCell = DestCell;
        while( atCell != InitCell ) {
            flo32 x = ( flo32 )( ( atCell % xCell ) + Bound.min.x ) * TILE_WIDTH  + ( TILE_WIDTH  * 0.5f );
            flo32 y = ( flo32 )( ( atCell / xCell ) + Bound.min.y ) * TILE_HEIGHT + ( TILE_HEIGHT * 0.5f );
            Beast->Path[ Beast->nPath++ ] = Vec2( x, y );
            atCell = FromCell[ atCell ];
        }
        
        flo32 x = ( flo32 )( ( atCell % xCell ) + Bound.min.x ) * TILE_WIDTH  + ( TILE_WIDTH  * 0.5f );
        flo32 y = ( flo32 )( ( atCell / xCell ) + Bound.min.y ) * TILE_HEIGHT + ( TILE_HEIGHT * 0.5f );
        Beast->Path[ Beast->nPath++ ] = Vec2( x, y );
    } else {
        Beast->PathIsValid = false;
    }
    
    uint32 BreakHere = 10;
    
    _popArray( TempMemory, uint32, nCell );
    _popArray( TempMemory, uint32, nCell );
    
    _popArray( TempMemory, flo32,  nCell );
    _popArray( TempMemory, boo32,  nCell );
    _popArray( TempMemory, boo32,  nCell );
    _popArray( TempMemory, uint32, nCell );
}

internal void
CanBeastSeePlayer( APP_STATE * AppState, BEAST * Entity, PLAYER_STATE * Player ) {
    vec2 BeastP  = Entity->Position;
    vec2 PlayerP = Player->Position;
    
    boo32 CanSeePlayer = true;
    
    COLLISION_STATE * Collision = &AppState->Collision;
    for( uint32 iEdge = 0; ( CanSeePlayer ) && ( iEdge < Collision->nEdge ); iEdge++ ) {
        vec4 Edge = Collision->Edge[ iEdge ];
        
        boo32 DoIntersect = DoIntersectEE( BeastP, PlayerP, Edge.P, Edge.Q );
        CanSeePlayer = !DoIntersect;
    }
    
    // TODO: Intersect against doors and other geometry!
    
    Entity->CanSeePlayer = CanSeePlayer;
}

internal void
ProfileStart( APP_STATE * AppState, char * Label ) {
    PROFILE_STATE * State = &AppState->Profile;
    
    Assert( !State->ProfilingIsActive );
    State->ProfilingIsActive = true;
    
    Assert( State->nProfileBlock < PROFILE_BLOCK_MAX_COUNT );
    PROFILE_BLOCK * ProfileBlock = State->ProfileBlock + State->nProfileBlock++;
    
    strcpy( ProfileBlock->Label, Label );
    ProfileBlock->CycleCounter_Start = __rdtsc();
}

internal void
ProfileEnd( APP_STATE * AppState ) {
    PROFILE_STATE * State = &AppState->Profile;
    
    Assert( State->ProfilingIsActive );
    State->ProfilingIsActive = false;
    
    PROFILE_BLOCK * ProfileBlock = State->ProfileBlock + ( State->nProfileBlock - 1 );
    ProfileBlock->CycleCounter_End = __rdtsc();
}

internal void
UpdateBeast( APP_STATE * AppState, MEMORY * TempMemory, flo32 dt ) {
    BEAST_STATE  * State  = &AppState->Beast;
    BEAST        * Entity = &State->Entity;
    PLAYER_STATE * Player = &AppState->Player;
    
    ProfileStart( AppState, "CanBeastSeePlayer" );
    CanBeastSeePlayer( AppState, Entity, Player );
    ProfileEnd( AppState );
    
    if( Entity->CanSeePlayer ) {
        Entity->DoesKnowPlayerP  = true;
        Entity->LastKnownPlayerP = Player->Position;
    }
    
    ProfileStart( AppState, "InitBeastWalkableGrid" );
    InitBeastWalkableGrid( AppState );
    ProfileEnd( AppState );
    
    if( Entity->DoesKnowPlayerP ) {
        AddPlayerPositionToBeastWalkableGrid( AppState, Entity->LastKnownPlayerP );
    } else {
        // add patrol post
    }
    
    ProfileStart( AppState, "FindGridPath" );
    FindGridPath( AppState, TempMemory, Entity );
    ProfileEnd( AppState );
    
    // CHASE BEHAVIOR
    if( ( Entity->PathIsValid ) && ( Entity->nPath > 1 ) ) {
        uint32 iPath = 0;
        if( ( BEAST_FOV_LOOKAHEAD_TILE_MAX_COUNT + 1 ) <= Entity->nPath ) {
            iPath = Entity->nPath - ( BEAST_FOV_LOOKAHEAD_TILE_MAX_COUNT + 1 );
        }
        vec2  P       = Entity->Path[ iPath ];
        flo32 Radians = ToRadians( P - Entity->Position );
        
        Entity->FacingRadians = Radians;
        
        // TODO: Temporary! This will change with path smoothing and searching behavior!
    }
    // else : look at DestP
    
    flo32 MaxSpeed = BEAST_WALK_SPEED;
    flo32 Friction = BEAST_WALK_FRICTION;
    
    MaxSpeed = BEAST_RUN_SPEED;
    Friction = BEAST_RUN_FRICTION;
    
    vec2 Accel = -Entity->Velocity * Friction;
    
#if 1   
    if( ( Entity->PathIsValid ) && ( Entity->nPath > 1 ) ) {
        // NOTE: The Beast' current cell is the last position stored in the path, so we grab the second to last for the next destination.
        
        vec2 P   = Entity->Path[ Entity->nPath - 2 ];
        vec2 Dir = GetNormal( P - Entity->Position );
        Accel = ( Dir * MaxSpeed - Entity->Velocity ) * Friction;
    }
#endif
    
    Entity->dPos      = Accel * ( dt * dt * 0.5f ) + Entity->Velocity * dt;
    Entity->Velocity += Accel * dt;
    
    COLLISION_RESULT Coll = UpdatePlayerCollisionT( AppState, Entity->Position, Entity->Velocity, Entity->dPos, BEAST_RADIUS );
    // TODO: Temporary! I'm currently using this function because it runs rounded rectangle collision! I will probably need to change it later if I find that player and beast need to collide with different things.
    
    Entity->Position = Coll.Position;
    Entity->Velocity = Coll.Velocity;
    
}

internal void
DrawBeast( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    BEAST_STATE * State  = Draw->Beast;
    BEAST         Entity = State->Entity;
    
    { // FACING DIRECTION
        vec2 N      = ToDirection2D( Entity.FacingRadians );
        vec2 Offset = GetPerp( N ) * BEAST_FOV_OFFSET_DIST;
        
        vec2 VA = ToDirection2D( Entity.FacingRadians + BEAST_FOV_HALF_ANGLE );
        vec2 VB = ToDirection2D( Entity.FacingRadians - BEAST_FOV_HALF_ANGLE );
        vec2 PA = Entity.Position + Offset;
        vec2 PB = Entity.Position - Offset;
        
        vec4 Color = COLOR_RED;
        if( Entity.CanSeePlayer ) {
            Color = COLOR_GREEN;
        }
        
        DrawLine( Pass, PA, PA + VA * ( TILE_WIDTH * 50.0f ), Color );
        DrawLine( Pass, PB, PB + VB * ( TILE_WIDTH * 50.0f ), Color );
    }
    
    vec4 Color = COLOR_GRAY( 0.2f );
    
    DrawCircle( Pass, Entity.Position, BEAST_RADIUS, Color );
}

internal void
DrawBeastPath( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    BEAST_STATE * State  = Draw->Beast;
    BEAST         Entity = State->Entity;
    
    if( Entity.PathIsValid ) {
        COLLISION_STATE * Collision = Draw->Collision;
        UINT32_QUAD Bound = Collision->Grid_Bound;
        uint32      xCell = ( Bound.max.x - Bound.min.x ) + 1;
        uint32      yCell = ( Bound.max.y - Bound.min.y ) + 1;
        
        for( uint32 iCell = 0; iCell < ( Entity.nPath - 1 ); iCell++ ) {
            vec2 P = Entity.Path[ iCell     ];
            vec2 Q = Entity.Path[ iCell + 1 ];
            
            DrawLine( Pass, P, Q, COLOR_CYAN );
        }
    }
}

internal void
DrawBeastWalkableCells( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = Draw->Collision;
    
    for( uint32 iCell = 0; iCell < Collision->Grid_nCell; iCell++ ) {
        if( Collision->Grid[ iCell ] == CellType_Interior ) {
            vec2 P = GetWorldP( iCell, Collision->Grid_Bound );
            DrawPoint( Pass, P, TILE_DIM * 0.25f, COLOR_MAGENTA );
        }
        if( Collision->Grid[ iCell ] == CellType_Interior_Goal ) {
            vec2 P = GetWorldP( iCell, Collision->Grid_Bound );
            DrawPoint( Pass, P, TILE_DIM * 0.25f, COLOR_YELLOW );
        }
    }
}

internal void
DrawBeastCanSeePlayer( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    BEAST_STATE *  State  = Draw->Beast;
    BEAST          Entity = State->Entity;
    PLAYER_STATE * Player = Draw->Player;
    
    vec2 BeastP  = Entity.Position;
    vec2 PlayerP = Player->Position;
    
    vec4 Color = COLOR_RED;
    if( Entity.CanSeePlayer ) {
        Color = COLOR_GREEN;
    }
    
    DrawLine( Pass, BeastP, PlayerP, Color );
}

internal void
DrawBeastPanicPosts( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    PANIC_POST_STATE * PanicPost = Draw->PanicPost;
    
    for( uint32 iPanicPost = 0; iPanicPost < PanicPost->nPanicPost; iPanicPost++ ) {
        PANIC_POST Post = PanicPost->PanicPost[ iPanicPost ];
        
        DrawCircleOutline( Pass, Post.Position, PANIC_POST_RADIUS, COLOR_CYAN );
    }
}

internal void
FinalizeBeast( APP_STATE * AppState ) {
    
}