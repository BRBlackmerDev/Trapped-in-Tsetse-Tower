
internal void
initDebugPanel( APP_STATE * AppState ) {
    PANEL * panel = &AppState->debug;
    
    {
        rect R = AppState->App_Bound;
        R.Left = R.Right - 320.0f;
        
        panel->Bound = R;
    }
}

#if 0
internal void
DEBUG_addButton( PANEL * panel, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, DRAW_STATE * Draw, char * label, RENDER_FUNC * func ) {
    vec2 button_dim   = Vec2( 44.0f, 28.0f );
    vec2 inner_margin = Vec2( 4.0f, 4.0f );
    
    rect Bound = rectTLD( panel->atPos, button_dim );
    
    panel->atPos.x += ( button_dim.x + inner_margin.x );
    if( ( panel->atPos.x + button_dim.x + inner_margin.x ) >= panel->inner.Right ) {
        panel->atPos.x  = panel->inner.Left;
        panel->atPos.y -= ( button_dim.y + inner_margin.y );
    }
    
    if( ( wasPressed( Mouse, MouseButton_Left ) ) && ( IsInBound( Mouse->Position, Bound ) ) ) {
        panel->isEnabled[ panel->nButton ] = !panel->isEnabled[ panel->nButton ];
    }
    
    if( panel->isEnabled[ panel->nButton ] ) {
        func( Pass_Game, Draw );
    }
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( panel->isEnabled[ panel->nButton ] ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    }
    DrawRect( Pass_UI, Bound, Color );
    DrawRectOutline( Pass_UI, Bound, COLOR_BLACK );
    DrawString( Pass_UI, Draw->font, label, getCenter( Bound ), textAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    panel->nButton++;
}
#endif

internal void
UpdateAndDrawDebugPanel( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, DRAW_STATE * Draw ) {
    PANEL * panel = &AppState->debug;
    if( panel->show ) {
        panel->inner = addRadius( panel->Bound, -4.0f );
        panel->atPos = getTL( panel->inner );
        panel->advanceY = 0.0f;
        
        DrawRect( Pass_UI, panel->Bound, COLOR_WHITE );
        
        addLabel( panel, Pass_UI, Draw->font, "TERRAIN", PANEL_LABEL_Y );
        wrapPanel( panel );
        
        boo32 * IsActive = AppState->Debug_IsActive;
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "COLL", DrawCollisionEdges, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "JUMP", DrawJumpBound, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "WLJP", DrawWallSlideBound, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "LDGE", DrawLedgeGrabBound, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "EXIT", DrawExitDebug, IsActive++ );
        wrapPanel( panel );
        
        addLabel( panel, Pass_UI, Draw->font, "JUMPER", PANEL_LABEL_Y );
        
        addS32Counter( panel, Mouse, Pass_UI, Draw->font, "iEntity:", Vec2( 120.0f, 16.0f ), &AppState->Debug_iEntity, -1, Draw->Jumper->nEntity );
        
        wrapPanel( panel );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "JBND", DrawJumperBound, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "JEDG", DrawJumperEdge, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "JPTH", DrawJumperPath, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "PATH", DrawJumperPathLinks, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "HP",   DrawJumperHP, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "ATTK", DrawJumperAttack, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "FLNK", DrawJumperFlank, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "STAT", debugJumperStats, IsActive++ );
        
        wrapPanel( panel );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "GRND", DrawGrenadeDebug, IsActive++ );
        
        wrapPanel( panel );
        addLabel( panel, Pass_UI, Draw->font, "BEAST", PANEL_LABEL_Y );
        wrapPanel( panel );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "CELL", DrawBeastWalkableCells, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "PATH", DrawBeastPath, IsActive++ );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "SEE",  DrawBeastCanSeePlayer, IsActive++ );
        wrapPanel( panel );
        addDebugButton( panel, Mouse, Pass_Game, Pass_UI, Draw, "PANC",  DrawBeastPanicPosts, IsActive++ );
        
        
        
        Assert( ( IsActive - AppState->Debug_IsActive ) <= DEBUG_MAX_COUNT );
    }
}