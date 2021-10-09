
internal void
shaft01_Event01( APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    PLAYER_STATE * Player = &AppState->Player;
    EVENT_STATE  * Event  = &AppState->Event;
    
    vec2 PlayerP = Player->Position;
    
    rect R = _read( ptr, rect );
    if( IsInBound( PlayerP, R ) ) {
        rect S0 = _read( ptr, rect );
        rect S1 = _read( ptr, rect );
        rect S2 = _read( ptr, rect );
        rect S3 = _read( ptr, rect );
        rect S4 = _read( ptr, rect );
        rect S5 = _read( ptr, rect );
        rect S6 = _read( ptr, rect );
        rect S7 = _read( ptr, rect );
        
        uint32 nJumper = 3;
        for( uint32 iJumper = 0; iJumper < nJumper; iJumper++ ){
            vec2 P = getRandomPoint( S0 );
            spawnJumper( AppState, P );
        }
        for( uint32 iJumper = 0; iJumper < nJumper; iJumper++ ){
            vec2 P = getRandomPoint( S1 );
            spawnJumper( AppState, P );
        }
        for( uint32 iJumper = 0; iJumper < nJumper; iJumper++ ){
            vec2 P = getRandomPoint( S2 );
            spawnJumper( AppState, P );
        }
        for( uint32 iJumper = 0; iJumper < nJumper; iJumper++ ){
            vec2 P = getRandomPoint( S3 );
            spawnJumper( AppState, P );
        }
        for( uint32 iJumper = 0; iJumper < nJumper; iJumper++ ){
            vec2 P = getRandomPoint( S4 );
            spawnJumper( AppState, P );
        }
        for( uint32 iJumper = 0; iJumper < nJumper; iJumper++ ){
            vec2 P = getRandomPoint( S5 );
            spawnJumper( AppState, P );
        }
        for( uint32 iJumper = 0; iJumper < nJumper; iJumper++ ){
            vec2 P = getRandomPoint( S6 );
            spawnJumper( AppState, P );
        }
        for( uint32 iJumper = 0; iJumper < nJumper; iJumper++ ){
            vec2 P = getRandomPoint( S7 );
            spawnJumper( AppState, P );
        }
        
        Event->Event[ iEvent ].IsActive = false;
        // activate next Event
    }
}