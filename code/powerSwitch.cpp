
internal rect
getUseBound( POWER_SWITCH Entity ) {
    rect result = Entity.Bound;
    result = addRadius( result, TILE_WIDTH );
    return result;
}

internal void
updatePowerSwitch( APP_STATE * AppState, flo32 dt ) {
    PLAYER_STATE * Player = &AppState->Player;
    vec2 PlayerP = GetPlayerUseP( *Player );
    
    POWER_SWITCH_STATE * State = &AppState->power;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        POWER_SWITCH * Entity = State->Entity + iEntity;
        
        if( Entity->isEnabled ) {
            rect R = getUseBound( *Entity );
            if( ( Player->Use_IsActive ) && ( IsInBound( PlayerP, R ) ) ) {
                Entity->doPower = true;
                Entity->power_timer   += dt;
                
                flo32 t = clamp01( Entity->power_timer / POWER_SWITCH_TARGET_TIME );
                Player->Use_t = t;
            } else {
                Entity->doPower = false;
                Entity->power_timer    = 0.0f;
            }
        }
    }
}

internal void
DrawPowerSwitch( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    POWER_SWITCH_STATE * State = Draw->power;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        POWER_SWITCH Entity = State->Entity[ iEntity ];
        
        rect R = Entity.Bound;
        { // panel
            vec4 Color = COLOR_GRAY( 0.5f );
            if( !Entity.isEnabled ) {
                Color = COLOR_GRAY( 0.25f );
            }
            
            DrawRect( Pass, R, Color );
            DrawRectOutline( Pass, R, COLOR_BLACK );
        }
        
        rect S = R;
        { // rail
            S.Left   += ( TILE_WIDTH * 1.0f );
            S.Right  -= ( TILE_WIDTH * 1.0f );
            S.Bottom += ( TILE_HEIGHT * 0.75f );
            S.Top     = S.Bottom + ( TILE_HEIGHT * 0.5f );
            DrawRect( Pass, S, COLOR_GRAY( 0.15f ) );
        }
        
        { // switch
            vec2 P = getBL( S );
            vec2 Q = getTL( S );
            if( Entity.isOn ) {
                P = getBR( S );
                Q = getTR( S );
            }
            rect T = rectMM( P, Q );
            T = addDim( T, Vec2( TILE_WIDTH * 0.375f, TILE_HEIGHT * 0.5f ) );
            
            rect U = addRadius( T, -TILE_WIDTH * 0.125f );
            vec4 Color = COLOR_RED;
            if( Entity.isOn ) {
                Color = COLOR_GREEN;
            }
            
            DrawRect( Pass, T, COLOR_GRAY( 0.1f ) );
            DrawRectOutline( Pass, T, COLOR_BLACK );
            
            DrawRect( Pass, U, Color );
        }
    }
}

internal void
FinalizePowerSwitch( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    vec2 PlayerP = GetPlayerUseP( *Player );
    
    POWER_SWITCH_STATE * State = &AppState->power;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        POWER_SWITCH * Entity = State->Entity + iEntity;
        
        if( Entity->isEnabled ) {
            if( Entity->power_timer >= POWER_SWITCH_TARGET_TIME ) {
                Entity->isOn  = !Entity->isOn;
                Entity->power_timer = 0.0f;
                
                Player->Use_IsActive = false;
                Player->Use_t        = 0.0f;
            }
        }
    }
}