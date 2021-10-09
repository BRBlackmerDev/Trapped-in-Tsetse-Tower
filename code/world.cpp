
#if 0
internal void
setMechPathLink( APP_STATE * AppState, uint32 iRoom, uint32 iEntity, boo32 isClosed ) {
    WORLD_STATE * World = &AppState->World;
    
    ROOM Room = World->Room[ iRoom ];
    WORLD__DOOR * Door = World->Door + ( Room.MechDoor.m + iEntity );
    Door->isClosed = ( uint8 )isClosed;
    
    PATH_LINK__ACTIVE_DATA * Link = World->PathLink + Room.PathLink.m;
    Link[ Door->iPathLink.m ].IsActive = !isClosed;
    Link[ Door->iPathLink.n ].IsActive = !isClosed;
}
#endif

internal uint8
setJumperHasPathToExit( APP_STATE * AppState, MEMORY * TempMemory, uint32 iRoom, uint32 iEdge ) {
    WORLD_STATE * World = &AppState->World;
    ROOM          Room  = World->Room[ iRoom ];
    
    PATH_STATE Path = {};
    Path.nNodeLink = Room.NodeLink.n;
    Path.nPathLink = Room.PathLink.n;
    Path.NodeLink            = World->NodeLink + Room.NodeLink.m;
    Path.PathLink_ActiveData = World->PathLink + Room.PathLink.m;
    
    uint8 flag = 0;
    for( uint32 iExit = 0; iExit < Room.nExit; iExit++ ) {
        boo32 isValid = false;
        if( Room.Exit_iJumperEdge_Exit[ iExit ] > -1 ) {
            isValid = doesPathExist( Path, TempMemory, iEdge, Room.Exit_iJumperEdge_Exit[ iExit ] );
        }
        if( isValid ) {
            flag |= ( 1 << iExit );
        }
    }
    return flag;
}

internal void
AddJumper( APP_STATE * AppState, MEMORY * TempMemory, uint32 iRoom, uint32 iJumperEdge ) {
    WORLD_STATE * World = &AppState->World;
    if( World->nJumper < WORLD__JUMPER_MAX_COUNT ) {
        ROOM * Room = World->Room + iRoom;
        
        // TODO: Check that Room is not at maxJumper
        
        World->nJumper++;
        if( iRoom < ( World->nRoom - 1 ) ) {
            ROOM * RoomA = World->Room + ( iRoom + 1 );
            uint32 iJumperA = RoomA->Jumper.m;
            for( uint32 iter = World->nJumper - 1; iter > RoomA->Jumper.m; iter-- ) {
                World->Jumper[ iter ] = World->Jumper[ iter - 1 ];
            }
        }
        
        WORLD__JUMPER Jumper = {};
        Jumper.health        = JUMPER_HEALTH;
        
        Assert( iJumperEdge < 255 );
        Jumper.iJumperEdge   = ( uint8 )iJumperEdge;
        Jumper.hasPathToExit = setJumperHasPathToExit( AppState, TempMemory, iRoom, iJumperEdge );
        
        World->Jumper[ Room->Jumper.m + Room->Jumper.n ] = Jumper;
        Room->Jumper.n++;
        
        uint32 nJumper = 0;
        for( uint32 iter = 0; iter < World->nRoom; iter++ ) {
            ROOM * R = World->Room + iter;
            R->Jumper.m = nJumper;
            nJumper += R->Jumper.n;
        }
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to add new WORLD__JUMPER, but WORLD_STATE was full! MaxCount = %u", WORLD__JUMPER_MAX_COUNT );
        CONSOLE_STRING( str );
        globalVar_debugSystem.errorOccurred = true;
    }
}

internal void
TransferJumper( APP_STATE * AppState, MEMORY * TempMemory, uint32 iRoom, uint32 iExit, uint32 iJumper ) {
    WORLD_STATE * World = &AppState->World;
    
    ROOM * RoomA = World->Room + iRoom;
    WORLD__EXIT   Exit   = RoomA->Exit[ iExit ];
    WORLD__JUMPER Jumper = World->Jumper[ RoomA->Jumper.m + iJumper ];
    
    Assert( Jumper.hasPathToExit & ( 1 << iExit ) );
    
    ROOM * RoomB = World->Room + Exit.toRoom_iRoom;
    uint32 iJumperA = RoomA->Jumper.m + iJumper;
    uint32 iJumperB = RoomB->Jumper.m + RoomB->Jumper.n;
    if( iJumperA < iJumperB ) {
        iJumperB--;
    }
    Assert( iJumperA < World->nJumper );
    Assert( iJumperB < World->nJumper );
    
    if( iJumperA < iJumperB ) {
        for( uint32 iter = iJumperA; iter < iJumperB; iter++ ) {
            World->Jumper[ iter ] = World->Jumper[ iter + 1 ];
        }
    }
    if( iJumperA > iJumperB ) {
        for( uint32 iter = iJumperA; iter > iJumperB; iter-- ) {
            World->Jumper[ iter ] = World->Jumper[ iter - 1 ];
        }
    }
    
    RoomA->Jumper.n--;
    RoomB->Jumper.n++;
    
    uint32 nJumper = 0;
    for( uint32 iter = 0; iter < World->nRoom; iter++ ) {
        ROOM * Room = World->Room + iter;
        Room->Jumper.m = nJumper;
        nJumper += Room->Jumper.n;
    }
    
    Jumper.iJumperEdge   = ( uint8 )Exit.toRoom_iJumperEdge;
    Assert( Jumper.iJumperEdge < 255 );
    Jumper.hasPathToExit = setJumperHasPathToExit( AppState, TempMemory, Exit.toRoom_iRoom, Jumper.iJumperEdge );
    World->Jumper[ iJumperB ] = Jumper;
}

internal void
updateWorld( APP_STATE * AppState, MEMORY * TempMemory, flo32 dt ) {
    WORLD_STATE * World = &AppState->World;
    
    { // TRANSFER JUMPER
        WORLD__ROOM_TRANSFER * Transfer = _pushArray_clear( TempMemory, WORLD__ROOM_TRANSFER, World->nRoom );
        uint32 nJumperCameHere[ EXIT_MAX_COUNT ] = {};
        
        uint32 nMaxJumperPerRoom = JUMPER_MAX_COUNT;
        uint32 nAvgJumperPerRoom = nMaxJumperPerRoom / 2;
        // TODO: Adjust outward push based on equilibrium for Entity density.
        
        for( uint32 iRoom = 0; iRoom < World->nRoom; iRoom++ ) {
            if( iRoom != AppState->iRoom ) {
                ROOM Room = World->Room[ iRoom ];
                if( Room.Jumper.n > 0 ) {
                    uint32 nHasPath[ EXIT_MAX_COUNT ] = {};
                    uint32 totalHasPath = 0;
                    
                    WORLD__JUMPER * Jumper = World->Jumper + Room.Jumper.m;
                    for( uint32 iJumper = 0; iJumper < Room.Jumper.n; iJumper++ ) {
                        WORLD__JUMPER J = Jumper[ iJumper ];
                        for( uint32 iExit = 0; iExit < Room.nExit; iExit++ ) {
                            if( J.hasPathToExit & ( 1 << iExit ) ) {
                                nHasPath[ iExit ]++;
                                totalHasPath++;
                            }
                        }
                    }
                    
                    if( totalHasPath > 0 ) {
                        uint32 max = 1;
                        // TODO: Account for Entity density in connecting Room
                        for( uint32 iter = 0; iter < max; iter++ ) {
                            if( ( rand() % 100 ) == 0 ) {
                                flo32 percent[ EXIT_MAX_COUNT ] = {};
                                for( uint32 iExit = 0; iExit < Room.nExit; iExit++ ) {
                                    percent[ iExit ] = ( flo32 )nHasPath[ iExit ] / ( flo32 )totalHasPath;
                                }
                                
                                flo32 atPercent = 0.0f;
                                vec2 range[ EXIT_MAX_COUNT ] = {};
                                for( uint32 iExit = 0; iExit < Room.nExit; iExit++ ) {
                                    if( percent[ iExit ] > 0.0f ) {
                                        range[ iExit ] = Vec2( atPercent, atPercent + percent[ iExit ] );
                                        atPercent += percent[ iExit ];
                                    }
                                }
                                
                                flo32 f = RandomF32();
                                f = maxValue( f, 0.0001f );
                                
                                int32 bExit = -1;
                                for( uint32 iExit = 0; iExit < Room.nExit; iExit++ ) {
                                    if( ( percent[ iExit ] > 0.0f ) && ( f > range[ iExit ].x ) && ( f <= range[ iExit ].y ) ) {
                                        bExit = iExit;
                                    }
                                }
                                
                                Assert( bExit > -1 );
                                
                                WORLD__EXIT Exit = Room.Exit[ bExit ];
                                
                                // TODO: This check 'Exit.isValid' will be removed when the game is done. This is just a sanity check against an Exit because the game is mid-construction and the Exit might be broken or not connected yet.
                                if( Exit.isValid ) {
                                    WORLD__ROOM_TRANSFER * Tran = Transfer + iRoom;
                                    Tran->nJumperToTransfer[ bExit ] = 1;
                                    
                                    if( Exit.toRoom_iRoom == AppState->iRoom ) {
                                        nJumperCameHere[ Exit.toRoom_iExit ]++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        uint32 MaxJumper = 24;
        for( uint32 iRoom = 0; iRoom < World->nRoom; iRoom++ ) {
            ROOM                 Room = World->Room[ iRoom ];
            WORLD__ROOM_TRANSFER Tran = Transfer   [ iRoom ];
            
            for( uint32 iExit = 0; iExit < Room.nExit; iExit++ ) {
                if( ( Tran.nJumperToTransfer[ iExit ] > 0 ) && ( Room.Jumper.n < MaxJumper ) ) {
                    Assert( iRoom != AppState->iRoom );
                    
                    WORLD__EXIT Exit = Room.Exit[ iExit ];
                    Assert( Exit.isValid );
                    
                    WORLD__JUMPER * Jumper = World->Jumper + Room.Jumper.m;
                    
                    int32 bJumper = -1;
                    for( uint32 iJumper = 0; ( bJumper == -1 ) && ( iJumper < Room.Jumper.n ); iJumper++ ) {
                        WORLD__JUMPER J = Jumper[ iJumper ];
                        if( ( 1 << iExit ) & J.hasPathToExit ) {
                            bJumper = iJumper;
                        }
                    }
                    Assert( bJumper > -1 );
                    
                    TransferJumper( AppState, TempMemory, iRoom, iExit, bJumper );
                }
            }
        }
        
        { // JUMPER
            EXIT_STATE * State = &AppState->Exit;
            ROOM                 Room = World->Room[ AppState->iRoom ];
            WORLD__ROOM_TRANSFER Tran = Transfer   [ AppState->iRoom ];
            JUMPER_STATE * JumperS = &AppState->Jumper;
            
            if( JumperS->nEntity < MaxJumper ) {
                for( uint32 iExit = 0; iExit < State->nEntity; iExit++ ) {
                    uint32 nJumper = nJumperCameHere[ iExit ];
                    if( nJumper > 0 ) {
                        WORLD__JUMPER * Jumper = World->Jumper + ( Room.Jumper.m + Room.Jumper.n - nJumper );
                        for( uint32 iJumper = 0; iJumper < nJumper; iJumper++ ) {
                            WORLD__JUMPER J = Jumper[ iJumper ];
                            vec2 P = getEnterP( State->Entity[ iExit ], JUMPER_HEIGHT ) + Vec2( 0.0f, TILE_HEIGHT * 0.5f );
                            spawnDelayedJumper( AppState, P, J.health );
                        }
                    }
                }
            }
        }
        _popArray( TempMemory, WORLD__ROOM_TRANSFER, World->nRoom );
    }
    
    { // UPDATE NEST
        for( uint32 iRoom = 0; iRoom < World->nRoom; iRoom++ ) {
            if( iRoom != AppState->iRoom ) {
                ROOM Room = World->Room[ iRoom ];
                
                WORLD__NEST * NestW = World->Nest + Room.Nest.m;
                for( uint32 iNest = 0; iNest < Room.Nest.n; iNest++ ) {
                    WORLD__NEST * Nest = NestW + iNest;
                    if( Nest->Health > 0 ) {
                        Nest->Timer += dt;
                        
                        flo32 TargetTime = NEST_SPAWN_TARGET_TIME;
                        if( Nest->isAwake ) {
                            TargetTime = NEST_AWAKE_SPAWN_TARGET_TIME;
                        }
                        
                        if( Nest->Timer >= TargetTime ) {
                            Nest->Timer = 0.0f;
                            
                            AddJumper( AppState, TempMemory, iRoom, Nest->iJumperEdge );
                        }
                    }
                }
            }
        }
    }
    
    DISPLAY_VALUE( uint32, AppState->iRoom );
    for( uint32 iRoom = 0; iRoom < World->nRoom; iRoom++ ) {
        ROOM Room = World->Room[ iRoom ];
        //DISPLAY_VALUE( uint32, Room.Jumper.n );
    }
    
    for( uint32 iJumper = 0; iJumper < World->nJumper; iJumper++ ) {
        WORLD__JUMPER Jumper = World->Jumper[ iJumper ];
        //DISPLAY_VALUE( int32, Jumper.health );
        //DISPLAY_VALUE( uint8, Jumper.iJumperEdge );
        //DISPLAY_VALUE( uint8, Jumper.hasPathToExit );
    }
    //DISPLAY_VALUE( uint32, World->nJumper );
    
    for( uint32 iNest = 0; iNest < World->nNest; iNest++ ) {
        WORLD__NEST Nest = World->Nest[ iNest ];
        
        //DISPLAY_VALUE( int32, Nest.Health  );
        //DISPLAY_VALUE( flo32, Nest.Timer   );
        //DISPLAY_VALUE( uint8, Nest.isAwake );
    }
}