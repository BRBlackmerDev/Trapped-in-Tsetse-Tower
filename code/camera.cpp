
internal vec3
getCameraPos( CAMERA * Camera ) {
    vec3 result = Camera->Position + Camera->zAxis * Camera->dist;
    return result;
}

internal mat4
genCameraTransform( vec3 Position, vec3 xAxis, vec3 yAxis, vec3 zAxis ) {
	mat3 rotMatrix   = mat3_byRow( xAxis, yAxis, zAxis );
	vec3 adjPosition = -( rotMatrix * Position );
	
	mat4 result = {};
	result.elem[  0 ] = rotMatrix.elem[ 0 ];
	result.elem[  1 ] = rotMatrix.elem[ 1 ];
	result.elem[  2 ] = rotMatrix.elem[ 2 ];
	result.elem[  3 ] = 0.0f;
	result.elem[  4 ] = rotMatrix.elem[ 3 ];
	result.elem[  5 ] = rotMatrix.elem[ 4 ];
	result.elem[  6 ] = rotMatrix.elem[ 5 ];
	result.elem[  7 ] = 0.0f;
	result.elem[  8 ] = rotMatrix.elem[ 6 ];
	result.elem[  9 ] = rotMatrix.elem[ 7 ];
	result.elem[ 10 ] = rotMatrix.elem[ 8 ];
	result.elem[ 11 ] = 0.0f;
	result.elem[ 12 ] = adjPosition.x;
	result.elem[ 13 ] = adjPosition.y;
	result.elem[ 14 ] = adjPosition.z;
	result.elem[ 15 ] = 1.0f;
	return result;
}

internal void
updateCameraMovement( CAMERA * c, KEYBOARD_STATE * Keyboard, MOUSE_STATE * Mouse, flo32 dt, rect Bound ) {
    flo32 Camera_hRadians = c->hRadians;
    vec3  Camera_xAxis    = c->xAxis;
    vec3  Camera_yAxis    = c->yAxis;
    vec3  Camera_zAxis    = c->zAxis;
    
    if( ( IsInBound( Mouse->Position, Bound ) ) && ( Mouse->wheelClicks != 0 ) ) {
        c->dist -= ( ( flo32 )Mouse->wheelClicks * 1.0f );
    }
    
    // Camera movement
    if( c->MouseMove_IsActive ) {
        vec2 move_vector = Mouse->dPos * -0.005f;
        c->Position += ( Camera_xAxis * ( move_vector.x ) + Camera_yAxis * ( move_vector.y ) );
    }
    if( c->MouseOrbit_IsActive ) {
        vec2 radians = Mouse->dPos * 0.005f;
        c->hRadians -= radians.x;
        c->vRadians -= radians.y;
    }
    if( wasReleased( Mouse, MouseButton_middle ) ) {
        Mouse->resetPos = false;
        
        c->MouseMove_IsActive  = false;
        c->MouseOrbit_IsActive = false;
    }
    if( wasPressed( Mouse, MouseButton_middle ) ) {
        Mouse->downPos  = Mouse->Position;
        Mouse->resetPos = true;
        if( Keyboard->flags == KEYBOARD_FLAGS__SHIFT ) {
            c->MouseMove_IsActive = true;
        } else {
            c->MouseOrbit_IsActive = true;
        }
    }
    
    flo32 hCos = cosf( c->hRadians );
    flo32 hSin = sinf( c->hRadians );
    flo32 vCos = cosf( c->vRadians );
    flo32 vSin = sinf( c->vRadians );
    
    c->zAxis = Vec3( hSin * vCos, vSin,  hCos * vCos );
    c->xAxis = Vec3( hCos,        0.0f, -hSin        );
    c->yAxis = GetNormal( cross( c->zAxis, c->xAxis ) );
    
#if USE_IMVU_CAMERA
    c->zAxis = Vec3( hSin * vCos, -hCos * vCos, vSin );
    c->xAxis = Vec3( hCos,        hSin,  0.0f        );
    c->yAxis = GetNormal( cross( c->zAxis, c->xAxis ) );
#endif
    
    flo32 speed = 12.5f;
    if( c->moveSlow_IsActive ) { speed = 5.0f; }
    if( c->move_Left     ) { c->Position -= c->xAxis * ( speed * dt ); }
    if( c->move_Right    ) { c->Position += c->xAxis * ( speed * dt ); }
    if( c->move_down     ) { c->Position -= c->yAxis * ( speed * dt ); }
    if( c->move_up       ) { c->Position += c->yAxis * ( speed * dt ); }
    if( c->move_forward  ) { c->Position -= c->zAxis * ( speed * dt ); }
    if( c->move_backward ) { c->Position += c->zAxis * ( speed * dt ); }
}      

internal void
updateCameraInput( CAMERA * Camera, KEYBOARD_STATE * Keyboard ) {
    if( wasPressed ( Keyboard, KeyCode_num4 ) ) { Camera->move_Left     = true;  }
    if( wasReleased( Keyboard, KeyCode_num4 ) ) { Camera->move_Left     = false; }
    if( wasPressed ( Keyboard, KeyCode_num6 ) ) { Camera->move_Right    = true;  }
    if( wasReleased( Keyboard, KeyCode_num6 ) ) { Camera->move_Right    = false; }
    if( wasPressed ( Keyboard, KeyCode_num9 ) ) { Camera->move_forward  = true;  }
    if( wasReleased( Keyboard, KeyCode_num9 ) ) { Camera->move_forward  = false; }
    if( wasPressed ( Keyboard, KeyCode_num7 ) ) { Camera->move_backward = true;  }
    if( wasReleased( Keyboard, KeyCode_num7 ) ) { Camera->move_backward = false; }
    if( wasPressed ( Keyboard, KeyCode_num8 ) ) { Camera->move_up       = true;  }
    if( wasReleased( Keyboard, KeyCode_num8 ) ) { Camera->move_up       = false; }
    if( wasPressed ( Keyboard, KeyCode_num5 ) ) { Camera->move_down     = true;  }
    if( wasReleased( Keyboard, KeyCode_num5 ) ) { Camera->move_down     = false; }
    if( wasPressed ( Keyboard, KeyCode_num0 ) ) { Camera->moveSlow_IsActive = true;  }
    if( wasReleased( Keyboard, KeyCode_num0 ) ) { Camera->moveSlow_IsActive = false; }
    
    if( wasPressed( Keyboard, KeyCode_num1 ) ) {
        Camera->hRadians = 0.0f;
        Camera->vRadians = PI * 0.5f;
    }
    if( wasPressed( Keyboard, KeyCode_num2 ) ) {
        Camera->hRadians = 0.0f;
        Camera->vRadians = 0.0f;
    }
    if( wasPressed( Keyboard, KeyCode_num3 ) ) {
        Camera->hRadians = PI * 0.5f;
        Camera->vRadians = 0.0f;
    }
}

internal mat4
genCameraTransform( CAMERA * Camera, vec2 view_dim ) {
    vec3 Camera_Position  = getCameraPos( Camera );
    mat4 main_Camera      = genCameraTransform( Camera_Position, Camera->xAxis, Camera->yAxis, Camera->zAxis );
    
    mat4 main_perspective = mat4_perspective ( view_dim, Camera->fovAngle, Camera->nearPlane, Camera->farPlane );
    
    mat4 result = main_perspective * main_Camera;
    return result;
}