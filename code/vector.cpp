
#ifdef	STD_INCLUDE_DECL

//----------
// vector declarations
//----------

union vec2 {
	struct {
		flo32 x;
		flo32 y;
	};
	flo32 elem[2];
};

union vec3 {
	struct {
		flo32 x;
		flo32 y;
		flo32 z;
	};
	struct {
		flo32 r;
		flo32 g;
		flo32 b;
	};
    struct {
        vec2  xy;
        flo32 z;
    };
	flo32 elem[3];
};

union vec4 {
	struct {
		flo32 x;
		flo32 y;
		flo32 z;
		flo32 w;
	};
	struct {
		flo32 r;
		flo32 g;
		flo32 b;
		flo32 a;
	};
	struct {
		vec3 xyz;
		flo32  w;
	};
    struct {
        vec3 rgb;
        flo32  w;
    };
    struct {
        vec2 xy;
        vec2 zw;
    };
    struct {
        vec2 P;
        vec2 Q;
    };
	flo32 elem[4];
};

//----------
// quaternion declarations
//----------

union quat {
    struct {
        flo32 x;
        flo32 y;
        flo32 z;
        flo32 w;
    };
    struct {
        vec3  xyz;
        flo32 w;
    };
    flo32 elem[ 4 ];
};

//----------
// rect declarations
//----------

union rect {
	struct {
		vec2 min;
		vec2 max;
	};
	struct {
		flo32 Left;
		flo32 Bottom;
		flo32 Right;
		flo32 Top;
	};
	flo32 elem[4];
};

//----------
// oriented rect declarations
//----------

struct orect {
    vec2 Center;
    vec2 halfDim;
    vec2 xAxis;
    vec2 yAxis;
};

//----------
// circle declarations
//----------

struct circ {
    vec2  Center;
    flo32 Radius;
};

//----------
// capsule2 declarations
//----------

struct CAPSULE2 {
    vec2  P;
    vec2  Q;
    flo32 Radius;
};

//----------
// oriented box declarations
//----------

struct obox {
    vec3 Center;
    vec3 halfDim;
    vec3 xAxis;
    vec3 yAxis;
    vec3 zAxis;
};

//----------
// mat 3x3 declarations
//----------

union mat3 {
	vec3  column[ 3 ];
	flo32 elem[ 9 ];
};

//----------
// mat 4x4 declarations
//----------

union mat4 {
	vec4 column[ 4 ];
	flo32 elem[ 16 ];
};

//----------
// UINT16_PAIR declarations
//----------

union UINT16_QUAD {
    struct {
        uint16 m;
        uint16 n;
        uint16 o;
        uint16 p;
    };
    struct {
        uint16 x;
        uint16 y;
        uint16 z;
        uint16 w;
    };
    uint16 elem[ 4 ];
};

//----------
// INT32_PAIR declarations
//----------

union INT32_PAIR {
    struct {
        int32 m;
        int32 n;
    };
    struct {
        int32 x;
        int32 y;
    };
    int32 elem[ 2 ];
};

//----------
// INT32_TRI declarations
//----------

union INT32_TRI {
    struct {
        int32 m;
        int32 n;
        int32 o;
    };
    struct {
        int32 x;
        int32 y;
        int32 z;
    };
    int32 elem[ 3 ];
};

//----------
// UINT32_PAIR declarations
//----------

union UINT32_PAIR {
    struct {
        uint32 m;
        uint32 n;
    };
    struct {
        uint32 x;
        uint32 y;
    };
    uint32 elem[ 2 ];
};

//----------
// UINT32_TRI declarations
//----------

union UINT32_TRI {
    struct {
        uint32 m;
        uint32 n;
        uint32 o;
    };
    struct {
        uint32 x;
        uint32 y;
        uint32 z;
    };
    uint32 elem[ 3 ];
};

//----------
// UINT32_QUAD declarations
//----------

union UINT32_QUAD {
    struct {
        uint32 m;
        uint32 n;
        uint32 o;
        uint32 p;
    };
    struct {
        uint32 x;
        uint32 y;
        uint32 z;
        uint32 w;
    };
    struct {
        UINT32_PAIR min;
        UINT32_PAIR max;
    };
    uint32 elem[ 4 ];
};

#endif   // STD_INCLUDE_DECL
#ifdef   STD_INCLUDE_FUNC

//----------
// vec2 functions
//----------

inline vec2 Vec2( flo32 a, flo32 b ) { vec2 result = { a, b }; return result; }
inline vec2 operator+( vec2 a, vec2 b ) { vec2 result = { a.x + b.x, a.y + b.y }; return result; }
inline vec2 operator-( vec2 a, vec2 b ) { vec2 result = { a.x - b.x, a.y - b.y }; return result; }
inline vec2 operator-( vec2 a ) { vec2 result = { -a.x, -a.y }; return result; }
inline vec2 operator*( vec2 a, flo32 f ) { vec2 result = { a.x * f, a.y * f }; return result; }
inline vec2 operator*( flo32 f, vec2 a ) { vec2 result = a * f; return result; }
inline vec2 operator*( vec2 a, vec2 b ) { vec2 result = { a.x * b.x, a.y * b.y }; return result; }
inline vec2 operator/( vec2 a, flo32 f ) { vec2 result = {}; if( f != 0.0f ) { result = Vec2( a.x / f, a.y / f ); } return result; }
inline vec2 operator/( vec2 a, vec2 b ) { vec2 result = { ( b.x != 0.0f ) ? a.x / b.x : 0.0f, ( b.y != 0.0f ) ? a.y / b.y : 0.0f }; return result; }
inline vec2 & operator+=( vec2 & a, vec2 b ) { a = a + b; return a; }
inline vec2 & operator-=( vec2 & a, vec2 b ) { a = a - b; return a; }
inline vec2 & operator*=( vec2 & a, flo32 f ) { a = a * f; return a; }
inline vec2 & operator*=( vec2 & a, vec2 b ) { a.x *= b.x; a.y *= b.y; return a; }
inline vec2 & operator/=( vec2 & a, flo32 f ) { a = ( f != 0.0f ) ? a / f : Vec2( 0.0f, 0.0f );	return a; }
inline boo32 operator==( vec2 a, vec2 b ) { boo32 result = ( a.x == b.x ) && ( a.y == b.y ); return result; }
inline boo32 operator!=( vec2 a, vec2 b ) { boo32 result = ( a.x != b.x ) || ( a.y != b.y ); return result; }
inline flo32 GetLengthSq( vec2 a ) { flo32 result = ( a.x * a.x ) + ( a.y * a.y ); return result; }
inline flo32 GetLength( vec2 a ) { flo32 result = sqrtf( GetLengthSq( a ) ); return result; }
inline vec2 GetNormal( vec2 a ) { flo32 denom = GetLength( a ); vec2 result = a / denom; return result; }
inline vec2 GetPerp( vec2 a ) { vec2 result = { -a.y, a.x }; return result; }
inline vec2 getVector( flo32 degrees ) { vec2 result = { cosDegrees( degrees ), sinDegrees( degrees ) }; return result; }
inline flo32 dot( vec2 a, vec2 b ) { flo32 result = a.x * b.x + a.y * b.y; return result; }
inline vec2 lerp( vec2 a, flo32 t, vec2 b ) { vec2 result = Vec2( lerp( a.x, t, b.x ), lerp( a.y, t, b.y ) ); return result; }

inline vec2 clamp01( vec2 a ) {
    vec2 result = {
        clamp01( a.x ),
        clamp01( a.y ),
    };
    return result;
}

internal boo32
IsCCW( vec2 A, vec2 B, vec2 C ) {
    boo32 Result = false;
    
    vec2 AB = B - A;
    vec2 AC = C - A;
    if( dot( GetPerp( AB ), AC ) > 0.0f ) {
        Result = true;
    }
    return Result;
}

//----------
// vec3 functions
//----------

inline vec3 Vec3( flo32 a ) { vec3 result = { a, a, a }; return result; }
inline vec3 Vec3( flo32 a, flo32 b, flo32 c ) { vec3 result = { a, b, c }; return result; }
inline vec3 Vec3( vec2 v, flo32 z ) { vec3 result = { v.x, v.y, z }; return result; }
inline vec3 operator+( vec3 a, vec3 b ) { vec3 result = { a.x + b.x, a.y + b.y, a.z + b.z }; return result; }
inline vec3 operator-( vec3 a, vec3 b ) { vec3 result = { a.x - b.x, a.y - b.y, a.z - b.z }; return result; }
inline vec3 operator-( vec3 a ) { vec3 result = { -a.x, -a.y, -a.z }; return result; }
inline vec3 operator*( vec3 a, flo32 f ) { vec3 result = { a.x * f, a.y * f, a.z * f }; return result; }
inline vec3 operator*( flo32 f, vec3 a ) { vec3 result = a * f; return result; }
inline vec3 operator/( vec3 a, flo32 f ) { vec3 result = {}; if( f != 0.0f ) { result = Vec3( a.x / f, a.y / f, a.z / f ); } return result; }
inline vec3 & operator+=( vec3 & a, vec3 b ) { a = a + b; return a; }
inline vec3 & operator-=( vec3 & a, vec3 b ) { a = a - b; return a; }
inline vec3 & operator*=( vec3 & a, flo32 f ) { a = a * f; return a; }
inline vec3 & operator/=( vec3 & a, flo32 f ) { a = ( f != 0.0f ) ? a / f : Vec3( 0.0f, 0.0f, 0.0f ); return a; }
inline boo32 operator==( vec3 a, vec3 b ) { boo32 result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ); return result; }
inline boo32 operator!=( vec3 a, vec3 b ) { boo32 result = ( a.x != b.x ) || ( a.y != b.y ) || ( a.z != b.z ); return result; }
inline flo32 GetLengthSq( vec3 a ) { flo32 result = ( a.x * a.x ) + ( a.y * a.y ) + ( a.z * a.z ); return result; }
inline flo32 GetLength( vec3 a ) { flo32 result = sqrtf( GetLengthSq( a ) ); return result; }
inline vec3  GetNormal( vec3 a ) { flo32 denom = GetLength( a ); vec3 result = a / denom; return result; }
inline vec3 cross( vec3 a, vec3 b ) {
	vec3 result = {};
	result.x = ( a.y * b.z ) - ( a.z * b.y );
	result.y = ( a.z * b.x ) - ( a.x * b.z );
	result.z = ( a.x * b.y ) - ( a.y * b.x );
	return result;
}
inline vec3 cross( vec3 a, vec3 b, vec3 c ) { vec3 result = cross( b - a, c - a ); return result; }
inline flo32 dot( vec3 a, vec3 b ) { flo32 result = a.x * b.x + a.y * b.y + a.z * b.z; return result; }
inline vec3 lerp( vec3 a, flo32 t, vec3 b ) { vec3 result = Vec3( lerp( a.x, t, b.x ), lerp( a.y, t, b.y ), lerp( a.z, t, b.z ) ); return result; }

//----------
// vec4 functions
//----------

inline vec4 Vec4() { vec4 result = {}; return result; }
inline vec4 Vec4( flo32 a, flo32 b, flo32 c, flo32 d ) { vec4 result = { a, b, c, d }; return result; }
inline vec4 Vec4( vec2 v, flo32 z, flo32 w ) { vec4 result = { v.x, v.y, z, w }; return result; }
inline vec4 Vec4( vec2 xy, vec2 zw ) { vec4 result = { xy.x, xy.y, zw.x, zw.y }; return result; }
inline vec4 Vec4( vec3 vector, flo32 w ) { vec4 result = { vector.x, vector.y, vector.z, w }; return result; }
inline vec4 Vec4( flo32 a ) { vec4 result = { a, a, a, a }; return result; }
inline vec4 operator+( vec4 a, vec4 b ) { vec4 result = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; return result; }
inline vec4 operator-( vec4 a, vec4 b ) { vec4 result = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; return result; }
inline vec4 operator*( vec4 a, vec4 b ) { vec4 result = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; return result; }
inline vec4 operator*( vec4 a, flo32 f ) { vec4 result = { a.x * f, a.y * f, a.z * f, a.w * f }; return result; }
inline vec4 operator*( flo32 f, vec4 a ) { vec4 result = a * f; return result; }
inline vec4 operator/( vec4 a, flo32 f ) { vec4 result = {}; if( f != 0.0f ) { result = Vec4( a.x / f, a.y / f, a.z / f, a.w / f ); } return result; }
inline vec4 operator-( vec4 a ) { vec4 result = { -a.x, -a.y, -a.z, -a.w }; return result; }
inline vec4 & operator+=( vec4 & a, vec4 b ) { a = a + b; return a; }
inline vec4 & operator-=( vec4 & a, vec4 b ) { a = a - b; return a; }
inline vec4 & operator*=( vec4 & a, flo32 f ) { a = a * f; return a; }
inline vec4 & operator*=( vec4 & a, vec4  b ) { a = a * b; return a; }
inline vec4 & operator/=( vec4 & a, flo32 f ) { a = ( f != 0.0f ) ? a / f : Vec4( 0.0f, 0.0f, 0.0f, 0.0f ); return a; }
inline boo32 operator==( vec4 a, vec4 b ) { boo32 result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ) && ( a.w == b.w ); return result; }
inline boo32 operator!=( vec4 a, vec4 b ) { boo32 result = ( a.x != b.x ) || ( a.y != b.y ) || ( a.z != b.z ) || ( a.w != b.w ); return result; }
inline flo32 GetLengthSq( vec4 a ) { flo32 result = ( a.x * a.x ) + ( a.y * a.y ) + ( a.z * a.z ) + ( a.w * a.w ); return result; }
inline flo32 GetLength( vec4 a ) { flo32 result = sqrtf( GetLengthSq( a ) ); return result; }
inline vec4 GetNormal( vec4 a ) { flo32 denom = GetLength( a ); vec4 result = a / denom; return result; }
inline flo32 dot( vec4 a, vec4 b ) { flo32 result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; return result; }
inline vec4 lerp( vec4 a, flo32 t, vec4 b ) { vec4 result = Vec4( lerp( a.x, t, b.x ), lerp( a.y, t, b.y ), lerp( a.z, t, b.z ), lerp( a.w, t, b.w ) ); return result; }

internal vec4
toV4( uint8 r, uint8 g, uint8 b, uint8 a ) {
	vec4 result = { ( flo32 )r / 255.0f, ( flo32 )g / 255.0f, ( flo32 )b / 255.0f, ( flo32 )a / 255.0f };
	return result;
}

internal vec4
modAlpha( vec4 Color, flo32 alpha ) {
    vec4 result = Color;
    result.rgb *= alpha;
    result.a    = alpha;
    return result;
}

//----------
// quaternion functions
//----------

inline quat Quat( flo32 x, flo32 y, flo32 z, flo32 w ) { quat result = { x, y, z, w }; return result; }
inline quat Quat( vec3 axis, flo32 radians ) {
    flo32 rad = radians * 0.5f;
    flo32 c   = cosf( rad );
    flo32 s   = sinf( rad );
    
    quat result = {};
    result.x = axis.x * s;
    result.y = axis.y * s;
    result.z = axis.z * s;
    result.w = c;
    return result;
}
inline quat QuatDef() { quat result = Quat( 0.0f, 0.0f, 0.0f, 1.0f ); return result; }
inline quat QuatInv( quat A ) { quat result = { -A.x, -A.y, -A.z, A.w }; return result; }

internal quat
GetNormal( quat a ) {
    flo32 length = sqrtf( a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w );
    
    quat result = {};
    for( uint32 iter = 0; iter < 4; iter++ ) {
        result.elem[ iter ] = a.elem[ iter ] / length;
    }
    return result;
}

internal quat
operator*( quat b, quat a ) {
    vec3 vA = Vec3( a.x, a.y, a.z );
    vec3 vB = Vec3( b.x, b.y, b.z );
    vec3 vAxis = ( vB * a.w ) + ( vA * b.w ) + cross( vB, vA );
    //vec3 vAxis = ( vB * a.w ) + ( vA * b.w ) + cross( vA, vB );
    
    quat result = {};
    result.x = vAxis.x;
    result.y = vAxis.y;
    result.z = vAxis.z;
    result.w = ( a.w * b.w ) - dot( vA, vB );
    result = GetNormal( result );
    return result;
}

internal quat
lerp( quat a, flo32 t, quat b ) {
    quat result = {};
    for( uint32 iter = 0; iter < 4; iter++ ) {
        result.elem[ iter ] = lerp( a.elem[ iter ], t, b.elem[ iter ] );
    }
    result = GetNormal( result );
    return result;
}

internal quat
operator*( quat a, flo32 t ) {
    quat result = a;
    for( uint32 iter = 0; iter < 4; iter++ ) {
        result.elem[ iter ] *= t;
    }
    return result;
}

internal quat
RightToLeft( quat a ) {
    quat result = {};
    result.x = -a.x;
    result.y = -a.z;
    result.z = -a.y;
    result.w =  a.w;
    return result;
}

//----------
// rect functions
//----------

inline rect Rect( flo32 Left, flo32 Bottom, flo32 Right, flo32 Top ) { rect result = { Left, Bottom, Right, Top }; return result; }
inline vec2 getP( rect r, vec2 t ) {
    vec2 result = {};
    result.x = r.Left   + ( r.Right - r.Left   ) * t.x;
    result.y = r.Bottom + ( r.Top   - r.Bottom ) * t.y;
    return result;
}
inline flo32 getWidth(  rect r ) { flo32 result = ( r.Right - r.Left ); return result; }
inline flo32 getHeight( rect r ) { flo32 result = ( r.Top - r.Bottom ); return result; }
inline vec2 getDim( rect r ) { vec2 result = { getWidth( r ), getHeight( r ) }; return result; }
inline vec2 getHalfDim( rect r ) { vec2 result = getDim( r ) * 0.5f; return result; }
inline vec2 getTL( rect r ) { vec2 result = Vec2( r.Left,  r.Top    ); return result; }
inline vec2 getTR( rect r ) { vec2 result = Vec2( r.Right, r.Top    ); return result; }
inline vec2 getBL( rect r ) { vec2 result = Vec2( r.Left,  r.Bottom ); return result; }
inline vec2 getBR( rect r ) { vec2 result = Vec2( r.Right, r.Bottom ); return result; }
inline vec2 getTC( rect r ) { vec2 result = Vec2( ( r.Left + r.Right ) * 0.5f, r.Top    ); return result; }
inline vec2 getBC( rect r ) { vec2 result = Vec2( ( r.Left + r.Right ) * 0.5f, r.Bottom ); return result; }
inline vec2 getLC( rect r ) { vec2 result = Vec2( r.Left,  ( r.Bottom + r.Top ) * 0.5f );  return result; }
inline vec2 getRC( rect r ) { vec2 result = Vec2( r.Right, ( r.Bottom + r.Top ) * 0.5f );  return result; }
inline rect rectTLD( vec2 TopLeft,     vec2 dim ) { rect result = Rect( TopLeft.x,             TopLeft.y - dim.y,  TopLeft.x + dim.x,    TopLeft.y             ); return result; }
inline rect rectTRD( vec2 TopRight,    vec2 dim ) { rect result = Rect( TopRight.x - dim.x,    TopRight.y - dim.y, TopRight.x,           TopRight.y            ); return result; }
inline rect rectBLD( vec2 BottomLeft,  vec2 dim ) { rect result = Rect( BottomLeft.x,          BottomLeft.y,       BottomLeft.x + dim.x, BottomLeft.y  + dim.y ); return result; }
inline rect rectBRD( vec2 BottomRight, vec2 dim ) { rect result = Rect( BottomRight.x - dim.x, BottomRight.y,      BottomRight.x,        BottomRight.y + dim.y ); return result; }
inline rect rectLCD( vec2   LeftCenter, vec2 dim ) { flo32 halfDimY = dim.y * 0.5f; rect result = Rect(  LeftCenter.x,  LeftCenter.y - halfDimY,  LeftCenter.x + dim.x,           LeftCenter.y + halfDimY ); return result; }
inline rect rectRCD( vec2  RightCenter, vec2 dim ) { flo32 halfDimY = dim.y * 0.5f; rect result = Rect( RightCenter.x - dim.x,         RightCenter.y - halfDimY, RightCenter.x, RightCenter.y + halfDimY ); return result; }
inline rect rectBCD( vec2 BottomCenter, vec2 dim ) { flo32 halfDimX = dim.x * 0.5f; rect result = Rect( BottomCenter.x - halfDimX, BottomCenter.y,      BottomCenter.x + halfDimX, BottomCenter.y + dim.y ); return result; }
inline rect rectTCD( vec2    TopCenter, vec2 dim ) { flo32 halfDimX = dim.x * 0.5f; rect result = Rect(    TopCenter.x - halfDimX, TopCenter.y - dim.y,    TopCenter.x + halfDimX, TopCenter.y            ); return result; }
inline rect rectCD(  vec2 Center, vec2 dim ) { vec2 halfDim = dim * 0.5f; rect result = Rect( Center.x - halfDim.x, Center.y - halfDim.y, Center.x + halfDim.x, Center.y + halfDim.y ); return result; }
inline rect rectCHD( vec2 Center, vec2 halfDim ) { rect result = Rect( Center.x - halfDim.x, Center.y - halfDim.y, Center.x + halfDim.x, Center.y + halfDim.y ); return result; }
inline rect rectCR(  vec2 Center, flo32 Radius ) { rect result = Rect( Center.x - Radius, Center.y - Radius, Center.x + Radius, Center.y + Radius ); return result; }
inline rect rectMM(  vec2 min, vec2 max ) { rect result = Rect( min.x, min.y, max.x, max.y ); return result; }
inline rect addDim( rect r, flo32 x0, flo32 y0, flo32 x1, flo32 y1 ) { rect result = Rect( r.Left - x0, r.Bottom - y0, r.Right + x1, r.Top + y1 ); return result; }
inline rect addDim( rect r, vec2 dim ) { rect result = Rect( r.Left - dim.x, r.Bottom - dim.y, r.Right + dim.x, r.Top + dim.y ); return result; }
inline rect addRadius( rect r, flo32 Radius ) { rect result = Rect( r.Left - Radius, r.Bottom - Radius, r.Right + Radius, r.Top + Radius ); return result; }
inline vec2 getCenter( rect r ) { vec2 result = Vec2( ( r.Left + r.Right ) * 0.5f, ( r.Bottom + r.Top ) * 0.5f ); return result; }
inline rect rectCompare( vec2 point0, vec2 point1 ) {
    boo32 xMin_usePoint0 = ( point0.x < point1.x );
    boo32 yMin_usePoint0 = ( point0.y < point1.y );
    rect result = Rect(
                       ( xMin_usePoint0 ? point0.x : point1.x ), ( yMin_usePoint0 ? point0.y : point1.y ),
                       ( xMin_usePoint0 ? point1.x : point0.x ), ( yMin_usePoint0 ? point1.y : point0.y )
                       );
    return result;
}
inline rect rectExtrema( vec2 P, vec2 Q ) {
    rect result = { minValue( P.x, Q.x ), minValue( P.y, Q.y ), maxValue( P.x, Q.x ), maxValue( P.y, Q.y ) };
    return result;
}
inline rect rectExtrema( rect R, rect S ) {
    rect result = { minValue( R.Left, S.Left ), minValue( R.Bottom, S.Bottom ), maxValue( R.Right, S.Right ), maxValue( R.Top, S.Top ) };
    return result;
}
inline rect addOffset( rect r, vec2 offset ) { rect result = Rect( r.Left + offset.x, r.Bottom + offset.y, r.Right + offset.x, r.Top + offset.y ); return result; }

inline boo32 IsInBound( vec2 point, rect Bound ) { boo32 result = ( point.x >= Bound.Left ) && ( point.y >= Bound.Bottom ) && ( point.x <= Bound.Right ) && ( point.y <= Bound.Top ); return result; }

internal vec2
ToDirection2D( flo32 radians ) {
    vec2 result = Vec2( cosf( radians ), sinf( radians ) );
    return result;
}

internal flo32
ToRadians( vec2 v ) {
    flo32 result = atan2f( v.y, v.x );
    if( result < 0.0f ) {
        result += ( PI * 2.0f );
    }
    return result;
}

internal vec2
RandomV2() {
    vec2 result = { RandomF32(), RandomF32() };
    return result;
}

internal vec2
RandomDirection2D() {
    flo32 radians = RandomF32() * 2.0f * PI;
    vec2 result = ToDirection2D( radians );
    return result;
}

internal vec2
getT( rect a, vec2 t ) {
    vec2 result = {};
    result.x = lerp( a.Left,   t.x, a.Right );
    result.y = lerp( a.Bottom, t.y, a.Top   );
    return result;
}

internal vec2
getRandomPoint( rect a ) {
    vec2 t = RandomV2();
    vec2 result = {};
    result.x = lerp( a.Left,   t.x, a.Right );
    result.y = lerp( a.Bottom, t.y, a.Top   );
    return result;
}

internal vec2
clampToBound( vec2 P, rect A ) {
    vec2 result = P;
    result.x = maxValue( result.x, A.Left   );
    result.x = minValue( result.x, A.Right  );
    result.y = maxValue( result.y, A.Bottom );
    result.y = minValue( result.y, A.Top    );
    return result;
}

internal rect
getIntersect( rect A, rect B ) {
    rect result = {};
    if( !( ( A.Right < B.Left ) || ( A.Left > B.Right ) || ( A.Top < B.Bottom ) || ( A.Bottom > B.Top ) ) ) {
        result.Left   = maxValue( A.Left,   B.Left   );
        result.Right  = minValue( A.Right,  B.Right  );
        result.Bottom = maxValue( A.Bottom, B.Bottom );
        result.Top    = minValue( A.Top,    B.Top    );
    }
    return result;
}

internal boo32
doIntersect( rect A, rect B ) {
    boo32 result = !( ( A.Right < B.Left ) || ( A.Left > B.Right ) || ( A.Top < B.Bottom ) || ( A.Bottom > B.Top ) );
    return result;
}

internal vec2
clamp( vec2 a, flo32 lo, flo32 hi ) {
    vec2 result = {};
    result.x = clamp( a.x, lo, hi );
    result.y = clamp( a.y, lo, hi );
    return result;
}

//----------
// oriented rect functions
//----------

internal orect
toORect( rect R ) {
    orect Result = {};
    Result.Center  = getCenter( R );
    Result.xAxis   = Vec2( 1.0f, 0.0f );
    Result.yAxis   = Vec2( 0.0f, 1.0f );
    Result.halfDim = getHalfDim( R );
    return Result;
}

internal orect
addLeft( orect R, flo32 t ) {
    flo32 tA = t * 0.5f;
    
    orect Result = R;
    Result.Center    -= ( R.xAxis * tA );
    Result.halfDim.x += tA;
    return Result;
}

internal orect
addRight( orect R, flo32 t ) {
    flo32 tA = t * 0.5f;
    
    orect Result = R;
    Result.Center    += ( R.xAxis * tA );
    Result.halfDim.x += tA;
    return Result;
}

internal orect
addTop( orect R, flo32 t ) {
    flo32 tA = t * 0.5f;
    
    orect Result = R;
    Result.Center    += ( R.yAxis * tA );
    Result.halfDim.y += tA;
    return Result;
}

internal orect
orectCD( vec2 Center, vec2 dim, flo32 radians = 0.0f ) {
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = GetPerp( xAxis );
    
    orect result = {};
    result.xAxis   = xAxis;
    result.yAxis   = yAxis;
    result.Center  = Center;
    result.halfDim = dim * 0.5f;
    return result;
}

internal orect
orectCHD( vec2 Center, vec2 halfDim, flo32 radians = 0.0f ) {
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = GetPerp( xAxis );
    
    orect result = {};
    result.xAxis   = xAxis;
    result.yAxis   = yAxis;
    result.Center  = Center;
    result.halfDim = halfDim;
    return result;
}

internal orect
orectXD( vec2 Left, vec2 xAxis, vec2 dim ) {
    vec2 yAxis = GetPerp( xAxis );
    
    orect result = {};
    result.xAxis   = xAxis;
    result.yAxis   = yAxis;
    result.Center  = Left + xAxis * ( dim.x * 0.5f );
    result.halfDim = dim * 0.5f;
    return result;
}

internal orect
orectCXD( vec2 Center, vec2 xAxis, vec2 dim ) {
    vec2 yAxis = GetPerp( xAxis );
    
    orect result = {};
    result.xAxis   = xAxis;
    result.yAxis   = yAxis;
    result.Center  = Center;
    result.halfDim = dim * 0.5f;
    
    return result;
}

internal boo32
IsInBound( vec2 P, orect Bound ) {
    vec2 V = P - Bound.Center;
    boo32 result = ( fabsf( dot( V, Bound.xAxis ) ) <= Bound.halfDim.x ) && ( fabsf( dot( V, Bound.yAxis ) ) <= Bound.halfDim.y );
    return result;
}

//----------
// circle functions
//----------

internal circ
Circ( vec2 Center, flo32 Radius ) {
    circ result = {};
    result.Center = Center;
    result.Radius = Radius;
    return result;
}

internal boo32
IsInBound( vec2 p, circ c ) {
    boo32 result = false;
    
    flo32 RadiusSq = c.Radius * c.Radius;
    flo32 distSq   = GetLengthSq( p - c.Center );
    if( distSq <= RadiusSq ) {
        result = true;
    }
    return result;
}

//----------
// tri functions
//----------

internal boo32
IsInBound( vec2 P, vec2 A, vec2 B, vec2 C ) {
    boo32 Result = false;
    
    vec2 X = B - A;
    vec2 Y = C - A;
    vec2 Z = P - A;
    
    flo32 dotXX = dot( X, X );
    flo32 dotXY = dot( X, Y );
    flo32 dotXZ = dot( X, Z );
    flo32 dotYY = dot( Y, Y );
    flo32 dotYZ = dot( Y, Z );
    flo32 dotZZ = dot( Z, Z );
    
    flo32 detA = ( dotXX * dotYY ) - ( dotXY * dotXY );
    if( detA != 0.0f ) {
        flo32 detX = ( dotXZ * dotYY ) - ( dotXY * dotYZ );
        flo32 detY = ( dotXX * dotYZ ) - ( dotXY * dotXZ );
        
        flo32 v = detX / detA;
        flo32 w = detY / detA;
        if( ( v >= 0.0f ) && ( w >= 0.0f ) && ( v + w <= 1.0f ) ) {
            Result = true;
        }
    }
    
    return Result;
}

//----------
// capsule2 functions
//----------

internal CAPSULE2
Capsule2( vec2 P, vec2 Q, flo32 Radius ) {
    CAPSULE2 result = {};
    result.P = P;
    result.Q = Q;
    result.Radius = Radius;
    return result;
}

//----------
// 3x3 matrix functions
//----------

inline mat3 Mat3( vec3 a, vec3 b, vec3 c ) {
	mat3 result = { a, b, c };
	return result;
}

inline vec3 row( mat3 matrix, int32 rowIndex ) {
	vec3 result = {};
	for( int32 counter = 0; counter < 3; counter++ ) {
		result.elem[ counter ] = matrix.elem[ counter * 3 + rowIndex ];
	}
	return result;
}

internal mat3
mat3_transpose( mat3 a ) {
	mat3 result = {};
	for( int32 index = 0; index < 3; index++ ) {
		result.column[ index ] = row( a, index );
	}
	return result;
}

internal mat3
mat3_inverse( mat3 a ) {
    mat3 result = {};
    
    vec3 U = a.column[ 0 ];
    vec3 V = a.column[ 1 ];
    vec3 W = a.column[ 2 ];
    
    flo32 denom = dot( U, cross( V, W ) );
    if( denom != 0.0f ) {
        vec3 A = cross( V, W ) / denom;
        vec3 B = cross( W, U ) / denom;
        vec3 C = cross( U, V ) / denom;
        
        result = mat3_transpose( Mat3( A, B, C ) );
    }
    
    return result;
}

internal mat3
mat3_byRow( vec3 rowA, vec3 rowB, vec3 rowC ) {
	mat3 result = { rowA.x, rowB.x, rowC.x, rowA.y, rowB.y, rowC.y, rowA.z, rowB.z, rowC.z };
	return result;
}

// internal vec3
// operator*( mat3 a, vec3 b ) {
// vec3 result = {};
// for( int32 index = 0; index < 3; index++ ) {
// result.elem[ index ] = dot( row( a, index ), b );
// }
// return result;
// }

internal vec3
operator*( mat3 a, vec3 b ) {
	vec3 result = {};
    for( int32 index = 0; index < 3; index++ ) {
        result.elem[ index ] = dot( row( a, index ), b );
    }
	return result;
}

internal mat3
operator*( mat3 a, mat3 b ) {
    mat3 result = {};
    for( int32 columnIndex = 0; columnIndex < 3; columnIndex++ ) {
        for( int32 rowIndex = 0; rowIndex < 3; rowIndex++ ) {
            int32 elemIndex = columnIndex * 3 + rowIndex;
            result.elem[ elemIndex ] = dot( row( a, rowIndex ), b.column[ columnIndex ] );
        }
    }
    return result;
}

internal mat3
mat3_toRotation( flo32 x, flo32 y, flo32 z, flo32 radians ) {
	flo32 cos0 = cosf( radians );
	flo32 sin0 = sinf( radians );
	flo32 invCos = 1.0f - cos0;
	
	flo32 xyInv = x * y * invCos;
	flo32 xzInv = x * z * invCos;
	flo32 yzInv = y * z * invCos;
	flo32 x2Inv = x * x * invCos;
	flo32 y2Inv = y * y * invCos;
	flo32 z2Inv = z * z * invCos;
	flo32 xSin  = x * sin0;
	flo32 ySin  = y * sin0;
	flo32 zSin  = z * sin0;
	
	mat3 result = {
		x2Inv + cos0, xyInv + zSin, xzInv - ySin,
		xyInv - zSin, y2Inv + cos0, yzInv + xSin,
		xzInv + ySin, yzInv - xSin, z2Inv + cos0,
	};
	return result;
}

internal flo32
mat3_cofactor( mat3 A, uint32 row_index, uint32 column_index ) {
    // cofactor = -1^i+j * det( minor )
    flo32  mat2_elem[ 4 ] = {};
    uint32 dest_index = 0;
    for( uint32 j = 0; j < 3; j++ ) {
        if( j != column_index ) {
            for( uint32 i = 0; i < 3; i++ ) {
                if( i != row_index ) {
                    mat2_elem[ dest_index++ ] = A.elem[ j * 3 + i ];
                }
            }
        }
    }
    
    flo32 result = mat2_elem[ 0 ] * mat2_elem[ 3 ] - mat2_elem[ 1 ] * mat2_elem[ 2 ];
    if( ( ( row_index + column_index ) % 2 ) == 1 ) {
        result = -result;
    }
    return result;
}

internal flo32
mat3_determinant( mat3 A ) {
    // det = sum of element * cofactor
    flo32 result = 0.0f;
    for( uint32 iter = 0; iter < 3; iter++ ) {
        vec3 column = A.column[ iter ];
        
        flo32 element  = column.z;
        flo32 cofactor = mat3_cofactor( A, 2, iter );
        
        result += ( element * cofactor );
    }
    return result;
}

//----------
// 4x4 matrix functions
//----------

inline mat4
operator-( mat4 a ) {
    mat4 result = {};
    for( uint32 iElem = 0; iElem < 16; iElem++ ) {
        result.elem[ iElem ] = -a.elem[ iElem ];
    }
    return result;
}

inline vec4 getRow( mat4 matrix, int32 rowIndex ) {
	vec4 result = {};
	for( int32 counter = 0; counter < 4; counter++ ) {
		result.elem[ counter ] = matrix.elem[ counter * 4 + rowIndex ];
	}
	return result;
}

inline mat4 mat4_idEntity() {
    mat4 result = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return result;
}

internal mat4
mat4_frustum( flo32 Left, flo32 Right, flo32 Bottom, flo32 Top, flo32 nearPlane, flo32 farPlane ) {
	flo32 width = Right - Left;
	flo32 Height = Top - Bottom;
	flo32 depth = farPlane - nearPlane;
	flo32 nearPlane2x = nearPlane * 2.0f;
	
	mat4 result = {
        nearPlane2x / width, 							  0.0f, 										 0.0f,  0.0f,	// column 1
        0.0f, 		 nearPlane2x / Height, 										 0.0f,  0.0f,	// column 2
		( Right + Left ) / width, ( Top + Bottom ) / Height, -( farPlane + nearPlane ) / depth, -1.0f,	// column 3
        0.0f, 							  0.0f, 	 -nearPlane2x * farPlane / depth,  0.0f,	// column 4
	};
	
	return result;
}

// NOTE: opengl
// internal mat4
// mat4_perspective( vec2 window_dim, flo32 fovAngle, flo32 nearPlane, flo32 farPlane ) {
// flo32 aspectRatio = window_dim.x / window_dim.y;
// flo32 fovHalfAngle = fovAngle * 0.5f;
// flo32 cot = cosDegrees( fovHalfAngle ) / sinDegrees( fovHalfAngle );
// flo32 depth = nearPlane - farPlane;

// mat4 result = {
// cot / aspectRatio, 0.0f, 0.0f, 0.0f,	// column 1
// 0.0f, cot, 0.0f, 0.0f,	// column 2
// 0.0f, 0.0f, ( farPlane + nearPlane ) / depth, -1.0f,	// column 3
// 0.0f, 0.0f, ( 2.0f * nearPlane * farPlane ) / depth,  0.0f,	// column 4
// };
// return result;
// }

// NOTE: direct3d
internal mat4
mat4_perspective( vec2 window_dim, flo32 fovRadians, flo32 nearPlane, flo32 farPlane ) {
	flo32 aspectRatio = window_dim.x / window_dim.y;
	flo32 fovHalfRadians = fovRadians * 0.5f;
	flo32 cot = cosf( fovHalfRadians ) / sinf( fovHalfRadians );
	//flo32 depth = farPlane - nearPlane;
	flo32 depth = nearPlane - farPlane;
	
	//mat4 result = {
    //cot / aspectRatio, 0.0f, 0.0f, 0.0f,	// column 1
    //0.0f, cot, 0.0f, 0.0f,	// column 2
    //0.0f, 0.0f, farPlane / depth, 1.0f,	// column 3
    //0.0f, 0.0f, -( farPlane * nearPlane ) / depth, 0.0f,	// column 4
	//};
	
	mat4 result = {
		cot / aspectRatio, 0.0f, 0.0f, 0.0f,	// column 1
		0.0f, cot, 0.0f, 0.0f,	// column 2
		0.0f, 0.0f, ( farPlane + nearPlane ) / depth, -1.0f,	// column 3
		0.0f, 0.0f, ( 2.0f * nearPlane * farPlane ) / depth,  0.0f,	// column 4
	};
    
	return result;
}

// NOTE: opengl z-coord [-1..1]
// internal mat4
// mat4_orthographic( flo32 Left, flo32 Bottom, flo32 Right, flo32 Top, flo32 nearPlane, flo32 farPlane ) {
// flo32 width = Right - Left;
// flo32 Height = Top - Bottom;
// flo32 depth = farPlane - nearPlane;

// mat4 result = {
// 2.0f / width, 0.0f, 0.0f, 0.0f,	// column 1
// 0.0f, 2.0f / Height, 0.0f, 0.0f,	// column 2
// 0.0f, 0.0f, -2.0f / depth, 0.0f,	// column 3
// -( Right + Left ) / width, -( Top + Bottom ) / Height, -( farPlane + nearPlane ) / depth, 1.0f,	// column 4
// };
// return result;
// }

// NOTE: direct3d z-coord [0..1]
internal mat4
mat4_orthographic( flo32 Left, flo32 Bottom, flo32 Right, flo32 Top ) {
	flo32 width = Right - Left;
	flo32 Height = Top - Bottom;
	
	mat4 result = {
		2.0f / width, 0.0f, 0.0f, 0.0f,	// column 1
		0.0f, 2.0f / Height, 0.0f, 0.0f,	// column 2
		0.0f, 0.0f, 1.0f, 0.0f,	// column 3
		-( Right + Left ) / width, -( Top + Bottom ) / Height, 0.0f, 1.0f,	// column 4
	};
	return result;
}

internal mat4
mat4_orthographic( rect Bound ) {
	mat4 result = mat4_orthographic( Bound.Left, Bound.Bottom, Bound.Right, Bound.Top );
	return result;
}

internal mat4
mat4_transpose( mat4 a ) {
	mat4 result = {};
	for( int32 index = 0; index < 4; index++ ) {
		result.column[ index ] = getRow( a, index );
	}
	return result;
}

internal mat4
mat4_translate( flo32 x, flo32 y, flo32 z ) {
	mat4 result = mat4_idEntity();
	result.column[3] = Vec4( x, y, z, 1.0f );
	return result;
}

inline mat4
mat4_translate( vec2 xy, flo32 z = 0.0f ) {
	mat4 result = mat4_translate( xy.x, xy.y, z );
	return result;
}

inline mat4
mat4_translate( vec3 a ) {
	mat4 result = mat4_translate( a.x, a.y, a.z );
	return result;
}

inline mat4
mat4_rotate( vec3 axis, flo32 radians ) {
	axis = GetNormal( axis );
	
	flo32 cosValue = cosf( radians );
	flo32 sinValue = sinf( radians );
	flo32 invCos = 1.0f - cosValue;
	
	flo32 xyInv = axis.x * axis.y * invCos;
	flo32 xzInv = axis.x * axis.z * invCos;
	flo32 yzInv = axis.y * axis.z * invCos;
	flo32 x2Inv = squareValue( axis.x ) * invCos;
	flo32 y2Inv = squareValue( axis.y ) * invCos;
	flo32 z2Inv = squareValue( axis.z ) * invCos;
	flo32 xSin = axis.x * sinValue;
	flo32 ySin = axis.y * sinValue;
	flo32 zSin = axis.z * sinValue;
	
	mat4 result = {
		x2Inv + cosValue, xyInv + zSin, xzInv - ySin, 0.0f,
		xyInv - zSin, y2Inv + cosValue, yzInv + xSin, 0.0f,
		xzInv + ySin, yzInv - xSin, z2Inv + cosValue, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	return result;
}

internal mat4
operator*( mat4 a, mat4 b ) {
	mat4 result = {};
	for( int32 columnIndex = 0; columnIndex < 4; columnIndex++ ) {
		for( int32 rowIndex = 0; rowIndex < 4; rowIndex++ ) {
			int32 elemIndex = columnIndex * 4 + rowIndex;
			result.elem[ elemIndex ] = dot( getRow( a, rowIndex ), b.column[ columnIndex ] );
		}
	}
	return result;
}

internal vec4
operator*( mat4 a, vec4 b ) {
	vec4 result = {};
	for( int32 elemIndex = 0; elemIndex < 4; elemIndex++ ) {
		result.elem[ elemIndex ] = dot( getRow( a, elemIndex ), b );
	}
	return result;
}

inline mat4 & operator*=( mat4 & a, mat4 b ) { a = a * b; return a; }

internal mat4
mat4_scale( flo32 xScale, flo32 yScale, flo32 zScale ) {
	mat4 result = { xScale, 0.0f, 0.0f, 0.0f,
        0.0f, yScale, 0.0f, 0.0f,
        0.0f, 0.0f, zScale, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
	};
	return result;
}

internal mat4
mat4_scale( vec2 xyScale, flo32 zScale ) {
    mat4 result = mat4_scale( xyScale.x, xyScale.y, zScale );
    return result;
}

inline mat4
mat4_scale( vec3 scale ) {
	mat4 result = mat4_scale( scale.x, scale.y, scale.z );
	return result;
}

inline mat4
mat4_reflect( vec3 normal ) {
    vec3 X = Vec3( 1.0f, 0.0f, 0.0f );
    vec3 Y = Vec3( 0.0f, 1.0f, 0.0f );
    vec3 Z = Vec3( 0.0f, 0.0f, 1.0f );
    
    vec3 X0 = X - normal * ( 2.0f * dot( X, normal ) );
    vec3 Y0 = Y - normal * ( 2.0f * dot( Y, normal ) );
    vec3 Z0 = Z - normal * ( 2.0f * dot( Z, normal ) );
    
    mat4 result = {};
    result.column[ 0 ] = Vec4( X0, 0.0f );
    result.column[ 1 ] = Vec4( Y0, 0.0f );
    result.column[ 2 ] = Vec4( Z0, 0.0f );
    result.column[ 3 ] = Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
    return result;
}

internal mat4
operator*( mat4 a, flo32 f ) {
    mat4 result = a;
    for( uint32 elem_index = 0; elem_index < 16; elem_index++ ) {
        result.elem[ elem_index ] *= f;
    }
    return result;
}

internal mat4
mat4_column( vec3 X, vec3 Y, vec3 Z, vec3 W = Vec3( 0.0f, 0.0f, 0.0f ) ) {
    mat4 result = {};
    result.column[ 0 ] = Vec4( X, 0.0f );
    result.column[ 1 ] = Vec4( Y, 0.0f );
    result.column[ 2 ] = Vec4( Z, 0.0f );
    result.column[ 3 ] = Vec4( W, 1.0f );
    return result;
}

internal flo32
mat4_cofactor( mat4 A, uint32 row_index, uint32 column_index ) {
    // cofactor = -1^i+j * det( minor )
    mat3   minor = {};
    uint32 dest_index = 0;
    for( uint32 j = 0; j < 4; j++ ) {
        if( j != column_index ) {
            for( uint32 i = 0; i < 4; i++ ) {
                if( i != row_index ) {
                    minor.elem[ dest_index++ ] = A.elem[ j * 4 + i ];
                }
            }
        }
    }
    
    flo32 result = mat3_determinant( minor );
    if( ( ( row_index + column_index ) % 2 ) == 1 ) {
        result = -result;
    }
    return result;
}

internal flo32
mat4_determinant( mat4 A ) {
    // sum of element * cofactor
    flo32 result = 0.0f;
    for( uint32 iter = 0; iter < 4; iter++ ) {
        vec4 column = A.column[ iter ];
        
        flo32 element  = column.w;
        flo32 cofactor = mat4_cofactor( A, 3, iter );
        
        result += ( element * cofactor );
    }
    return result;
}

internal mat4
mat4_cofactor( mat4 A ) {
    mat4 result = {};
    for( uint32 j = 0; j < 4; j++ ) {
        for( uint32 i = 0; i < 4; i++ ) {
            result.elem[ j * 4 + i ] = mat4_cofactor( A, i, j );
        }
    }
    return result;
}

internal mat4
mat4_adjugate( mat4 A ) {
    mat4 cofactor = mat4_cofactor( A );
    mat4 result   = mat4_transpose( cofactor );
    return result;
}

internal mat4
mat4_inverse( mat4 A ) {
    // 1 / |A| * M
    mat4 result = {};
    
    flo32 det = mat4_determinant( A );
    mat4  M   = mat4_adjugate   ( A );
    
    if( det != 0.0f ) {
        result = M * ( 1.0f / det );
    }
    
    return result;
}

internal mat4
mat4_quaternion( quat orientation ) {
    flo32 _2xSq = 2.0f * ( orientation.x * orientation.x );
    flo32 _2ySq = 2.0f * ( orientation.y * orientation.y );
    flo32 _2zSq = 2.0f * ( orientation.z * orientation.z );
    
    flo32 _2wx = 2.0f * orientation.w * orientation.x;
    flo32 _2wy = 2.0f * orientation.w * orientation.y;
    flo32 _2wz = 2.0f * orientation.w * orientation.z;
    flo32 _2xy = 2.0f * orientation.x * orientation.y;
    flo32 _2xz = 2.0f * orientation.x * orientation.z;
    flo32 _2yz = 2.0f * orientation.y * orientation.z;
    
    mat4 result = {};
    result.elem[  0 ] = 1.0f - _2ySq - _2zSq;
    result.elem[  1 ] = _2xy + _2wz;
    result.elem[  2 ] = _2xz - _2wy;
    
    result.elem[  4 ] = _2xy - _2wz;
    result.elem[  5 ] = 1.0f - _2xSq - _2zSq;
    result.elem[  6 ] = _2yz + _2wx;
    
    result.elem[  8 ] = _2xz + _2wy;
    result.elem[  9 ] = _2yz - _2wx;
    result.elem[ 10 ] = 1.0f - _2xSq - _2ySq;
    
    result.elem[ 15 ] = 1.0f;
    
    return result;
};

#endif	// STD_INCLUDE_FUNC