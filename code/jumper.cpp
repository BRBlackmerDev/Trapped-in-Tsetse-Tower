
internal vec2
getJumperAttackP( JUMPER Jumper ) {
    vec2 result = Jumper.Position + Vec2( 0.0f, JUMPER_HEIGHT * 0.5f );
    return result;
}

internal rect
getJumperBound( JUMPER Entity ) {
    rect result = rectBCD( Entity.Position, JUMPER_DIM );
    return result;
}

internal vec2
getJumperGrenadeP( JUMPER Entity ) {
    vec2 Result = Entity.Position + Vec2( 0.0f, JUMPER_HEIGHT * 0.5f );
    return Result;
}

internal void
stunJumper( JUMPER * Entity ) {
    Entity->stunned_timer = 0.0f;
    Entity->Mode = EntityMode_Stunned;
    Entity->attack_isEnabled = false;
}

internal vec4
getJumperEdge( APP_STATE * AppState, uint32 iJumperEdge ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    Assert( iJumperEdge < Collision->nJumperEdge );
    vec4 Result = Collision->JumperEdge[ iJumperEdge ];
    return Result;
}

internal void
hopToP( JUMPER * Entity, vec2 P ) {
    Entity->Mode = EntityMode_Jump;
    
    flo32 gravity = JUMPER_GRAVITY;
    flo32 xDir   = ( P.x < Entity->Position.x ) ? -1.0f : 1.0f;
    flo32 width  = fabsf( P.x - Entity->Position.x );
    flo32 tA     = ( width / Entity->Move_MaxSpeed ) * 0.5f;
    flo32 Height = 0.5f * tA * tA * gravity;
    
    Entity->jump_xDir      = xDir;
    Entity->jump_xMaxSpeed = Entity->Move_MaxSpeed;
    Entity->Velocity.x = Entity->jump_xDir * Entity->Move_MaxSpeed;
    Entity->Velocity.y = tA * gravity;
    
    Entity->jump_destP = P;
    Entity->jump_HeightP = Vec2( lerp( Entity->Position.x, 0.5f, P.x ), Entity->Position.y + Height );
}

internal void
attackP( JUMPER * Entity, vec2 P ) {
    Entity->Mode = EntityMode_Jump;
    Entity->attack_isEnabled = true;
    
    flo32 minHeight = 2.5f * TILE_HEIGHT;
    flo32 maxHeight = maxValue( Entity->Position.y, P.y );
    
    flo32 yDist = fabsf( Entity->Position.y - P.y );
    if( yDist < minHeight ) {
        maxHeight = minValue( Entity->Position.y + minHeight, P.y + minHeight );
    }
    
    flo32 yMargin = RandomF32InRange( TILE_HEIGHT * 0.5f, TILE_HEIGHT * 2.0f );
    maxHeight += yMargin;
    
    flo32 HeightA = maxHeight - Entity->Position.y;
    flo32 HeightB = maxHeight - P.y;
    flo32 width   = fabsf( Entity->Position.x - P.x );
    
    flo32 gravity = JUMPER_GRAVITY;
    flo32 tA = sqrtf( ( 2.0f * HeightA ) / gravity );
    flo32 tB = sqrtf( ( 2.0f * HeightB ) / gravity );
    flo32 t = tA + tB;
    
    flo32 xDir = ( P.x < Entity->Position.x ) ? -1.0f : 1.0f;
    
    Entity->jump_xDir      = xDir;
    Entity->jump_xMaxSpeed = ( width / t );
    Entity->Velocity.x  = Entity->jump_xDir * ( width / t ) * 2.0f;
    // TODO: clamp to some maximum value?
    Entity->Velocity.y  = tA * gravity;
    
    Entity->jump_destP = P;
    Entity->jump_HeightP = Vec2( lerp( Entity->Position.x, tA / t, P.x ), maxHeight );
}

internal void
jumpToP( JUMPER * Entity, vec2 P ) {
    Entity->Mode = EntityMode_Jump;
    
    flo32 minHeight = 2.5f * TILE_HEIGHT;
    flo32 maxHeight = maxValue( Entity->Position.y, P.y );
    
    flo32 yDist = fabsf( Entity->Position.y - P.y );
    if( yDist < minHeight ) {
        maxHeight = minValue( Entity->Position.y + minHeight, P.y + minHeight );
    }
    
    flo32 yMargin = RandomF32InRange( TILE_HEIGHT * 0.5f, TILE_HEIGHT * 2.0f );
    maxHeight += yMargin;
    
    flo32 HeightA = maxHeight - Entity->Position.y;
    flo32 HeightB = maxHeight - P.y;
    flo32 width   = fabsf( Entity->Position.x - P.x );
    
    flo32 gravity = JUMPER_GRAVITY;
    flo32 tA = sqrtf( ( 2.0f * HeightA ) / gravity );
    flo32 tB = sqrtf( ( 2.0f * HeightB ) / gravity );
    flo32 t = tA + tB;
    
    flo32 xDir = ( P.x < Entity->Position.x ) ? -1.0f : 1.0f;
    
    Entity->jump_xDir      = xDir;
    Entity->jump_xMaxSpeed = ( width / t );
    Entity->Velocity.x = Entity->jump_xDir * ( width / t );
    Entity->Velocity.y = tA * gravity;
    
    Entity->jump_destP = P;
    Entity->jump_HeightP = Vec2( lerp( Entity->Position.x, tA / t, P.x ), maxHeight );
}

internal boo32
PathLinkIsActive( PATH_STATE Path, uint32 iNodeLink, uint32 iPathLink ) {
    UINT32_PAIR NodeLink = Path.NodeLink[ iNodeLink ];
    
    Assert( iPathLink < NodeLink.n );
    PATH_LINK__ACTIVE_DATA Active = Path.PathLink_ActiveData[ NodeLink.m + iPathLink ];
    
    boo32 Result = Active.IsActive;
    return Result;
}

internal boo32
doesPathExist( PATH_STATE Path, MEMORY * TempMemory, uint32 iNodeA, uint32 iNodeB ) {
    uint32 nNode = Path.nNodeLink;
    
#define STACK_MAX_COUNT  ( 128 )
    uint32 nStack = 0;
    uint32 * stack      = _pushArray_clear( TempMemory, uint32, STACK_MAX_COUNT );
    boo32  * wasVisited = _pushArray_clear( TempMemory, boo32,  STACK_MAX_COUNT );
    
    stack[ nStack++ ] = iNodeA;
    
    while( nStack > 0 ) {
        uint32 iNode = stack[ --nStack ];
        wasVisited[ iNode ] = true;
        
        UINT32_PAIR NodeLink = Path.NodeLink[ iNode ];
        
        for( uint32 iLink = 0; iLink < NodeLink.n; iLink++ ) {
            PATH_LINK__ACTIVE_DATA A = Path.PathLink_ActiveData[ NodeLink.m + iLink ];
            if( ( A.IsActive ) && ( !wasVisited[ A.iNode ] ) ) {
                Assert( nStack < STACK_MAX_COUNT );
                stack[ nStack++ ] = A.iNode;
            }
        }
    }
    
    boo32 result = wasVisited[ iNodeB ];
    
    _popArray( TempMemory, boo32,  STACK_MAX_COUNT );
    _popArray( TempMemory, uint32, STACK_MAX_COUNT );
    
    return result;
}

internal void
findPath( APP_STATE * AppState, MEMORY * TempMemory, JUMPER * Entity, uint32 goalNode ) {
    if( Entity->CurrentPlatform != goalNode ) {
        COLLISION_STATE * Collision = &AppState->Collision;
        
        PATH_STATE Path = Collision->JumperPath;
        uint32 nNode = Path.nNodeLink;
        
        boo32 * wasVisited = _pushArray_clear( TempMemory, boo32, nNode );
        flo32 * score      = _pushArray_clear( TempMemory, flo32, nNode );
        int32 * fromNode   = _pushArray_clear( TempMemory, int32, nNode );
        int32 * fromLink   = _pushArray_clear( TempMemory, int32, nNode );
        vec2  * fromP      = _pushArray_clear( TempMemory, vec2,  nNode );
        for( uint32 iter = 0; iter < nNode; iter++ ) {
            score[ iter ] = FLT_MAX;
        }
        
        int32 atNode = Entity->CurrentPlatform;
        score   [ atNode ] = 0.0f;
        fromNode[ atNode ] = -1;
        fromP   [ atNode ] = Entity->Position;  
        
        Entity->nPath = 0;
        while( ( atNode != ( int32 )goalNode ) && ( atNode != -1 ) ) {
            wasVisited[ atNode ] = true;
            
            UINT32_PAIR NodeLink = Path.NodeLink[ atNode ];
            
            vec2 atP = fromP[ atNode ];
            for( uint32 iLink = 0; iLink < NodeLink.n; iLink++ ) {
                PATH_LINK__ACTIVE_DATA   A = Path.PathLink_ActiveData  [ NodeLink.m + iLink ];
                PATH_LINK__POSITION_DATA P = Path.PathLink_PositionData[ NodeLink.m + iLink ];
                
                if( A.IsActive ) {
                    flo32 distA = GetLength( atP     - P.ExitP   );
                    flo32 distB = GetLength( P.ExitP - P.toNodeP );
                    flo32 dist = distA + distB;
                    
                    if( !wasVisited[ A.iNode ] ) {
                        flo32 sco = score[ atNode ] + dist;
                        if( sco < score[ A.iNode ] ) {
                            score   [ A.iNode ] = sco;
                            fromNode[ A.iNode ] = atNode;
                            fromLink[ A.iNode ] = iLink;
                            fromP   [ A.iNode ] = P.toNodeP;
                        }
                    }
                }
            }
            
            int32 nextNode = -1;
            flo32 minScore = FLT_MAX;
            
            for( uint32 iNode = 0; iNode < nNode; iNode++ ) {
                if( ( !wasVisited[ iNode ] ) && ( score[ iNode ] < minScore ) ) {
                    nextNode = iNode;
                    minScore = score[ iNode ];
                }
            }
            
            atNode = nextNode;
        }
        
        Entity->noPath = true;
        if( atNode > -1 ) {
            Entity->noPath = false;
            Assert( score[ goalNode ] < FLT_MAX );
            
            int32 at = goalNode;
            int32 li = -1;
            
            uint32       nPath  = 0;
            UINT32_PAIR * Path0 = _pushArray_clear( TempMemory, UINT32_PAIR, PATH_MAX_COUNT );
            
            while( at > -1 ) {
                Assert( nPath < PATH_MAX_COUNT );
                
                Path0[ nPath++ ] = UInt32Pair( ( uint32 )at, ( uint32 )li );
                
                li = fromLink[ at ];
                at = fromNode[ at ];
            }
            
            vec2 atP = Entity->Position;
            
            Entity->nPath = nPath - 1;
            for( int32 iPath = nPath - 1; iPath > 0; iPath-- ) {
                UINT32_PAIR p = Path0[ iPath ];
                
                UINT32_PAIR NodeLink = Path.NodeLink[ p.m ];
                
                PATH_LINK__ACTIVE_DATA   A = Path.PathLink_ActiveData  [ NodeLink.m + p.n ];
                PATH_LINK__POSITION_DATA P = Path.PathLink_PositionData[ NodeLink.m + p.n ];
                
                PATH_LINK Link = {};
                Link.iNode = A.iNode;
                Link.NodeP = P.toNodeP;
                Link.ExitP = P.ExitP;
                
                Entity->Path[ Entity->nPath - iPath ] = Link;
                
                atP = Link.NodeP;
            }
            
            _popArray( TempMemory, UINT32_PAIR, PATH_MAX_COUNT );
        }
        
        _popArray( TempMemory, boo32,  nNode );
        _popArray( TempMemory, uint32, nNode );
        _popArray( TempMemory, int32,  nNode );
        _popArray( TempMemory, int32,  nNode );
        _popArray( TempMemory, vec2,   nNode );
    } else {
        Entity->noPath = false;
        Entity->nPath  = 0;
    }
}

internal void
updateJumper( APP_STATE * AppState, MEMORY * TempMemory, flo32 dt ) {
    JUMPER_STATE * State  = &AppState->Jumper;
    PLAYER_STATE * Player = &AppState->Player;
    
    if( !Player->roll_IsActive ) { // damage Player
        rect attack_Bound = getJumperAttackBound( *Player );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            JUMPER * Entity = State->Entity + iEntity;
            if( Entity->attack_isEnabled ) {
                vec2 attackP = getJumperAttackP( *Entity );
                if( IsInBound( attackP, attack_Bound ) ) {
                    Entity->attack_isEnabled = false;
                    
                    damagePlayer( AppState, JUMPER_ATTACK_DAMAGE );
                }
            }
        }
    }
    
    { // update Path
        if( Player->ChangePlatform ) {
#define FLANK_EDGE_MAX_COUNT  ( 16 )
            uint32  FlankEdge[ FLANK_EDGE_MAX_COUNT ] = {};
            uint32 nFlankEdge = 0;
            
            COLLISION_STATE * Collision = &AppState->Collision;
            PATH_STATE        Path      = Collision->JumperPath;
            
            UINT32_PAIR NodeLinkIn = Path.NodeLinkIn[ Player->CurrentPlatform ];
            for( uint32 iPathLink = 0; iPathLink < NodeLinkIn.n; iPathLink++ ) {
                PATH_LINK__IN_DATA In = Path.PathLink_InData[ NodeLinkIn.m + iPathLink ];
                if( PathLinkIsActive( Path, In.iNode, In.iPathLink ) ) {
                    Assert( nFlankEdge < FLANK_EDGE_MAX_COUNT );
                    FlankEdge[ nFlankEdge++ ] = In.iNode;
                }
            }
            
            if( nFlankEdge > 0 ) {
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    JUMPER * Entity = State->Entity + iEntity;
                    Entity->DoFlank = false;
                    
                    uint32 DoFlank = RandomU32InRange( 0, 1 );
                    if( DoFlank == 0 ) {
                        Entity->DoFlank   = true;
                        uint32 iFlankEdge = RandomU32InRange( 0, nFlankEdge - 1 );
                        Entity->DoFlank_iJumperEdge = FlankEdge[ iFlankEdge ];
                        
                        // TODO: Check if Jumper is reasonable distance from FlankEdge
                        // TODO: Check if Jumper hasPath to FlankEdge
                    }
                }
            }
        }
        
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            JUMPER * Entity = State->Entity + iEntity;
            
            boo32 canUpdate = ( Entity->Mode == EntityMode_Move );
            if( canUpdate ) {
                uint32 iJumperEdge = Player->CurrentPlatform;
                if( Entity->DoFlank ) {
                    iJumperEdge = Entity->DoFlank_iJumperEdge;
                }
                findPath( AppState, TempMemory, Entity, iJumperEdge );
            }
        }
        
    }
    
    vec2 PlayerP = GetPlayerAttackP( *Player );
    { // can attack Player
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            JUMPER * Entity = State->Entity + iEntity;
            boo32 canAttack = ( Entity->health > 0 ) && ( Entity->Mode == EntityMode_Move );
            
            if( canAttack ) {
                vec2  EntityP = Entity->Position + Vec2( 0.0f, JUMPER_HEIGHT * 0.5f );
                
                vec2 P = EntityP;
                vec2 Q = PlayerP;
                
                { // check dist to Player
                    flo32 maxXDist    = JUMPER_ATTACK_MAX_X_DIST;
                    flo32 posMaxYDist = JUMPER_ATTACK_POS_MAX_Y_DIST;
                    flo32 negMaxYDist = JUMPER_ATTACK_NEG_MAX_Y_DIST;
                    
                    flo32 xDist = fabsf( Q.x - P.x );
                    flo32 yDist = Q.y - P.y;
                    
                    boo32 isValidA = true;
                    if( xDist > maxXDist ) {
                        isValidA = false;
                    }
                    if( yDist > posMaxYDist ) {
                        isValidA = false;
                    }
                    if( yDist < negMaxYDist ) {
                        isValidA = false;
                    }
                    if( ( isValidA ) && ( yDist > 0.0f ) ) {
                        flo32 distSq = GetLengthSq( P - Q );
                        flo32 maxDistSq = maxXDist * maxXDist;
                        if( distSq >= maxDistSq ) {
                            isValidA = false;
                        }
                    }
                    if( !isValidA ) {
                        canAttack = false;
                    }
                }
                
                if( canAttack ) {
                    RAY2 ray = Ray2( P, Q - P );
                    
                    { // COLLISION
                        COLLISION_STATE * Collision = &AppState->Collision;
                        uint32 nEdge = Collision->nEdge;
                        vec4 *  Edge = Collision->Edge;
                        
                        for( uint32 iEdge = 0; iEdge < nEdge; iEdge++ ) {
                            vec4 e = Edge[ iEdge ];
                            
                            RAY2_INTERSECT intersect = DoesIntersect( ray, e.xy, e.zw );
                            if( ( intersect.isValid ) && ( intersect.t < 1.0f ) ) {
                                canAttack = false;
                            }
                        }
                    }
                    
                    { // MECH DOOR
                        MECH_DOOR_STATE * Mech = &AppState->Mech;
                        for( uint32 iMechDoor = 0; iMechDoor < Mech->nEntity; iMechDoor++ ) {
                            MECH_DOOR e = Mech->Entity[ iMechDoor ];
                            if( e.isClosed ) {
                                RAY2_INTERSECT intersect = DoesIntersect( ray, e.Bound_Door );
                                if( ( intersect.isValid ) && ( intersect.t < 1.0f ) ) {
                                    canAttack = false;
                                }
                            }
                        }
                    }
                    
                    { // SECURE DOOR
                        SECURE_DOOR_STATE * Secure = &AppState->Secure;
                        for( uint32 iSecureDoor = 0; iSecureDoor < Secure->nEntity; iSecureDoor++ ) {
                            SECURE_DOOR e = Secure->Entity[ iSecureDoor ];
                            if( e.isClosed ) {
                                RAY2_INTERSECT intersect = DoesIntersect( ray, e.Bound );
                                if( ( intersect.isValid ) && ( intersect.t < 1.0f ) ) {
                                    canAttack = false;
                                }
                            }
                        }
                    }
                    
                    Entity->canAttack = canAttack;
                    if( canAttack ) {
                        vec2 Offset  = Vec2( RandomF32InRange( -1.0f, 1.0f ), RandomF32InRange( -1.0f, 0.05f ) ) * TILE_DIM * 1.5f;
                        attackP( Entity, PlayerP + Offset );
                    }
                }
            }
        }
    }
    
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        JUMPER * Entity = State->Entity + iEntity;
        
        flo32 maxSpeed = Entity->Move_MaxSpeed;
        if( Entity->HitByBullet ) {
            Entity->HitByBullet = false;
            maxSpeed = JUMPER_HIT_BY_BULLET_MOVE_SPEED;
        }
        flo32 friction = JUMPER_MOVE_FRICTION;
        
        flo32 gravity = 0.0f;
        flo32 xDir    = 0.0f;
        switch( Entity->Mode ) {
            case EntityMode_Move: {
                Entity->attack_isEnabled = false;
                
                if( ( Entity->DoFlank ) && ( Entity->CurrentPlatform == Entity->DoFlank_iJumperEdge ) ) {
                    Entity->DoFlank = false;
                }
                
                if( !Entity->noPath ) {
                    flo32 xDest = 0.0f;
                    
                    uint32 TargetPlatform = Player->CurrentPlatform;
                    if( Entity->DoFlank ) {
                        TargetPlatform = Entity->DoFlank_iJumperEdge;
                    }
                    
                    if( Entity->CurrentPlatform == TargetPlatform ) {
                        vec4 Edge = getJumperEdge( AppState, Entity->CurrentPlatform );
                        xDest = clamp( PlayerP.x, Edge.P.x, Edge.Q.x );
                    } else {
                        PATH_LINK Path = Entity->Path[ 0 ];
                        xDest = Path.ExitP.x;
                    }
                    
                    flo32 v = xDest - Entity->Position.x;
                    if( v != 0.0f ) {
                        xDir = v / fabsf( v );
                        
                        Entity->move_xDist = fabsf( v );
                        Entity->move_xDir  = xDir;
                    } else {
                        Entity->move_xDist = 0.0f;
                        Entity->move_xDir  = 0.0f;
                    }
                    
                    Entity->Hop_Timer += dt;
                }
            } break;
            
            case EntityMode_Stunned: {
                Entity->stunned_timer += dt;
                gravity = -JUMPER_GRAVITY;
            } break;
            
            case EntityMode_Delay: {
                Entity->Delay_Timer += dt;
                gravity = -JUMPER_GRAVITY;
            } break;
            
            case EntityMode_Jump: {
                maxSpeed = Entity->jump_xMaxSpeed;
                xDir     = Entity->jump_xDir;
                gravity  = -JUMPER_GRAVITY;
            } break;
        }
        
        vec2 accel = {};
        accel.x = ( xDir * maxSpeed - Entity->Velocity.x ) * friction;
        accel.y = ( gravity );
        
        vec2 dPos = accel * ( dt * dt * 0.5f ) + Entity->Velocity * dt;
        Entity->dPos      = dPos;
        Entity->Velocity += accel * dt;
        
        if( ( Entity->Mode == EntityMode_Jump ) || ( Entity->Mode == EntityMode_Stunned ) || ( Entity->Mode == EntityMode_Delay ) ) {
            COLLISION_RESULT coll = UpdateCollision( AppState, Entity->Position, Entity->Velocity, dPos, JUMPER_DIM );
            Entity->Position = coll.Position;
            Entity->Velocity = coll.Velocity;
        } else {
            Entity->Position += dPos;
        }
    }
}

internal void
DrawJumper( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    JUMPER_STATE * State = Draw->Jumper;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        JUMPER Entity = State->Entity[ iEntity ];
        
        vec4 Color = Entity.Color;
        if( Entity.Mode == EntityMode_Stunned ) {
            Color.r *= 0.5f;
            Color.g *= 0.5f;
            Color.b *= 0.5f;
            
            flo32 Height = JUMPER_HEIGHT * 0.85f;
            vec2 P = Entity.Position + Vec2( 0.0f, Height * 0.5f );
            vec2 scale = Vec2( JUMPER_WIDTH * 1.2f, Height ) * 0.5f;
            DrawModel( Pass, ModelID_Jumper_stunned, TextureID_whiteTexture, P, scale, Color );
        } else {
            if( Entity.attack_isEnabled ) {
                Color.r  = 0.8f;
                Color.g *= 0.5f;
                Color.b *= 0.5f;
            }
            
            rect R = rectBCD( Entity.Position, JUMPER_DIM );
            DrawRect( Pass, R, Color );
            DrawRectOutline( Pass, R, COLOR_BLACK );
        }
        
        //char str[ 4 ] = {};
        //sprintf( str, "%u", iEntity );
        //DrawString( Pass, Draw->font, str, Entity.Position, textAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
        //DrawPoint( Pass, Entity.jump_destP,   Vec2( 0.1f, 0.1f ), COLOR_GREEN );
        //DrawPoint( Pass, Entity.jump_HeightP, Vec2( 0.1f, 0.1f ), COLOR_RED );
    }
}

internal void
DrawJumperBound( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = Draw->Collision;
    uint32 nBound = Collision->nJumperBound;
    rect *  Bound = Collision->JumperBound;
    
    for( uint32 iBound = 0; iBound < nBound; iBound++ ) {
        rect R = Bound[ iBound ];
        DrawRectOutline( Pass, R, COLOR_GREEN );
    }
}

internal void
DrawJumperEdge( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = Draw->Collision;
    
    uint32 nEdge = Collision->nJumperEdge;
    vec4 *  Edge = Collision->JumperEdge;
    
    for( uint32 iEdge = 0; iEdge < nEdge; iEdge++ ) {
        vec4 e = Edge[ iEdge ];
        
        DrawLine( Pass, e.xy, e.zw, COLOR_RED );
        
        char str[ 16 ] = {};
        sprintf( str, "%u", iEdge );
        DrawString( Pass, Draw->font, str, e.P, textAlign_baseLeft, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
    }
}

internal void
DrawJumperPath( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    PLAYER_STATE * Player = Draw->Player;
    JUMPER_STATE * Jumper = Draw->Jumper;
    
    JUMPER * Entity = Jumper->Entity;
    uint32  nEntity = Jumper->nEntity;      
    
    if( Draw->Debug_iEntity > -1 ) {
        Entity  = Jumper->Entity + Draw->Debug_iEntity;
        nEntity = 1;
    }
    
    for( uint32 iEntity = 0; iEntity < nEntity; iEntity++ ) {
        JUMPER e = Entity[ iEntity ];
        
        vec2 prev = e.Position;
        if( e.nPath > 0 ) {
            COLLISION_STATE * Collision = Draw->Collision;
            vec4 * Edge = Collision->JumperEdge;
            
            flo32 Path_dist = 0.0f;
            
            char str[ 16 ] = {};
            for( uint32 iPath = 0; iPath < e.nPath; iPath++ ) {
                PATH_LINK link = e.Path[ iPath ];
                
                Path_dist += GetLength( prev - link.ExitP );
                
                flo32 distA = GetLength( prev - link.ExitP );
                flo32 distB = GetLength( link.ExitP - link.NodeP );
                
                sprintf( str, "%.02f", distA );
                DrawString( Pass, Draw->font, str, link.ExitP, textAlign_BottomCenter, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
                
                sprintf( str, "%.02f", distB );
                DrawString( Pass, Draw->font, str, link.NodeP, textAlign_BottomCenter, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
                
                Path_dist += ( distA + distB );
                
                DrawLine( Pass, prev,       link.ExitP, COLOR_CYAN );
                DrawLine( Pass, link.ExitP, link.NodeP, COLOR_CYAN );
                prev = link.NodeP;
            }
        }
        
        DrawLine( Pass, prev, Player->Position, COLOR_CYAN );
    }
}

internal void
DrawJumperAttack( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    PLAYER_STATE * Player = Draw->Player;
    JUMPER_STATE * State  = Draw->Jumper;
    
    vec2 PlayerP = GetPlayerAttackP( *Player );
    rect PlayerB = getJumperAttackBound ( *Player );
    rect PlayerD = getJumperDisableBound( *Player );
    
    DrawRectOutline( Pass, PlayerB, COLOR_YELLOW );
    DrawRectOutline( Pass, PlayerD, COLOR_RED );
    
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        JUMPER Entity = State->Entity[ iEntity ];
        
        vec2 EntityP = getJumperAttackP( Entity );
        vec4 Color = Entity.canAttack ? COLOR_GREEN : COLOR_RED;
        
        DrawLine( Pass, EntityP, PlayerP, Color );
    }
}

internal void
DrawJumperHP( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    JUMPER_STATE * State  = Draw->Jumper;
    
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        JUMPER Entity = State->Entity[ iEntity ];
        
        vec2 P = Entity.Position + Vec2( 0.0f, JUMPER_HEIGHT * 0.5f );
        
        char str[ 4 ] = {};
        sprintf( str, "%u", Entity.health );
        DrawString( Pass, Draw->font, str, P, textAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
    }
}

internal void
DrawJumperFlank( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    JUMPER_STATE * State  = Draw->Jumper;
    
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        JUMPER Entity = State->Entity[ iEntity ];
        
        if( Entity.DoFlank ) {
            vec2 P = Entity.Position + Vec2( 0.0f, JUMPER_HEIGHT * 0.5f );
            
            char str[ 4 ] = {};
            sprintf( str, "%u", Entity.DoFlank_iJumperEdge );
            DrawString( Pass, Draw->font, str, P, textAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
        }
    }
}

internal void
debugJumperStats( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    if( Draw->Debug_iEntity > -1 ) {
        JUMPER_STATE * Jumper = Draw->Jumper;
        JUMPER Debug_Jumper = Jumper->Entity[ Draw->Debug_iEntity ];
        
        switch( Debug_Jumper.Mode ) {
            case EntityMode_Move:    { DISPLAY_STRING( "Mode: Move"    ); } break;
            case EntityMode_Jump:    { DISPLAY_STRING( "Mode: Jump"    ); } break;
            case EntityMode_Stunned: { DISPLAY_STRING( "Mode: Stunned" ); } break;
            case EntityMode_Delay:   { DISPLAY_STRING( "Mode: Delay"   ); } break;
            default: { InvalidCodePath; } break;
        }
        
        DISPLAY_VALUE( vec2,   Debug_Jumper.Position );
        DISPLAY_VALUE( uint32, Debug_Jumper.TargetPlatform );
        DISPLAY_VALUE( uint32, Debug_Jumper.CurrentPlatform );
        DISPLAY_VALUE( boo32,  Debug_Jumper.DoFlank );
        DISPLAY_VALUE( uint32, Debug_Jumper.DoFlank_iJumperEdge );
        DISPLAY_VALUE( boo32,  Debug_Jumper.noPath );
        DISPLAY_VALUE( uint32, Debug_Jumper.nPath );
        
        PATH_LINK PathLink = Debug_Jumper.Path[ 0 ];
        DISPLAY_VALUE( uint32, PathLink.iNode );
        DISPLAY_VALUE( vec2,   PathLink.NodeP );
        DISPLAY_VALUE( vec2,   PathLink.ExitP );
        
        
    }
}

internal void
FinalizeJumper( APP_STATE * AppState, MEMORY * TempMemory ) {
    JUMPER_STATE * State  = &AppState->Jumper;
    PLAYER_STATE * Player = &AppState->Player;
    
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        JUMPER * Entity = State->Entity + iEntity;
        
        switch( Entity->Mode ) {
            case EntityMode_Move: {
                if( !Entity->noPath ) {
                    uint32 TargetPlatform = Player->CurrentPlatform;
                    if( Entity->DoFlank ) {
                        TargetPlatform = Entity->DoFlank_iJumperEdge;
                    }
                    if( Entity->CurrentPlatform != TargetPlatform ) {
                        PATH_LINK Path = Entity->Path[ 0 ];
                        
                        flo32 distToStartJump   = TILE_WIDTH * 0.25f;
                        flo32 distToStartJumpSq = distToStartJump * distToStartJump;
                        flo32 distSq = GetLengthSq( Path.ExitP - Entity->Position );
                        if( distSq < distToStartJumpSq ) {
                            jumpToP( Entity, Path.NodeP );
                            Entity->TargetPlatform = Path.iNode;
                        }
                    }
                    
                    if( Entity->Hop_Timer >= Entity->Hop_TargetTime ) {
                        Entity->Hop_Timer = 0.0f;
                        Entity->Hop_TargetTime = RandomF32InRange( 0.1f, 0.45f );
                        
                        flo32 margin = 1.0f * TILE_WIDTH;
                        
                        if( Entity->move_xDist >= JUMPER_HOP_MIN_VALID_DIST ) {
                            flo32 min = JUMPER_HOP_MIN_VALID_DIST - margin;
                            flo32 max = minValue( Entity->move_xDist, JUMPER_HOP_MAX_VALID_DIST ) - margin;
                            flo32 dist = RandomF32InRange( min, max );
                            vec2 P = Entity->Position + Vec2( Entity->move_xDir * dist, 0.0f );
                            hopToP( Entity, P );
                        }
                    }
                }
            } break;
            
            case EntityMode_Stunned: {
                if( Entity->stunned_timer >= JUMPER_STUNNED_TARGET_TIME ) {
                    Entity->Mode = EntityMode_Jump;
                    Entity->stunned_timer = 0.0f;
                }
            } break;
            
            case EntityMode_Delay: {
                if( Entity->Delay_Timer >= Entity->Delay_TargetTime ) {
                    Entity->Mode = EntityMode_Jump;
                    Entity->Delay_Timer = 0.0f;
                }
            } break;
            
            case EntityMode_Jump: {
                // TODO: this is not the best way to do this! This performs movement, then backs it up to its point at the start of the frame! Like, what if it hits some Collision, and then it hits a Jumper Bound! We need to move this earlier with the Collision and write data to Finalize it here.
                
                uint32 nBound = AppState->Collision.nJumperBound;
                rect *  Bound = AppState->Collision.JumperBound;
                
                RAY2 ray = {};
                ray.origin = Entity->Position - Entity->dPos;
                ray.vector = Entity->dPos;
                
                for( uint32 iBound = 0; iBound < nBound; iBound++ ) {
                    rect R = Bound[ iBound ];
                    
                    boo32 isValid = false;
                    vec2  fromP   = {};
                    if( IsInBound( Entity->Position, R ) ) {
                        isValid = true;
                        fromP   = Entity->Position;
                    }
                    if( !isValid ) {
                        RAY2_INTERSECT intersect = DoesIntersect( ray, R );
                        if( ( intersect.isValid ) && ( intersect.t < 1.0f ) ) {
                            isValid = true;
                            fromP   = intersect.P;
                        }
                    }
                    if( isValid ) {
                        Entity->Mode            = EntityMode_Move;
                        Entity->Velocity.y      = 0.0f;
                        Entity->CurrentPlatform = iBound;
                        Entity->Move_MaxSpeed   = RandomF32InRange( JUMPER_MOVE_MAX_SPEED_LO, JUMPER_MOVE_MAX_SPEED_HI );
                        
                        vec4 Edge = AppState->Collision.JumperEdge[ iBound ];
                        vec2 P = closestPE( fromP, Edge.P, Edge.Q );
                        Entity->Position = P;
                    }
                }
            } break;
        }
    }
    
    uint32 nEntity = 0;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        JUMPER Entity = State->Entity[ iEntity ];
        
        if( Entity.health > 0 ) {
            State->Entity[ nEntity++ ] = Entity;
        }
    }
    State->nEntity = nEntity;
}

internal void
spawnJumper( APP_STATE * AppState, vec2 pos, int32 health = JUMPER_HEALTH ) {
#if ( !DISABLE_ENEMY_SPAWN )
    flo32 c = RandomF32InRange( 0.35f, 0.6f );
    
    JUMPER Entity = {};
    Entity.Mode     = EntityMode_Jump;
    Entity.Position = pos;
    Entity.Color    = COLOR_GRAY( c );
    Entity.health   = health;
    
    Entity.Hop_TargetTime = RandomF32InRange( 0.1f, 0.45f );
    
    JUMPER_STATE * State = &AppState->Jumper;
    if( State->nEntity < JUMPER_MAX_COUNT ) {
        State->Entity[ State->nEntity++ ] = Entity;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to spawn JUMPER, but JUMPER_STATE was full! MaxCount = %u", JUMPER_MAX_COUNT );
        CONSOLE_STRING( str );
        globalVar_debugSystem.errorOccurred = true;
    }
#endif
}

internal void
spawnDelayedJumper( APP_STATE * AppState, vec2 pos, int32 health = JUMPER_HEALTH ) {
#if ( !DISABLE_ENEMY_SPAWN )
    flo32 c = RandomF32InRange( 0.35f, 0.6f );
    
    JUMPER Entity = {};
    Entity.Mode     = EntityMode_Delay;
    Entity.Delay_TargetTime = RandomF32InRange( JUMPER_SPAWN_DELAY_TARGET_TIME_LO, JUMPER_SPAWN_DELAY_TARGET_TIME_HI );
    
    Entity.Position = pos;
    Entity.Color    = COLOR_GRAY( c );
    Entity.health   = health;
    
    JUMPER_STATE * State = &AppState->Jumper;
    if( State->nEntity < JUMPER_MAX_COUNT ) {
        State->Entity[ State->nEntity++ ] = Entity;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to spawn JUMPER, but JUMPER_STATE was full! MaxCount = %u", JUMPER_MAX_COUNT );
        CONSOLE_STRING( str );
        globalVar_debugSystem.errorOccurred = true;
    }
#endif
}