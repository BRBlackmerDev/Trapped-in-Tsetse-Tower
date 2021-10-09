
internal void
captureReplay( APP_STATE * AppState ) {
    REPLAY_STATE * replay = &AppState->replay;
    
    if( !replay->show ) {
        REPLAY_FRAME * frame = replay->frame + replay->atFrame;
        memcpy( &frame->Beast,   &AppState->Beast,   sizeof( BEAST_STATE        ) );
        memcpy( &frame->Player,  &AppState->Player,  sizeof( PLAYER_STATE       ) );
        memcpy( &frame->Grenade, &AppState->Grenade, sizeof( GRENADE_STATE      ) );
        memcpy( &frame->Jumper,  &AppState->Jumper,  sizeof( JUMPER_STATE       ) );
        memcpy( &frame->Nest,    &AppState->Nest,    sizeof( NEST_STATE         ) );
        memcpy( &frame->Secure,  &AppState->Secure,  sizeof( SECURE_DOOR_STATE  ) );
        memcpy( &frame->Mech,    &AppState->Mech,    sizeof( MECH_DOOR_STATE    ) );
        memcpy( &frame->power,   &AppState->power,   sizeof( POWER_SWITCH_STATE ) );
        
        replay->atFrame = ( replay->atFrame + 1 ) % REPLAY_FRAME_MAX_COUNT;
    }
}