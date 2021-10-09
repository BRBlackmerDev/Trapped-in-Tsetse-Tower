
//----------
// LAYOUT
//----------

internal void
reset( LAYOUT_STATE * layout ) {
    { // init Camera
        flo32 aspectRatio = 1920.0f / 1080.0f;
        flo32 dimY = 1080.0f / LAYOUT_Y_PIXELS_PER_UNIT;
        flo32 dimX = dimY * aspectRatio;
        
        CAMERA_STATE * Camera = &layout->Camera;
        Camera->pos = {};
        Camera->dim = Vec2( dimX, dimY );
    }
    
    reset( &layout->SaveOpen );
    
    uint8 * addrA = ( uint8 * )layout;
    uint8 * addrB = ( uint8 * )&layout->reset_reset;
    uint32 reset_size = sizeof( LAYOUT_STATE ) - ( uint32 )( addrB - addrA );
    memset( addrB, 0, reset_size );
}

internal UINT32_PAIR
UploadBMPFromFile( PLATFORM * Platform, TEXTURE_ID TextureID, char * SaveDir, char * filename ) {
    TEXTURE_DATA Texture = BMP_readFile( Platform, SaveDir, filename );
    UploadTexture( &Platform->Renderer, TextureID, Texture.width, Texture.Height, Texture.texelData );
    
    UINT32_PAIR result = UInt32Pair( Texture.width, Texture.Height );
    return result;
}


#if 0
internal uint32
getILink( LAYOUT_STATE * layout, uint32 iRoom, uint32 iExit ) {
    LAYOUT_ROOM Room = layout->Room[ iRoom ];
    
    uint32 result = Room.Exit.m + iExit;
    return result;
}
#endif

internal boo32
LinkWasBroken( LAYOUT_STATE * layout, uint32 iRoom, uint32 iExit ) {
    LAYOUT_ROOM Room = layout->Room[ iRoom ];
    boo32 result = Room.Link_wasBroken[ iExit ];
    return result;
}

internal int32
getIRoom( LAYOUT_STATE * layout, char * filename ) {
    int32 result = -1;
    for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
        if( matchString( filename, layout->Room[ iRoom ].name ) ) {
            result = iRoom;
        }
    }
    return result;
}

internal uint32
getNShow( LAYOUT_STATE * layout ) {
    uint32 result = 0;
    for( uint32 iRoom = 0; iRoom < layout->nRoom; iRoom++ ) {
        LAYOUT_ROOM Room = layout->Room[ iRoom ];
        if( Room.show ) {
            result++;
        }
    }
    return result;
}