
internal vec2
getGroundP( GRENADE Entity ) {
    vec2 Result = Entity.Position + Vec2( 0.0f, -GRENADE_RADIUS );
    return Result;
}

internal void
updateGrenade( APP_STATE * AppState, flo32 dT ) {
    GRENADE_STATE   * State     = &AppState->Grenade;
    COLLISION_STATE * Collision = &AppState->Collision;
    PLAYER_STATE    * Player    = &AppState->Player;
    
    if( ( Player->Throw_WasPressed ) && ( Player->GrenadeThrow_CooldownTimer <= 0.0f ) ) {
        Player->Throw_WasPressed           = false;
        Player->GrenadeThrow_CooldownTimer = PLAYER_GRENADE_COOLDOWN_TARGET_TIME;
        vec2 PlayerP = GetPlayerAttackP( *Player );
        
        flo32 Speed = GetLength( Player->Velocity ) * 0.5f;
        vec2  Dir   = GetNormal( Player->Velocity );
        if( GetLengthSq( Player->aim_dir ) > 0.0f ) {
            Dir     = Player->aim_dir;
            flo32 t = clamp01( ( Dir.y + 1.0f ) / 2.0f );
            Speed   = lerp( GRENADE_THROW_SPEED_LO, t, GRENADE_THROW_SPEED_HI );;
        }
        
        GRENADE Entity = {};
        Entity.Position = PlayerP;
        Entity.Velocity = Dir * Speed;
        
        if( State->nEntity < GRENADE_MAX_COUNT ) {
            State->Entity[ State->nEntity++ ] = Entity;
        } else {
            char str[ 512 ] = {};
            sprintf( str, "ERROR! Attempted to spawn GRENADE, but GRENADE_STATE was full! MaxCount = %u", GRENADE_MAX_COUNT );
            CONSOLE_STRING( str );
            globalVar_debugSystem.errorOccurred = true;
        }
    }
    
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        GRENADE * Entity = State->Entity + iEntity;
        
        flo32 Friction = GRENADE_AIR_FRICTION;
        boo32 isOnGround = false;
        { // isOnGround : TERRAIN
            for( uint32 iBound = 0; iBound < Collision->nJumpBound; iBound++ ) {
                rect R = Collision->jumpBound[ iBound ];
                
                vec2 EntityP = getGroundP( *Entity );
                if( IsInBound( EntityP, R ) ) {
                    isOnGround = true;
                }
            }
        }
        if( isOnGround ) {
            Friction = GRENADE_GROUND_FRICTION;
        }
        
        vec2 Accel = {};
        Accel.x = -Entity->Velocity.x * Friction;
        Accel.y = -GRENADE_GRAVITY;
        
        vec2 dP = Accel * ( dT * dT * 0.5f ) + Entity->Velocity * dT;
        Entity->Velocity += Accel * dT;
        
        COLLISION_RESULT Coll = updateGrenadeCollision( AppState, Entity->Position, Entity->Velocity, dP, GRENADE_RADIUS );
        
        Entity->Position = Coll.Position;
        Entity->Velocity = Coll.Velocity;
        
        Entity->Timer += dT;
        if( Entity->Timer >= GRENADE_DETONATE_TARGET_TIME ) {
            // TODO: different damages at different Radius?
            addGrenadeDetonate( &AppState->Particle, Entity->Position );
            
            { // PLAYER
                rect R = GetPlayerGrenadeBound( *Player );
                if( IsInBound( Entity->Position, R ) ) {
                    damagePlayer( AppState, GRENADE_DAMAGE_TO_PLAYER );
                }
            }
            
            { // JUMPER
                JUMPER_STATE * JumperState = &AppState->Jumper;
                for( uint32 iJumper = 0; iJumper < JumperState->nEntity; iJumper++ ) {
                    JUMPER * Jumper = JumperState->Entity + iJumper;
                    vec2 P = getJumperGrenadeP( *Jumper );
                    if( IsInBound( P, Circ( Entity->Position, GRENADE_DAMAGE_RADIUS ) ) ) {
                        Jumper->health = maxValue( Jumper->health - GRENADE_DAMAGE_TO_JUMPER, 0 );
                        if( Jumper->health > 0 ) {
                            stunJumper( Jumper );
                        } else {
                            addJumperKill( &AppState->Particle, Jumper->Position );
                        }
                    }
                }
            }
            
            { // NEST
                NEST_STATE * NestState = &AppState->Nest;
                for( uint32 iNest = 0; iNest < NestState->nEntity; iNest++ ) {
                    NEST * Nest = NestState->Entity;
                    if( ( !Nest->IsDead ) && ( Nest->isAwake ) ) {
                        rect R = getNestGrenadeBound( *Nest );
                        if( IsInBound( Entity->Position, R ) ) {
                            Nest->health = maxValue( Nest->health - GRENADE_DAMAGE_TO_NEST, 0 );
                            addNestPop( &AppState->Particle, R );
                        }
                    }
                }
            }
        }
    }
}

internal void
DrawGrenade( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    GRENADE_STATE * State = Draw->Grenade;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        GRENADE Entity = State->Entity[ iEntity ];
        
        vec4 Color = GRENADE_COLOR;
        
        flo32 CycleTimeA  = ( 6.0f / 60.0f );
        flo32 CycleTimeB  = ( 4.0f / 60.0f );
        flo32 TargetTimeB = GRENADE_DETONATE_TARGET_TIME - ( CycleTimeB * 3.0f );
        flo32 TargetTimeA = TargetTimeB - ( CycleTimeA * 2.0f );
        if( Entity.Timer >= TargetTimeB ) {
            uint32 Cycle = ( uint32 )( ( Entity.Timer - TargetTimeB ) / CycleTimeB );
            if( ( Cycle % 2 ) == 0 ) {
                Color = toColor( 220, 0, 0 );
            }
        } else if( Entity.Timer >= TargetTimeA ) {
            uint32 Cycle = ( uint32 )( ( Entity.Timer - TargetTimeA ) / CycleTimeA );
            if( ( Cycle % 2 ) == 0 ) {
                Color = toColor( 220, 0, 0 );
            }
        }
        DrawCircle       ( Pass, Entity.Position, GRENADE_RADIUS, Color );
        DrawCircleOutline( Pass, Entity.Position, GRENADE_RADIUS, COLOR_BLACK );
    }
}

internal void
DrawGrenadeDebug( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    { // GRENADE
        GRENADE_STATE * State = Draw->Grenade;
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            GRENADE Entity = State->Entity[ iEntity ];
            
            if( Entity.Timer >= GRENADE_DETONATE_TARGET_TIME ) {
                DrawCircleOutline( Pass, Entity.Position, GRENADE_DAMAGE_RADIUS, COLOR_YELLOW );
            }
        }
    }
    
    { // PLAYER
        PLAYER_STATE * Player = Draw->Player;
        rect R = GetPlayerGrenadeBound( *Player );
        DrawRectOutline( Pass, R, COLOR_YELLOW );
    }
    
    { // JUMPER
        JUMPER_STATE * State = Draw->Jumper;
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            JUMPER Entity = State->Entity[ iEntity ];
            
            vec2 P = getJumperGrenadeP( Entity );
            DrawPoint( Pass, P, TILE_DIM * 0.25f, COLOR_YELLOW );
        }
    }
    
    { // NEST
        NEST_STATE * State = Draw->Nest;
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            NEST Entity = State->Entity[ iEntity ];
            if( !Entity.IsDead ) {
                rect R = getNestGrenadeBound( Entity );
                DrawRectOutline( Pass, R, COLOR_YELLOW );
            }
        }
    }
}

internal void
FinalizeGrenade( APP_STATE * AppState ) {
    GRENADE_STATE * State = &AppState->Grenade;
    
    uint32 nEntity = 0;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        GRENADE Entity = State->Entity[ iEntity ];
        if( Entity.Timer < GRENADE_DETONATE_TARGET_TIME ) {
            State->Entity[ nEntity++ ] = Entity;
        }
    }
    State->nEntity = nEntity;
}