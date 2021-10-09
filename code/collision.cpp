
internal void
DrawCollisionEdges( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = Draw->Collision;
    
    for( uint32 iEdge = 0; iEdge < Collision->nEdge; iEdge++ ) {
        vec4 Edge = Collision->Edge[ iEdge ];
        DrawLine( Pass, Edge.xy, Edge.zw, COLOR_YELLOW );
    }
}

#if 0
internal void
DrawInteriorVertex( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = Draw->Collision;
    
    for( uint32 iVertex = 0; iVertex < Collision->nInteriorVertex; iVertex++ ) {
        vec2 P = Collision->InteriorVertex[ iVertex ];
        DrawPoint( Pass, P, TILE_DIM * 0.5f, COLOR_RED );
        
        char str[ 16 ] = {};
        sprintf( str, "%u", iVertex );
        DrawString( Pass, Draw->font, str, P, textAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
    }
    
#if 0
    for( uint32 iEdge = 0; iEdge < Collision->nInteriorEdge; iEdge++ ) {
        UINT32_PAIR Edge = Collision->InteriorEdge[ iEdge ];
        
        vec2 P = Collision->InteriorVertex[ Edge.m ];
        vec2 Q = Collision->InteriorVertex[ Edge.n ];
        
        vec2 N = GetNormal( GetPerp( Q - P ) ) * ( TILE_WIDTH * 0.25f );
        P += N;
        Q += N;
        
        DrawLine( Pass, P, Q, COLOR_MAGENTA );
    }
#endif
}
#endif

#if 0
internal void
DrawCircumcircle( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = Draw->Collision;
    
    for( uint32 iCircle = 0; iCircle < Collision->nCircumcircle; iCircle++ ) {
        circ C = Collision->Circumcircle[ iCircle ];
        DrawCircleOutline( Pass, C.Center, C.Radius, COLOR_MAGENTA );
    }
}
#endif

#if 0
internal void
DrawNavMesh( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = Draw->Collision;
    
    flo32 Offset = TILE_WIDTH * 0.25f;
    for( uint32 iTri = 0; iTri < Collision->nTri; iTri++ ) {
        UINT32_TRI Tri = Collision->Tri[ iTri ];
        
        vec2 A = Collision->InteriorVertex[ Tri.x ];
        vec2 B = Collision->InteriorVertex[ Tri.y ];
        vec2 C = Collision->InteriorVertex[ Tri.z ];
        
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
        
        DrawLine( Pass, A0, B0, COLOR_CYAN );
        DrawLine( Pass, B0, C0, COLOR_RED );
        DrawLine( Pass, C0, A0, COLOR_GREEN );
    }
}
#endif

internal void
DrawJumpBound( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = Draw->Collision;
    
    for( uint32 iBound = 0; iBound < Collision->nJumpBound; iBound++ ) {
        rect R = Collision->jumpBound[ iBound ];
        DrawRectOutline( Pass, R, COLOR_RED );
    }
}

internal void
DrawWallSlideBound( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = Draw->Collision;
    
    for( uint32 iBound = 0; iBound < Collision->nWallSlideLeft; iBound++ ) {
        rect R = Collision->wallSlideLeft[ iBound ];
        DrawRectOutline( Pass, R, COLOR_BLUE );
    }
    
    for( uint32 iBound = 0; iBound < Collision->nWallSlideRight; iBound++ ) {
        rect R = Collision->wallSlideRight[ iBound ];
        DrawRectOutline( Pass, R, COLOR_BLUE );
    }
}

internal void
DrawLedgeGrabBound( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = Draw->Collision;
    
    for( uint32 iBound = 0; iBound < Collision->nLedgeGrabLeft; iBound++ ) {
        rect R = Collision->lEdgeGrabLeft[ iBound ];
        DrawRectOutline( Pass, R, COLOR_MAGENTA );
    }
    
    for( uint32 iBound = 0; iBound < Collision->nLedgeGrabRight; iBound++ ) {
        rect R = Collision->lEdgeGrabRight[ iBound ];
        DrawRectOutline( Pass, R, COLOR_MAGENTA );
    }
}

internal void
DrawJumperPathLinks( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    PATH_STATE * Path = &Draw->Collision->JumperPath;
    
    for( uint32 iLink = 0; iLink < Path->nPathLink; iLink++ ) {
        PATH_LINK__ACTIVE_DATA   A = Path->PathLink_ActiveData  [ iLink ];
        PATH_LINK__POSITION_DATA P = Path->PathLink_PositionData[ iLink ];
        
        vec4 Color = COLOR_GRAY( 0.5f );
        if( A.IsActive ) {
            Color = COLOR_MAGENTA;
        }
        
        vec2 N  = GetNormal( P.toNodeP - P.ExitP );
        vec2 VA = GetPerp( N ) * ( TILE_WIDTH * 0.25f );
        vec2 VB = VA * 4.0f;
        
        DrawLine( Pass, P.ExitP + VA, P.toNodeP + VA, Color );
        DrawLine( Pass, P.ExitP + VA, P.ExitP   + VB, Color );
    }
}