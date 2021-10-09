
#ifdef	STD_INCLUDE_DECL

//----------
// profile declarations
//----------

#define FUNCTION_NAME_MAX_LENGTH  48
struct PROFILE_EVENT {
	char functionName[ FUNCTION_NAME_MAX_LENGTH + 1 ];
	uint64 clock_start;
	uint64 clock_end;
	uint32 threadID;
    
#define PROFILE__SUBCLOCK_COUNT  ( 8 )
    uint64  subclock[ PROFILE__SUBCLOCK_COUNT ];
    uint32 nSubclock;
};

#define PROFILE_EVENT_LIST_MAX_COUNT  16384
struct PROFILE_EVENT_LIST {
	PROFILE_EVENT Event[ PROFILE_EVENT_LIST_MAX_COUNT ];
	uint64 frameClockStart;
	int32 count;
};

struct PROFILE_NODE {
	char functionName[ FUNCTION_NAME_MAX_LENGTH ];
	uint64 clock_total;
	uint32 count_total;
    
    uint64  subclock[ PROFILE__SUBCLOCK_COUNT ];
    uint32 nSubclock;
};

#define PROFILE_NODE_LIST_MAX_COUNT  128
struct PROFILE_NODE_LIST {
	PROFILE_NODE node[ PROFILE_NODE_LIST_MAX_COUNT ];
	int32 count;
};

#define PROFILE_EVENT_STACK_MAX_COUNT  16
struct PROFILE_EVENT_STACK {
	PROFILE_EVENT Event[ PROFILE_EVENT_STACK_MAX_COUNT ];
	int32 count;
};

#define PROFILE_EVENT_DATABASE_MAX_COUNT  ( 5 * 60 )
struct PROFILE_EVENT_DATABASE {
	PROFILE_EVENT_LIST * EventList;
	int32 maxCount;
	int32 index;
	int32 selectIndex;
};

#if DEBUG_BUILD
#define PROFILE_FUNCTION()  PROFILE_OBJ profileObj = PROFILE_OBJ( globalVar_profileSystem.EventList_currentFrame, __FUNCTION__ )
#else
#define PROFILE_FUNCTION()
#endif

#if DEBUG_BUILD
#define PROFILE_BREAK()  profileObj.Event->subclock[ profileObj.Event->nSubclock++ ] = __rdtsc();
#else
#define PROFILE_BREAK()
#endif // DEBUG_BUILD

struct PROFILE_EVENT_DATABASE_CLOCKS {
	uint64 frameStart;
	uint64 PlatformStart;
	uint64 appStart;
	uint64 renderStart;
	uint64 frameEnd;
	int64  counter_frameStart;
	int64  counter_frameEnd;
};

class PROFILE_OBJ {
	public:
    PROFILE_EVENT * Event;
    PROFILE_OBJ( PROFILE_EVENT_LIST * EventList, char * funcName );
    ~PROFILE_OBJ();
};

typedef flo32 ( GET_MS_ELAPSED )( int64 startCounter, int64 endCounter, int64 perfFrequency );

struct PROFILE_SYSTEM {
	boo32 showProfile;
	PROFILE_EVENT_DATABASE	EventDatabase;
	PROFILE_EVENT_LIST *		EventList_currentFrame;
	PROFILE_NODE_LIST * 		nodeList_currentFrame;
	
	GET_MS_ELAPSED * getMSElapsed;
	
	PROFILE_EVENT_DATABASE_CLOCKS		EventDatabase_clocks_currentFrame;
	PROFILE_EVENT_DATABASE_CLOCKS 	EventDatabase_clocks_previousFrame;
	PROFILE_EVENT_DATABASE_CLOCKS *	EventDatabase_clocks;
	
	flo32 EventDatabase_posOffset;
	flo32 EventDatabase_width;
	flo32 EventDatabase_Height;
	boo32 EventDatabase_scrollActive;
	boo32 frameBreakdown_scrollActive_bar;
	boo32 frameBreakdown_scrollActive_Left;
	boo32 frameBreakdown_scrollActive_Right;
	flo32 frameBreakdown_scrollX_Left;
	flo32 frameBreakdown_scrollX_Right;
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

#if DEBUG_BUILD
#define PROFILE_SYSTEM__SIZE_IN_BYTES	( ( ( PROFILE_EVENT_DATABASE_MAX_COUNT + 1 ) * sizeof( PROFILE_EVENT_LIST ) ) + sizeof( PROFILE_NODE_LIST ) + ( PROFILE_EVENT_DATABASE_MAX_COUNT * sizeof( PROFILE_EVENT_DATABASE_CLOCKS ) ) )
PROFILE_SYSTEM globalVar_profileSystem = {};

//----------
// profile functions
//----------

internal uint32
getThreadID() {
	uint8 * thread_memory = ( uint8 * )__readgsqword( 0x30 );
	uint32  threadID = *( ( uint32 * )( thread_memory + 0x48 ) );
	return threadID;
}

internal void
setString( char * srcStr, char * destStr ) {
	char * dest = destStr;
	char * src = srcStr;
	while( *src ) {
		*dest++ = *src++;
	}
	*dest = 0;
}

internal boo32
matchString( char * string0, char * string1 ) {
    boo32 doMatch = true;
    while( ( *string0 ) && ( *string1 ) ) {
        if( *string0 != *string1 ) { doMatch = false; }
        string0++;
        string1++;
    }
    if( ( *string0 ) || ( *string1 ) ) { doMatch = false; }
    return doMatch;
}

internal PROFILE_EVENT *
newEvent( PROFILE_EVENT_LIST * profileList ) {
	int32 Event_index = InterlockedIncrement( ( volatile long * )&profileList->count ) - 1;
	PROFILE_EVENT * Event = profileList->Event + Event_index;
	Assert( profileList->count < PROFILE_EVENT_LIST_MAX_COUNT );
	Event->clock_start = 0;
	Event->clock_end = 0;
    Event->nSubclock = 0;
	for( int32 iter = 0; iter < FUNCTION_NAME_MAX_LENGTH; iter++ ) {
		Event->functionName[ iter ] = 0;
	}
	return Event;
}

internal PROFILE_NODE *
newNode( PROFILE_NODE_LIST * profileList ) {
	PROFILE_NODE * node = profileList->node + profileList->count++;
	Assert( profileList->count < PROFILE_NODE_LIST_MAX_COUNT );
	node->clock_total = 0;
	node->count_total = 0;
    node->nSubclock   = 0;
    memset( node->subclock, 0, sizeof( uint64 ) * PROFILE__SUBCLOCK_COUNT );
	for( int32 iter = 0; iter < FUNCTION_NAME_MAX_LENGTH; iter++ ) {
		node->functionName[ iter ] = 0;
	}
	return node;
}

internal void
collateEvents( PROFILE_EVENT_LIST * EventList, PROFILE_NODE_LIST * nodeList ) {
	PROFILE_EVENT_STACK _EventStack = {};
	PROFILE_EVENT_STACK * EventStack = &_EventStack;
	
	for( int32 Event_index = 0; Event_index < EventList->count; Event_index++ ) {
		PROFILE_EVENT * Event = EventList->Event + Event_index;
		
		PROFILE_NODE * currentNode = 0;
		boo32 isInList = false;
		
		for( int32 node_index = 0; node_index < nodeList->count && !isInList; node_index++ ) {
			PROFILE_NODE * node = nodeList->node + node_index;
			
			if( matchString( Event->functionName, node->functionName ) ) {
				isInList = true;
				currentNode = node;
			}
		}
		
		if( !isInList ) {
			currentNode = newNode( nodeList );
			setString( Event->functionName, currentNode->functionName );
		}
		Assert( currentNode );
		currentNode->count_total++;
		uint64 clock_total = Event->clock_end - Event->clock_start;
		currentNode->clock_total += clock_total;
        
        currentNode->nSubclock = Event->nSubclock;
        for( uint32 iSubclock = 0; iSubclock < currentNode->nSubclock; iSubclock++ ) {
            currentNode->subclock[ iSubclock ] += ( Event->subclock[ iSubclock ] - Event->clock_start );
        }
		
		for( int32 iter = EventStack->count - 1; iter >= 0; iter-- ) {
			PROFILE_EVENT * stackEvent = EventStack->Event + iter;
			
			if( Event->clock_start > stackEvent->clock_end ) {
				EventStack->count--;
			}
		}
		
		if( EventStack->count > 0 ) {
			PROFILE_EVENT * parentEvent = EventStack->Event + EventStack->count - 1;
			for( int32 node_index = 0; node_index < nodeList->count; node_index++ ) {
				PROFILE_NODE * node = nodeList->node + node_index;
				
				if( matchString( parentEvent->functionName, node->functionName ) ) {
					node->clock_total -= clock_total;
					break;
				}
			}
		}
		
		PROFILE_EVENT * pushEvent = EventStack->Event + EventStack->count++;
		Assert( EventStack->count < PROFILE_EVENT_STACK_MAX_COUNT );
		*pushEvent = *Event;
	}
	
	boo32 isSorted = true;
	do {
		isSorted = true;
		for( int32 node_index = 0; node_index < nodeList->count - 1; node_index++ ) {
			PROFILE_NODE * nodeA = nodeList->node + node_index;
			PROFILE_NODE * nodeB = nodeList->node + node_index + 1;
			
			if( nodeA->clock_total < nodeB->clock_total ) {
				PROFILE_NODE _temp = {};
				PROFILE_NODE * temp = &_temp;
				
				setString( nodeA->functionName, temp->functionName );
				temp->clock_total = nodeA->clock_total;
				temp->count_total = nodeA->count_total;
				
				setString( nodeB->functionName, nodeA->functionName );
				nodeA->clock_total = nodeB->clock_total;
				nodeA->count_total = nodeB->count_total;
				
				setString( temp->functionName, nodeB->functionName );
				nodeB->clock_total = temp->clock_total;
				nodeB->count_total = temp->count_total;
				
				isSorted = false;
			}
		}
	} while( !isSorted );
}

PROFILE_OBJ::PROFILE_OBJ( PROFILE_EVENT_LIST * EventList, char * funcName ) {
	Event = newEvent( EventList );
	setString( funcName, Event->functionName );
	Event->threadID = getThreadID();
	Event->clock_start = __rdtsc();
}

PROFILE_OBJ::~PROFILE_OBJ() {
	Event->clock_end = __rdtsc();
}

internal void
updateProfile( PROFILE_SYSTEM * profileSystem, PLATFORM * Platform, int64 perfFrequency, RENDERER * Renderer, rect client_Bound ) {
	if( globalVar_profileSystem.showProfile ) {
		vec2  EventDatabase_basePos 	= getBL( client_Bound ) + Vec2( profileSystem->EventDatabase_posOffset, profileSystem->EventDatabase_posOffset );
		rect  EventDatabase_Bound		= rectBLD( EventDatabase_basePos, Vec2( profileSystem->EventDatabase_width, profileSystem->EventDatabase_Height ) );
		flo32 EventDatabase_slotWidth = getWidth( EventDatabase_Bound ) / ( flo32 )PROFILE_EVENT_DATABASE_MAX_COUNT;
		
		MOUSE_STATE * Mouse = &Platform->Mouse;
		rect frameBreakdown_scrollBound = rectBLD( getTL( EventDatabase_Bound ) + Vec2( 0.0f, 4.0f ), Vec2( getWidth( EventDatabase_Bound ), 12.0f ) );
		
		vec2 scrollBar_pos = Vec2( profileSystem->frameBreakdown_scrollX_Left, frameBreakdown_scrollBound.Bottom );
		vec2 scrollBar_dim = Vec2( profileSystem->frameBreakdown_scrollX_Right - profileSystem->frameBreakdown_scrollX_Left, getHeight( frameBreakdown_scrollBound ) );
		rect frameBreakdown_scrollBarBound = rectBLD( scrollBar_pos, scrollBar_dim );
		rect frameBreakdown_LeftBound  = rectBLD( getBL( frameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_dim.y ) );
		rect frameBreakdown_RightBound = rectBLD( getBR( frameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_dim.y ) );
		
		rect EventDatabase_resizeBound = rectBLD( getTL( EventDatabase_Bound ) + Vec2( -2.0f, -2.0f ), Vec2( getWidth( EventDatabase_Bound ) + 4.0f, 4.0f ) );
		if( profileSystem->EventDatabase_scrollActive ) {
			profileSystem->EventDatabase_Height = maxValue( Mouse->Position.y - EventDatabase_Bound.Bottom, 100.0f );
		}
		if( profileSystem->frameBreakdown_scrollActive_Left ) {
			profileSystem->frameBreakdown_scrollX_Left  = clamp( Mouse->Position.x, profileSystem->EventDatabase_posOffset, profileSystem->frameBreakdown_scrollX_Right - 40.0f );
		}
		if( profileSystem->frameBreakdown_scrollActive_Right ) {
			profileSystem->frameBreakdown_scrollX_Right = clamp( Mouse->Position.x, profileSystem->frameBreakdown_scrollX_Left + 40.0f, frameBreakdown_scrollBound.Right );
		}
		if( profileSystem->frameBreakdown_scrollActive_bar ) {
			flo32 width = profileSystem->frameBreakdown_scrollX_Right - profileSystem->frameBreakdown_scrollX_Left;
			
			flo32 Left  = Mouse->Position.x - width * 0.5f;
			flo32 Right = Mouse->Position.x + width * 0.5f;
			if( Left < profileSystem->EventDatabase_posOffset ) {
				Left  = profileSystem->EventDatabase_posOffset;
				Right = profileSystem->EventDatabase_posOffset + width;
			}
			if( Right > frameBreakdown_scrollBound.Right ) {
				Right = frameBreakdown_scrollBound.Right;
				Left  = frameBreakdown_scrollBound.Right - width;
			}
			
			profileSystem->frameBreakdown_scrollX_Left  = Left;
			profileSystem->frameBreakdown_scrollX_Right = Right;
		}
		if( IsInBound( Mouse->Position, EventDatabase_resizeBound ) ) {
			if( wasPressed( Mouse, MouseButton_Left ) ) {
				profileSystem->EventDatabase_scrollActive = true;
			}
		}
		if( IsInBound( Mouse->Position, frameBreakdown_LeftBound ) ) {
			if( wasPressed( Mouse, MouseButton_Left ) ) {
				profileSystem->frameBreakdown_scrollActive_Left = true;
			}
		} else if( IsInBound( Mouse->Position, frameBreakdown_RightBound ) ) {
			if( wasPressed( Mouse, MouseButton_Left ) ) {
				profileSystem->frameBreakdown_scrollActive_Right = true;
			}
		} else if( IsInBound( Mouse->Position, frameBreakdown_scrollBarBound ) ) {
			if( wasPressed( Mouse, MouseButton_Left ) ) {
				profileSystem->frameBreakdown_scrollActive_bar = true;
			}
		}
		if( wasReleased( Mouse, MouseButton_Left ) ) {
			profileSystem->EventDatabase_scrollActive        = false;
			profileSystem->frameBreakdown_scrollActive_bar   = false;
			profileSystem->frameBreakdown_scrollActive_Left  = false;
			profileSystem->frameBreakdown_scrollActive_Right = false;
		}
		scrollBar_pos = Vec2( profileSystem->frameBreakdown_scrollX_Left, frameBreakdown_scrollBound.Bottom );
		scrollBar_dim = Vec2( profileSystem->frameBreakdown_scrollX_Right - profileSystem->frameBreakdown_scrollX_Left, getHeight( frameBreakdown_scrollBound ) );
		frameBreakdown_scrollBarBound = rectBLD( scrollBar_pos, scrollBar_dim );
		frameBreakdown_LeftBound  = rectBLD( getBL( frameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_dim.y ) );
		frameBreakdown_RightBound = rectBLD( getBR( frameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_dim.y ) );
		
		int32 collateIndex = globalVar_profileSystem.EventDatabase.index;
		if( wasPressed( Mouse, MouseButton_Right ) ) {
			globalVar_profileSystem.EventDatabase.selectIndex = -1;
		}
		if( globalVar_profileSystem.EventDatabase.selectIndex != -1 ) {
			collateIndex = globalVar_profileSystem.EventDatabase.selectIndex;
		}
		if( IsInBound( Mouse->Position, EventDatabase_Bound ) ) {
			int32 hoverIndex = ( int32 )( ( Mouse->Position.x - EventDatabase_Bound.Left ) / EventDatabase_slotWidth );
			if( wasPressed( Mouse, MouseButton_Left ) ) {
				globalVar_profileSystem.EventDatabase.selectIndex = hoverIndex;
				collateIndex = hoverIndex;
			}
			if( globalVar_profileSystem.EventDatabase.selectIndex == -1 ) {
				collateIndex = hoverIndex;
			}
		}
		PROFILE_EVENT_LIST * collateList = globalVar_profileSystem.EventDatabase.EventList + collateIndex;
		PROFILE_EVENT_DATABASE_CLOCKS * collateClocks = globalVar_profileSystem.EventDatabase_clocks + collateIndex;
        
		collateEvents( collateList, globalVar_profileSystem.nodeList_currentFrame );
		
#define CLOCK_BOUND_MAX_STACK_COUNT  16
		uint64 clockBound_stack[ CLOCK_BOUND_MAX_STACK_COUNT ] = {};
		int32 stack_count = 0;
		
		flo32 frameBreakdown_minWidth = 40.0f;
		flo32 frameBreakdown_maxWidth = getWidth( frameBreakdown_scrollBound );
		flo32 frameBreakdown_ratio = ( profileSystem->frameBreakdown_scrollX_Right - profileSystem->frameBreakdown_scrollX_Left - frameBreakdown_minWidth ) / ( frameBreakdown_maxWidth - frameBreakdown_minWidth );
		// DISPLAY_VALUE( flo32, frameBreakdown_ratio );
		
		vec2	client_dim = getDim( client_Bound );
		flo32 client_width = client_dim.x;
		flo32 profile_minWidth = client_dim.x;
		flo32 profile_maxWidth = profile_minWidth * 10.0f;
		flo32 profile_width   = lerp( profile_maxWidth, frameBreakdown_ratio, profile_minWidth );
		
		vec2  profile_basePos = Vec2( 0.0f, EventDatabase_Bound.Top + 20.0f );
		flo32 row_Height = 20.0f;
		
		flo32 valid_width = getWidth( frameBreakdown_scrollBound ) - ( profileSystem->frameBreakdown_scrollX_Right - profileSystem->frameBreakdown_scrollX_Left );
		if( valid_width > 0.0f ) {
			flo32 offset_ratio = ( profileSystem->frameBreakdown_scrollX_Left - frameBreakdown_scrollBound.Left ) / valid_width;
			profile_basePos.x -= ( profile_width - client_width ) * offset_ratio;
		}
		uint64 profile_frameClockStart = collateList->frameClockStart;
		uint64 profile_clocksPerFrame = 60000000;
		
		rect clear_BoundA = Rect( client_Bound.Left, client_Bound.Bottom, client_Bound.Right, EventDatabase_Bound.Top );
		DEBUG_DrawRect( clear_BoundA, Vec4( 0.0f, 0.0f, 0.0f, 0.95f ) );
		
		DEBUG_DrawRect( EventDatabase_Bound, COLOR_GRAY( 0.80f ) );
		for( int32 database_index = 0; database_index < PROFILE_EVENT_DATABASE_MAX_COUNT; database_index++ ) {
			PROFILE_EVENT_DATABASE_CLOCKS clock = globalVar_profileSystem.EventDatabase_clocks[ database_index ];
			
			rect BoundA = {};
			BoundA.Left  = EventDatabase_basePos.x + EventDatabase_slotWidth * ( flo32 )database_index;
			BoundA.Right = BoundA.Left + EventDatabase_slotWidth;
			BoundA.Bottom = EventDatabase_basePos.y + ( ( flo32 )( clock.PlatformStart	- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->EventDatabase_Height;
			BoundA.Top 	 = EventDatabase_basePos.y + ( ( flo32 )( clock.appStart 		- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->EventDatabase_Height;
			DEBUG_DrawRect( BoundA, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
			
			rect BoundB = {};
			BoundB.Left  = EventDatabase_basePos.x + EventDatabase_slotWidth * ( flo32 )database_index;
			BoundB.Right = BoundB.Left + EventDatabase_slotWidth;
			BoundB.Bottom = EventDatabase_basePos.y + ( ( flo32 )( clock.appStart		- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->EventDatabase_Height;
			BoundB.Top 	 = EventDatabase_basePos.y + ( ( flo32 )( clock.renderStart	- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->EventDatabase_Height;
			DEBUG_DrawRect( BoundB, Vec4( 0.0f, 0.0f, 0.5f, 1.0f ) );
			
			rect BoundC = {};
			BoundC.Left  = EventDatabase_basePos.x + EventDatabase_slotWidth * ( flo32 )database_index;
			BoundC.Right = BoundC.Left + EventDatabase_slotWidth;
			BoundC.Bottom = EventDatabase_basePos.y + ( ( flo32 )( clock.renderStart	- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->EventDatabase_Height;
			BoundC.Top 	 = EventDatabase_basePos.y + ( ( flo32 )( clock.frameEnd		- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->EventDatabase_Height;
			DEBUG_DrawRect( BoundC, Vec4( 0.5f, 0.0f, 0.0f, 1.0f ) );
		}
		
		rect clear_BoundB = Rect( client_Bound.Left, EventDatabase_Bound.Top, client_Bound.Right, client_Bound.Top );
		DEBUG_DrawRect( clear_BoundB, Vec4( 0.0f, 0.0f, 0.0f, 0.95f ) );
		
		rect EventDatabase_highlightBound = rectBLD( EventDatabase_basePos + Vec2( EventDatabase_slotWidth * ( flo32 )globalVar_profileSystem.EventDatabase.index, 0.0f ), Vec2( EventDatabase_slotWidth, profileSystem->EventDatabase_Height ) );
		DEBUG_DrawRect( EventDatabase_highlightBound, Vec4( 0.25f, 0.25f, 0.0f, 0.25f ) );
		
		if( collateIndex != -1 ) {
			vec2 collateIndex_Position = EventDatabase_basePos + Vec2( EventDatabase_slotWidth * ( flo32 )collateIndex, 0.0f );
			DEBUG_DrawRect( rectBLD( collateIndex_Position, Vec2( EventDatabase_slotWidth, profileSystem->EventDatabase_Height ) ), Vec4( 0.5f, 0.0f, 0.0f, 0.5f ) );
		}
		
		DEBUG_DrawRect( frameBreakdown_scrollBound,    COLOR_GRAY( 0.20f ) );
		DEBUG_DrawRect( frameBreakdown_scrollBarBound, COLOR_GRAY( 0.50f ) );
		
		if( IsInBound( Mouse->Position, EventDatabase_resizeBound ) ) {
			DEBUG_DrawRect( EventDatabase_resizeBound, COLOR_RED );
		}
		if( IsInBound( Mouse->Position, frameBreakdown_LeftBound ) ) {
			DEBUG_DrawRect( frameBreakdown_LeftBound, COLOR_RED );
		} else if( IsInBound( Mouse->Position, frameBreakdown_RightBound ) ) {
			DEBUG_DrawRect( frameBreakdown_RightBound, COLOR_RED );
		} else if( IsInBound( Mouse->Position, frameBreakdown_scrollBarBound ) ) {
			DEBUG_DrawRect( frameBreakdown_scrollBarBound, COLOR_RED );
		}
		
		vec4 ColorTable[ 15 ] = {};
		ColorTable[  0 ] = Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
		ColorTable[  1 ] = Vec4( 0.4f, 0.2f, 0.2f, 1.0f );
		ColorTable[  2 ] = Vec4( 0.6f, 0.2f, 0.2f, 1.0f );
		ColorTable[  3 ] = Vec4( 0.2f, 0.4f, 0.2f, 1.0f );
		ColorTable[  4 ] = Vec4( 0.2f, 0.6f, 0.2f, 1.0f );
		ColorTable[  5 ] = Vec4( 0.2f, 0.2f, 0.4f, 1.0f );
		ColorTable[  6 ] = Vec4( 0.2f, 0.2f, 0.6f, 1.0f );
		ColorTable[  7 ] = Vec4( 0.4f, 0.4f, 0.2f, 1.0f );
		ColorTable[  8 ] = Vec4( 0.6f, 0.6f, 0.2f, 1.0f );
		ColorTable[  9 ] = Vec4( 0.4f, 0.2f, 0.4f, 1.0f );
		ColorTable[ 10 ] = Vec4( 0.6f, 0.2f, 0.6f, 1.0f );
		ColorTable[ 11 ] = Vec4( 0.2f, 0.4f, 0.4f, 1.0f );
		ColorTable[ 12 ] = Vec4( 0.2f, 0.6f, 0.6f, 1.0f );
		ColorTable[ 13 ] = Vec4( 0.4f, 0.4f, 0.4f, 1.0f );
		ColorTable[ 14 ] = Vec4( 0.6f, 0.6f, 0.6f, 1.0f );
		int32 Color_select = 0;
		
        int32 threadID_count = 0;
		uint32 threadID[ 12 ] = {};
		
		PROFILE_EVENT * profile_tooltip = 0;
		for( int32 Event_index = 0; Event_index < collateList->count; Event_index++ ) {
			PROFILE_EVENT * Event = collateList->Event + Event_index;
			
			int32 level = -1;
			for( int32 threadID_index = 0; ( level == -1 ) && ( threadID_index < threadID_count ); threadID_index++ ) {
				if( threadID[ threadID_index ] == Event->threadID ) {
					level = threadID_index;
				}
			}
			if( level == -1 ) {
				threadID[ threadID_count ] = Event->threadID;
				level = threadID_count++;
			}
			Assert( threadID_count < 12 );
			
			rect Bound = {};
			Bound.Left  = profile_basePos.x + ( ( flo32 )( Event->clock_start - profile_frameClockStart ) / ( flo32 )profile_clocksPerFrame ) * profile_width;
			Bound.Right = profile_basePos.x + ( ( flo32 )( Event->clock_end   - profile_frameClockStart ) / ( flo32 )profile_clocksPerFrame ) * profile_width;
			Bound.Top 	 = profile_basePos.y + row_Height * ( flo32 )( level + 1 );
			Bound.Bottom = profile_basePos.y + row_Height * ( flo32 )level;
			
			if( IsInBound( Mouse->Position, Bound ) ) {
				profile_tooltip = Event;
			}
			
			DEBUG_DrawRect( Bound, ColorTable[ Color_select ] );
			
			Color_select = ( Color_select + 2 ) % _arrayCount( ColorTable );
		}
		
		if( profile_tooltip ) {
			PROFILE_tooltip( profile_tooltip->functionName, Mouse->Position );
		}
		
		flo32  frame_msElapsed = profileSystem->getMSElapsed( collateClocks->counter_frameStart, collateClocks->counter_frameEnd, perfFrequency );
		uint64 frame_cycElapsed = collateClocks->frameEnd    - collateClocks->frameStart;
		uint64   app_cycElapsed = collateClocks->renderStart - collateClocks->appStart;
		
		char time_stringA[ 128 ] = {};
		sprintf( time_stringA, "Prev Frame: %10.04f ms  %10I64u cyc  %6I64u MC", frame_msElapsed, frame_cycElapsed, frame_cycElapsed / 1000 );
		
		char time_stringB[ 128 ] = {};
		sprintf( time_stringB, "App Update:                %10I64u cyc  %6I64u MC  %.02f", app_cycElapsed, app_cycElapsed / 1000, ( flo32 )app_cycElapsed / ( flo32 )frame_cycElapsed * 100.0f );
		
		PROFILE_string( time_stringA );
		PROFILE_string( time_stringB );
		
		if( globalVar_profileSystem.nodeList_currentFrame->count > 0 ) {
			PROFILE_string( "------------------------|---nCyc---||-MCyc-||-count-||-%frame-||-%app-|" );
		}
		for( int32 node_index = 0; node_index < globalVar_profileSystem.nodeList_currentFrame->count; node_index++ ) {
			PROFILE_NODE * node = globalVar_profileSystem.nodeList_currentFrame->node + node_index;
			
			char profile_string[ 512 ] = {};
			sprintf( profile_string, "%-24s %10I64u  %6I64u  %7d  %8.02f  %6.02f", node->functionName, node->clock_total, node->clock_total / 1000, node->count_total, ( ( flo32 )node->clock_total / ( flo32 )frame_cycElapsed ) * 100.0f, ( ( flo32 )node->clock_total / ( flo32 )app_cycElapsed ) * 100.0f );
			PROFILE_string( profile_string );
            
            for( uint32 iSubclock = 0; iSubclock < node->nSubclock; iSubclock++ ) {
                flo32 percent = ( flo32 )node->subclock[ iSubclock ] / ( flo32 )node->clock_total;
                
                char str[ 512 ] = {};
                sprintf( str, "  %.02f", percent );
                PROFILE_string( str );
            }
            // average each subclock for the Event??
            // should this only work for nodes that have a single call
		}
	} else {
		flo32  frame_msElapsed 	= profileSystem->getMSElapsed( globalVar_profileSystem.EventDatabase_clocks_previousFrame.counter_frameStart, globalVar_profileSystem.EventDatabase_clocks_previousFrame.counter_frameEnd, perfFrequency );
		uint64 frame_cycElapsed = globalVar_profileSystem.EventDatabase_clocks_previousFrame.frameEnd - globalVar_profileSystem.EventDatabase_clocks_previousFrame.frameStart;
		uint64 app_cycElapsed = globalVar_profileSystem.EventDatabase_clocks_previousFrame.renderStart - globalVar_profileSystem.EventDatabase_clocks_previousFrame.appStart;
		char time_stringA[ 128 ] = {};
		sprintf( time_stringA, "Prev Frame: %10.04f ms  %10I64u cyc  %6I64u MC", frame_msElapsed, frame_cycElapsed, frame_cycElapsed / 1000 );
		
		char time_stringB[ 128 ] = {};
		sprintf( time_stringB, "App Update:                %10I64u cyc  %6I64u MC  %.02f", app_cycElapsed, app_cycElapsed / 1000, ( flo32 )app_cycElapsed / ( flo32 )frame_cycElapsed * 100.0f );
		
		DISPLAY_STRING( time_stringA );
		DISPLAY_STRING( time_stringB );
		
		PROFILE_EVENT_LIST * database_EventList = globalVar_profileSystem.EventDatabase.EventList + globalVar_profileSystem.EventDatabase.index;
		database_EventList->count = 0;
		database_EventList->frameClockStart = globalVar_profileSystem.EventDatabase_clocks_previousFrame.frameStart;
		
		globalVar_profileSystem.EventDatabase_clocks[ globalVar_profileSystem.EventDatabase.index ] = globalVar_profileSystem.EventDatabase_clocks_previousFrame;
		
		for( int32 Event_index = 0; Event_index < globalVar_profileSystem.EventList_currentFrame->count; Event_index++ ) {
			PROFILE_EVENT * srcEvent = globalVar_profileSystem.EventList_currentFrame->Event + Event_index;
			PROFILE_EVENT * destEvent = database_EventList->Event + database_EventList->count++;
			Assert( database_EventList->count < PROFILE_EVENT_LIST_MAX_COUNT );
			
			*destEvent = *srcEvent;
		}
		globalVar_profileSystem.EventDatabase.index = ( globalVar_profileSystem.EventDatabase.index + 1 ) % globalVar_profileSystem.EventDatabase.maxCount;
	}
	globalVar_profileSystem.EventList_currentFrame->count = 0;
	globalVar_profileSystem.nodeList_currentFrame->count = 0;
	
	KEYBOARD_STATE * Keyboard = &Platform->Keyboard;
	if( wasPressed( Keyboard, KeyCode_p ) ) {
		if( globalVar_profileSystem.showProfile ) {
			profileSystem->EventDatabase_scrollActive = false;
		}
	}
    
	
	globalVar_profileSystem.EventDatabase_clocks_currentFrame.PlatformStart = __rdtsc();
}

#endif // DEBUG_BUILD

#endif	// STD_INCLUDE_FUNC