
internal void
tut03_Event01( APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    EVENT_STATE  * Event  = &AppState->Event;
    PLAYER_STATE * Player = &AppState->Player;
    
    vec2 P = Player->Position;
    rect R = _read( ptr, rect );
    
    if( IsInBound( P, R ) ) {
        Event->Event[ iEvent     ].IsActive = false;
        Event->Event[ iEvent + 1 ].IsActive = true;
        Event->Event[ iEvent + 2 ].IsActive = true;
    }
}

internal void
tut03_Event02( APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    EVENT_STATE * Event = &AppState->Event;
    EVENT       * e     = Event->Event + iEvent;
    
    e->timer += AppState->dt;
    if( e->timer >= 0.4f ) {
        vec2 P = _read( ptr, vec2 );
        spawnJumper( AppState, P );
        
        WORLD_STATE * World = &AppState->World;
        ROOM * Room = World->Room + AppState->iRoom;
        Room->nEntity[ EntityType_Jumper ]++;
        
        Event->Event[ iEvent ].IsActive = false;
    }
}

internal void
tut03_Event03( APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    EVENT_STATE * Event = &AppState->Event;
    EVENT       * e     = Event->Event + iEvent;
    
    e->timer += AppState->dt;
    if( e->timer >= 1.0f ) {
        CONSOLE_STRING( "EVENT is complete!!" );
        Event->Event[ iEvent ].IsActive = false;
    }
}