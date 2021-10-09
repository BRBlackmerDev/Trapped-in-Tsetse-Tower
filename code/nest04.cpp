
internal void
Nest04_Event01( APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    PLAYER_STATE * Player = &AppState->Player;
    EVENT_STATE  * Event  = &AppState->Event;
    
    vec2 P = Player->Position;
    rect R = _read( ptr, rect );
    
    if( IsInBound( P, R ) ) {
        NEST_STATE * Nest = &AppState->Nest;
        Assert( Nest->nEntity > 0 );
        
        // get Nest ID
        NEST * eB = Nest->Entity + 0;
        eB->isAwake  = true;
        eB->maxSpawn = NEST_JUMPER_INIT_SPAWN;
        
        Event->Event[ iEvent ].IsActive = false;
        // activate next Event
    }
}