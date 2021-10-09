
#pragma pack( push, 1 )
struct BMP_HEADER {
	uint16 fileType;             // MUST be 0x4d42
	uint32 fileSize;
	uint16 reserved01;           // UNUSED
	uint16 reserved02;           // UNUSED
	uint32 offsetToData;         // sizeof( BMP_HEADER )
	uint32 bitmapInfoHeaderSize; // MUST be 40
	uint32 width;
	uint32 Height;
	uint16 planes;               // MUST be 1
	uint16 bitsPerPixel;
	uint32 compressionType;      // UNUSED
	uint32 bitmapSize_inBytes;   // UNUSED
	uint32 xPixelsPerMeter;      // UNUSED
	uint32 yPixelsPerMeter;      // UNUSED
	uint32 ColorCount;           // UNUSED
	uint32 importantColorCount;  // UNUSED
};
#pragma pack( pop )

struct TEXTURE_DATA {
	uint32  width;
	uint32  Height;
	uint32  bytesPerPixel;
    uint8 * texelData;
};

internal TEXTURE_DATA
BMP_readFile( PLATFORM * Platform, char * SaveDir, char * filename ) {
	TEXTURE_DATA result = {};
	
	// TODO: verify requested file extension
	FILE_DATA file = Platform->readFile( &Platform->TempMemory, SaveDir, filename, "bmp" );
	
	if( file.contents ) {
		BMP_HEADER * info = ( BMP_HEADER * )file.contents;
		result.width         = info->width;
		result.Height        = info->Height;
		result.bytesPerPixel = 4;
		
		uint32 BMP_bytesPerPixel = info->bitsPerPixel / 8;
		
		uint8 * BMP_Data = ( uint8 * )file.contents + info->offsetToData;
		uint8 * ptr = BMP_Data;
		
		uint32 pixelCount  = result.width * result.Height;
		uint32 TextureSize = pixelCount * result.bytesPerPixel;
		result.texelData = ( uint8 * )_pushSize( &Platform->permMemory, TextureSize );
		
		flo32 inv255 = 1.0f / 255.0f;
		uint32 * resultData  = ( uint32 * )result.texelData;
        
        if( BMP_bytesPerPixel == 4 ) {
            uint32 * TextureData = ( uint32 * )BMP_Data;
            for( uint32 counter = 0; counter < pixelCount; counter++ ) {
                // TODO: RGBA vs BGRA
                uint32 inColor = TextureData[ counter ];
                
                uint8 r = ( uint8 )( ( inColor >>  0 ) & 0xFF );
                uint8 g = ( uint8 )( ( inColor >>  8 ) & 0xFF );
                uint8 b = ( uint8 )( ( inColor >> 16 ) & 0xFF );
                uint8 a = ( uint8 )( ( inColor >> 24 ) & 0xFF );
                flo32 alpha = ( flo32 )a * inv255;
                
                uint32 outColor =
                    ( ( uint8 )( ( flo32 )b * alpha ) <<  0 ) |
                    ( ( uint8 )( ( flo32 )g * alpha ) <<  8 ) |
                    ( ( uint8 )( ( flo32 )r * alpha ) << 16 ) |
                    ( a << 24 );
                
                resultData[ counter ] = outColor;
            }
        } else if( BMP_bytesPerPixel == 3 ) {
            uint8 * TextureData = BMP_Data;
            for( uint32 counter = 0; counter < pixelCount; counter++ ) {
                // TODO: RGBA vs BGRA
                uint8 r = *TextureData++;
                uint8 g = *TextureData++;
                uint8 b = *TextureData++;
                uint8 a = 255;
                
                uint32 outColor = ( b <<  0 ) | ( g <<  8 ) | ( r << 16 ) | ( a << 24 );
                
                resultData[ counter ] = outColor;
            }
        }
		
		_popSize( &Platform->TempMemory, file.size );
	}
	
	return result;
}