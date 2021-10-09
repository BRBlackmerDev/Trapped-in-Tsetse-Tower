
#ifdef	STD_INCLUDE_DECL

#define FONT__START_CHAR      ( '!' )
#define FONT__END_CHAR        ( '~' )
#define FONT__CHAR_COUNT      ( '~' - '!' + 1 )

struct FONT_CHAR {
    vec2 dim;
    vec2 offset;
    vec2 texCoord_min;
    vec2 texCoord_max;
    flo32 advanceWidth;
};

struct FONT {
    flo32 advanceWidth_space;
    flo32 ascent;
    flo32 descent;
    flo32 lineGap;
    flo32 advanceHeight;
    
    FONT_CHAR alphabet[ FONT__CHAR_COUNT ];
    
    uint32 * Texture_data;
    uint32   Texture_width;
    uint32   Texture_Height;
};

enum TEXT_ALIGNMENT {
    textAlign_default,
    
    textAlign_BottomLeft,
    textAlign_BottomCenter,
    textAlign_BottomRight,
    
    textAlign_CenterLeft,
    textAlign_Center,
    textAlign_CenterRight,
    
    textAlign_TopLeft,
    textAlign_TopCenter,
    textAlign_TopRight,
    
    textAlign_baseLeft,
    textAlign_baseCenter,
    textAlign_baseRight,
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

// NOTE: This expects MONOSPACE FONTS only!
internal flo32
getWidth( FONT * font, const char * string ) {
    uint32 nChar = ( uint32 )strlen( string );
    flo32 result = font->advanceWidth_space * ( flo32 )nChar;
    return result;
}

internal vec2
getDim( FONT * font, const char * string ) {
    vec2 result = {};
    result.x = getWidth( font, string );
    result.y = font->ascent + font->descent;
    return result;
}

internal vec2
newLine( FONT * font, uint32 nLines = 1 ) {
    // NOTE: This is just a hacky utility function so I don't have to do this everytime: Vec2( 0.0f, font->advanceHeight * ( flo32 )nLines )
    vec2 result = {};
    result.y = -font->advanceHeight * ( flo32 )nLines;
    return result;
}

internal vec2
getOffsetFromAlignment( FONT * font, const char * string, vec2 scale, TEXT_ALIGNMENT align ) {
    vec2 result = {};
    
    flo32 string_width = getWidth( font, string );
    flo32 font_ascent  = font->ascent;
    flo32 font_descent = font->descent;
	
    flo32 width     = string_width * scale.x;
    flo32 halfWidth = width        * 0.5f;
    
    flo32 ascent  = font_ascent  * scale.y;
    flo32 descent = font_descent * scale.y;
    
    flo32 Height     = ascent + descent;
    flo32 halfHeight = Height * 0.5f;
    
    switch( align ) {
        case textAlign_BottomLeft:   { result = Vec2(       0.0f, descent ); } break;
        case textAlign_BottomCenter: { result = Vec2( -halfWidth, descent ); } break;
        case textAlign_BottomRight:  { result = Vec2(     -width, descent ); } break;
        
        case textAlign_CenterLeft:   { result = Vec2(       0.0f, -halfHeight + descent ); } break;
        case textAlign_Center:       { result = Vec2( -halfWidth, -halfHeight + descent ); } break;
        case textAlign_CenterRight:  { result = Vec2(     -width, -halfHeight + descent ); } break;
        
        case textAlign_TopLeft:      { result = Vec2(       0.0f, -ascent ); } break;
        case textAlign_TopCenter:    { result = Vec2( -halfWidth, -ascent ); } break;
        case textAlign_TopRight:     { result = Vec2(     -width, -ascent ); } break;
        
        case textAlign_default:
        case textAlign_baseLeft:     {} break;
        case textAlign_baseCenter:   { result = Vec2( -halfWidth, 0.0f ); } break;
        case textAlign_baseRight:    { result = Vec2(     -width, 0.0f ); } break;
        
        default: { InvalidCodePath; } break;
    };
    
    return result;
}

internal rect
getBound( FONT * font, const char * string, TEXT_ALIGNMENT align, vec2 Position, vec2 scale ) {
    Position   += getOffsetFromAlignment( font, string, scale, align );
    Position.y += font->ascent * scale.y;
    vec2 dim = getDim( font, string ) * scale;
    rect result = rectTLD( Position, dim );
    return result;
}

internal FONT
loadFont( flo32 font_Height ) {
    FONT result = {};
    
    char file_Path[ 128 ] = {};
    sprintf( file_Path, "../../art/LiberationMono.ttf" );
    
    FILE * fontFile = fopen( file_Path, "rb" );
    if( fontFile ) {
        fseek( fontFile, 0, SEEK_END );
        uint32 fontFile_bytes = ftell( fontFile );
        fseek( fontFile, 0, SEEK_SET );
        
        uint8 * memory = ( uint8 * )calloc( fontFile_bytes, 1 );
        uint32 bytesRead = ( uint32 )fread( memory, 1, fontFile_bytes, fontFile );
        Assert( fontFile_bytes == bytesRead );
        
        stbtt_fontinfo fontInfo = {};
        stbtt_InitFont( &fontInfo, ( uint8 * )memory, 0 );
        flo32 scale = stbtt_ScaleForPixelHeight( &fontInfo, font_Height );
        
        int32 ascent = 0;
        int32 descent = 0;
        int32 lineGap = 0;
        stbtt_GetFontVMetrics( &fontInfo, &ascent, &descent, &lineGap );
        
        result.ascent  = fabsf( scale * ( flo32 )ascent  );
        result.descent = fabsf( scale * ( flo32 )descent );
        result.lineGap = fabsf( scale * ( flo32 )lineGap );
        result.advanceHeight = result.ascent + result.descent + result.lineGap;
        
        int32 cell_maxWidth = 0;
        int32 cell_maxHeight = 0;
        
        int32 maxIndex = FONT__CHAR_COUNT;
        for( int32 index = 0; index < maxIndex; index++ ) {
            int32 x0 = 0;
            int32 x1 = 0;
            int32 y0 = 0;
            int32 y1 = 0;
            
            int32 char_index = index + '!';
            
            stbtt_GetCodepointBitmapBox( &fontInfo, char_index, scale, scale, &x0, &y0, &x1, &y1 );
            
            int32 width  = x1 - x0;
            int32 Height = y1 - y0;
            
            cell_maxWidth  = maxValue( cell_maxWidth, width );
            cell_maxHeight = maxValue( cell_maxHeight, Height );
        }
        
        // 1-pixel apron around char
        cell_maxWidth  += 1;
        cell_maxHeight += 1;
        
        int32 cell_xCount = 10;
        int32 cell_yCount = 10;
        int32 cell_count  = cell_xCount * cell_yCount;
        int32 pixel_count = cell_count * cell_maxWidth * cell_maxHeight;
        uint32 * Texture_data = ( uint32 * )calloc( pixel_count, sizeof( uint32 ) );
        
        uint32 Texture_width  = cell_maxWidth  * cell_xCount;
        uint32 Texture_Height = cell_maxHeight * cell_yCount;
        uint32 xPixel = Texture_width;
        uint32 yPixel = Texture_Height;
        
        int32 advanceWidth    = 0;
        int32 LeftSideBearing = 0;
        
        int32 char_count = FONT__CHAR_COUNT;
        for( int32 index = 0; index < char_count; index++ ) {
            int32 width   = 0;
            int32 Height  = 0;
            int32 xOffset = 0;
            int32 yOffset = 0;
            
            int32 char_index = index + FONT__START_CHAR;
            
            uint8 * bitmap = stbtt_GetCodepointBitmap( &fontInfo, 0, scale, char_index, &width, &Height, &xOffset, &yOffset );
            stbtt_GetCodepointHMetrics( &fontInfo, char_index, &advanceWidth, &LeftSideBearing );
            
            int32 xCell = index % cell_yCount;
            int32 yCell = index / cell_yCount;
            int32 xMin = xCell * cell_maxWidth;
            int32 yMin = yCell * cell_maxHeight;
            
            FONT_CHAR _char = {};
            _char.dim				= Vec2( ( flo32 )width, 	( flo32 )Height );
            _char.offset			= Vec2( ( flo32 )xOffset, 	( flo32 )-yOffset );
            _char.texCoord_min.x = ( flo32 )xMin / ( flo32 )xPixel;
            _char.texCoord_min.y = ( flo32 )yMin / ( flo32 )yPixel;
            _char.texCoord_max.x = ( flo32 )( xMin + width  ) / ( flo32 )xPixel;
            _char.texCoord_max.y = ( flo32 )( yMin + Height ) / ( flo32 )yPixel;
            _char.advanceWidth	= ( flo32 )( ( int32 )( ( flo32 )advanceWidth * scale ) );
            result.alphabet[ index ] = _char;
            
            uint32 * destCell = Texture_data + ( ( index / cell_yCount ) * xPixel * cell_maxHeight ) + ( ( index % cell_yCount ) * cell_maxWidth );
            
            uint8  * srcRow  = bitmap + ( ( Height - 1 ) * width );
            uint32 * destRow = destCell;
            for( int32 y = 0; y < Height; y++ ) {
                uint8 * srcPixel  = srcRow;
                uint8 * destPixel = ( uint8 * )destRow;
                for( int32 x = 0; x < width; x++ ) {
                    uint8 alpha = *srcPixel++;
                    
                    flo32 _alpha   = ( flo32 )alpha / 255.0f;
                    flo32 _alphaSq = _alpha * _alpha;
                    uint8 rgb = ( uint8 )( _alphaSq * 255.0f );
                    
                    *destPixel++ = rgb;
                    *destPixel++ = rgb;
                    *destPixel++ = rgb;
                    *destPixel++ = alpha;
                }
                srcRow  -= width;
                destRow += xPixel;
            }
            
            stbtt_FreeBitmap( bitmap, 0 );
        }
        
        stbtt_GetCodepointHMetrics( &fontInfo, ' ', &advanceWidth, &LeftSideBearing );
        result.advanceWidth_space = ( ( flo32 )advanceWidth * scale );
        
        free( memory );
        
        result.Texture_data   = Texture_data;
        result.Texture_width  = Texture_width;
        result.Texture_Height = Texture_Height;
    }
    
    return result;
}

#endif	// STD_INCLUDE_FUNC