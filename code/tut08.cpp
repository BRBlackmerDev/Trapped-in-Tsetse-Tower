
internal void
Tut08_Event01( APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    PLAYER_STATE * Player = &AppState->Player;
    EVENT_STATE  * Event  = &AppState->Event;
    
    vec2 P = Player->Position;
    rect R = _read( ptr, rect );
    
    if( IsInBound( P, R ) ) {
        rect S = _read( ptr, rect );
        vec2 Q = getBC( S );
        
        spawnJumper( AppState, Q );
        
        Event->Event[ iEvent ].IsActive = false;
        // activate next Event
    }
}