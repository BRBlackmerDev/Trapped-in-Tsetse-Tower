
//----------
// SAVE LEVEL
//----------

internal void
getFileList( APP_STATE * AppState, STRING_BUFFER * list, char * filetag ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    
    char workingDir[ 512 + 1 ] = {};
    GetCurrentDirectory( 512, workingDir );
    SetCurrentDirectory( ROOM_SAVE_DIRECTORY );
    
    uint32 length = ( uint32 )strlen( filetag );
    reset( list );
    
    WIN32_FIND_DATA file_data   = {};
    HANDLE          file_handle = FindFirstFile( "*", &file_data ); // .
    FindNextFile( file_handle, &file_data ); // ..
    while( FindNextFile( file_handle, &file_data ) ) {
        char * ptr = file_data.cFileName;
        
        uint32 len = ( uint32 )strlen( ptr );
        if( len > length ) {
            while( *ptr ) { ptr++; }   ptr -= length;
            
            if( matchSegment( filetag, ptr, length ) ) {
                ptr--;
                ptr[ 0 ] = 0;
                
                char * status = add( list, file_data.cFileName );
                if( !status ) {
                    globalVar_debugSystem.errorOccurred = true;
                    char str[ 512 ] = {};
                    sprintf( str, "ERROR! FILE LIST ran out of space in STRING_BUFFER for storing filenames! Max Count = %u", list->nStrMax );
                    CONSOLE_STRING( str );
                }
            }
        }
    }
    
    SetCurrentDirectory( workingDir );
}

#define VERSION__PLAYER_SPAWN  ( 1 )
#define FILETAG__PLAYER_SPAWN  ( "PLYRSPWN" )
internal void
savePlayerSpawn( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    uint32 nSpawn = Editor->Player_nSpawnPos;
    
    if( nSpawn > 0 ) {
        uint32 catalog_size = 0;
        catalog_size += sizeof( uint32 ); // nPlayerSpawn
        
        uint32 output_size = sizeof( ENTITY_FILE_HEADER ) + catalog_size;
        output_size += sizeof( vec2 ) * nSpawn;
        
        MEMORY  _output = subArena( TempMemory, output_size );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, nSpawn );
        for( uint32 iSpawn = 0; iSpawn < nSpawn; iSpawn++ ) {
            _writem( output, vec2, Editor->Player_SpawnPos[ iSpawn ] );
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag ); 
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

#define VERSION__NEST  ( 2 )
#define FILETAG__NEST  ( "NEST____" )
internal void
saveNest( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    EDITOR__NEST_STATE * State = &Editor->Nest;
    if( State->nEntity > 0 ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, State->nEntity );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            _writem( output, EDITOR__NEST, State->Entity[ iEntity ] );
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

#define VERSION__SECURE_DOOR  ( 2 )
#define FILETAG__SECURE_DOOR  ( "SECUREDR" )
internal void
saveSecureDoor( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    EDITOR__SECURE_DOOR_STATE * State = &Editor->Secure;
    
    if( State->nEntity > 0 ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, State->nEntity );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            _writem( output, EDITOR__SECURE_DOOR, State->Entity[ iEntity ] );
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

internal void
saveMechDoor( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    EDITOR__MECH_DOOR_STATE * State = &Editor->Mech;
    
    if( State->nEntity > 0 ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, State->nEntity );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            _writem( output, EDITOR__MECH_DOOR, State->Entity[ iEntity ] );
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

#define VERSION__POWER_SWITCH  ( 1 )
#define FILETAG__POWER_SWITCH  ( "POWERSWT" )
internal void
savePowerSwitch( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    EDITOR__POWER_SWITCH_STATE * State = &Editor->power;
    
    if( State->nEntity > 0 ) {
        uint32 catalog_size = 0;
        catalog_size += sizeof( uint32 ); // nEntity
        
        uint32 output_size = sizeof( ENTITY_FILE_HEADER ) + catalog_size;
        output_size += sizeof( EDITOR__POWER_SWITCH ) * State->nEntity;
        
        MEMORY  _output = subArena( TempMemory, output_size );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, State->nEntity );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            _writem( output, EDITOR__POWER_SWITCH, State->Entity[ iEntity ] );
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

internal void
saveRoomStats( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    uint32 catalog_size = 0;
    
    uint32 output_size = sizeof( ENTITY_FILE_HEADER ) + catalog_size;
    output_size += sizeof( ROOM_STATS );
    
    MEMORY  _output = subArena( TempMemory, output_size );
    MEMORY * output = &_output;
    
    writeEntityFileHeader( output, filetag, version );
    
    _writem( output, ROOM_STATS, Editor->stat );
    
    outputFile( Platform, output, SaveDir, filename, filetag );
    _popSize( TempMemory, output->size );
}

#define VERSION__EXIT_EDIT  ( 1 )
#define FILETAG__EXIT_EDIT  ( "EXITEDIT" )
internal void
saveExit( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    EDITOR__EXIT_STATE * State = &Editor->Exit;
    if( State->nEntity > 0 ) {
        uint32 catalog_size = 0;
        catalog_size += sizeof( uint32 ); // State->nEntity
        
        uint32 output_size = sizeof( ENTITY_FILE_HEADER ) + catalog_size;
        output_size += sizeof( EDITOR__EXIT ) * State->nEntity;
        
        MEMORY  _output = subArena( TempMemory, output_size );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, State->nEntity );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            EDITOR__EXIT Entity = State->Entity[ iEntity ];
            _writem( output, EDITOR__EXIT, Entity );
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

#define VERSION__TERRAIN  ( 1 )
#define FILETAG__TERRAIN  ( "TERRAIN_" )
internal void
saveTerrain( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    uint32 nTerrain = 0;
    for( uint32 iRow = 0; iRow < EDITOR__GRID_MAX_HEIGHT; iRow++ ) {
        for( uint32 iCol = 0; iCol < EDITOR__GRID_MAX_WIDTH; iCol++ ) {
            GRID_CELL_TYPE Type = ( GRID_CELL_TYPE )Editor->grid[ iRow ][ iCol ];
            if( Type == CellType_Terrain ) {
                nTerrain++;
            }
        }
    }
    
    if( nTerrain > 0 ) {
        uint32 catalog_size = 0;
        catalog_size += sizeof( uint32 ); // nTerrain
        
        uint32 output_size = sizeof( ENTITY_FILE_HEADER ) + catalog_size;
        output_size += sizeof( UINT32_PAIR ) * nTerrain;
        
        MEMORY  _output = subArena( TempMemory, output_size );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, nTerrain );
        for( uint32 iRow = 0; iRow < EDITOR__GRID_MAX_HEIGHT; iRow++ ) {
            for( uint32 iCol = 0; iCol < EDITOR__GRID_MAX_WIDTH; iCol++ ) {
                GRID_CELL_TYPE Type = ( GRID_CELL_TYPE )Editor->grid[ iRow ][ iCol ];
                if( Type == CellType_Terrain ) {
                    _writem( output, uint32, iCol );
                    _writem( output, uint32, iRow );
                }
            }
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

#define VERSION__ENTITY_COUNT  ( 1 )
#define FILETAG__ENTITY_COUNT  ( "ENTCOUNT" )
internal void
saveEntityCount( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    boo32  canSave = false;
    for( uint32 iType = 0; iType < EntityType_count; iType++ ) {
        if( Editor->nEntity > 0 ) {
            canSave = true;
        }
    }
    
    if( canSave ) {
        uint32 catalog_size = 0;
        catalog_size += sizeof( uint32 ); // nType
        
        uint32 output_size = sizeof( ENTITY_FILE_HEADER ) + catalog_size;
        output_size += ( 4 * EntityType_count );
        output_size += ( sizeof( uint32 ) * EntityType_count );
        
        MEMORY  _output = subArena( TempMemory, output_size );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        ENTITY_TYPE_TAGS;
        
        _writem( output, uint32, EntityType_count );
        for( uint32 iType = 0; iType < EntityType_count; iType++ ) {
            writeSegment( output, EntityTypeTag[ iType ] );
            _writem( output, uint32, Editor->nEntity[ iType ] );
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

#define VERSION__EVENTS_EDIT  ( 1 )
#define FILETAG__EVENTS_EDIT  ( "EVENTSED" )
internal void
SaveEventsForEdit( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename ) {
    char * filetag = FILETAG__EVENTS_EDIT;
    uint32 version = VERSION__EVENTS_EDIT;
    
    if( Editor->nEvent > 0 ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        DATA_TYPE_LABELS;
        DATA_TYPE_SIZES;
        
        _writem( output, uint32, Editor->nEvent );
        for( uint32 iEvent = 0; iEvent < Editor->nEvent; iEvent++ ) {
            EDITOR__EVENT Event = Editor->Event[ iEvent ];
            writeString( output, Event.name );
            _writem( output, boo32, Event.IsActive );
            _writem( output, uint32, Event.nData );
            for( uint32 iData = 0; iData < Event.nData; iData++ ) {
                EDITOR_EVENT__DATA_TYPE Type = ( EDITOR_EVENT__DATA_TYPE )Event.data_Type[ iData ];
                writeSegment( output, dataTypeLabel[ Type ] );
                
                writeString( output, Event.data_name[ iData ] );
                
                uint32 size = dataTypeSize[ Type ];
                uint8 * dest = ( uint8 * )_pushSize( output, size );
                memcpy( dest, Event.data_ptr[ iData ], size );
            }
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

#define VERSION__EVENTS_GAME  ( 1 )
#define FILETAG__EVENTS_GAME  ( "EVENTSGM" )
internal void
SaveEventsForGame( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename ) {
    char * filetag = FILETAG__EVENTS_GAME;
    uint32 version = VERSION__EVENTS_GAME;
    
    if( Editor->nEvent > 0 ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, Editor->nEvent );
        for( uint32 iEvent = 0; iEvent < Editor->nEvent; iEvent++ ) {
            EDITOR__EVENT Event = Editor->Event[ iEvent ];
            
            _writem( output, boo32, Event.IsActive );
            
            _writem( output, uint32,              Event.data_size );
            _writeb( output, Event.data_ptr[ 0 ], Event.data_size );
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

internal void
saveEvents( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename ) {
    SaveEventsForEdit( Platform, Editor, TempMemory, SaveDir, filename );
    SaveEventsForGame( Platform, Editor, TempMemory, SaveDir, filename );
}

internal void
saveBitmap( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename ) {
    uint32 nCell = EDITOR__GRID_MAX_WIDTH * EDITOR__GRID_MAX_HEIGHT;
    
    uint32 xMin = EDITOR__GRID_MAX_WIDTH;
    uint32 xMax = 0;
    uint32 yMin = EDITOR__GRID_MAX_HEIGHT;
    uint32 yMax = 0;
    for( uint32 iCell = 0; iCell < nCell; iCell++ ) {
        uint32 x = iCell % EDITOR__GRID_MAX_WIDTH;
        uint32 y = iCell / EDITOR__GRID_MAX_WIDTH;
        
        if( Editor->grid[ y ][ x ] != 0 ) {
            xMin = minValue( xMin, x );
            xMax = maxValue( xMax, x );
            yMin = minValue( yMin, y );
            yMax = maxValue( yMax, y );
        }
    }
    
    uint32 xCell = ( xMax - xMin ) + 1;
    uint32 yCell = ( yMax - yMin ) + 1;
    
    if( ( xCell > 0 ) && ( yCell > 0 ) ) {
        uint32 margin = 4;
        uint32 xPixel = xCell + margin * 2;
        uint32 yPixel = yCell + margin * 2;
        uint32 nPixel = xPixel * yPixel;
        
        uint32   output_size = sizeof( BMP_HEADER ) + sizeof( uint32 ) * nPixel;
        MEMORY  _output      = subArena( TempMemory, output_size );
        MEMORY * output      = &_output;
        
        BMP_HEADER * header = _pushType( output, BMP_HEADER );
        header->fileType     = 0x4d42;
        header->fileSize     = output_size;
        header->offsetToData = sizeof( BMP_HEADER );
        header->bitmapInfoHeaderSize = 40;
        header->width        = xPixel;
        header->Height       = yPixel;
        header->planes       = 1;
        header->bitsPerPixel = 32;
        
        uint32 * pixel = _pushArray( output, uint32, nPixel );
        for( uint32 iPixel = 0; iPixel < nPixel; iPixel++ ) {
            pixel[ iPixel ] = 0xFF000000;
        }
        
        for( uint32 y = yMin; y <= yMax; y++ ) {
            for( uint32 x = xMin; x <= xMax; x++ ) {
                uint32 i = ( y - yMin + margin ) * xPixel + ( x - xMin ) + margin;
                
                switch( Editor->grid[ y ][ x ] ) {
                    case CellType_Terrain: {
                        pixel[ i ] = TERRAIN_COLOR_U32;
                    } break;
                }
            }
        }
        
        outputFile( Platform, output, SaveDir, filename, "bmp" );
        _popSize( TempMemory, output_size );
    }
}

#define VERSION__COLLISION_TERRAIN  ( 1 )
#define FILETAG__COLLISION_TERRAIN  ( "COLLTERR" )
internal void
saveCollisionTerrain( PLATFORM * Platform, COLLISION_STATE * Collision, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    if( Collision->nEdge > 0 ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        for( uint32 iType = 0; iType < 4; iType++ ) {
            _writem( output, UINT32_PAIR, Collision->Type[ iType ] );
        }
        
        _writem( output, uint32, Collision->nEdge );
        _writeb( output, Collision->Edge, sizeof( vec4 ) * Collision->nEdge );
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

#define VERSION__COLLISION_JUMP     ( 1 )
#define FILETAG__COLLISION_JUMP     ( "COLLJUMP" )
internal void
saveCollisionJump( PLATFORM * Platform, COLLISION_STATE * Collision, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    uint32 nEntity = Collision->nJumpBound;
    uint8 * Entity = ( uint8 * )Collision->jumpBound;
    uint32    size = sizeof( rect ) * nEntity;
    
    if( nEntity > 0 ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, nEntity );
        _writeb( output, Entity, size );
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

#define VERSION__WALL_SLIDE     ( 1 )
#define FILETAG__WALL_SLIDE     ( "WALLSLID" )
internal void
saveWallSlide( PLATFORM * Platform, COLLISION_STATE * Collision, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    uint32 nEntityA = Collision->nWallSlideLeft;
    uint8 * EntityA = ( uint8 * )Collision->wallSlideLeft;
    uint32    sizeA = sizeof( rect ) * nEntityA;
    uint32 nEntityB = Collision->nWallSlideRight;
    uint8 * EntityB = ( uint8 * )Collision->wallSlideRight;
    uint32    sizeB = sizeof( rect ) * nEntityB;
    
    if( ( nEntityA > 0 ) || ( nEntityB > 0 ) ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, nEntityA );
        _writem( output, uint32, nEntityB );
        _writeb( output, EntityA, sizeA );
        _writeb( output, EntityB, sizeB );
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

#define VERSION__JUMPER_BOUND     ( 1 )
#define FILETAG__JUMPER_BOUND     ( "JUMPERBD" )
internal void
saveJumperBound( PLATFORM * Platform, COLLISION_STATE * Collision, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    uint32 nEntity = Collision->nJumperBound;
    uint8 * Entity = ( uint8 * )Collision->JumperBound;
    uint32    size = sizeof( rect ) * nEntity;
    
    if( nEntity > 0 ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, nEntity );
        _writeb( output, Entity, size );
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

#define VERSION__JUMPER_EDGE     ( 1 )
#define FILETAG__JUMPER_EDGE     ( "JUMPERED" )
internal void
saveJumperEdge( PLATFORM * Platform, COLLISION_STATE * Collision, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    uint32 nEntity = Collision->nJumperEdge;
    uint8 * Entity = ( uint8 * )Collision->JumperEdge;
    uint32    size = sizeof( vec4 ) * nEntity;
    
    if( nEntity > 0 ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, nEntity );
        _writeb( output, Entity, size );
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

#define VERSION__PATHS__ACTIVE_DATA    ( 1 )
#define FILETAG__PATHS__ACTIVE_DATA    ( "PTHAC" )
#define VERSION__PATHS__POSITION_DATA  ( 1 )
#define FILETAG__PATHS__POSITION_DATA  ( "PTHPO" )
#define VERSION__PATHS__IN_DATA  ( 1 )
#define FILETAG__PATHS__IN_DATA  ( "PTHIN" )

#define FILETAG_PREFIX__JUMPER  ( "JMP" )

internal void
savePathState( PLATFORM * Platform, PATH_STATE * Path, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag_prefix ) {
    char filetagA[ 12 ] = {};
    char filetagB[ 12 ] = {};
    char filetagC[ 12 ] = {};
    sprintf( filetagA, "%s%s", filetag_prefix, FILETAG__PATHS__ACTIVE_DATA   );
    sprintf( filetagB, "%s%s", filetag_prefix, FILETAG__PATHS__POSITION_DATA );
    sprintf( filetagC, "%s%s", filetag_prefix, FILETAG__PATHS__IN_DATA );
    
    uint32 versionA = VERSION__PATHS__ACTIVE_DATA;
    uint32 versionB = VERSION__PATHS__POSITION_DATA;
    uint32 versionC = VERSION__PATHS__IN_DATA;
    
    if( Path->nNodeLink > 0 ) {
        { // ACTIVE DATA
            MEMORY  _output = subArena( TempMemory );
            MEMORY * output = &_output;
            writeEntityFileHeader( output, filetagA, versionA );
            
            _writem( output, uint32, Path->nNodeLink );
            _writem( output, uint32, Path->nPathLink );
            _writea( output, Path->NodeLink,            UINT32_PAIR,            Path->nNodeLink );
            _writea( output, Path->PathLink_ActiveData, PATH_LINK__ACTIVE_DATA, Path->nPathLink );
            
            outputFile( Platform, output, SaveDir, filename, filetagA );
            _popSize( TempMemory, output->size );
        }
        
        { // POSITION DATA
            MEMORY  _output = subArena( TempMemory );
            MEMORY * output = &_output;
            writeEntityFileHeader( output, filetagB, versionB );
            
            _writem( output, uint32, Path->nPathLink );
            _writea( output, Path->PathLink_PositionData, PATH_LINK__POSITION_DATA, Path->nPathLink );
            
            outputFile( Platform, output, SaveDir, filename, filetagB );
            _popSize( TempMemory, output->size );
        }
        
        { // IN DATA
            MEMORY  _output = subArena( TempMemory );
            MEMORY * output = &_output;
            writeEntityFileHeader( output, filetagC, versionC );
            
            _writem( output, uint32, Path->nNodeLink );
            _writem( output, uint32, Path->nPathLink );
            _writea( output, Path->NodeLinkIn,      UINT32_PAIR,        Path->nNodeLink );
            _writea( output, Path->PathLink_InData, PATH_LINK__IN_DATA, Path->nPathLink );
            
            outputFile( Platform, output, SaveDir, filename, filetagC );
            _popSize( TempMemory, output->size );
        }
    } else {
        if( Platform->doesFileExist( SaveDir, filename, filetagA ) ) {
            Platform->deleteFile( SaveDir, filename, filetagA );
        }
        if( Platform->doesFileExist( SaveDir, filename, filetagB ) ) {
            Platform->deleteFile( SaveDir, filename, filetagB );
        }
        if( Platform->doesFileExist( SaveDir, filename, filetagC ) ) {
            Platform->deleteFile( SaveDir, filename, filetagC );
        }
    }
}

#define VERSION__JUMPER_INIT     ( 1 )
#define FILETAG__JUMPER_INIT     ( "JUMPINIT" )
internal void
saveJumperInit( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    EDITOR__JUMPER_STATE * State = &Editor->Jumper;
    if( State->nEntity > 0 ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, State->nEntity );
        _writea( output, State->Entity, EDITOR__JUMPER, State->nEntity );
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

internal void
saveCollision( PLATFORM * Platform, COLLISION_STATE * Collision, MEMORY * TempMemory, char * SaveDir, char * filename ) {
    saveCollisionTerrain( Platform, Collision, TempMemory, SaveDir, filename, FILETAG__COLLISION_TERRAIN, VERSION__COLLISION_TERRAIN );
    saveCollisionJump( Platform, Collision, TempMemory, SaveDir, filename, FILETAG__COLLISION_JUMP, VERSION__COLLISION_JUMP );
    saveWallSlide( Platform, Collision, TempMemory, SaveDir, filename, FILETAG__WALL_SLIDE, VERSION__WALL_SLIDE );
    saveJumperBound( Platform, Collision, TempMemory, SaveDir, filename, FILETAG__JUMPER_BOUND, VERSION__JUMPER_BOUND );
    saveJumperEdge( Platform, Collision, TempMemory, SaveDir, filename, FILETAG__JUMPER_EDGE, VERSION__JUMPER_EDGE );
    
    savePathState( Platform, &Collision->JumperPath, TempMemory, SaveDir, filename, FILETAG_PREFIX__JUMPER );
}

internal void
setRoomStats( APP_STATE * AppState, MEMORY * TempMemory ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    ROOM_STATS   * stat   = &Editor->stat;
    
    COLLISION_STATE * Collision = &AppState->Collision;
    
    if( !stat->isTopDown ) { // PLAYER SPAWN
        UINT32_PAIR Type = Collision->Type[ 3 ];
        for( uint32 iPos = 0; iPos < Editor->Player_nSpawnPos; iPos++ ) {
            vec2  P = Editor->Player_SpawnPos[ iPos ];
            flo32 MaxY = -1000000.0f;
            
            for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                vec4 Edge = Collision->Edge[ Type.m + iEdge ];
                if( ( P.y >= Edge.P.y ) && ( P.x >= Edge.P.x ) && ( P.x <= Edge.Q.x ) ) {
                    if( Edge.P.y > MaxY ) {
                        MaxY = Edge.P.y;
                    }
                }
            }
            
            Editor->Player_SpawnPos[ iPos ].y = MaxY;
        }
    }
    
    { // Exits
        EDITOR__EXIT_STATE * State = &Editor->Exit;
        stat->nExit = State->nEntity;
        
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            EDITOR__EXIT Entity = State->Entity[ iEntity ];
            rect R = Entity.Bound;
            
            int32 bEdge_Exit  = -1;
            int32 bEdge_Enter = -1;
            for( uint32 iEdge = 0; iEdge < Collision->nJumperEdge; iEdge++ ) {
                vec4 e = Collision->JumperEdge[ iEdge ];
                vec2 P = e.P;
                vec2 Q = e.Q;
                
                if( ( P.y > R.Bottom ) && ( P.y < R.Top ) ) {
                    if( ( P.x <= R.Left ) && ( Q.x >= R.Right ) ) {
                        bEdge_Exit = iEdge;
                    }
                }
            }
            if( bEdge_Exit == -1 ) {
                vec2  origin = getBC( R );
                flo32 t      = FLT_MAX;
                
                for( uint32 iEdge = 0; iEdge < Collision->nJumperEdge; iEdge++ ) {
                    vec4 e = Collision->JumperEdge[ iEdge ];
                    vec2 P = e.P;
                    vec2 Q = e.Q;
                    
                    if( ( P.y < origin.y ) && ( origin.x >= P.x ) && ( origin.x <= Q.x ) ) {
                        flo32 t0 = origin.y - P.y;
                        if( t0 < t ) {
                            t = t0;
                            bEdge_Enter = iEdge;
                        }
                    }
                }
            } else {
                bEdge_Enter = bEdge_Exit;
            }
            
            stat->Exit_iJumperEdge_Exit [ iEntity ] = bEdge_Exit;
            stat->Exit_iJumperEdge_Enter[ iEntity ] = bEdge_Enter;
        }
    }
    
    { // Path links
        PATH_STATE Path = Collision->JumperPath;
        stat->nNodeLink = Path.nNodeLink;
        stat->nPathLink = Path.nPathLink;
    }
    
    { // Jumper
        if( Collision->nJumperEdge > 0 ) {
            EDITOR__JUMPER_STATE * State = &Editor->Jumper;
            State->nEntity = Editor->nEntity[ EntityType_Jumper ];
            for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                EDITOR__JUMPER * Entity = State->Entity + iEntity;
                Entity->iJumperEdge = ( uint8 )RandomU32InRange( 0, Collision->nJumperEdge - 1 );
                
                uint8 flag = 0;
                for( uint32 iExit = 0; iExit < stat->nExit; iExit++ ) {
                    boo32 isValid = false;
                    if( stat->Exit_iJumperEdge_Exit[ iExit ] > -1 ) {
                        isValid = doesPathExist( Collision->JumperPath, TempMemory, Entity->iJumperEdge, stat->Exit_iJumperEdge_Exit[ iExit ] );
                    }
                    if( isValid ) {
                        flag |= ( 1 << iExit );
                    }
                }
                Entity->hasPathToExit = flag;
            }
        }
    }
}

internal void
saveRoom( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    FILE_SAVE_OPEN * save = &Editor->SaveOpen;
    
    AppState->Mode = appMode_Editor;
    
    { // COLLISION
        uint32 nTerrain = 0;
        for( uint32 y = 0; y < EDITOR__GRID_MAX_HEIGHT; y++ ) {
            for( uint32 x = 0; x < EDITOR__GRID_MAX_WIDTH; x++ ) {
                if( Editor->grid[ y ][ x ] == CellType_Terrain ) {
                    nTerrain++;
                }
            }
        }
        if( nTerrain > 0 ) {
            UINT32_PAIR * terrain_pair = _pushArray_clear( TempMemory, UINT32_PAIR, nTerrain );
            
            fillInterior( AppState, TempMemory );
            
            nTerrain = 0;
            for( uint32 y = 0; y < EDITOR__GRID_MAX_HEIGHT; y++ ) {
                for( uint32 x = 0; x < EDITOR__GRID_MAX_WIDTH; x++ ) {
                    if( Editor->grid[ y ][ x ] == CellType_Terrain ) {
                        terrain_pair[ nTerrain++ ] = UInt32Pair( x, y );
                    }
                }
            }
            genTerrainCollision( AppState, terrain_pair, nTerrain, TempMemory );
            
            _popArray( TempMemory, UINT32_PAIR, nTerrain );
            
            saveCollision( Platform, &AppState->Collision, TempMemory, ROOM_SAVE_DIRECTORY, save->current );
            
            if( Editor->stat.isTopDown ) {
                SaveCellGrid( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__CELL_GRID, VERSION__CELL_GRID );
            }
        }
    }
    
    
    
    setRoomStats( AppState, TempMemory );
    
    { // set iEdge for Nests
        COLLISION_STATE    * Collision = &AppState->Collision;
        EDITOR__NEST_STATE * State     = &Editor->Nest;
        
        if( Collision->nJumperEdge > 0 ) {
            for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                EDITOR__NEST * Entity = State->Entity + iEntity;
                
                int32 bEdge = -1;
                for( uint32 iEdge = 0; iEdge < Collision->nJumperEdge; iEdge++ ) {
                    vec4 Edge = Collision->JumperEdge[ iEdge ];
                    
                    if( doesHorSegmentIntersectRect( Entity->Bound, Edge.P, Edge.Q ) ) {
                        bEdge = iEdge;
                    }
                }
                
                if( bEdge == -1 ) {
                    vec2  P = getBC( Entity->Bound );
                    flo32 t = FLT_MAX;
                    
                    for( uint32 iEdge = 0; iEdge < Collision->nJumperEdge; iEdge++ ) {
                        vec4 Edge = Collision->JumperEdge[ iEdge ];
                        
                        if( ( Edge.P.y < P.y ) && ( P.x >= Edge.P.x ) && ( P.x <= Edge.Q.x ) ) {
                            flo32 t0 = P.y - Edge.P.y;
                            if( t0 < t ) {
                                t = t0;
                                bEdge = iEdge;
                            }
                        }
                    }
                    
                }
                Assert( bEdge > -1 );
                
                Entity->iJumperEdge = ( uint32 )bEdge;
            }
        }
    }
    
    saveTerrain( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__TERRAIN, VERSION__TERRAIN );
    savePlayerSpawn( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__PLAYER_SPAWN, VERSION__PLAYER_SPAWN );
    saveExit( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__EXIT_EDIT, VERSION__EXIT_EDIT );
    saveEntityCount( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__ENTITY_COUNT, VERSION__ENTITY_COUNT );
    saveNest( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__NEST, VERSION__NEST );
    saveSecureDoor( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__SECURE_DOOR, VERSION__SECURE_DOOR );
    saveMechDoor( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__MECH_DOOR, VERSION__MECH_DOOR );
    savePowerSwitch( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__POWER_SWITCH, VERSION__POWER_SWITCH );
    saveEvents( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current );
    SaveCheckpoint( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__CHECKPOINT, VERSION__CHECKPOINT );
    
    saveBitmap( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current );
    
    saveRoomStats( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__ROOM_STATS, VERSION__ROOM_STATS );
    
    saveJumperInit( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__JUMPER_INIT, VERSION__JUMPER_INIT );
    //SaveNavMesh( Platform, Editor, TempMemory, ROOM_SAVE_DIRECTORY, save->current, FILETAG__NAV_MESH, VERSION__NAV_MESH );
    
    MEMORY  _output = subArena( TempMemory, 4 );
    MEMORY * output = &_output;
    
    boo32 isValid = Platform->writeFile( ROOM_SAVE_DIRECTORY, save->current, "dat", output->base, ( uint32 )output->size );
    if( isValid ) {
        save->unsavedChanges = false;
        save->recentSave     = true;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to save file: %s.dat", save->current );
        CONSOLE_STRING( str );
        globalVar_debugSystem.errorOccurred = true;
    }
    
    _popSize( TempMemory, 4 );
}

#define VERSION__LAYOUT_EDIT  ( 1 )
#define FILETAG__LAYOUT_EDIT  ( "LAYOUTED" )
internal void
SaveLayoutForEdit( PLATFORM * Platform, LAYOUT_STATE * layout, MEMORY * TempMemory, char * filename ) {
    uint32 nShow = getNShow( layout );
    if( nShow > 0 ) {
        char * SaveDir = ROOM_SAVE_DIRECTORY;
        char * filetag = FILETAG__LAYOUT_EDIT;
        uint32 version = VERSION__LAYOUT_EDIT;
        
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, nShow );
        for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
            LAYOUT_ROOM Room = layout->Room[ iRoom ];
            if( Room.show ) {
                { // write entry data
                    writeString( output, Room.name );
                    _writem( output, vec2, Room.pos );
                }
                
                { // write link data
                    _writem( output, uint32, Room.nExit );
                    for( uint32 iLink = 0; iLink < Room.nExit; iLink++ ) {
                        INT32_PAIR link = Room.Link[ iLink ];
                        _writem( output, int32, link.m );
                        _writem( output, int32, link.n );
                        
                        if( ( link.m > -1 ) && ( link.n > -1 ) ) {
                            LAYOUT_ROOM r0 = layout->Room[ link.m ];
                            writeString( output, r0.name );
                            
                            rect x0 = r0.Exit[ link.n ].Bound;
                            _writem( output, rect, x0 );
                        }
                    }
                }
            }
        }
        
        { // Room to start game
            LAYOUT_ROOM Room = layout->Room[ layout->iRoomToStartGame ];
            if( Room.show ) {
                writeString( output, Room.name );
            } else {
                uint32 findAltRoom = true;
                for( uint32 iRoom = 0; ( findAltRoom ) && ( iRoom < layout->nRoom ); iRoom++ ) {
                    LAYOUT_ROOM r = layout->Room[ iRoom ];
                    if( r.show ) {
                        findAltRoom = false;
                        writeString( output, r.name );
                    }
                }
            }
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    }
}

#define VERSION__EXIT_GAME  ( 1 )
#define FILETAG__EXIT_GAME  ( "EXITGAME" )
internal void
SaveExitsForGame( PLATFORM * Platform, LAYOUT_STATE * layout, MEMORY * TempMemory, char * filename ) {
    char * SaveDir = ROOM_SAVE_DIRECTORY;
    char * filetag = FILETAG__EXIT_GAME;
    uint32 version = VERSION__EXIT_GAME;
    
    MEMORY  _output = subArena( TempMemory );
    MEMORY * output = &_output;
    
    writeEntityFileHeader( output, filetag, version );
    
    uint32 * nShow = _pushType( output, uint32 );
    for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
        LAYOUT_ROOM Room = layout->Room[ iRoom ];
        
        if( Room.show ) {
            writeString( output, Room.name );
            _writem( output, uint32, Room.nExit );
            
            for( uint32 iEntity = 0; iEntity < Room.nExit; iEntity++ ) {
                INT32_PAIR link = Room.Link[ iEntity ];
                
                _writem( output, INT32_PAIR, link );
                if( ( link.m > -1 ) && ( link.n > -1 ) ) {
                    LAYOUT_ROOM r0 = layout->Room[ link.m ];
                    
                    writeString( output, r0.name );
                    _writem( output, uint32, link.n );
                    _writem( output, int32,  r0.Exit_iJumperEdge_Enter[ link.n ] );
                }
            }
            
            ( *nShow )++;
        }
    }
    outputFile( Platform, output, SaveDir, filename, filetag );
    _popSize( TempMemory, output->size );
}

#define VERSION__LAYOUT_ROOM_NAMES  ( 1 )
#define FILETAG__LAYOUT_ROOM_NAMES  ( "LAYOUTNM" )
internal void
SaveRoomNamesForGame( PLATFORM * Platform, LAYOUT_STATE * layout, MEMORY * TempMemory, char * filename ) {
    char * SaveDir = ROOM_SAVE_DIRECTORY;
    char * filetag = FILETAG__LAYOUT_ROOM_NAMES;
    uint32 version = VERSION__LAYOUT_ROOM_NAMES;
    
    MEMORY  _output = subArena( TempMemory );
    MEMORY * output = &_output;
    
    writeEntityFileHeader( output, filetag, version );
    
    uint32 nShow = getNShow( layout );
    
    if( nShow > 0 ) {
        _writem( output, uint32, nShow );
        for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
            LAYOUT_ROOM Room = layout->Room[ iRoom ];
            if( Room.show ) {
                writeString( output, Room.name );
            }
        }
        
        { // Room to start game
            LAYOUT_ROOM Room = layout->Room[ layout->iRoomToStartGame ];
            if( Room.show ) {
                writeString( output, Room.name );
            } else {
                uint32 findAltRoom = true;
                for( uint32 iRoom = 0; ( findAltRoom ) && ( iRoom < layout->nRoom ); iRoom++ ) {
                    LAYOUT_ROOM r = layout->Room[ iRoom ];
                    if( r.show ) {
                        findAltRoom = false;
                        writeString( output, r.name );
                    }
                }
            }
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    }
}

internal void
SaveLayoutForGame( PLATFORM * Platform, LAYOUT_STATE * layout, MEMORY * TempMemory, char * filename ) {
    uint32 nShow = getNShow( layout );
    if( nShow > 0 ) {
        SaveExitsForGame    ( Platform, layout, TempMemory, filename );
        SaveRoomNamesForGame( Platform, layout, TempMemory, filename );
    }
}

internal void
saveLayout( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    LAYOUT_STATE * layout = &AppState->layout;
    FILE_SAVE_OPEN * save = &layout->SaveOpen;
    
    AppState->Mode = appMode_layout;
    
    SaveLayoutForEdit( Platform, layout, TempMemory, save->current );
    SaveLayoutForGame( Platform, layout, TempMemory, save->current );
    
    save->unsavedChanges = false;
    save->recentSave     = true;
}