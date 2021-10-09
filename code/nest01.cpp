
internal void
Nest01_Event01( APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    EVENT_STATE * Event = &AppState->Event;
    WORLD_STATE * World = &AppState->World;
    
    POWER_SWITCH_STATE * power = &AppState->power;
    Assert( power->nEntity > 0 );
    
    // get switch ID
    POWER_SWITCH * eA = power->Entity + 0;
    Assert( eA->isEnabled );
    
    if( eA->isOn ) {
        eA->isEnabled = false;
        
        World->power_isOn[ 0 ] = true;
        
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