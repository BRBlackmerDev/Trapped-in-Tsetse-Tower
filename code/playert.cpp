
internal void
updatePlayerInputT( APP_STATE * AppState, CONTROLLER_STATE * control, flo32 dt ) {
    PLAYER_STATE * Player = &AppState->Player;
    Player->dPos = {};
    
    if( Player->IsDead ) {
        flo32 friction = PLAYER_DEAD_FRICTION;
        
        vec2 accel = -Player->Velocity * friction;
        
        Player->dPos      = accel * ( dt * dt * 0.5f ) + Player->Velocity * dt;
        Player->Velocity += accel * dt;
    } else {
        CONTROLLER_BUTTON_ID  id_run    = controllerButton_A;
        CONTROLLER_BUTTON_ID  id_use    = controllerButton_Y;
        CONTROLLER_THUMBSTICK id_move   = control->Left;
        CONTROLLER_THUMBSTICK id_aim    = control->Right;
        
        vec2 dir = {};
        { // move
            flo32 x = ( flo32 )id_move.xy.x;
            flo32 y = ( flo32 )id_move.xy.y;
            flo32 d = sqrtf( x * x + y * y );
            if( d > ( flo32 )id_move.dead ) {
                flo32 denom = ( flo32 )SHRT_MAX;
                x = clamp( x / denom, -1.0f, 1.0f );
                y = clamp( y / denom, -1.0f, 1.0f );
                dir = Vec2( x, y );
                
                Player->FaceDir = GetNormal( dir );
            }
        }
        
        { // aim dir
            vec2 V = Vec2( ( flo32 )id_aim.xy.x, ( flo32 )id_aim.xy.y );
            
            boo32 prev = Player->fire_IsActive;
            if( ( GetLength( V ) > ( flo32 )id_aim.dead ) && ( !Player->roll_IsActive ) ) {
                Player->aim_dir       = GetNormal( V );
                Player->fire_IsActive = true;
            } else {
                Player->aim_dir       = Vec2( 0.0f, 0.0f );
                Player->fire_IsActive = false;
            }
            
            if( ( Player->fire_IsActive != prev ) && ( Player->fire_IsActive ) ) {
                Player->fire_timer = 0.0f;
                RandomlyAdjustFireDirection( AppState );
            }
        }
        
        if( wasPressed( control, id_run ) ) {
            Player->run_isEnabled = true;
        }
        if( wasReleased( control, id_run ) ) {
            Player->run_isEnabled = false;
        }
        
        Player->run_IsActive = false;
        if( ( !Player->fire_IsActive ) && ( Player->run_isEnabled ) ) {
            Player->run_IsActive = true;
        }
        
        flo32 maxSpeed = PLAYER_WALK_SPEED;
        flo32 friction = PLAYER_WALK_FRICTION;
        if( Player->run_IsActive ) {
            maxSpeed = PLAYER_RUN_SPEED;
            friction = PLAYER_RUN_FRICTION;
        }
        
        vec2 accel = ( dir * maxSpeed - Player->Velocity ) * friction;
        
        Player->dPos      = accel * ( dt * dt * 0.5f ) + Player->Velocity * dt;
        Player->Velocity += accel * dt;
    }
    
    if( ( Player->roll_IsActive ) || ( Player->crouch_IsActive ) ) {
        Player->Height = PLAYER_HEIGHT_CROUCHING;
    } else {
        Player->Height = PLAYER_HEIGHT_STANDING;
    }
}

internal void
updatePlayerT( APP_STATE * AppState, flo32 dt ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    if( Player->IsDead ) {
        // TODO: This can change to show Player rect fallen on the ground
        Player->IsDead_Timer += dt;
    } else {
        { // can Player heal?
            Player->heal_isDisabled = false;
            rect disable_Bound = {};
            
            { // JUMPER
                disable_Bound = getJumperDisableBound( *Player );
                
                JUMPER_STATE * State = &AppState->Jumper;
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    JUMPER Entity = State->Entity[ iEntity ];
                    
                    if( IsInBound( Entity.Position, disable_Bound ) ) {
                        Player->heal_isDisabled = true;
                    }
                }
            }
        }
        
        if( Player->health >= PLAYER_HEALTH ) {
            Player->heal_warmupTimer = maxValue( Player->heal_warmupTimer - dt, 0.0f );
        }
        if( Player->showDamage_timer > 0.0f ) {
            Player->showDamage_timer = maxValue( Player->showDamage_timer - dt, 0.0f );
        }
        
        if( ( Player->health < PLAYER_HEALTH ) && ( !Player->heal_isDisabled ) ) {
            Player->heal_warmupTimer = minValue( Player->heal_warmupTimer + dt, PLAYER_HEAL_WARMUP_TARGET_TIME );
            
            if( Player->heal_warmupTimer >= PLAYER_HEAL_WARMUP_TARGET_TIME ) {
                Player->heal_timer += dt;
                if( Player->heal_timer >= PLAYER_HEAL_TARGET_TIME ) {
                    Player->health++;
                    Player->heal_timer = 0.0f;
                }
            }
            
            Player->heal_disableTimer = 0.0f;
        } else {
            Player->heal_timer = 0.0f;
        }
        if( Player->heal_isDisabled ) {
            // TODO: Adjust DISABLE_CYCLE_TARGET_TIME based on how low Player health is.
            Player->heal_disableTimer += dt;
            if( Player->heal_disableTimer >= PLAYER_HEAL_DISABLE_CYCLE_TARGET_TIME ) {
                Player->heal_disableTimer = 0.0f;
            }
            
            Player->heal_warmupTimer = 0.0f;
        }
        
        if( Player->fire_IsActive ) {
            Player->fire_timer += dt;
            if( Player->fire_timer >= PLAYER_BULLET_TARGET_TIME ) {
                Player->fire_timer = 0.0f;
                RandomlyAdjustFireDirection( AppState );
            }
        }
    }
    
    COLLISION_RESULT coll = UpdatePlayerCollisionT( AppState, Player->Position, Player->Velocity, Player->dPos, PLAYER_RADIUS );
    Player->Position = coll.Position;
    Player->Velocity = coll.Velocity;
}