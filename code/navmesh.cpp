
#define VERSION__NAV_MESH  ( 1 )
#define FILETAG__NAV_MESH  ( "NAVMESH_" )
internal void
SaveNavMesh( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag, uint32 version ) {
    EDITOR__NAV_MESH_STATE * State = &Editor->NavMesh;
    
    if( State->nEntity ) {
        MEMORY  _output = subArena( TempMemory );
        MEMORY * output = &_output;
        
        writeEntityFileHeader( output, filetag, version );
        
        _writem( output, uint32, State->nEntity );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            EDITOR__NAV_MESH_TRI Entity = State->Entity[ iEntity ];
            _writem( output, EDITOR__NAV_MESH_TRI, Entity );
        }
        
        outputFile( Platform, output, SaveDir, filename, filetag );
        _popSize( TempMemory, output->size );
    } else if( Platform->doesFileExist( SaveDir, filename, filetag ) ) {
        Platform->deleteFile( SaveDir, filename, filetag );
    }
}

internal EDITOR__NAV_MESH_TRI
ReadNavMeshTri( uint32 Version, uint8 ** Ptr ) {
    EDITOR__NAV_MESH_TRI Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            UINT32_TRI iInteriorVert = _read( ptr, UINT32_TRI );
            INT32_TRI  iPathLink     = _read( ptr, INT32_TRI );
            vec2       A             = _read( ptr, vec2 );
            vec2       B             = _read( ptr, vec2 );
            vec2       C             = _read( ptr, vec2 );
            
            EDITOR__NAV_MESH_TRI Entity = {};
            Entity.iInteriorVert = iInteriorVert;
            Entity.iPathLink     = iPathLink;
            Entity.A             = A;
            Entity.B             = B;
            Entity.C             = C;
            
            Result = Entity;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadNavMesh( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        
        NAV_MESH_STATE * State = &AppState->NavMesh;
        
        State->nEntity = _read( ptr, uint32 );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            EDITOR__NAV_MESH_TRI Src = ReadNavMeshTri( Version, &ptr );
            
            NAV_MESH_TRI Entity = {};
            Entity.iPathLink = Src.iPathLink;
            Entity.A         = Src.A;
            Entity.B         = Src.B;
            Entity.C         = Src.C;
            
            State->Entity[ iEntity ] = Entity;
        }
        
        _popSize( TempMemory, file.size );
    }
}

internal void
EDITOR_LoadNavMesh( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * filename, char * filetag ) {
    EDITOR_STATE           * Editor = &AppState->Editor;
    EDITOR__NAV_MESH_STATE * State  = &Editor->NavMesh;
    
    FILE_DATA file = Platform->readFile( TempMemory, SaveDir, filename, filetag );
    if( file.contents ) {
        uint8 * ptr = ( uint8 * )file.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, filetag );
        
        State->nEntity = _read( ptr, uint32 );
        for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
            EDITOR__NAV_MESH_TRI Entity = ReadNavMeshTri( Version, &ptr );
            State->Entity[ iEntity ] = Entity;
        }
        
        _popSize( TempMemory, file.size );
    }
}