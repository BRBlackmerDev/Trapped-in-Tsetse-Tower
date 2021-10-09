
internal void
addWallSlide( PARTICLE_STATE * Particle, vec2 PlayerP, boo32 onRight ) {
    vec2 P = PlayerP;
    if( onRight ) {
        P.x += PLAYER_HALF_WIDTH;
    } else {
        P.x -= PLAYER_HALF_WIDTH;
    }
    
    flo32 xDir = onRight ? -1.0f : 1.0f;
    
    uint32 n = RandomU32InRange( 1, 3 );
    
    for( uint32 i = 0; i < n; i++ ) {
        flo32 dim = lerp( 0.015f, RandomF32(), 0.025f );
        
        flo32 tA = RandomF32();
        flo32 xSpeed     = lerp( 0.05f, tA * tA, 2.0f );
        flo32 ySpeed     = lerp( PLAYER_WALL_SLIDE_SPEED, RandomF32(), PLAYER_WALL_SLIDE_SPEED * 2.0f );
        flo32 targetTime = lerp( 8.0f / 60.0f, RandomF32(), 10.0f / 60.0f );
        
        PARTICLE p = {};
        p.Position   = P;
        p.Velocity   = Vec2( xDir * xSpeed, -ySpeed );
        p.accel      = Vec2( 0.0f, WORLD_GRAVITY_ACCEL * 0.75f );
        p.dim        = Vec2( dim, dim );
        p.timer      = 0.0f;
        p.targetTime = targetTime;
        p.Color      = COLOR_WHITE;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal void
addWallHit( PARTICLE_STATE * Particle, vec2 hit_dir, RAY2_INTERSECT intersect ) {
    uint32 n = RandomU32InRange( 2, 4 );
    
    hit_dir -= ( 2.0f * dot( intersect.N, hit_dir ) ) * intersect.N;
    
    vec2  bDir    = GetNormal( hit_dir + intersect.N );
    flo32 radians = atan2f( bDir.y, bDir.x );
    
    // TODO: Improve Particle range to account for any Collision geometry and incoming angle. No Particles should fly into the wall.
    flo32 range = PI * 0.25f;
    
    for( uint32 i = 0; i < n; i++ ) {
        flo32 tA = RandomF32();
        flo32 t  = tA * tA * tA * tA;
        
        flo32 dim = lerp( 0.020f, t, 0.055f );
        
        flo32 speed      = lerp( 24.0f, t, 12.0f );
        flo32 targetTime = lerp( 4.0f / 60.0f, t, 2.0f / 60.0f ) * RandomF32();
        
        flo32 rad = radians + RandomF32InRange( -range, range );
        vec2  dir = Vec2( cosf( rad ), sinf( rad ) );
        
        PARTICLE p = {};
        p.Position   = intersect.P;
        p.Velocity   = dir * speed;
        p.dim        = Vec2( dim, dim );
        p.timer      = 0.0f;
        p.targetTime = targetTime;
        p.Color      = COLOR_WHITE;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
    
    { // on wall
        flo32 dim = RandomF32InRange( 0.055f, 0.065f );
        
        PARTICLE p = {};
        p.Position   = intersect.P + intersect.N * ( dim * 0.5f );
        p.dim        = Vec2( dim, dim );
        p.targetTime = ( 3.0f / 60.0f );
        p.Color      = COLOR_WHITE;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal void
addJumperHit( PARTICLE_STATE * Particle, vec2 hit_dir, RAY2_INTERSECT intersect ) {
    uint32 n = RandomU32InRange( 2, 4 );
    
    vec2  bDir    = hit_dir;
    flo32 radians = atan2f( bDir.y, bDir.x );
    
    flo32 range = PI * 0.5f;
    
    for( uint32 i = 0; i < n; i++ ) {
        flo32 tA = RandomF32();
        tA = tA * tA;
        flo32 dim = lerp( 0.205f, tA, 0.125f );
        
        flo32 speed      = lerp( 8.0f, tA, 14.0f );
        flo32 targetTime = lerp( 20.0f / 60.0f, tA, 16.0f / 60.0f ) * RandomF32();
        
        flo32 tB = RandomF32_4Dice();
        flo32 rad = radians + lerp( -range, tB, range );
        vec2  dir = Vec2( cosf( rad ), sinf( rad ) );
        
        uint8 r = ( uint8 )RandomU32InRange( 80, 255 );
        vec4 Color = toColor( r, 0, 0 );
        
        PARTICLE p = {};
        p.Position   = intersect.P;
        p.Velocity   = dir * speed;
        p.accel      = Vec2( 0.0f, WORLD_GRAVITY_ACCEL * 0.25f );
        p.dim        = Vec2( dim, dim );
        p.timer      = 0.0f;
        p.targetTime = targetTime;
        p.Color      = Color;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal void
addGrenadeDetonate( PARTICLE_STATE * Particle, vec2 P ) {
    uint32 nA = RandomU32InRange( 6,  8 );
    uint32 nB = RandomU32InRange( 32, 40 );
    
    flo32 Radius = GRENADE_DAMAGE_RADIUS * 0.95f;
    for( uint32 i = 0; i < nB; i++ ) {
        flo32 tA = RandomF32();
        tA = tA * tA;
        flo32 dim = lerp( 0.40f, tA, 0.125f );
        
        flo32 speed      = lerp( 3.0f, tA, 5.0f );
        flo32 targetTime = lerp( 10.0f / 60.0f, tA, 8.0f / 60.0f ) * RandomF32();
        
        flo32 rad  = RandomF32() * PI * 2.0f;
        flo32 dist = RandomF32() * Radius;
        vec2  N    = Vec2( cosf( rad ), sinf( rad ) );
        vec2  dir = Vec2( cosf( rad ), sinf( rad ) ) * speed;
        
        flo32 a     = RandomF32InRange( 0.1f, 0.8f );
        vec4  Color = COLOR_GRAY( a );
        
        PARTICLE p = {};
        p.Position   = P + N * dist;
        p.Velocity   = N * speed;
        p.accel      = Vec2( 0.0f, WORLD_GRAVITY_ACCEL * -0.1f );
        p.dim        = Vec2( dim, dim );
        p.timer      = 0.0f;
        p.targetTime = targetTime;
        p.Color      = Color;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
    
    for( uint32 i = 0; i < nA; i++ ) {
        flo32 tA = RandomF32();
        flo32 dim = lerp( 0.035f, tA, 0.075f );
        
        flo32 speed      = lerp( 11.0f, tA, 13.0f );
        flo32 targetTime = lerp( 40.0f / 60.0f, tA, 30.0f / 60.0f ) * RandomF32();
        
        flo32 rad = RandomF32() * PI * 2.0f;
        vec2  dir = Vec2( cosf( rad ), sinf( rad ) ) * speed;
        
        PARTICLE p = {};
        p.Position   = P;
        p.Velocity   = dir;
        p.accel      = Vec2( 0.0f, WORLD_GRAVITY_ACCEL * 0.25f );
        p.dim        = Vec2( dim, dim );
        p.timer      = 0.0f;
        p.targetTime = targetTime;
        p.Color      = COLOR_WHITE;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal void
addJumperKill( PARTICLE_STATE * Particle, vec2 hit_dir, RAY2_INTERSECT intersect ) {
    uint32 n = RandomU32InRange( 6, 8 );
    
    vec2 bias = hit_dir * 6.0f;
    for( uint32 i = 0; i < n; i++ ) {
        flo32 tA = RandomF32();
        tA = tA * tA;
        flo32 dim = lerp( 0.325f, tA, 0.225f );
        
        flo32 speed      = lerp( 7.0f, tA, 13.0f );
        flo32 targetTime = lerp( 40.0f / 60.0f, tA, 30.0f / 60.0f ) * RandomF32();
        
        flo32 rad = RandomF32() * PI * 2.0f;
        vec2  dir = Vec2( cosf( rad ), sinf( rad ) ) * speed;
        dir = GetNormal( dir + bias ) * speed;
        
        uint8 r = ( uint8 )RandomU32InRange( 80, 255 );
        vec4 Color = toColor( r, 0, 0 );
        
        PARTICLE p = {};
        p.Position   = intersect.P;
        p.Velocity   = dir;
        p.accel      = Vec2( 0.0f, WORLD_GRAVITY_ACCEL * 0.5f );
        p.dim        = Vec2( dim, dim );
        p.timer      = 0.0f;
        p.targetTime = targetTime;
        p.Color      = Color;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal void
addJumperKill( PARTICLE_STATE * Particle, vec2 P ) {
    uint32 n = RandomU32InRange( 6, 8 );
    
    for( uint32 i = 0; i < n; i++ ) {
        flo32 tA = RandomF32();
        tA = tA * tA;
        flo32 dim = lerp( 0.325f, tA, 0.225f );
        
        flo32 speed      = lerp( 7.0f, tA, 13.0f );
        flo32 targetTime = lerp( 40.0f / 60.0f, tA, 30.0f / 60.0f ) * RandomF32();
        
        flo32 rad = RandomF32() * PI * 2.0f;
        vec2  dir = Vec2( cosf( rad ), sinf( rad ) ) * speed;
        
        uint8 r = ( uint8 )RandomU32InRange( 80, 255 );
        vec4 Color = toColor( r, 0, 0 );
        
        PARTICLE p = {};
        p.Position   = P;
        p.Velocity   = dir;
        p.accel      = Vec2( 0.0f, WORLD_GRAVITY_ACCEL * 0.5f );
        p.dim        = Vec2( dim, dim );
        p.timer      = 0.0f;
        p.targetTime = targetTime;
        p.Color      = Color;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal void
addNestHit( PARTICLE_STATE * Particle, vec2 hit_dir, RAY2_INTERSECT intersect, rect Bound ) {
    uint32 n = RandomU32InRange( 2, 4 );
    
    vec2  bDir    = hit_dir;
    flo32 radians = atan2f( bDir.y, bDir.x );
    
    flo32 range = PI * 0.95f;
    
    for( uint32 i = 0; i < n; i++ ) {
        flo32 tA = RandomF32();
        tA = tA * tA;
        flo32 dim = lerp( 0.40f, tA, 0.125f );
        
        flo32 speed      = lerp( 8.0f, tA, 14.0f );
        flo32 targetTime = lerp( 20.0f / 60.0f, tA, 16.0f / 60.0f ) * RandomF32();
        
        flo32 tB = RandomF32_2Dice();
        flo32 rad = radians + lerp( -range, tB, range );
        vec2  dir = Vec2( cosf( rad ), sinf( rad ) );
        
        flo32 a = RandomF32InRange( 0.18f, 0.28f );
        flo32 r = a;
        if( ( RandomU32InRange( 0, 2 ) % 3 ) == 0 ) {
            r = RandomF32InRange( 0.3f, 1.0f );
            a = 0.0f;
        }
        vec4  Color = Vec4( r, a, a, 1.0f );
        
        // TODO: Change this to be influenced more by hit_dir and intersect!
        vec2 P = lerp( intersect.P, RandomF32InRange( 0.2f, 0.8f ), getRandomPoint( Bound ) );
        
        PARTICLE p = {};
        p.Position   = P;
        p.Velocity   = dir * speed;
        p.accel      = Vec2( 0.0f, WORLD_GRAVITY_ACCEL * 0.25f );
        p.dim        = Vec2( dim, dim );
        p.timer      = 0.0f;
        p.targetTime = targetTime;
        p.Color      = Color;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal void
addNestPost( PARTICLE_STATE * Particle, rect Bound ) {
    uint32 n = 200;
    
    vec2 P = lerp( getTC( Bound ), 0.3f, getBC( Bound ) );
    
    for( uint32 i = 0; i < n; i++ ) {
        flo32 tA = RandomF32();
        tA = tA * tA;
        flo32 dim = lerp( 0.5f, tA, 0.25f );
        
        flo32 speed      = lerp( 3.0f, tA, 9.0f );
        flo32 targetTime = lerp( 60.0f / 60.0f, tA, 30.0f / 60.0f ) * RandomF32();
        
        flo32 rad = RandomF32() * PI * 2.0f;
        vec2  dir = Vec2( cosf( rad ), sinf( rad ) ) * speed;
        
        flo32 a = RandomF32InRange( 0.18f, 0.28f );
        vec4  Color = COLOR_GRAY( a );
        
        uint32 lo = ( uint32 )( ( flo32 )n * 0.3f );
        if( i < lo ) {
            uint8 r = ( uint8 )RandomU32InRange( 80, 255 );
            Color = toColor( r, 0, 0 );
        }
        
        PARTICLE p = {};
        p.Position   = P;
        p.Velocity   = dir;
        p.accel      = Vec2( 0.0f, WORLD_GRAVITY_ACCEL * 0.15f );
        p.dim        = Vec2( dim, dim );
        p.timer      = 0.0f;
        p.targetTime = targetTime;
        p.Color      = Color;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal void
addNestKill( PARTICLE_STATE * Particle, rect Bound ) {
    uint32 n = 240;
    
    vec2 P = lerp( getTC( Bound ), 0.3f, getBC( Bound ) );
    
    for( uint32 i = 0; i < n; i++ ) {
        flo32 tA = RandomF32();
        tA = tA * tA;
        flo32 dim = lerp( 0.5f, tA, 0.25f );
        
        flo32 speed      = lerp( 5.0f, tA, 12.0f );
        flo32 targetTime = lerp( 60.0f / 60.0f, tA, 30.0f / 60.0f ) * RandomF32();
        
        flo32 rad = RandomF32() * PI * 2.0f;
        vec2  dir = Vec2( cosf( rad ), sinf( rad ) ) * speed;
        
        flo32 a = RandomF32InRange( 0.18f, 0.28f );
        vec4  Color = COLOR_GRAY( a );
        
        uint32 lo = ( uint32 )( ( flo32 )n * 0.3f );
        if( i < lo ) {
            uint8 r = ( uint8 )RandomU32InRange( 80, 255 );
            Color = toColor( r, 0, 0 );
        }
        
        PARTICLE p = {};
        p.Position   = P;
        p.Velocity   = dir;
        p.accel      = Vec2( 0.0f, WORLD_GRAVITY_ACCEL * 0.15f );
        p.dim        = Vec2( dim, dim );
        p.timer      = 0.0f;
        p.targetTime = targetTime;
        p.Color      = Color;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal void
addNestPop( PARTICLE_STATE * Particle, rect Bound ) {
    uint32 n = 40;
    
    for( uint32 i = 0; i < n; i++ ) {
        flo32 tA = RandomF32();
        tA = tA * tA;
        flo32 dim = lerp( 0.5f, tA, 0.25f );
        
        flo32 speed      = lerp( 3.0f, tA, 9.0f );
        flo32 targetTime = lerp( 60.0f / 60.0f, tA, 30.0f / 60.0f ) * RandomF32();
        
        flo32 rad = RandomF32() * PI * 2.0f;
        vec2  dir = Vec2( cosf( rad ), sinf( rad ) ) * speed;
        
        flo32 a = RandomF32InRange( 0.18f, 0.28f );
        vec4  Color = COLOR_GRAY( a );
        
        uint32 lo = ( uint32 )( ( flo32 )n * 0.3f );
        if( i < lo ) {
            uint8 r = ( uint8 )RandomU32InRange( 80, 255 );
            Color = toColor( r, 0, 0 );
        }
        
        PARTICLE p = {};
        p.Position   = getRandomPoint( Bound );
        p.Velocity   = dir;
        p.accel      = Vec2( 0.0f, WORLD_GRAVITY_ACCEL * 0.15f );
        p.dim        = Vec2( dim, dim );
        p.timer      = 0.0f;
        p.targetTime = targetTime;
        p.Color      = Color;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal void
addNestPing( PARTICLE_STATE * Particle, vec2 hit_dir, RAY2_INTERSECT intersect, rect Bound ) {
    uint32 n = RandomU32InRange( 2, 4 );
    
    flo32 range = PI;
    vec2  P = lerp( intersect.P, 0.4f, getRandomPoint( Bound ) );
    
    vec2 bias = hit_dir * 1.0f;
    for( uint32 i = 0; i < n; i++ ) {
        flo32 tA = RandomF32();
        flo32 t  = tA * tA * tA * tA;
        
        flo32 dim = lerp( 0.020f, t, 0.055f );
        
        flo32 speed      = lerp( 24.0f, t, 12.0f );
        flo32 targetTime = lerp( 4.0f / 60.0f, t, 2.0f / 60.0f ) * RandomF32();
        
        flo32 rad = RandomF32InRange( -range, range );
        vec2  dir = Vec2( cosf( rad ), sinf( rad ) );
        dir = GetNormal( dir + bias ) * speed;
        
        PARTICLE p = {};
        p.Position   = P;
        p.Velocity   = dir;
        p.dim        = Vec2( dim, dim );
        p.timer      = 0.0f;
        p.targetTime = targetTime;
        p.Color      = COLOR_WHITE;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
    
    { // on wall
        flo32 dim = RandomF32InRange( 0.055f, 0.065f );
        
        PARTICLE p = {};
        p.Position   = P;
        p.dim        = Vec2( dim, dim );
        p.targetTime = ( 3.0f / 60.0f );
        p.Color      = COLOR_WHITE;
        
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal void
updateParticle( APP_STATE * AppState, flo32 dt ) {
    PARTICLE_STATE * State = &AppState->Particle;
    
    for( uint32 iParticle = 0; iParticle < State->nParticle; iParticle++ ) {
        PARTICLE * p = State->Particle + iParticle;
        p->timer += dt;
        
        p->Position += p->accel * ( dt * dt * 0.5f ) + p->Velocity * dt;;
        p->Velocity += p->accel * dt;
    }
}

internal void
DrawParticle( RENDER_PASS * Pass, APP_STATE * AppState ) {
    PARTICLE_STATE * State = &AppState->Particle;
    
    for( uint32 iParticle = 0; iParticle < State->nParticle; iParticle++ ) {
        PARTICLE p = State->Particle[ iParticle ];
        
        orect R = orectCD( p.Position, p.dim, p.radians );
        DrawORect( Pass, R, p.Color );
    }
}

internal void
FinalizeParticle( APP_STATE * AppState ) {
    PARTICLE_STATE * State = &AppState->Particle;
    
    uint32 nParticle = 0;
    for( uint32 iParticle = 0; iParticle < State->nParticle; iParticle++ ) {
        PARTICLE p = State->Particle[ iParticle ];
        
        if( p.timer < p.targetTime ) {
            State->Particle[ nParticle++ ] = p;
        }
    }
    State->nParticle = nParticle;
}