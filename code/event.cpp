
internal void
updateEvent( APP_STATE * AppState ) {
    WORLD_STATE * World = &AppState->World;
    
    ROOM Room = World->Room[ AppState->iRoom ];
    
    EVENT * Event = AppState->Event.Event + Room.Event.m;
    uint32 nEvent = Room.Event.n;
    
    for( uint32 iEvent = 0; iEvent < nEvent; iEvent++ ) {
        EVENT e = Event[ iEvent ];
        if( e.IsActive ) {
            e.func( AppState, e.data, Room.Event.m + iEvent );
        }
    }
}