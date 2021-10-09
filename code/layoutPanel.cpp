
internal void
initLayoutPanel( APP_STATE * AppState ) {
    LAYOUT_STATE * layout = &AppState->layout;
    LAYOUT_PANEL * panel  = &layout->panel;
    
    { // set Bound
        rect R = AppState->App_Bound;
        R.Left = R.Right - 320.0f;
        
        panel->Bound = R;
    }
}

internal void
UpdateAndDrawLayoutPanel( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI ) {
    LAYOUT_STATE * layout = &AppState->layout;
    LAYOUT_PANEL * panel  = &layout->panel;
    
    flo32 margin = 4.0f;
    panel->inner = addRadius( panel->Bound, -margin );
    panel->atPos = getTL( panel->inner );
    
    DrawRect( Pass_UI, panel->Bound, COLOR_WHITE );
    
    INPUT_STRING * filter = &layout->filter_string;
    { // filter string
        vec2 dim = Vec2( getWidth( panel->inner ), 28.0f );
        rect R   = rectTLD( panel->atPos, dim );
        
        DrawRect( Pass_UI, R, COLOR_GRAY( 0.95f ) );
        DrawRectOutline( Pass_UI, R, COLOR_BLACK );
        
        if( filter->nChar > 0 ) {
            DrawString( Pass_UI, AppState->font, filter->string, panel->atPos + Vec2( 4.0f, -4.0f ), textAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
        }
        
        panel->atPos.y -= ( dim.y + margin );
    }
    
    { // file list
        vec2 dim = Vec2( getWidth( panel->inner ), 24.0f );
        for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
            LAYOUT_ROOM * Room = layout->Room + iRoom;
            if( strstr( Room->name, filter->string ) ) {
                DrawString( Pass_UI, AppState->font, Room->name, panel->atPos + Vec2( 6.0f, -4.0f ), textAlign_TopLeft, COLOR_BLACK );
                
                rect R = rectTLD( panel->atPos, dim );
                if( IsInBound( Mouse->Position, R ) ) {
                    DrawRectOutline( Pass_UI, R, COLOR_BLACK );
                    if( wasPressed( Mouse, MouseButton_Left ) ) {
                        Room->show = true;
                    }
                }
                
                panel->atPos.y -= ( dim.y + 2.0f );
            }
        }
    }
}