
struct FILE_PARSER {
    char * start;
    char * at;
    uint32 size;
};

internal FILE_PARSER
FileParser( void * memory, int32 size ) {
    FILE_PARSER result = { ( char * )memory, ( char * )memory, (uint32)size };
    return result;
}

internal FILE_PARSER
FileParser( STRING str ) {
    FILE_PARSER result = {};
    result.start = str.string;
    result.at    = str.string;
    result.size  = str.nChar;
    return result;
}

internal boo32
hasTextRemaining( FILE_PARSER * parser ) {
    boo32 result = ( ( parser->at - parser->start ) < parser->size );
    return result;
}

internal boo32
isWhiteSpace( char c ) {
    boo32 result = ( c == 0 ) || ( c == ' ' ) || ( c == '\n' ) || ( c == '\t' ) || ( c == '\r' );
    return result;
}

internal void
eatWhiteSpace( FILE_PARSER * parser ) {
    while( hasTextRemaining( parser ) && ( isWhiteSpace( *parser->at ) ) ) { parser->at++; }
}

internal void
eatNonWhiteSpace( FILE_PARSER * parser ) {
    while( hasTextRemaining( parser ) && ( !isWhiteSpace( *parser->at ) ) ) { parser->at++; }
}

internal char *
getEnd( char * str ) {
    Assert( ( str ) && ( str[ 0 ] ) );
    
    char * result = str;
    while( result[ 0 ] ) { result++; }
    return result;
}

internal STRING
parseToken( FILE_PARSER * parser ) {
    Assert( parser->size > 0 );
    
    STRING result = {};
    
    eatWhiteSpace( parser );
    
    if( hasTextRemaining( parser ) ) {
        result.string = parser->at;
    }
    
    eatNonWhiteSpace( parser );
    result.nChar = ( uint32 )( parser->at - result.string );
    
    eatWhiteSpace( parser );
    
    return result;
}

internal STRING
parseLine( FILE_PARSER * parser ) {
    STRING result = {};
    
    eatWhiteSpace( parser );
    
    if( hasTextRemaining( parser ) ) {
        result.string = parser->at;
    }
    
    while( ( *parser->at != 0 ) && ( *parser->at != '\n' ) && ( *parser->at != '\r' ) ) { parser->at++; }
    result.nChar = ( uint32 )( parser->at - result.string );
    
    eatWhiteSpace( parser );
    return result;
}

internal uint8
parseU8( STRING token ) {
    uint8 result = ( uint8 )strtoul( token.string, 0, 0 );
    return result;
}

internal uint8
parseU8( FILE_PARSER * parser ) {
    STRING token  = parseToken( parser );
    uint8  result = parseU8( token );
    return result;
}

internal uint16
parseU16( STRING token ) {
    uint16 result = ( uint16 )strtoul( token.string, 0, 0 );
    return result;
}

internal uint32
parseU32( STRING token ) {
    uint32 result = strtoul( token.string, 0, 0 );
    return result;
}

internal uint32
parseU32( FILE_PARSER * parser ) {
    STRING token  = parseToken( parser );
    uint32 result = parseU32( token );
    return result;
}

internal int32
parseS32( STRING token ) {
    int32  result = strtol( token.string, 0, 0 );
    return result;
}

internal int32
parseS32( FILE_PARSER * parser ) {
    STRING token  = parseToken( parser );
    int32  result = parseS32( token );
    return result;
}

internal flo32
parseF32( STRING token ) {
    flo32  result = strtof( token.string, 0 );
    return result;
}

internal flo32
parseF32( FILE_PARSER * parser ) {
    STRING token  = parseToken( parser );
    flo32  result = parseF32( token );
    return result;
}

internal vec2
parseV2( STRING token ) {
    FILE_PARSER parser = FileParser( token.string, token.nChar );
    
    vec2 result = {};
    for( uint32 iElem = 0; iElem < 2; iElem++ ) {
        result.elem[ iElem ] = parseF32( &parser );
    }
    return result;
}

internal vec3
parseV3( STRING token ) {
    FILE_PARSER parser = FileParser( token.string, token.nChar );
    
    vec3 result = {};
    for( uint32 iElem = 0; iElem < 3; iElem++ ) {
        result.elem[ iElem ] = parseF32( &parser );
    }
    return result;
}

internal vec4
parseV4( STRING token ) {
    FILE_PARSER parser = FileParser( token.string, token.nChar );
    
    vec4 result = {};
    for( uint32 iElem = 0; iElem < 4; iElem++ ) {
        result.elem[ iElem ] = parseF32( &parser );
    }
    return result;
}

internal quat
parseQuat( STRING token ) {
    FILE_PARSER parser = FileParser( token.string, token.nChar );
    
    quat result = {};
    for( uint32 iElem = 0; iElem < 4; iElem++ ) {
        result.elem[ iElem ] = parseF32( &parser );
    }
    return result;
}