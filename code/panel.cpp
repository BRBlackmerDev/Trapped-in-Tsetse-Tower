

internal void
wrapPanel( PANEL * panel, flo32 addY = 0.0f ) {
    flo32 inner_margin = 4.0f;
    
    panel->atPos.x  = panel->inner.Left;
    panel->atPos.y -= ( panel->advanceY + inner_margin + addY );
    
    panel->advanceY = 0.0f;
}

internal rect
getBound( PANEL * panel, vec2 dim ) {
    vec2 inner_margin = Vec2( 4.0f, 4.0f );
    vec2 advance      = dim + inner_margin;
    
    if( ( panel->atPos.x + advance.x ) >= panel->inner.Right ) {
        wrapPanel( panel );
    }
    panel->advanceY = maxValue( panel->advanceY, dim.y );
    
    rect result = rectTLD( panel->atPos, dim );
    panel->atPos.x += advance.x;
    
    return result;
}

internal void
addLabel( PANEL * panel, RENDER_PASS * Pass, FONT * font, char * label, flo32 advanceY ) {
    vec2 dim   = Vec2( getWidth( font, label ), advanceY );
    rect Bound = getBound( panel, dim );
    
    flo32 margin = 8.0f;
    
    DrawString( Pass, font, label, getTL( Bound ) + Vec2( margin, 0.0f ), textAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    panel->atPos.x += ( margin * 2.0f );
}

typedef void ( RENDER_FUNC )( RENDER_PASS * Pass, DRAW_STATE * Draw );

internal void
addDebugButton( PANEL * panel, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, DRAW_STATE * Draw, char * label, RENDER_FUNC * func, boo32 * value ) {
    vec2 button_dim   = Vec2( 44.0f, 28.0f );
    rect Bound = getBound( panel, button_dim );
    
    if( ( wasPressed( Mouse, MouseButton_Left ) ) && ( IsInBound( Mouse->Position, Bound ) ) ) {
        *value = !( *value );
    }
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( *value ) {
        func( Pass_Game, Draw );
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    }
    
    DrawRect( Pass_UI, Bound, Color );
    DrawRectOutline( Pass_UI, Bound, COLOR_BLACK );
    DrawString( Pass_UI, Draw->font, label, getCenter( Bound ), textAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
}

internal void
addS32Counter( PANEL * panel, MOUSE_STATE * Mouse, RENDER_PASS * Pass, FONT * font, char * label, vec2 dim, int32 * value, int32 lo, int32 hi ) {
    rect Bound = getBound( panel, dim );
    
    *value = clamp( *value, lo, hi );
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( IsInBound( Mouse->Position, Bound ) ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
        if( Mouse->wheelClicks != 0 ) {
            int32 at    = *value - lo;
            int32 range = hi - lo;
            *value      = lo + ( at + ( range * 2 ) + Mouse->wheelClicks ) % range;
        }
    }
    
    DrawRect( Pass, Bound, Color );
    DrawRectOutline( Pass, Bound, COLOR_BLACK );
    
    char str[ 32 ] = {};
    sprintf( str, "%s: %d", label, *value );
    DrawString( Pass, font, str, getCenter( Bound ), textAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
}