
#ifdef	STD_INCLUDE_DECL

//----------
// Mouse declarations
//----------

enum MOUSE_BUTTON_ID {
	MouseButton_Left,
	MouseButton_Right,
	MouseButton_middle,
	
	MouseButton_count,
};

struct MOUSE_BUTTON_STATE {
	boo32 isDown;
	int32 halfTransitionCount;
};

struct MOUSE_STATE {
	MOUSE_BUTTON_STATE button[ MouseButton_count ];
	vec2  Position;
    vec2  prevPosition;
    
    vec2  dPos;
    
    vec2  downPos;
    vec2  basePos;
    boo32 resetPos;
    
    uint8 flags;
	int32 wheelClicks;
};

//----------
// Keyboard declarations
//----------

enum KEY_STATE_CODE {
	KeyCode_null,
	
	KeyCode_backspace,
	KeyCode_shift,
	KeyCode_control,
	KeyCode_alt,
	KeyCode_capsLock,
	KeyCode_Escape,
	KeyCode_pageUp,
	KeyCode_pageDown,
	KeyCode_end,
	KeyCode_home,
	KeyCode_Left,
	KeyCode_Right,
	KeyCode_up,
	KeyCode_down,
	KeyCode_insert,
	KeyCode_delete,
	KeyCode_Enter,
	KeyCode_tab,
	KeyCode_space,
	KeyCode_0,
	KeyCode_1,
	KeyCode_2,
	KeyCode_3,
	KeyCode_4,
	KeyCode_5,
	KeyCode_6,
	KeyCode_7,
	KeyCode_8,
	KeyCode_9,
	KeyCode_a,
	KeyCode_b,
	KeyCode_c,
	KeyCode_d,
	KeyCode_e,
	KeyCode_f,
	KeyCode_g,
	KeyCode_h,
	KeyCode_i,
	KeyCode_j,
	KeyCode_k,
	KeyCode_l,
	KeyCode_m,
	KeyCode_n,
	KeyCode_o,
	KeyCode_p,
	KeyCode_q,
	KeyCode_r,
	KeyCode_s,
	KeyCode_t,
	KeyCode_u,
	KeyCode_v,
	KeyCode_w,
	KeyCode_x,
	KeyCode_y,
	KeyCode_z,
	KeyCode_period,
	KeyCode_comma,
	KeyCode_quote,
	KeyCode_semicolon,
	KeyCode_equal,
	KeyCode_dash,
	KeyCode_tilde,
	KeyCode_forwardSlash,
	KeyCode_backSlash,
	KeyCode_openBracket,
	KeyCode_closeBracket,
	KeyCode_num0,
	KeyCode_num1,
	KeyCode_num2,
	KeyCode_num3,
	KeyCode_num4,
	KeyCode_num5,
	KeyCode_num6,
	KeyCode_num7,
	KeyCode_num8,
	KeyCode_num9,
    KeyCode_numAdd,
    KeyCode_numSub,
    KeyCode_numMul,
    KeyCode_numDiv,
    KeyCode_numPeriod,
    KeyCode_F1,
    KeyCode_F2,
    KeyCode_F3,
    KeyCode_F4,
    KeyCode_F5,
    KeyCode_F6,
    KeyCode_F7,
    KeyCode_F8,
    KeyCode_F9,
    KeyCode_F10,
    KeyCode_F11,
    KeyCode_F12,
	
	KeyCode_count,
};

struct KEY_STATE {
    uint16 KeyCode;
    uint8  isDown;
	uint8  flags;
};

#define KEYBOARD_FLAGS__CONTROL  ( 0x01 )
#define KEYBOARD_FLAGS__ALT      ( 0x02 )
#define KEYBOARD_FLAGS__SHIFT    ( 0x04 )

#define KEYBOARD_STATE__MAX_EVENT_COUNT  ( 16 )
struct KEYBOARD_STATE {
	uint8    flags;
    uint32   nEvents;
    KEY_STATE Event[ KEYBOARD_STATE__MAX_EVENT_COUNT ];
};

//----------
// controller declarations
//----------

enum CONTROLLER_BUTTON_ID {
	controllerButton_dPad_up,
	controllerButton_dPad_down,
	controllerButton_dPad_Left,
	controllerButton_dPad_Right,
    
    controllerButton_start,
    controllerButton_back,
    
    controllerButton_thumb_Left,
    controllerButton_thumb_Right,
    controllerButton_shoulder_Left,
    controllerButton_shoulder_Right,
    controllerButton_trigger_Left,
    controllerButton_trigger_Right,
    
    controllerButton_A,
    controllerButton_B,
    controllerButton_X,
    controllerButton_Y,
	
	controllerButton_count,
};

struct CONTROLLER_BUTTON_STATE {
	boo32 isDown;
	int32 halfTransitionCount;
};

struct CONTROLLER_THUMBSTICK {
    INT32_PAIR xy;
    int32      dead;
};

struct CONTROLLER_STATE {
	CONTROLLER_BUTTON_STATE button[ controllerButton_count ];
    CONTROLLER_THUMBSTICK   Left;
    CONTROLLER_THUMBSTICK   Right;
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

//----------
// Mouse functions
//----------

internal void
MOUSE_endOfFrame( MOUSE_STATE * Mouse ) {
	Mouse->wheelClicks  = 0;
    //Mouse->prevPosition = Mouse->Position;
	for( uint32 buttonIndex = 0; buttonIndex < MouseButton_count; buttonIndex++ ) {
		MOUSE_BUTTON_STATE * button = &Mouse->button[ buttonIndex ];
		button->halfTransitionCount = 0;
	}
}

internal boo32
isDown( MOUSE_STATE * Mouse, MOUSE_BUTTON_ID buttonID, uint8 flags = 0 ) {
    MOUSE_BUTTON_STATE * button = Mouse->button + buttonID;
	boo32 result = ( button->isDown ) && ( Mouse->flags == flags );
	return result;
}

internal boo32
wasButtonPressed( MOUSE_BUTTON_STATE * button ) {
	boo32 result = false;
	if( ( button->isDown && button->halfTransitionCount >= 1 ) ||
       ( !button->isDown && button->halfTransitionCount >= 2 ) ) {
		result = true;
	}
	return result;
}

internal boo32
wasPressed( MOUSE_STATE * Mouse, MOUSE_BUTTON_ID buttonID, uint8 flags = 0 ) {
	MOUSE_BUTTON_STATE * button = &Mouse->button[ buttonID ];
	boo32 result = ( wasButtonPressed( button ) ) && ( Mouse->flags == flags );
	return result;
}

internal boo32
wasReleased( MOUSE_BUTTON_STATE * button ) {
	boo32 result = false;
	if( ( !button->isDown && button->halfTransitionCount >= 1 ) ||
       ( button->isDown && button->halfTransitionCount >= 2 ) ) {
		result = true;
	}
	return result;
}

internal boo32
wasReleased( MOUSE_STATE * Mouse, MOUSE_BUTTON_ID buttonID ) {
	MOUSE_BUTTON_STATE * button = &Mouse->button[ buttonID ];
	boo32 result = wasReleased( button );
	return result;
}

internal void
addButtonEvent( MOUSE_STATE * Mouse, MOUSE_BUTTON_ID buttonID, boo32 isDown, uint8 flags ) {
    Mouse->flags = flags;
    
    MOUSE_BUTTON_STATE * button = Mouse->button + buttonID;
    button->isDown = isDown;
    button->halfTransitionCount++;
}

//----------
// Keyboard functions
//----------

internal KEY_STATE
KeyState( KEY_STATE_CODE KeyCode, uint8 isDown, uint8 flags ) {
	KEY_STATE result = {};
	result.KeyCode     = ( uint16 )KeyCode;
	result.isDown      = isDown;
	result.flags       = flags;
	return result;
}

internal boo32
wasPressed( KEYBOARD_STATE * Keyboard, KEY_STATE_CODE KeyCode, uint8 targetFlags = 0 ) {
	boo32 result = false;
	for( uint32 iEvent = 0; iEvent < Keyboard->nEvents; iEvent++ ) {
		KEY_STATE Event = Keyboard->Event[ iEvent ];
		if( ( Event.KeyCode == KeyCode ) && ( Event.isDown ) && ( Event.flags == targetFlags ) ) {
			result = true;
		}
	}
	return result;
}

internal boo32
wasReleased( KEYBOARD_STATE * Keyboard, KEY_STATE_CODE KeyCode ) {
	boo32 result = false;
	for( uint32 iEvent = 0; iEvent < Keyboard->nEvents; iEvent++ ) {
		KEY_STATE Event = Keyboard->Event[ iEvent ];
		if( ( Event.KeyCode == KeyCode ) && ( !Event.isDown ) ) {
			result = true;
		}
	}
	return result;
}

internal void
addKeyEvent( KEYBOARD_STATE * Keyboard, KEY_STATE_CODE KeyCode, boo32 isDown, uint8 flags ) {
	Assert( Keyboard->nEvents < KEYBOARD_STATE__MAX_EVENT_COUNT );
	Keyboard->Event[ Keyboard->nEvents++ ] = KeyState( KeyCode, ( uint8 )isDown, flags );
}

//----------
// controller functions
//----------

internal boo32
isDown( CONTROLLER_STATE * controller, CONTROLLER_BUTTON_ID buttonID ) {
    CONTROLLER_BUTTON_STATE * button = controller->button + buttonID;
    boo32  result = button->isDown;
    return result;
}

internal boo32
wasButtonPressed( CONTROLLER_BUTTON_STATE * button ) {
	boo32 result = false;
	if( (  button->isDown && ( button->halfTransitionCount >= 1 ) ) ||
       ( !button->isDown && ( button->halfTransitionCount >= 2 ) ) ) {
		result = true;
	}
	return result;
}

internal boo32
wasPressed( CONTROLLER_STATE * controller, CONTROLLER_BUTTON_ID buttonID ) {
	CONTROLLER_BUTTON_STATE * button = controller->button + buttonID;
	boo32 result = wasButtonPressed( button );
	return result;
}

internal boo32
wasReleased( CONTROLLER_BUTTON_STATE * button ) {
	boo32 result = false;
	if( ( !button->isDown && ( button->halfTransitionCount >= 1 ) ) ||
       (  button->isDown && ( button->halfTransitionCount >= 2 ) ) ) {
		result = true;
	}
	return result;
}

internal boo32
wasReleased( CONTROLLER_STATE * controller, CONTROLLER_BUTTON_ID buttonID ) {
	CONTROLLER_BUTTON_STATE * button = controller->button + buttonID;
	boo32 result = wasReleased( button );
	return result;
}

internal void
updateButton( CONTROLLER_STATE * controller, CONTROLLER_BUTTON_ID buttonID, boo32 isDown ) {
    CONTROLLER_BUTTON_STATE * button = controller->button + buttonID;
    if( (  button->isDown && !isDown ) ||
       ( !button->isDown &&  isDown ) ) {
        button->halfTransitionCount++;
    }
    button->isDown = isDown;
}

#endif	// STD_INCLUDE_FUNC