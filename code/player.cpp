
internal vec2
GetPlayerAttackP( PLAYER_STATE Player ) {
    flo32 Height = ( Player.crouch_IsActive ) ? ( Player.Height * 0.5f ) : ( Player.Height * 0.75f );
    vec2  result = Player.Position + Vec2( 0.0f, Height );
    return result;
}

internal vec2
GetPlayerUseP( PLAYER_STATE Player ) {
    flo32 Height = PLAYER_HEIGHT_STANDING * 0.65f;
    vec2  result = Player.Position + Vec2( 0.0f, Height );
    return result;
}

internal vec2
GetPlayerCameraP( PLAYER_STATE Player ) {
    vec2  result = Player.Position + Vec2( 0.0f, PLAYER_HEIGHT_STANDING * 0.6f );
    return result;
}

internal rect
GetPlayerGrenadeBound( PLAYER_STATE Player ) {
    rect result = {};
    result = rectBCD( Player.Position, Vec2( PLAYER_WIDTH, Player.Height ) );
    result = addRadius( result, TILE_WIDTH * 4.0f );
    return result;
}

internal rect
getJumperAttackBound( PLAYER_STATE Player ) {
    rect result = {};
    result = rectBCD( Player.Position, Vec2( PLAYER_WIDTH, Player.Height ) );
    result = addRadius( result, TILE_WIDTH * 0.5f );
    return result;
}

internal rect
getJumperDisableBound( PLAYER_STATE Player ) {
    rect result = {};
    result = rectBCD( Player.Position, Vec2( PLAYER_WIDTH, PLAYER_HEIGHT_STANDING ) );
    result = addDim( result, Vec2( TILE_WIDTH * 4.0f, TILE_WIDTH * 1.0f ) );
    return result;
}

internal void
RandomlyAdjustFireDirection( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    Player->fire_doHitScan = true;
    
    flo32 radians = atan2f( Player->aim_dir.y, Player->aim_dir.x );
    
    flo32 range = 0.0625f / PI;
    radians += RandomF32InRange( -range, range );
    vec2 N = Vec2( cosf( radians ), sinf( radians ) );
    
    vec2 offset = GetPerp( N ) * RandomF32InRange( -0.0625f, 0.0625f );
    
    flo32 t = 0.85f;
    vec2 P = Player->Position + Vec2( 0.0f, Player->Height * 0.75f ) + offset;
    if( AppState->stat.isTopDown ) {
        P = Player->Position;
        t = 0.7f;
    }
    
    Player->fire_pos = P + offset;
    Player->fire_dir = N;
    
    { // muzzle flash
        flo32 dim = RandomF32InRange( 0.085f, 0.095f );
        
        PARTICLE p = {};
        p.Position   = P + Player->fire_dir * t;
        p.dim        = Vec2( dim, dim );
        p.radians    = RandomF32InRange( 0.0f, PI * 0.25f );
        p.targetTime = ( 1.0f / 60.0f );
        p.Color      = COLOR_WHITE;
        
        PARTICLE_STATE * Particle = &AppState->Particle;
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal int32
getCurrentPlatform( APP_STATE * AppState ) {
    PLAYER_STATE    * Player    = &AppState->Player;
    COLLISION_STATE * Collision = &AppState->Collision;
    
    rect R = rectBCD( Player->Position, Vec2( PLAYER_WIDTH, Player->Height ) );
    
    int32 bJumperEdge = -1;
    for( uint32 iJumperEdge = 0; iJumperEdge < Collision->nJumperEdge; iJumperEdge++ ) {
        vec4 Edge = Collision->JumperEdge[ iJumperEdge ];
        if( doesHorSegmentIntersectRect( R, Edge.P, Edge.Q ) ) {
            bJumperEdge = iJumperEdge;
        }
    }
    
    uint32 Result = bJumperEdge;
    return Result;
}

internal void
updatePlayerInput( APP_STATE * AppState, CONTROLLER_STATE * control, flo32 dt ) {
    PLAYER_STATE * Player = &AppState->Player;
    Player->dPos = {};
    Player->Use_WasPressed   = false;
    Player->Use_WasReleased  = false;
    Player->Throw_WasPressed = false;
    
    COLLISION_STATE * Collision = &AppState->Collision;
    
    CONTROLLER_BUTTON_ID  id_jump   = controllerButton_shoulder_Left;
    CONTROLLER_BUTTON_ID  id_crouch = controllerButton_shoulder_Right;
    CONTROLLER_BUTTON_ID  id_run    = controllerButton_A;
    CONTROLLER_BUTTON_ID  id_use    = controllerButton_Y;
    CONTROLLER_BUTTON_ID  id_throw  = controllerButton_trigger_Right;
    CONTROLLER_THUMBSTICK id_move   = control->Left;
    CONTROLLER_THUMBSTICK id_aim    = control->Right;
    
    if( Player->IsDead ) {
        flo32 friction = PLAYER_DEAD_FRICTION;
        
        vec2 accel = {};
        accel.x = -Player->Velocity.x * friction;
        accel.y = -PLAYER_GRAVITY;
        
        Player->dPos      = accel * ( dt * dt * 0.5f ) + Player->Velocity * dt;
        Player->Velocity += accel * dt;
        
        if( wasPressed( control, id_use ) ) {
            AppState->LoadCheckpoint_IsActive = true;
        }
        if( wasReleased( control, id_use ) ) {
            AppState->LoadCheckpoint_IsActive = false;
        }
    } else {
        
        vec2 dir = {};
        { // move Left/Right
            if( id_move.xy.x != 0 ) {
                flo32 accel = 0.0f;
                if( id_move.xy.x < -id_move.dead ) {
                    accel = -1.0f;
                }
                if( id_move.xy.x >  id_move.dead ) {
                    accel =  1.0f;
                }
                dir.x = accel;
            }
        }
        
        { // up/down
            if( id_move.xy.y != 0 ) {
                if( id_move.xy.y < -id_move.dead ) {
                    dir.y = -1.0f;
                    if( id_move.xy.y > SHRT_MIN ) {
                        flo32 numer = ( flo32 )( id_move.xy.y + id_move.dead );
                        flo32 denom = ( flo32 )( SHRT_MAX - id_move.dead );
                        dir.y = clamp( numer / denom, -1.0f, 0.0f );
                    }
                }
                if( id_move.xy.y >  id_move.dead ) {
                    dir.y =  1.0f;
                }
            }
        }
        
        Player->control_xDir = dir.x;
        
        boo32 isOnGround = false;
        boo32 canWallSlideLeft   = false;
        boo32 canWallSlideRight  = false;
        boo32 wallSlide_IsActive = false;
        boo32 canWallJumpLeft    = false;
        boo32 canWallJumpRight   = false;
        
        boo32 doWallJumpLeft  = false;
        boo32 doWallJumpRight = false;
        boo32 doRoll = false;
        
        boo32 lEdgeGrab_IsActive = ( Player->lEdgeGrabLeft_IsActive ) || ( Player->lEdgeGrabRight_IsActive );
        if( lEdgeGrab_IsActive ) {
            InvalidCodePath;
            if( ( Player->lEdgeGrabLeft_IsActive  ) && ( dir.x < 0.0f ) ) {
                canWallJumpLeft = true;
            }
            if( ( Player->lEdgeGrabRight_IsActive ) && ( dir.x > 0.0f ) ) {
                canWallJumpRight = true;
            }
            
            Player->wallJump_chargeTimer = 0.0f;
            Player->wallJump_dispelTimer = 0.0f;
            
            if( wasPressed( control, id_jump ) ) {
                if( canWallJumpLeft ) {
                    doWallJumpLeft  = true;
                } else if( canWallJumpRight ) {
                    doWallJumpRight = true;
                } else {
                    Player->Velocity.y = PLAYER_LEDGE_GRAB_JUMP_SPEED;
                }
                
                Player->lEdgeGrabLeft_IsActive  = false;
                Player->lEdgeGrabRight_IsActive = false;
            } else if( wasPressed( control, id_crouch ) ) {
                Player->crouch_IsActive  = true;
                
                Player->lEdgeGrabLeft_IsActive  = false;
                Player->lEdgeGrabRight_IsActive = false;
            }
        } else {
            { // isOnGround : TERRAIN
                for( uint32 iBound = 0; iBound < Collision->nJumpBound; iBound++ ) {
                    rect R = Collision->jumpBound[ iBound ];
                    
                    if( IsInBound( Player->Position, R ) ) {
                        isOnGround = true;
                    }
                }
            }
            
            { // isOnGround : NEST
                NEST_STATE * State = &AppState->Nest;
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    NEST Entity = State->Entity[ iEntity ];
                    
                    if( !Entity.IsDead ) {
                        vec2 P = getTL( Entity.Bound ) + Vec2( -( PLAYER_HALF_WIDTH + 0.125f ), -0.1f );
                        vec2 Q = getTR( Entity.Bound ) + Vec2(  ( PLAYER_HALF_WIDTH + 0.125f ),  0.1f );
                        rect R = rectMM( P, Q );
                        if( IsInBound( Player->Position, R ) ) {
                            isOnGround = true;
                        }
                    }
                }
            }
            
            
            if( isOnGround ) {
                Player->wallJump_chargeTimer = 0.0f;
                Player->wallJump_dispelTimer = 0.0f;
                
                if( wasPressed( control, id_jump ) ) {
                    Player->Velocity.y = PLAYER_JUMP_SPEED;
                    
                    Player->jump_timer    = 0.0f;
                    Player->jump_IsActive = true;
                    
                    Player->roll_IsActive = false;
                } else if( wasPressed( control, id_crouch ) ) {
                    if( !Player->roll_IsActive ) {
                        boo32 canRoll = ( dir.x != 0.0f ) && ( ( Player->run_IsActive ) || ( dir.y < -0.2f ) );
                        if( canRoll ) {
                            doRoll = true;
                        } else {
                            Player->crouch_IsActive  = true;
                        }
                    }
                }
            } else {
                for( uint32 iBound = 0; iBound < Collision->nWallSlideLeft; iBound++ ) {
                    rect R = Collision->wallSlideLeft[ iBound ];
                    
                    if( IsInBound( Player->Position, R ) ) {
                        canWallSlideLeft = true;
                        canWallJumpLeft  = true;
                    }
                }
                
                for( uint32 iBound = 0; iBound < Collision->nWallSlideRight; iBound++ ) {
                    rect R = Collision->wallSlideRight[ iBound ];
                    
                    if( IsInBound( Player->Position, R ) ) {
                        canWallSlideRight = true;
                        canWallJumpRight  = true;
                    }
                }
                
                if( Player->Velocity.y < 4.0f ) {
                    if( ( canWallSlideLeft  ) && ( dir.x > 0.0f ) ) {
                        wallSlide_IsActive = true;
                    }
                    if( ( canWallSlideRight ) && ( dir.x < 0.0f ) ) {
                        wallSlide_IsActive = true;
                    }
                }
                
                if( wallSlide_IsActive ) {
                    Player->wallJump_chargeTimer += dt;
                    Player->wallJump_dispelTimer  = 0.0f;
                    
                    Player->wallSlide_ParticleTimer += dt;
                    if( Player->wallSlide_ParticleTimer >= PLAYER_WALL_SLIDE_PARTICLE_CYCLE_TARGET_TIME ) {
                        Player->wallSlide_ParticleTimer = 0.0f;
                        addWallSlide( &AppState->Particle, Player->Position, canWallSlideLeft );
                    }
                } else {
                    Player->wallSlide_ParticleTimer = 0.0f;
                }
                
                if( Player->wallJump_chargeTimer >= PLAYER_WALL_JUMP_CHARGE_TARGET_TIME ) {
                    if( canWallSlideLeft ) {
                        canWallJumpLeft  = true;
                    }
                    if( canWallSlideRight ) {
                        canWallJumpRight = true;
                    }
                }
                
                if( ( canWallJumpLeft ) || ( canWallJumpRight ) ) {
                    Player->Draw_wallJump = true;
                }
                
                if( wasPressed( control, id_jump ) ) {
                    if( canWallJumpLeft ) {
                        doWallJumpLeft  = true;
                    }
                    if( canWallJumpRight ) {
                        doWallJumpRight = true;
                    }
                } else if( wasPressed( control, id_crouch ) ) {
                    Player->crouch_IsActive  = true;
                }
                
                if( ( !wallSlide_IsActive ) && ( !canWallJumpLeft ) && ( !canWallJumpRight ) ) {
                    Player->wallJump_dispelTimer += dt;
                    if( Player->wallJump_dispelTimer >= PLAYER_WALL_JUMP_DISPEL_TARGET_TIME ) {
                        canWallJumpLeft  = false;
                        canWallJumpRight = false;
                        
                        Player->wallJump_chargeTimer = 0.0f;
                        Player->wallJump_dispelTimer = 0.0f;
                    }
                }
            }
            
            if( wasPressed( control, id_use ) ) {
                Player->Use_WasPressed = true;
                Player->Use_IsActive   = true;
            }
            if( wasReleased( control, id_use ) ) {
                Player->Use_WasReleased = true;
                Player->Use_IsActive    = false;
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
        
        if( wasPressed( control, id_throw ) ) {
            Player->Throw_WasPressed = true;
        }
        Player->GrenadeThrow_CooldownTimer -= dt;
        
        Player->run_IsActive = false;
        if( ( !Player->fire_IsActive ) && ( Player->run_isEnabled ) ) {
            Player->run_IsActive = true;
        }
        
        if( wasPressed( control, id_jump ) ) {
            AppState->lastJump = Player->Position;
        }
        
        vec2 speed = Player->run_IsActive ? PLAYER_WALL_JUMP_RUN_SPEED : PLAYER_WALL_JUMP_WALK_SPEED;
        if( doWallJumpLeft ) {
            Player->Velocity.x = -speed.x;
            Player->Velocity.y =  speed.y;
            
            Player->lEdgeGrabLeft_IsActive  = false;
            
            Player->wallJump_chargeTimer = 0.0f;
            Player->wallJump_dispelTimer = 0.0f;
            
            wallSlide_IsActive = false;
            
            Player->jump_doDampen = false;
        }
        if( doWallJumpRight ) {
            Player->Velocity.x =  speed.x;
            Player->Velocity.y =  speed.y;
            
            Player->lEdgeGrabRight_IsActive = false;
            
            Player->wallJump_chargeTimer = 0.0f;
            Player->wallJump_dispelTimer = 0.0f;
            
            wallSlide_IsActive = false;
            
            Player->jump_doDampen = false;
        }
        if( wasReleased( control, id_jump ) ) {
            if( ( Player->jump_IsActive ) && ( Player->jump_timer <= PLAYER_JUMP_TARGET_TIME ) ) {
                Player->jump_doDampen = true;
            }
            Player->jump_IsActive    = false;
        }
        if( wasReleased( control, id_crouch ) ) {
            Player->crouch_IsActive  = false;
            // TODO: Player->crouch_IsActive needs to check if the Player is in a vent or something and can't stand up
        }
        if( doRoll ) {
            Player->roll_IsActive = true;
            Player->roll_timer    = 0.0f;
            Player->Velocity.x    = dir.x * PLAYER_ROLL_INIT_SPEED;
        }
        if( Player->roll_IsActive ) {
            Player->roll_timer += dt;
            if( Player->roll_timer >= PLAYER_ROLL_TARGET_TIME ) {
                Player->roll_IsActive = false;
                
                // TODO: need to check if the Player is in a vent or something and can't stand up
                if( isDown( control, id_crouch ) ) {
                    Player->crouch_IsActive = true;
                }
            }
        }
        if( Player->jump_IsActive ) {
            Player->jump_timer += dt;
            if( Player->jump_timer > PLAYER_JUMP_TARGET_TIME ) {
                Player->jump_IsActive = false;
            }
        }
        if( Player->jump_doDampen ) {
            if( Player->Velocity.y > 0.0f ) {
                Player->Velocity.y *= 0.75f;
            } else {
                Player->jump_doDampen = false;
            }
        }
        
        if( !lEdgeGrab_IsActive ) {
            flo32 maxSpeed = PLAYER_WALK_SPEED;
            flo32 friction = PLAYER_WALK_FRICTION;
            
            if( isOnGround ) {
                if( Player->roll_IsActive ) {
                    if( Player->run_IsActive ) {
                        maxSpeed = PLAYER_ROLL_RUN_SPEED;
                        friction = PLAYER_ROLL_RUN_FRICTION;
                    } else {
                        maxSpeed = PLAYER_ROLL_SPEED;
                        friction = PLAYER_ROLL_FRICTION;
                    }
                } else if( Player->crouch_IsActive ) {
                    if( Player->run_IsActive ) {
                        maxSpeed = PLAYER_CROUCH_RUN_SPEED;
                        friction = PLAYER_CROUCH_RUN_FRICTION;
                    } else {
                        maxSpeed = PLAYER_CROUCH_WALK_SPEED;
                        friction = PLAYER_CROUCH_WALK_FRICTION;
                    }
                } else if( Player->run_IsActive ) {
                    maxSpeed = PLAYER_RUN_SPEED;
                    friction = PLAYER_RUN_FRICTION;
                }
            } else {
                if( Player->run_IsActive ) {
                    maxSpeed = PLAYER_RUN_SPEED;
                    friction = PLAYER_AIR_RUN_FRICTION;
                } else {
                    friction = PLAYER_AIR_WALK_FRICTION;
                }
            }
            
            vec2 accel = {};
            accel.x = ( dir.x * maxSpeed - Player->Velocity.x ) * friction;
            
            // TODO: If Player drops from a lEdge and attempts to wall slide, the wall slide does not activate until the next frame.
            if( ( wallSlide_IsActive ) && ( Player->Velocity.y < 0.0f ) ) {
                accel.y = ( -1.0f * PLAYER_WALL_SLIDE_SPEED - Player->Velocity.y ) * PLAYER_WALL_SLIDE_FRICTION;
            } else if( !Player->jump_IsActive ) {
                accel.y = -PLAYER_GRAVITY;
            }
            
            Player->dPos      = accel * ( dt * dt * 0.5f ) + Player->Velocity * dt;
            Player->Velocity += accel * dt;
        }
        
        if( ( Player->roll_IsActive ) || ( Player->crouch_IsActive ) ) {
            Player->Height = PLAYER_HEIGHT_CROUCHING;
        } else {
            Player->Height = PLAYER_HEIGHT_STANDING;
        }
        
        Player->Draw_id = DrawID_default;
        if( wallSlide_IsActive ) {
            Player->Draw_id = DrawID_wallSlide;
            if( canWallSlideLeft ) {
                Player->Draw_faceLeft = true;
            }
            if( canWallSlideRight ) {
                Player->Draw_faceLeft = false;
            }
        } else if( Player->roll_IsActive ) {
            Player->Draw_id = DrawID_roll;
        } else if( !Player->fire_IsActive ) {
            if( dir.x < 0.0f ) {
                Player->Draw_faceLeft = true;
            }
            if( dir.x > 0.0f ) {
                Player->Draw_faceLeft = false;
            }
        }
    }
    
    { // CURRENT PLATFORM
        int32 CurrentPlatform = getCurrentPlatform( AppState );
        if( ( CurrentPlatform > -1 ) && ( ( uint32 )CurrentPlatform != Player->CurrentPlatform ) ) {
            Player->ChangePlatform  = true;
            Player->CurrentPlatform = CurrentPlatform;
        }
    }
}

internal void
damagePlayer( APP_STATE * AppState, uint32 damage ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    uint32 dmg = minValue( Player->health, damage );
    Player->health -= dmg;
    
    flo32 maxTime = PLAYER_SHOW_DAMAGE_CYCLE_TARGET_TIME * 3.0f;
    Player->showDamage_timer = maxTime - 0.0001f;
}

internal void
killPlayer( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    Player->IsDead       = true;
    Player->IsDead_Timer = 0.0f;
    
    Player->Height = PLAYER_HEIGHT_CROUCHING;
    
    Player->jump_IsActive           = false;
    Player->jump_doDampen           = false;
    Player->run_isEnabled           = false;
    Player->run_IsActive            = false;
    Player->lEdgeGrabLeft_IsActive  = false;
    Player->lEdgeGrabRight_IsActive = false;
    Player->crouch_IsActive         = false;
    Player->fire_IsActive           = false;
    Player->fire_doHitScan          = false;
    Player->roll_IsActive           = false;
    Player->Use_IsActive            = false;
}

internal void
updatePlayer( APP_STATE * AppState, flo32 dt ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    if( Player->IsDead ) {
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
    
    boo32 lEdgeGrab_IsActive = ( Player->lEdgeGrabLeft_IsActive ) || ( Player->lEdgeGrabRight_IsActive );
    if( !lEdgeGrab_IsActive ) {
        vec2 Player_dim = Vec2( PLAYER_WIDTH, Player->Height );
        
        if( Player->IsDead ) {
            Player_dim = PLAYER_DEAD_DIM;
        }
        
        COLLISION_RESULT coll = UpdatePlayerCollision( AppState, Player->Position, Player->Velocity, Player->dPos, Player_dim );
        Player->Position = coll.Position;
        Player->Velocity = coll.Velocity;
        
        Player->Velocity.y = maxValue( Player->Velocity.y, PLAYER_FALL_MAX_SPEED );
    }
}

internal void
DrawPlayer( RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, DRAW_STATE * Draw ) {
    PLAYER_STATE * Player = Draw->Player;
    
    if( Player->IsDead ) {
        flo32 t = clamp01( Player->IsDead_Timer / PLAYER_DEAD_TARGET_TIME );
        
        vec2 P = Player->Position + Vec2( 0.0f, PLAYER_HEIGHT_STANDING * 0.5f );
        vec2 Q = Player->Position + Vec2( 0.0f, PLAYER_WIDTH * 0.5f );
        
        flo32 radians = lerp( 0.0f, t, PI * 0.5f );
        vec2  pos     = lerp( P, t, Q );
        
        vec2  dim = Vec2( PLAYER_WIDTH, PLAYER_HEIGHT_STANDING );
        orect R   = orectCD( pos, dim, radians );
        
        vec4  Color = toColor( 0, 150, 0 );
        
        DrawORect( Pass_Game, R, Color );
    } else {
        flo32 a = 1.0f;
        { // Draw Player
            vec4  Color = toColor( 0, 150, 0 );
            
            { // show damage
                if( Player->showDamage_timer > 0.0f ) {
                    flo32 halfTime = PLAYER_SHOW_DAMAGE_CYCLE_TARGET_TIME * 0.5f;
                    uint32 cycle = ( uint32 )( Player->showDamage_timer / halfTime );
                    if( ( cycle % 2 ) == 1 ) {
                        a = 0.15f;
                    }
                }
            }
            
            Color *= a;
            
            switch( Player->Draw_id ) {
                case DrawID_wallSlide: {
                    vec2 pos   = Player->Position + Vec2( 0.0f, Player->Height * 0.5f );
                    vec2 scale = Vec2( PLAYER_WIDTH, Player->Height ) * 0.5f;
                    if( Player->Draw_faceLeft ) {
                        scale.x = -scale.x;
                    }
                    
                    DrawModel( Pass_Game, ModelID_Player_wallSlide, TextureID_whiteTexture, pos, scale, Color );
                } break;
                
                case DrawID_roll: {
                    vec2  pos    = Player->Position + Vec2( 0.0f, PLAYER_ROLL_RADIUS );
                    flo32 Radius = PLAYER_ROLL_RADIUS;
                    DrawCircle( Pass_Game, pos, Radius, Color );
                } break;
                
                case DrawID_default: {
                    rect R = rectBCD( Player->Position, Vec2( PLAYER_WIDTH, Player->Height ) );
                    DrawRect( Pass_Game, R, Color );
                } break;
            }
            
            // TODO: Clean up this code, so only one indicator is drawn at any given time. Currently, Save Indicator is overdrawing the Use Indicator.
            { // Draw use
                if( ( Player->Use_IsActive ) && ( Player->Use_t > 0.0f ) ) {
                    vec2 P   = Player->Position + Vec2( 0.0f, Player->Height + ( TILE_HEIGHT * 0.25f ) );
                    vec2 dim = Vec2( PLAYER_WIDTH * 0.9f, TILE_HEIGHT * 0.75f );
                    
                    rect R = rectBCD( P, dim );
                    DrawRect       ( Pass_Game, R, COLOR_GRAY( 0.05f  ) );
                    DrawRectOutline( Pass_Game, R, COLOR_GRAY( 0.15f  ) );
                    
                    rect S = addRadius( R, -TILE_WIDTH * 0.125f );
                    S.Right = lerp( S.Left, Player->Use_t, S.Right );
                    DrawRect( Pass_Game, S, COLOR_YELLOW );
                }
                
                //vec2 PlayerP = GetPlayerUseP( *Player );
                //DrawPoint( Pass_Game, PlayerP, TILE_DIM * 0.5f, COLOR_BLUE );
            }
            
            { // Draw enter
                if( Player->Draw_EnterIndicator ) {
                    vec2 P   = Player->Position + Vec2( 0.0f, Player->Height + ( TILE_HEIGHT * 0.25f ) );
                    vec2 dim = Vec2( PLAYER_WIDTH * 0.9f, TILE_HEIGHT * 0.75f );
                    
                    rect R = rectBCD( P, dim );
                    DrawRect       ( Pass_Game, R, COLOR_GRAY( 0.05f  ) );
                    DrawRectOutline( Pass_Game, R, COLOR_GRAY( 0.15f  ) );
                    
                    DrawString( Pass_Game, Draw->font, "ENTER", getCenter( R ), textAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
                }
            }
            
            { // DRAW SAVE INDICATOR
                if( Player->Draw_SaveIndicator ) {
                    vec2 P   = Player->Position + Vec2( 0.0f, Player->Height + ( TILE_HEIGHT * 0.25f ) );
                    vec2 Dim = Vec2( PLAYER_WIDTH * 2.5f, TILE_HEIGHT * 1.15f );
                    
                    rect R = rectBCD( P, Dim );
                    DrawRect       ( Pass_Game, R, COLOR_GRAY( 0.05f  ) );
                    DrawRectOutline( Pass_Game, R, COLOR_GRAY( 0.15f  ) );
                    
                    if( ( Player->Use_IsActive ) && ( Player->Use_t > 0.0f ) ) {
                        rect S = R;
                        S.Right = lerp( R.Left, Player->Use_t, R.Right );
                        DrawRect( Pass_Game, S, COLOR_YELLOW );
                    }
                    
                    DrawString( Pass_Game, Draw->font, "SAVE PROGRESS", getCenter( R ), textAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
                }
            }
        }
        
        { // Draw wall jump indicator
            if( Player->Draw_wallJump ) {
                vec2 pos = Player->Position;
                vec2 dim = Vec2( PLAYER_WIDTH, TILE_HEIGHT * 0.75f );
                
                flo32 offset = 0.0f;
                if( Player->Draw_id == DrawID_wallSlide ) {
                    dim.x  = PLAYER_WIDTH *  5.0f / 8.0f;
                    offset = PLAYER_WIDTH * -1.5f / 8.0f;
                    if( Player->Draw_faceLeft ) {
                        offset = -offset;
                    }
                }
                pos.x += offset;
                
                dim += Vec2( TILE_WIDTH, 0.0f );
                
                rect R = rectCD( pos, dim );
                DrawRect( Pass_Game, R, COLOR_YELLOW );
            }
        }
        
        if( !Player->roll_IsActive ) { // aim dir
            vec2 dim   = Vec2( 0.95f, 0.1f );
            vec4 Color = toColor( 87, 63, 7 );
            
            Color *= a;
            
            if( Player->fire_IsActive ) {
                vec2 P = Player->Position + Vec2( 0.0f, Player->Height * 0.75f );
                vec2 V = Player->aim_dir;
                
                orect R = orectXD( P - V * 0.1f, V, dim );
                DrawORect( Pass_Game, R, Color );
            } else {
                flo32 radians = -PI * 1.0f / 8.0f;
                if( Player->crouch_IsActive ) {
                    radians *= 0.5f;
                }
                
                vec2  P = Player->Position + Vec2( 0.0f, Player->Height * 0.6f );
                vec2  V = ToDirection2D( radians );
                if( Player->Draw_faceLeft ) {
                    V.x = -V.x;
                }
                
                orect R = orectCXD( P + V * 0.2f, V, dim );
                DrawORect( Pass_Game, R, Color );
            }
        }
        
        if( Player->fire_IsActive ) {
            vec2 P = Player->fire_pos;
            vec2 N = Player->fire_dir;
            
            vec2 A = P + N * Player->fire_lo;
            vec2 B = P + N * Player->fire_hi;
            DrawLine( Pass_Game, A, B, COLOR_WHITE );
        }
    }
    
    vec2 atPos = getTL( Draw->App_Bound );
    { // health bar
        vec2 bPos = getTL( Draw->App_Bound ) + Vec2( 32.0f, -24.0f );
        vec2 dim    = Vec2( 160.0f, 42.0f );
        vec2 margin = Vec2( 3.0f, 3.0f );
        
        rect R = rectTLD( bPos, dim );
        DrawRect       ( Pass_UI, R, COLOR_GRAY( 0.02f ) );
        DrawRectOutline( Pass_UI, R, COLOR_GRAY( 0.1f  ) );
        
        atPos = getTR( R );
        
        vec2 bar_dim = Vec2( dim.x - ( margin.x * 2.0f ), dim.y * 0.45f );
        rect S = rectBLD( getBL( R ) + margin, bar_dim );
        DrawRect( Pass_UI, S, COLOR_BLACK );
        
        vec4 outline_Color = COLOR_GRAY( 0.1f );
        if( Player->heal_isDisabled ) {
            flo32 t = clamp01( Player->heal_disableTimer / PLAYER_HEAL_DISABLE_CYCLE_TARGET_TIME );
            
            vec4 lo = Vec4( 0.2f, 0.1f, 0.1f, 1.0f );
            vec4 hi = Vec4( 1.0f, 0.1f, 0.1f, 1.0f );
            
            outline_Color = lerp( hi, t, lo );
        } else {
            flo32 t = clamp01( Player->heal_warmupTimer / PLAYER_HEAL_WARMUP_TARGET_TIME );
            
            flo32 c = lerp( 0.1f, t, 0.6f );
            outline_Color = COLOR_GRAY( c );
        }
        DrawRectOutline( Pass_UI, S, outline_Color );
        
        flo32 t = clamp01( ( flo32 )Player->health / ( flo32 )PLAYER_HEALTH );
        vec2  hp_dim = Vec2( bar_dim.x * t, bar_dim.y );
        rect  T = rectBLD( getBL( S ), hp_dim );
        DrawRect( Pass_UI, T, COLOR_RED );
        
        DrawString( Pass_UI, Draw->font, "HP:", getTL( R ) + Vec2( 8.0f, -2.0f ), textAlign_TopLeft, Vec2( 1.1f, 1.1f ), COLOR_WHITE );
    }
    
    { // grenade indicator
        atPos.x += 12.0f;
        vec2 dim = Vec2( 42.0f, 42.0f );
        
        rect R = rectTLD( atPos, dim );
        DrawRect       ( Pass_UI, R, COLOR_GRAY( 0.02f ) );
        DrawRectOutline( Pass_UI, R, COLOR_GRAY( 0.1f  ) );
        
        flo32 targetTime = PLAYER_GRENADE_COOLDOWN_TARGET_TIME * 0.75f;
        flo32 t = 1.0f;
        if( Player->GrenadeThrow_CooldownTimer < 0.0f ) {
            t = 1.0f - clamp01( Player->GrenadeThrow_CooldownTimer / -0.5f );
        } else if( Player->GrenadeThrow_CooldownTimer > targetTime ) {
            flo32 denom = PLAYER_GRENADE_COOLDOWN_TARGET_TIME - targetTime;
            t = 1.0f - clamp01( ( Player->GrenadeThrow_CooldownTimer - targetTime ) / ( denom ) );
        }
        vec4 background_Color_lo = COLOR_GRAY( 0.1f  );
        vec4 background_Color_hi = COLOR_GRAY( 0.35f );
        vec4 background_Color = lerp( background_Color_lo, t, background_Color_hi );
        
        flo32 tA = clamp01( Player->GrenadeThrow_CooldownTimer / PLAYER_GRENADE_COOLDOWN_TARGET_TIME );
        rect S = R;
        S.Top = lerp( S.Bottom, 1.0f - tA, S.Top );
        DrawRect( Pass_UI, S, background_Color );
        
        vec4 Color = GRENADE_COLOR;
        if( Player->GrenadeThrow_CooldownTimer > 0.0f ) {
            Color = COLOR_GRAY( 0.2f );
        }
        
        flo32 Radius = 10.0f;
        DrawCircle       ( Pass_UI, getCenter( R ), Radius, Color );
        DrawCircleOutline( Pass_UI, getCenter( R ), Radius, COLOR_BLACK );
    }
}

internal void
DrawPlayerT( RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, DRAW_STATE * Draw ) {
    PLAYER_STATE * Player = Draw->Player;
    
    if( Player->IsDead ) {
        flo32 t = clamp01( Player->IsDead_Timer / PLAYER_DEAD_TARGET_TIME );
        
        vec2 P = Player->Position + Vec2( 0.0f, PLAYER_HEIGHT_STANDING * 0.5f );
        vec2 Q = Player->Position + Vec2( 0.0f, PLAYER_WIDTH * 0.5f );
        
        flo32 radians = lerp( 0.0f, t, PI * 0.5f );
        vec2  pos     = lerp( P, t, Q );
        
        vec2  dim = Vec2( PLAYER_WIDTH, PLAYER_HEIGHT_STANDING );
        orect R   = orectCD( pos, dim, radians );
        
        vec4  Color = toColor( 0, 150, 0 );
        
        DrawORect( Pass_Game, R, Color );
    } else {
        flo32 a = 1.0f;
        { // Draw Player
            vec4  Color = toColor( 0, 150, 0 );
            
            { // show damage
                if( Player->showDamage_timer > 0.0f ) {
                    flo32 halfTime = PLAYER_SHOW_DAMAGE_CYCLE_TARGET_TIME * 0.5f;
                    uint32 cycle = ( uint32 )( Player->showDamage_timer / halfTime );
                    if( ( cycle % 2 ) == 1 ) {
                        a = 0.15f;
                    }
                }
            }
            
            Color *= a;
            
            DrawCircle( Pass_Game, Player->Position, PLAYER_HALF_WIDTH, Color );
            
            { // Draw use
                if( ( Player->Use_IsActive ) && ( Player->Use_t > 0.0f ) ) {
                    vec2 P   = Player->Position + Vec2( 0.0f, PLAYER_HALF_WIDTH + ( TILE_HEIGHT * 0.25f ) );
                    vec2 dim = Vec2( PLAYER_WIDTH * 0.9f, TILE_HEIGHT * 0.75f );
                    
                    rect R = rectBCD( P, dim );
                    DrawRect       ( Pass_Game, R, COLOR_GRAY( 0.05f  ) );
                    DrawRectOutline( Pass_Game, R, COLOR_GRAY( 0.15f  ) );
                    
                    rect S = addRadius( R, -TILE_WIDTH * 0.125f );
                    S.Right = lerp( S.Left, Player->Use_t, S.Right );
                    DrawRect( Pass_Game, S, COLOR_YELLOW );
                }
            }
        }
        
        vec2 dim   = Vec2( 0.8f, 0.1f );
        vec4 Color = toColor( 87, 63, 7 );
        
        Color *= a;
        
        if( Player->fire_IsActive ) {
            vec2 P = Player->Position;
            vec2 V = Player->aim_dir;
            
            orect R = orectXD( P - V * 0.1f, V, dim );
            DrawORect( Pass_Game, R, Color );
        } else {
            flo32 radians = ( PI * 18.0f / 32.0f ) + ToRadians( Player->FaceDir );
            
            vec2  P = Player->Position + Player->FaceDir * ( PLAYER_RADIUS * 0.75f );
            orect R = orectCD( P, dim, radians );
            DrawORect( Pass_Game, R, Color );
        }
        
        if( Player->fire_IsActive ) {
            vec2 P = Player->fire_pos;
            vec2 N = Player->fire_dir;
            
            vec2 A = P + N * Player->fire_lo;
            vec2 B = P + N * Player->fire_hi;
            DrawLine( Pass_Game, A, B, COLOR_WHITE );
        }
    }
    
    { // health bar
        vec2 bPos = getTL( Draw->App_Bound ) + Vec2( 32.0f, -24.0f );
        vec2 dim    = Vec2( 160.0f, 42.0f );
        vec2 margin = Vec2( 3.0f, 3.0f );
        
        rect R = rectTLD( bPos, dim );
        DrawRect       ( Pass_UI, R, COLOR_GRAY( 0.02f ) );
        DrawRectOutline( Pass_UI, R, COLOR_GRAY( 0.1f  ) );
        
        vec2 bar_dim = Vec2( dim.x - ( margin.x * 2.0f ), dim.y * 0.45f );
        rect S = rectBLD( getBL( R ) + margin, bar_dim );
        DrawRect( Pass_UI, S, COLOR_BLACK );
        
        vec4 outline_Color = COLOR_GRAY( 0.1f );
        if( Player->heal_isDisabled ) {
            flo32 t = clamp01( Player->heal_disableTimer / PLAYER_HEAL_DISABLE_CYCLE_TARGET_TIME );
            
            vec4 lo = Vec4( 0.2f, 0.1f, 0.1f, 1.0f );
            vec4 hi = Vec4( 1.0f, 0.1f, 0.1f, 1.0f );
            
            outline_Color = lerp( hi, t, lo );
        } else {
            flo32 t = clamp01( Player->heal_warmupTimer / PLAYER_HEAL_WARMUP_TARGET_TIME );
            
            flo32 c = lerp( 0.1f, t, 0.6f );
            outline_Color = COLOR_GRAY( c );
        }
        DrawRectOutline( Pass_UI, S, outline_Color );
        
        flo32 t = clamp01( ( flo32 )Player->health / ( flo32 )PLAYER_HEALTH );
        vec2  hp_dim = Vec2( bar_dim.x * t, bar_dim.y );
        rect  T = rectBLD( getBL( S ), hp_dim );
        DrawRect( Pass_UI, T, COLOR_RED );
        
        DrawString( Pass_UI, Draw->font, "HP:", getTL( R ) + Vec2( 8.0f, -2.0f ), textAlign_TopLeft, Vec2( 1.1f, 1.1f ), COLOR_WHITE );
    }
}

internal void
FinalizePlayer( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    Player->Draw_EnterIndicator = false;
    Player->Draw_wallJump = false;
    Player->Use_t = 0.0f;
    Player->ChangePlatform = false;
    Player->Draw_SaveIndicator = false;
    
    if( ( !Player->IsDead ) && ( Player->health == 0 ) ) {
        killPlayer( AppState );
    }
    
    if( !Player->IsDead ) {
        // lEdge grab
        if( Player->Velocity.y < 0.0f ) {
            vec2 P = Player->Position + Vec2( 0.0f, Player->Height );
            
            COLLISION_STATE * Collision = &AppState->Collision;
            
            if( Player->control_xDir > 0.0f ) {
                for( uint32 iBound = 0; iBound < Collision->nLedgeGrabLeft; iBound++ ) {
                    rect R = Collision->lEdgeGrabLeft[ iBound ];
                    if( IsInBound( P, R ) ) {
                        Player->crouch_IsActive  = false;
                        Player->roll_IsActive    = false;
                        Player->Height           = PLAYER_HEIGHT_STANDING;
                        
                        Player->lEdgeGrabLeft_IsActive = true;
                        Player->Velocity = {};
                        Player->Position = getTR( R ) + Vec2( -( PLAYER_HALF_WIDTH + COLLISION_EPSILON_OFFSET ), -Player->Height );
                    }
                }
            }
            
            if( Player->control_xDir < 0.0f ) {
                for( uint32 iBound = 0; iBound < Collision->nLedgeGrabRight; iBound++ ) {
                    rect R = Collision->lEdgeGrabRight[ iBound ];
                    if( IsInBound( P, R ) ) {
                        Player->crouch_IsActive  = false;
                        Player->roll_IsActive    = false;
                        Player->Height           = PLAYER_HEIGHT_STANDING;
                        
                        Player->lEdgeGrabRight_IsActive = true;
                        Player->Velocity = {};
                        Player->Position = getTL( R ) + Vec2( ( PLAYER_HALF_WIDTH + COLLISION_EPSILON_OFFSET ), -Player->Height );
                    }
                }
            }
        }
    }
}