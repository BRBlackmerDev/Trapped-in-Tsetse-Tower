
//----------
// CLOSEST POINT
//----------

// P : point
// E : Edge
// L : line
// R : ray

internal vec2
closestPE( vec2 C, vec2 A, vec2 B ) {
    vec2 AB = B - A;
    vec2 AC = C - A;
    
    flo32 t      = clamp01( dot( AC, AB ) / dot( AB, AB ) );
    vec2  result = A + AB * t;
    return result;
}

internal boo32
DoIntersectEE( vec2 A, vec2 B, vec2 C, vec2 D ) {
    boo32 result = false;
    
    vec2 AB = B - A;
    vec2 CD = D - C;
    
    vec2 N = GetNormal( GetPerp( CD ) );
    
    flo32 denom = dot( N, AB );
    if( denom <= -DOT__COLLINEAR_EPSILON ) {
        flo32 t = dot( N, C - A ) / denom;
        if( ( t >= 0.0f ) && ( t <= 1.0f ) ) {
            vec2  P = A + AB * t;
            flo32 s = dot( P - C, CD ) / dot( CD, CD );
            
            if( ( s >= 0.0f ) && ( s <= 1.0f ) ) {
                result = true;
            }
        }
    }
    return result;
}

internal boo32
doesHorSegmentIntersectRect( rect R, vec2 P, vec2 Q ) {
    Assert( P.x <= Q.x );
    Assert( P.y == Q.y );
    
    boo32 Result = false;
    // TODO: Should this be  >=  and  <=?
    if( ( P.y > R.Bottom ) && ( P.y < R.Top ) && ( !( ( P.x > R.Right ) || ( Q.x < R.Left ) ) ) ) {
        Result = true;
    }
    return Result;
}

//----------
// COLLISION
//----------

struct COLLISION_RESULT {
    vec2 Position;
    vec2 Velocity;
};

internal COLLISION_RESULT
updateGrenadeCollision( APP_STATE * AppState, vec2 P, vec2 V, vec2 dP, flo32 Radius ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    COLLISION_RESULT result = {};
    result.Position = P;
    result.Velocity = V;
    
    flo32 length    = GetLength( dP );
    flo32 minLength = 0.0001f;
    if( length > minLength ) {
        for( uint32 iter = 0; iter < 4; iter++ ) {
            RAY2 ray = Ray2( P, dP );
            RAY2_INTERSECT bestIntersect = Ray2IntersectInit();
            
            { // TERRAIN COLLISION
                for( uint32 iType = 0; iType < 4; iType++ ) {
                    UINT32_PAIR Type = Collision->Type[ iType ];
                    
                    vec4 * Edge = Collision->Edge + Type.m;
                    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                        vec4 e = Edge[ iEdge ];
                        
                        RAY2_INTERSECT intersect = DoesIntersect( ray, Capsule2( e.P, e.Q, Radius ) );
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            { // MECH DOOR COLLISION
                MECH_DOOR_STATE * State = &AppState->Mech;
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    MECH_DOOR Entity = State->Entity[ iEntity ];
                    
                    if( Entity.isClosed ) {
                        RAY2_INTERSECT intersect = DoesIntersect( ray, Entity.Bound_Door, Radius );
                        
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            { // SECURE DOOR COLLISION
                SECURE_DOOR_STATE * State = &AppState->Secure;
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    SECURE_DOOR Entity = State->Entity[ iEntity ];
                    
                    if( Entity.isClosed ) {
                        RAY2_INTERSECT intersect = DoesIntersect( ray, Entity.Bound, Radius );
                        
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            if( ( bestIntersect.isValid ) && ( bestIntersect.t <= 1.0f ) ) {
                V -= dot( bestIntersect.N, V ) * bestIntersect.N;
                
                P   = bestIntersect.P + bestIntersect.N * COLLISION_EPSILON_OFFSET;
                dP -= dot( bestIntersect.N, dP ) * bestIntersect.N;
                dP *= ( 1.0f - bestIntersect.t );
                length = GetLength( dP );
                if( length <= minLength ) {
                    dP = {};
                    iter = 4;
                }
            } else {
                iter = 4;
            }
        }
        
        result.Position = P + dP;
        result.Velocity = V;
    }
    
    return result;
}

internal COLLISION_RESULT
UpdateCollision( APP_STATE * AppState, vec2 P, vec2 V, vec2 dP, vec2 dim ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    COLLISION_RESULT result = {};
    result.Position = P;
    result.Velocity = V;
    
    vec2 offset = Vec2( dim.x * 0.5f, dim.y );
    
    flo32 length    = GetLength( dP );
    flo32 minLength = 0.0001f;
    if( length > minLength ) {
        for( uint32 iter = 0; iter < 4; iter++ ) {
            RAY2 ray = Ray2( P, dP );
            RAY2_INTERSECT bestIntersect = Ray2IntersectInit();
            
            vec2 offsetA[ 4 ] = {
                -offset,
                Vec2( offset.x, -offset.y ),
                Vec2( offset.x, 0.0f ),
                Vec2( -offset.x, 0.0f ),
            };
            
            vec2 offsetB[ 4 ] = {
                Vec2( -offset.x, 0.0f ),
                -offset,
                Vec2( offset.x, -offset.y ),
                Vec2( offset.x, 0.0f ),
            };
            
            { // TERRAIN COLLISION
                for( uint32 iType = 0; iType < 4; iType++ ) {
                    UINT32_PAIR Type = Collision->Type[ iType ];
                    
                    vec2 oA = offsetA[ iType ];
                    vec2 oB = offsetB[ iType ];
                    
                    vec4 * Edge = Collision->Edge + Type.m;
                    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                        vec4 e = Edge[ iEdge ];
                        
                        vec2 A = e.xy + oA;
                        vec2 B = e.zw + oB;
                        RAY2_INTERSECT intersect = DoesIntersect( ray, A, B );
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            { // MECH DOOR COLLISION
                MECH_DOOR_STATE * State = &AppState->Mech;
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    MECH_DOOR Entity = State->Entity[ iEntity ];
                    if( Entity.isClosed ) {
                        rect R = addDim( Entity.Bound_Door, dim.x * 0.5f, dim.y, dim.x * 0.5f, 0.0f );
                        RAY2_INTERSECT intersect = DoesIntersect( ray, R );
                        
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            { // SECURE DOOR COLLISION
                SECURE_DOOR_STATE * State = &AppState->Secure;
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    SECURE_DOOR Entity = State->Entity[ iEntity ];
                    
                    if( Entity.isClosed ) {
                        rect R = addDim( Entity.Bound, dim.x * 0.5f, dim.y, dim.x * 0.5f, 0.0f );
                        RAY2_INTERSECT intersect = DoesIntersect( ray, R );
                        
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            if( ( bestIntersect.isValid ) && ( bestIntersect.t <= 1.0f ) ) {
                V -= dot( bestIntersect.N, V ) * bestIntersect.N;
                
                P   = bestIntersect.P + bestIntersect.N * COLLISION_EPSILON_OFFSET;
                dP -= dot( bestIntersect.N, dP ) * bestIntersect.N;
                dP *= ( 1.0f - bestIntersect.t );
                length = GetLength( dP );
                if( length <= minLength ) {
                    dP = {};
                    iter = 4;
                }
            } else {
                iter = 4;
            }
        }
        
        result.Position = P + dP;
        result.Velocity = V;
    }
    
    return result;
}

internal COLLISION_RESULT
UpdatePlayerCollision( APP_STATE * AppState, vec2 P, vec2 V, vec2 dP, vec2 dim ) {
    // TODO: This is a singular function for Player Collision. Player is the only Entity to collide with Nests. There is, of course, the code duplication between Collision with terrain for Player and Jumpers. There might be a better way to clean this up later.
    COLLISION_STATE * Collision = &AppState->Collision;
    
    COLLISION_RESULT result = {};
    result.Position = P;
    result.Velocity = V;
    
    vec2 offset = Vec2( dim.x * 0.5f, dim.y );
    
    flo32 length    = GetLength( dP );
    flo32 minLength = 0.0001f;
    if( length > minLength ) {
        for( uint32 iter = 0; iter < 4; iter++ ) {
            RAY2 ray = Ray2( P, dP );
            RAY2_INTERSECT bestIntersect = Ray2IntersectInit();
            
            vec2 offsetA[ 4 ] = {
                -offset,
                Vec2( offset.x, -offset.y ),
                Vec2( offset.x, 0.0f ),
                Vec2( -offset.x, 0.0f ),
            };
            
            vec2 offsetB[ 4 ] = {
                Vec2( -offset.x, 0.0f ),
                -offset,
                Vec2( offset.x, -offset.y ),
                Vec2( offset.x, 0.0f ),
            };
            
            { // TERRAIN COLLISION
                for( uint32 iType = 0; iType < 4; iType++ ) {
                    UINT32_PAIR Type = Collision->Type[ iType ];
                    
                    vec2 oA = offsetA[ iType ];
                    vec2 oB = offsetB[ iType ];
                    
                    vec4 * Edge = Collision->Edge + Type.m;
                    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                        vec4 e = Edge[ iEdge ];
                        
                        vec2 A = e.xy + oA;
                        vec2 B = e.zw + oB;
                        RAY2_INTERSECT intersect = DoesIntersect( ray, A, B );
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            { // NEST COLLISION
                NEST_STATE * State = &AppState->Nest;
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    NEST Entity = State->Entity[ iEntity ];
                    
                    if( !Entity.IsDead ) {
                        rect R = addDim( Entity.Bound, dim.x * 0.5f, dim.y, dim.x * 0.5f, 0.0f );
                        RAY2_INTERSECT intersect = DoesIntersect( ray, R );
                        
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            { // SECURE DOOR COLLISION
                SECURE_DOOR_STATE * State = &AppState->Secure;
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    SECURE_DOOR Entity = State->Entity[ iEntity ];
                    
                    if( Entity.isClosed ) {
                        rect R = addDim( Entity.Bound, dim.x * 0.5f, dim.y, dim.x * 0.5f, 0.0f );
                        RAY2_INTERSECT intersect = DoesIntersect( ray, R );
                        
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            { // MECH DOOR COLLISION
                MECH_DOOR_STATE * State = &AppState->Mech;
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    MECH_DOOR Entity = State->Entity[ iEntity ];
                    
                    if( Entity.isClosed ) {
                        rect R = addDim( Entity.Bound_Door, dim.x * 0.5f, dim.y, dim.x * 0.5f, 0.0f );
                        RAY2_INTERSECT intersect = DoesIntersect( ray, R );
                        
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            if( ( bestIntersect.isValid ) && ( bestIntersect.t <= 1.0f ) ) {
                V -= dot( bestIntersect.N, V ) * bestIntersect.N;
                
                P   = bestIntersect.P + bestIntersect.N * COLLISION_EPSILON_OFFSET;
                dP -= dot( bestIntersect.N, dP ) * bestIntersect.N;
                dP *= ( 1.0f - bestIntersect.t );
                length = GetLength( dP );
                if( length <= minLength ) {
                    dP = {};
                    iter = 4;
                }
            } else {
                iter = 4;
            }
        }
        
        result.Position = P + dP;
        result.Velocity = V;
    }
    
    return result;
}

internal COLLISION_RESULT
UpdatePlayerCollisionT( APP_STATE * AppState, vec2 P, vec2 V, vec2 dP, flo32 Radius ) {
    // TODO: This is a singular function for Player Collision. Player is the only Entity to collide with Nests. There is, of course, the code duplication between Collision with terrain for Player and Jumpers. There might be a better way to clean this up later.
    COLLISION_STATE * Collision = &AppState->Collision;
    
    COLLISION_RESULT result = {};
    result.Position = P;
    result.Velocity = V;
    
    flo32 length    = GetLength( dP );
    flo32 minLength = 0.0001f;
    if( length > minLength ) {
        for( uint32 iter = 0; iter < 4; iter++ ) {
            RAY2 ray = Ray2( P, dP );
            RAY2_INTERSECT bestIntersect = Ray2IntersectInit();
            
            { // TERRAIN COLLISION
                for( uint32 iType = 0; iType < 4; iType++ ) {
                    UINT32_PAIR Type = Collision->Type[ iType ];
                    
                    vec4 * Edge = Collision->Edge + Type.m;
                    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                        vec4 e = Edge[ iEdge ];
                        
                        RAY2_INTERSECT intersect = DoesIntersect( ray, Capsule2( e.P, e.Q, Radius ) );
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            { // MECH DOOR COLLISION
                MECH_DOOR_STATE * State = &AppState->Mech;
                for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
                    MECH_DOOR Entity = State->Entity[ iEntity ];
                    
                    if( Entity.isClosed ) {
                        RAY2_INTERSECT intersect = DoesIntersect( ray, Entity.Bound_Door, Radius );
                        
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            if( ( bestIntersect.isValid ) && ( bestIntersect.t <= 1.0f ) ) {
                V -= dot( bestIntersect.N, V ) * bestIntersect.N;
                
                P   = bestIntersect.P + bestIntersect.N * COLLISION_EPSILON_OFFSET;
                dP -= dot( bestIntersect.N, dP ) * bestIntersect.N;
                dP *= ( 1.0f - bestIntersect.t );
                length = GetLength( dP );
                if( length <= minLength ) {
                    dP = {};
                    iter = 4;
                }
            } else {
                iter = 4;
            }
        }
        
        result.Position = P + dP;
        result.Velocity = V;
    }
    
    return result;
}

//----------
// convert screen and World coordinates
//----------

internal vec2
toScreenCoord( mat4 Camera_transform, vec3 point ) {
    vec4 Position = Camera_transform * Vec4( point, 1.0f );
    vec2 result   = ( ( Vec2( Position.x, Position.y ) / Position.w ) * 0.5f + Vec2( 0.5f, 0.5f ) );
    return result;
}

internal vec3
screenCoordToWorld( vec2 pos, vec2 dim, mat4 Camera_transform, vec3 Camera_Position ) {
    flo32 tX = ( pos.x / dim.x ) * 2.0f - 1.0f;
    flo32 tY = ( pos.y / dim.y ) * 2.0f - 1.0f;
    
    mat4 inverse_transform = mat4_inverse( Camera_transform );
    vec4 nearPoint         = inverse_transform * Vec4( tX, tY, -1.0f, 1.0f );
    nearPoint.xyz    /= nearPoint.w;
    
    vec3 result = GetNormal( nearPoint.xyz - Camera_Position );
    return result;
}

//----------
// Color functions
//----------

internal vec4
toColor( uint8 r, uint8 g, uint8 b ) {
    vec4 result = {};
    result.r = ( flo32 )r / 255.0f;
    result.g = ( flo32 )g / 255.0f;
    result.b = ( flo32 )b / 255.0f;
    result.a = 1.0f;
    return result;
}

internal uint32
toColor_U32( uint8 r, uint8 g, uint8 b, uint8 a = 255 ) {
    uint32 Result = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | ( b << 0 );
    return Result;
}

//----------
// UINT32_PAIR functions
//----------

internal UINT32_PAIR
UInt32Pair( uint32 x, uint32 y ) {
    UINT32_PAIR result = { x, y };
    return result;
}

internal boo32
operator==( UINT32_PAIR a, UINT32_PAIR b ) {
    boo32 result = ( a.x == b.x ) && ( a.y == b.y );
    return result;
}

internal boo32
operator!=( UINT32_PAIR a, UINT32_PAIR b ) {
    boo32 result = !( a == b );
    return result;
}

//----------
// UINT32_TRI functions
//----------

internal UINT32_TRI
UInt32Tri( uint32 x, uint32 y, uint32 z ) {
    UINT32_TRI result = { x, y, z };
    return result;
}

internal boo32
operator==( UINT32_TRI a, UINT32_TRI b ) {
    boo32 result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z );
    return result;
}

internal boo32
operator!=( UINT32_TRI a, UINT32_TRI b ) {
    boo32 result = !( a == b );
    return result;
}

//----------
// INT32_PAIR functions
//----------

internal INT32_PAIR
Int32Pair( int32 x, int32 y ) {
    INT32_PAIR result = { x, y };
    return result;
}

//----------
// INT32_TRI functions
//----------

internal INT32_TRI
Int32Tri( int32 x, int32 y, int32 z ) {
    INT32_TRI result = { x, y, z };
    return result;
}

internal boo32
operator==( INT32_TRI a, INT32_TRI b ) {
    boo32 result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z );
    return result;
}

internal boo32
operator!=( INT32_TRI a, INT32_TRI b ) {
    boo32 result = !( a == b );
    return result;
}

internal UINT32_TRI
ToUInt32Tri( INT32_TRI Tri ) {
    Assert( Tri.x > -1 );
    Assert( Tri.y > -1 );
    Assert( Tri.z > -1 );
    
    UINT32_TRI Result = { ( uint32 )Tri.x, ( uint32 )Tri.y, ( uint32 )Tri.z };
    return Result;
}

//----------
// GEN TEXTURE FUNCTIONS
//----------

internal void
GenTexture_ExitTypeS_Walk_EnterLeft( DIRECTX_12_RENDERER * Renderer, MEMORY * TempMemory, TEXTURE_ID TextureID ) {
    uint32 Width  = 256;
    uint32 Height = 256;
    uint32 nTexel = Width * Height;
    uint32 * Data = _pushArray_clear( TempMemory, uint32, nTexel );
    
    uint8 hi = 40;
    uint8 lo = 0;
    
    for( uint32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
        uint32 iRow = ( iTexel / Width );
        uint32 iCol = ( iTexel % Width );
        
        flo32 t = ( flo32 )iCol / ( flo32 )Width;
        uint8 c = ( uint8 )lerp( ( flo32 )lo, t, ( flo32 )hi );
        
        uint32 Color = toColor_U32( c, c, c );
        Data[ iTexel ] = Color;
    }
    
    UploadTexture( Renderer, TextureID, Width, Height, Data );
    _popArray( TempMemory, uint32, nTexel );
}

//----------
// gen Model functions
//----------

internal MODEL_DATA
genModel_PlayerWallSlide( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    VERTEX1 vertex[ 4 ] = {
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        0.25f, -1.0f,  0.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        1.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    };
    uint32 nVertex = _arrayCount( vertex );
    
    uint32 index[] = {
        0, 1, 2,
        3, 2, 1,
    };
    uint32 nIndex = _arrayCount( index );
    
    MODEL_DATA result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return result;
}

internal MODEL_DATA
genModel_JumperStunned( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    VERTEX1 vertex[ 4 ] = {
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.85f,  1.0f, 0.0f,  0.0f, 1.0f,
        0.85f,  1.0f,  0.0f,  1.0f, 1.0f,
    };
    uint32 nVertex = _arrayCount( vertex );
    
    uint32 index[] = {
        0, 1, 2,
        3, 2, 1,
    };
    uint32 nIndex = _arrayCount( index );
    
    MODEL_DATA result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return result;
}

//----------
// SAVE/LOAD ENTITY
//----------

struct ENTITY_FILE_HEADER {
    char EntityTag[ 8 ];
    char version  [ 4 ];
};

internal void
writeEntityFileHeader( MEMORY * output, char * filetag, uint32 version ) {
    writeSegment( output, filetag );
    
    char str[ 4 + 1 ] = {};
    sprintf( str, "%04u", version );
    writeSegment( output, str );
}

internal void
outputFile( PLATFORM * Platform, MEMORY * output, char * SaveDir, char * filename, char * filetag ) {
    boo32 isValid = Platform->writeFile( SaveDir, filename, filetag, output->base, ( uint32 )output->used );
    if( !isValid ) {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to save file: %s.%s", filename, filetag );
        CONSOLE_STRING( str );
        globalVar_debugSystem.errorOccurred = true;
    }
}

internal uint32
VerifyEntityHeaderAndGetVersion( uint8 ** ptr, char * filetag ) {
    ENTITY_FILE_HEADER * header = _addr( *ptr, ENTITY_FILE_HEADER );
    Assert( matchSegment( header->EntityTag, filetag, 8 ) );
    
    char str[ 4 + 1 ] = {};
    memcpy( str, header->version, 4 );
    
    uint32 result = strtoul( str, 0, 0 );
    return result;
}