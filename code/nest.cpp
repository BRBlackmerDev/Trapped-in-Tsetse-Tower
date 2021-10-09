
internal vec2
getNestSpawnP( NEST Nest ) {
    vec2 result = getT( Nest.Bound, Vec2( 0.5f, 0.2f ) );
    return result;
}

internal rect
getNestGrenadeBound( NEST Entity ) {
    rect Result = Entity.Bound;
    Result = addRadius( Result, TILE_WIDTH * 4.0f );
    return Result;
}

internal void
updateNest( APP_STATE * AppState, flo32 dt ) {
    NEST_STATE * State = &AppState->Nest;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        NEST * Entity = State->Entity + iEntity;
        
        if( !Entity->IsDead ) {
            Entity->timer += dt;
            if( Entity->isAwake ) {
                Entity->awake_timer += dt;
                if( Entity->nSpawn < Entity->maxSpawn ) {
                    Entity->spawn_timer += dt;
                }
                if( Entity->doKill ) {
                    Entity->kill_timer += dt;
                }
            }
        }
    }
}

internal void
DrawNest( RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, DRAW_STATE * Draw ) {
    NEST_STATE * State = Draw->Nest;
    
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        NEST Entity = State->Entity[ iEntity ];
        
        if( !Entity.IsDead ) {
            switch( Entity.Type ) {
                case EntityType_Jumper: {
                    flo32 t = clamp01( Entity.awake_timer / NEST_AWAKE_TARGET_TIME );
                    
                    vec2 P = getTC( Entity.Bound );
                    vec2 Q = getBC( Entity.Bound );
                    vec2 R = lerp( P, lerp( 0.65f, t, 0.5f ), Q );
                    
                    flo32 radA = lerp(  PI * 0.5f / 64, t,  PI *  2.0f / 64.0f );
                    flo32 radB = lerp( -PI * 1.0f / 64, t, -PI *  7.0f / 64.0f );
                    flo32 radC = lerp(  PI * 1.5f / 64, t,  PI * 11.0f / 64.0f );
                    flo32 radD = lerp( -PI * 2.0f / 64, t, -PI * 15.0f / 64.0f );
                    
                    vec2 dimA = getDim( Entity.Bound ) * lerp( 1.0f, t, 1.25f );
                    vec2 dimB = dimA * Vec2( lerp( 0.8f,  t, 0.9f  ),  1.0f );
                    vec2 dimC = dimA * Vec2( lerp( 0.65f, t, 0.85f ), 1.0f );
                    vec2 dimD = dimA * Vec2( lerp( 0.5f,  t, 0.8f  ), 1.0f );
                    
                    DrawORect( Pass_Game, orectCD( R, dimA, radA ), COLOR_GRAY( 0.10f ) );
                    DrawORect( Pass_Game, orectCD( R, dimB, radB ), COLOR_GRAY( 0.12f ) );
                    DrawORect( Pass_Game, orectCD( R, dimC, radC ), COLOR_GRAY( 0.15f ) );
                    DrawORect( Pass_Game, orectCD( R, dimD, radD ), COLOR_GRAY( 0.08f ) );
                    
                    { // opening
                        vec2 U   = lerp( P, lerp( 0.8f, t, 0.6f ), Q );
                        vec2 dim = dimA * Vec2( 0.5f, 0.6f ) * lerp( 0.5f, t, 1.0f );
                        DrawRect( Pass_Game, rectCD( U, dim ), COLOR_BLACK );
                    }
                } break;
            }
            
            if( Entity.isAwake ) { // health bar
                // TODO: multiple Nests
                vec2  dim = Draw->app_dim * Vec2( 0.6f, 0.025f );
                vec2 bPos = getBC( Draw->App_Bound ) + Vec2( 0.0f, Draw->app_dim.y * 0.025f );
                vec2 margin = Vec2( 3.0f, 3.0f );
                
                rect R = rectBCD( bPos, dim );
                DrawRect       ( Pass_UI, R, COLOR_GRAY( 0.05f ) );
                DrawRectOutline( Pass_UI, R, COLOR_GRAY( 0.15f ) );
                
                flo32 tA = clamp01( Entity.awake_timer / NEST_AWAKE_TARGET_TIME );
                flo32 tB = clamp01( ( flo32 )Entity.health / ( flo32 )NEST_HEALTH );
                flo32 t = minValue( tA, tB );
                
                vec2 bar_pos = getBL( R ) + margin;
                vec2 bar_dim = Vec2( t * ( dim.x - ( margin.x * 2.0f ) ), dim.y - ( margin.y * 2.0f ) );
                rect S = rectBLD( getBL( R ) + margin, bar_dim );
                DrawRect( Pass_UI, S, COLOR_RED );
            }
        }
    }
}

internal void
FinalizeNest( APP_STATE * AppState ) {
    NEST_STATE * State = &AppState->Nest;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        NEST * Entity = State->Entity + iEntity;
        
        if( !Entity->IsDead ) {
            if( Entity->health > 0 ) {
                flo32 targetTime = NEST_SPAWN_TARGET_TIME;
                if( Entity->isAwake ) {
                    targetTime = NEST_AWAKE_SPAWN_TARGET_TIME;
                }
                if( Entity->timer >= targetTime ) {
                    Entity->timer = 0.0f;
                    
                    switch( Entity->Type ) {
                        case EntityType_Jumper: {
                            vec2 P = getNestSpawnP( *Entity );
                            spawnJumper( AppState, P );
                        } break;
                    }
                }
                
                { // added spawn
                    uint32 threshold = NEST_HEALTH / 2;
                    if( ( Entity->maxSpawn < NEST_JUMPER_MAX_SPAWN ) && ( ( uint32 )Entity->health < threshold ) ){
                        Entity->maxSpawn = NEST_JUMPER_MAX_SPAWN;
                    }
                    
                    if( ( Entity->health > 0 ) && ( Entity->nSpawn < Entity->maxSpawn ) ) {
                        if( Entity->spawn_timer >= NEST_ADDED_SPAWN_TARGET_TIME ) {
                            Entity->spawn_timer = 0.0f;
                            
                            vec2 P = getNestSpawnP( *Entity );
                            spawnJumper( AppState, P );
                            
                            Entity->nSpawn++;
                        }
                        
                    }
                }
            }
            
            if( ( Entity->health == 0 ) && ( !Entity->doKill ) ) {
                Entity->doKill = true;
            }
            if( Entity->doKill ) {
                if( Entity->kill_timer >= NEST_KILL_TARGET_TIME ) {
                    JUMPER_STATE * Jumper = &AppState->Jumper;
                    if( Jumper->nEntity > 0 ) {
                        Entity->kill_timer = 0.0f;
                        Entity->nKill++;
                        
                        uint32 toKill = 1;
                        if( Entity->nKill >= 5 ) {
                            toKill = 2;
                        }
                        if( Entity->nKill >= 10 ) {
                            toKill = 4;
                        }
                        
                        toKill = minValue( toKill, Jumper->nEntity );
                        
                        for( uint32 iKill = 0; iKill < toKill; iKill++ ) {
                            JUMPER j = Jumper->Entity[ --Jumper->nEntity ];
                            rect   Bound = getJumperBound( j );
                            addJumperKill( &AppState->Particle, getCenter( Bound ) );
                        }
                        
                        addNestPop( &AppState->Particle, Entity->Bound );
                    } else if( Entity->nPost < 2 ) {
                        Entity->kill_timer = 0.0f;
                        
                        Entity->nPost++;
                        addNestPost( &AppState->Particle, Entity->Bound );
                    } else {
                        Entity->IsDead = true;
                        addNestKill( &AppState->Particle, Entity->Bound );
                        
                        Entity->doKill = false;
                    }
                }
            }
        }
    }
}