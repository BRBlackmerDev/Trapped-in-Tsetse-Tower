
internal void
updatePlayerHitScan( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    if( Player->fire_doHitScan ) {
        Player->fire_doHitScan = false;
        
        uint32  MaxHit           = 1;
        uint32    nHit           = 1;
        BULLET_HIT Hit[ 12 + 1 ] = {};
        for( uint32 iter = 0; iter < ( 12 + 1 ); iter++ ) {
            Hit[ iter ].bHit = Ray2IntersectInit();
            Hit[ iter ].iHit = -1;
        }
        
        RAY2 ray = Ray2( Player->fire_pos, Player->fire_dir );
        
        { // Collision
            COLLISION_STATE * Collision = &AppState->Collision;
            
            for( uint32 iType = 0; iType < 4; iType++ ) {
                UINT32_PAIR t = Collision->Type[ iType ];
                
                vec4 * Edge = Collision->Edge + t.m;
                for( uint32 iEdge = 0; iEdge < t.n; iEdge++ ) {
                    vec4 e = Edge[ iEdge ];
                    
                    RAY2_INTERSECT intersect = DoesIntersect( ray, e.xy, e.zw );
                    if( ( intersect.isValid ) && ( intersect.t < Hit[ nHit - 1 ].bHit.t ) ) {
                        int32 at = nHit - 1;
                        while( ( at > 0 ) && ( Hit[ at - 1 ].bHit.t > intersect.t ) ) {
                            Hit[ at ] = Hit[ at - 1 ];
                            at--;
                        }
                        
                        Hit[ at ] = BulletHit( HitType_Wall, iEdge, intersect );
                    }
                }
            }
        }
        
        { // SECURE DOOR
            SECURE_DOOR_STATE * State = &AppState->Secure;
            
            for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                SECURE_DOOR Entity = State->Entity[ iEntity ];
                if( Entity.isClosed ) {
                    RAY2_INTERSECT intersect = DoesIntersect( ray, Entity.Bound );
                    if( ( intersect.isValid ) && ( intersect.t < Hit[ nHit - 1 ].bHit.t ) ) {
                        int32 at = nHit - 1;
                        while( ( at > 0 ) && ( Hit[ at - 1 ].bHit.t > intersect.t ) ) {
                            Hit[ at ] = Hit[ at - 1 ];
                            at--;
                        }
                        
                        Hit[ at ] = BulletHit( HitType_Wall, iEntity, intersect );
                    }
                }
            }
        }
        
        { // MECH DOOR
            MECH_DOOR_STATE * State = &AppState->Mech;
            
            for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                MECH_DOOR Entity = State->Entity[ iEntity ];
                if( Entity.isClosed ) {
                    RAY2_INTERSECT intersect = DoesIntersect( ray, Entity.Bound_Door );
                    if( ( intersect.isValid ) && ( intersect.t < Hit[ nHit - 1 ].bHit.t ) ) {
                        int32 at = nHit - 1;
                        while( ( at > 0 ) && ( Hit[ at - 1 ].bHit.t > intersect.t ) ) {
                            Hit[ at ] = Hit[ at - 1 ];
                            at--;
                        }
                        
                        Hit[ at ] = BulletHit( HitType_Wall, iEntity, intersect );
                    }
                }
            }
        }
        
        { // Nest
            NEST_STATE * State = &AppState->Nest;
            
            for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                NEST * Entity = State->Entity + iEntity;
                
                if( !Entity->IsDead ) {
                    RAY2_INTERSECT intersect = DoesIntersect( ray, Entity->Bound );
                    if( ( intersect.isValid ) && ( intersect.t < Hit[ nHit - 1 ].bHit.t ) ) {
                        int32 at = nHit - 1;
                        while( ( at > 0 ) && ( Hit[ at - 1 ].bHit.t > intersect.t ) ) {
                            Hit[ at ] = Hit[ at - 1 ];
                            at--;
                        }
                        
                        Hit[ at ] = BulletHit( HitType_Nest, iEntity, intersect );
                    }
                }
            }
        }
        
        { // Jumper
            JUMPER_STATE * State = &AppState->Jumper;
            
            for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                JUMPER * Entity = State->Entity + iEntity;
                
                rect R = getJumperBound( *Entity );
                RAY2_INTERSECT intersect = DoesIntersect( ray, R );
                if( ( intersect.isValid ) && ( intersect.t < Hit[ nHit - 1 ].bHit.t ) ) {
                    nHit = minValue( nHit + 1, MaxHit );
                    
                    int32 at = nHit - 1;
                    while( ( at > 0 ) && ( Hit[ at - 1 ].bHit.t > intersect.t ) ) {
                        Hit[ at ] = Hit[ at - 1 ];
                        at--;
                    }
                    
                    Hit[ at ] = BulletHit( HitType_Jumper, iEntity, intersect );
                }
            }
        }
        
        Player->fire_lo = RandomF32InRange( 3.0f,  5.0f );
        Player->fire_hi = RandomF32InRange( 9.0f, 12.0f );
        
        if( Hit[ 0 ].bHit.isValid  ) {
            Player->fire_lo = minValue( Player->fire_lo, Hit[ nHit - 1 ].bHit.t );
            Player->fire_hi = minValue( Player->fire_hi, Hit[ nHit - 1 ].bHit.t );
            
            for( uint32 iHit = 0; iHit < nHit; iHit++ ) {
                BULLET_HIT H = Hit[ iHit ];
                switch( H.Type ) {
                    case HitType_Wall: {
                        addWallHit( &AppState->Particle, ray.vector, H.bHit );
                    } break;
                    
                    case HitType_Jumper: {
                        JUMPER_STATE * State  = &AppState->Jumper;
                        JUMPER       * Entity = State->Entity + H.iHit;
                        Entity->health = maxValue( Entity->health - 1, 0 );
                        if( Entity->health > 0 ) {
                            addJumperHit ( &AppState->Particle, Player->fire_dir, H.bHit );
                        } else {
                            addJumperKill( &AppState->Particle, Player->fire_dir, H.bHit );
                        }
                        
                        switch( Entity->Mode ) {
                            case EntityMode_Move: {
                                Entity->HitByBullet = true;
                            } break;
                            
                            case EntityMode_Jump: {
                                Entity->HitByBullet = true;
                                
                                vec2  N = GetNormal( ray.vector );
                                flo32 x = N.x * JUMPER_HIT_BY_BULLET_AIR_SPEED_X;
                                flo32 y = N.y * JUMPER_HIT_BY_BULLET_AIR_SPEED_Y;
                                vec2 Dir = Vec2( x, y );
                                Entity->Velocity += Dir;
                            } break;
                            
                            case EntityMode_Stunned: {
                                
                            } break;
                        }
                    } break;
                    
                    case HitType_Nest: {
                        NEST_STATE * State  = &AppState->Nest;
                        NEST       * Entity = State->Entity + iHit;
                        if( Entity->isAwake ) {
                            Entity->health = maxValue( Entity->health - 1, 0 );
                            if( Entity->health > 0 ) {
                                addNestHit ( &AppState->Particle, Player->fire_dir, H.bHit, Entity->Bound );
                            }
                        } else {
                            addNestPing( &AppState->Particle, Player->fire_dir, H.bHit, Entity->Bound );
                        }
                    } break;
                }
            }
        }
    }
}