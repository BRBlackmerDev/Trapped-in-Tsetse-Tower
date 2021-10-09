
internal void
reset( FILE_SAVE_OPEN * State ) {
    State->overwrite      = false;
    State->unsavedChanges = false;
    State->recentSave     = false;
    State->timer          = 0.0f;
    State->confirmNew     = false;
    memset( State->current, 0, FILE_SAVE_OPEN__FILENAME_MAX_CHAR );
}

internal void
DrawFileListForSaveOpen( RENDER_PASS * Pass, APP_STATE * AppState, FILE_SAVE_OPEN * SaveOpen ) {
    INPUT_STRING * i = &SaveOpen->input_string;
    
    vec2 atPos = getTL( AppState->App_Bound ) + Vec2( 4.0f, -4.0f );
    vec2 dim   = Vec2( 512.0f, 18.0f );
    rect Bound = rectTLD( atPos, dim );
    vec4 Color = COLOR_GRAY( 0.1f );
    if( SaveOpen->overwrite ) {
        Color = Vec4( 0.6f, 0.0f, 0.0f, 1.0f );
    }
    DrawRect( Pass, Bound, Color );
    
    atPos += Vec2( 2.0f, -2.0f );
    DrawString( Pass, AppState->font, "FILENAME:", atPos, textAlign_TopLeft, COLOR_WHITE );
    
    DrawString( Pass, AppState->font, i->string, atPos + Vec2( 100.0f, 0.0f ), textAlign_TopLeft, COLOR_WHITE );
    
    STRING_BUFFER * fileList = &SaveOpen->fileList;
    atPos += Vec2( 8.0f, -28.0f );
    for( uint32 iFile = 0; iFile < fileList->nStr; iFile++ ) {
        char * filename = fileList->str[ iFile ];
        if( strstr( filename, i->string ) ) {
            DrawString( Pass, AppState->font, fileList->str[ iFile ], atPos, textAlign_TopLeft, COLOR_WHITE );
            atPos.y -= 20.0f;
        }
    }
}