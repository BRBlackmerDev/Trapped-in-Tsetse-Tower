
//----------
// vec2 check functions
//----------

#define T_VALID_EPSILON ( -0.0001f )
#define RAY_INTERSECT__VALID_NEG_DIST_EPSILON ( 0.025f )
#define DOT__DEGENERATE_EPSILON  ( 0.000001f )
#define DOT__COLLINEAR_EPSILON   ( 0.000001f )
#define DOT__ORTHOGONAL_EPSILON  ( 0.000001f )

internal boo32
isDegenerate( vec2 V ) {
    flo32 dotVV  = dot( V, V );
    boo32 result = ( dotVV <= DOT__DEGENERATE_EPSILON );
    return result;
}

internal boo32
isOrthogonalTo( vec2 U, vec2 V ) {
    flo32 dotUV = dot( U, V );
    dotUV = fabsf( dotUV );
    boo32 result = ( dotUV <= DOT__ORTHOGONAL_EPSILON );
    return result;
}

internal boo32
isCollinearTo( vec2 U, vec2 V ) {
    flo32 dotUV = dot( U, V );
    boo32 result = ( fabsf( dotUV ) >= ( 1.0f - DOT__COLLINEAR_EPSILON ) );
    return result;
}

internal boo32
isOppositeTo( vec2 U, vec2 V ) {
    flo32 dotUV = dot( U, V );
    boo32 result = ( dotUV < -DOT__ORTHOGONAL_EPSILON );
    return result;
}

//----------
// LINE2 functions
//----------

struct LINE2 {
    vec2 Origin;
    vec2 Vector;
};

internal LINE2
Line2( vec2 Origin, vec2 Vector ) {
    LINE2 Result = {};
    Result.Origin = Origin;
    Result.Vector = Vector;
    return Result;
}

struct LINE2_INTERSECT {
    boo32 IsValid;
    flo32 tA;
    flo32 tB;
    vec2  P;
};

internal LINE2_INTERSECT
DoesIntersect( LINE2 A, LINE2 B ) {
    LINE2_INTERSECT Result = {};
    
    vec2 N = GetNormal( GetPerp( B.Vector ) );
    
    flo32 Denom = dot( N, A.Vector );
    if( Denom != 0.0f ) {
        Result.IsValid = true;
        Result.tA      = dot( N, B.Origin - A.Origin ) / Denom;
        Result.tB      = 0.0f; // TODO: Solve for this!
        Result.P       = A.Origin + A.Vector * Result.tA;
    }
    
    return Result;
}

//----------
// ray2 functions
//----------

struct RAY2 {
    vec2 origin;
    vec2 vector;
};

struct RAY2_INTERSECT {
    boo32 isValid;
    flo32 t;
    vec2  P;
    vec2  N;
    
    flo32 denom;
    flo32 tMin;
    flo32 s;
};

internal RAY2
Ray2( vec2 origin, vec2 vector ) {
    RAY2 result = {};
    result.origin = origin;
    result.vector = vector;
    return result;
}

internal RAY2_INTERSECT
Ray2IntersectInit() {
    RAY2_INTERSECT result = {};
    result.t = FLT_MAX;
    return result;
}

internal RAY2_INTERSECT
DoesIntersect( RAY2 ray, vec2 P, vec2 Q ) {
    RAY2_INTERSECT result = {};
    
    vec2 V = Q - P;
    vec2 W = P - ray.origin;
    result.N = GetNormal( GetPerp( V ) );
    
    result.denom = dot( result.N, ray.vector );
    if( result.denom <= -DOT__COLLINEAR_EPSILON ) {
        result.tMin = ( -RAY_INTERSECT__VALID_NEG_DIST_EPSILON / GetLength( ray.vector ) );
        result.t = dot( result.N, W ) / result.denom;
        if( result.t >= result.tMin ) {
            result.P = ray.origin + ray.vector * result.t;
            result.s = dot( result.P - P, V ) / dot( V, V );
            
            if( ( result.s >= 0.0f ) && ( result.s <= 1.0f ) ) {
                result.isValid = true;
            }
        }
    }
    return result;
}

internal RAY2_INTERSECT
DoesIntersect( RAY2 ray, rect Bound ) {
    RAY2_INTERSECT result = Ray2IntersectInit();
    
    vec2 point[ 4 + 1 ] = {};
    point[ 0 ] = Vec2( Bound.Left,  Bound.Bottom );
    point[ 1 ] = Vec2( Bound.Left,  Bound.Top    );
    point[ 2 ] = Vec2( Bound.Right, Bound.Top    );
    point[ 3 ] = Vec2( Bound.Right, Bound.Bottom );
    point[ 4 ] = Vec2( Bound.Left,  Bound.Bottom );
    
    for( uint32 iter = 0; iter < 4; iter++ ) {
        vec2 P = point[ iter     ];
        vec2 Q = point[ iter + 1 ];
        RAY2_INTERSECT intersect = DoesIntersect( ray, P, Q );
        if( ( intersect.isValid ) && ( intersect.t < result.t ) ) {
            result = intersect;
        }
    }
    
    return result;
}

internal RAY2_INTERSECT
DoesIntersect( RAY2 ray, orect Bound ) {
    RAY2_INTERSECT result = Ray2IntersectInit();
    
    vec2 xAxis = Bound.xAxis * Bound.halfDim.x;
    vec2 yAxis = Bound.yAxis * Bound.halfDim.y;
    
    vec2 point[ 4 + 1 ] = {};
    point[ 0 ] = Bound.Center - xAxis - yAxis;
    point[ 1 ] = Bound.Center - xAxis + yAxis;
    point[ 2 ] = Bound.Center + xAxis + yAxis;
    point[ 3 ] = Bound.Center + xAxis - yAxis;
    point[ 4 ] = point[ 0 ];
    
    for( uint32 iter = 0; iter < 4; iter++ ) {
        vec2 P = point[ iter     ];
        vec2 Q = point[ iter + 1 ];
        RAY2_INTERSECT intersect = DoesIntersect( ray, P, Q );
        if( ( intersect.isValid ) && ( intersect.t < result.t ) ) {
            result = intersect;
        }
    }
    
    return result;
}

internal RAY2_INTERSECT
DoesIntersect( RAY2 ray, circ C ) {
    RAY2_INTERSECT result = {};
    
    vec2 U = GetNormal( ray.vector );
    vec2 V = ray.origin - C.Center;
    
    flo32 dotUV = dot( U, V );
    
    flo32 discriminant = ( dotUV * dotUV ) + ( C.Radius * C.Radius ) - dot( V, V );
    result.denom = discriminant;
    if( discriminant >= 0.0f ) {
        flo32 tMin = -RAY_INTERSECT__VALID_NEG_DIST_EPSILON;
        result.tMin = tMin;
        flo32 t = -dotUV - sqrtf( discriminant );
        result.t = t / GetLength( ray.vector );
        //NOTE: only checks if the near side of the sphere is valid
        //NOTE: t = -dotUV + sqrtf( disc ) would check the far side of the sphere
        if( t >= tMin ) {
            result.isValid = true;
            result.P       = ray.origin + ray.vector * result.t;
            result.N       = GetNormal( result.P - C.Center );
        }
    }
    
    return result;
}

internal RAY2_INTERSECT
DoesIntersect( RAY2 ray, CAPSULE2 cap ) {
    RAY2_INTERSECT result = Ray2IntersectInit();
    
    vec2 xAxis = GetNormal( cap.Q - cap.P );
    vec2 yAxis = GetPerp( xAxis );
    yAxis *= cap.Radius;
    
    RAY2_INTERSECT intersect[ 2 ] = {};
    intersect[ 0 ] = DoesIntersect( ray, cap.P + yAxis, cap.Q + yAxis );
    intersect[ 1 ] = DoesIntersect( ray, Circ( cap.P, cap.Radius ) );
    
    // TODO: NOTE: This eliminates duplicate and unnecessary calculation. Probably temporary. This is only possible because Collision and obstacles are very strict about consistent direction and closure.
    //intersect[ 0 ] = DoesIntersect( ray, LineSeg( cap.P + yAxis, cap.Q + yAxis ) );
    //intersect[ 1 ] = DoesIntersect( ray, LineSeg( cap.Q - yAxis, cap.P - yAxis ) );
    //intersect[ 2 ] = DoesIntersect( ray, Circle( cap.P, cap.Radius ) );
    //intersect[ 3 ] = DoesIntersect( ray, Circle( cap.Q, cap.Radius ) );
    
    for( uint32 iter = 0; iter < 2; iter++ ) {
        RAY2_INTERSECT i = intersect[ iter ];
        if( ( i.isValid ) && ( i.t < result.t ) ) {
            result = i;
        }
    }
    
    return result;
}

internal RAY2_INTERSECT
DoesIntersect( RAY2 ray, rect Bound, flo32 Radius ) {
    RAY2_INTERSECT result = Ray2IntersectInit();
    
    vec2 point[ 4 + 1 ] = {};
    point[ 0 ] = Vec2( Bound.Left,  Bound.Bottom );
    point[ 1 ] = Vec2( Bound.Left,  Bound.Top    );
    point[ 2 ] = Vec2( Bound.Right, Bound.Top    );
    point[ 3 ] = Vec2( Bound.Right, Bound.Bottom );
    point[ 4 ] = Vec2( Bound.Left,  Bound.Bottom );
    
    for( uint32 iter = 0; iter < 4; iter++ ) {
        vec2 P = point[ iter     ];
        vec2 Q = point[ iter + 1 ];
        RAY2_INTERSECT intersect = DoesIntersect( ray, Capsule2( P, Q, Radius ) );
        if( ( intersect.isValid ) && ( intersect.t < result.t ) ) {
            result = intersect;
        }
    }
    
    return result;
}