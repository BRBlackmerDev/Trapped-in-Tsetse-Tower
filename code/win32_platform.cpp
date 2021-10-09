
internal void
genFilePath( char * filePath, char * SaveDir, char * filename, char * extension ) {
	if( SaveDir ) {
        strcpy( filePath, SaveDir );
        strcat( filePath, "/" );
	}
    strcat( filePath, filename );
    if( extension ) {
        strcat( filePath, "." );
        strcat( filePath, extension );
    }
}

internal FILE_DATA
win32_readFile( MEMORY * memory, char * SaveDir, char * filename, char * extension ) {
	FILE_DATA result = {};
	
	char filePath[ 2048 ] = {};
    genFilePath( filePath, SaveDir, filename, extension );
	
	HANDLE file = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if( file != INVALID_HANDLE_VALUE ) {
		LARGE_INTEGER _fileSize = {};
		if( GetFileSizeEx( file, &_fileSize ) ) {
            uint32 fileSize = ( uint32 )_fileSize.QuadPart;
            uint8 * dest = ( uint8 * )_pushSize( memory, fileSize );
			
			DWORD bytesRead = {};
            BOOL  status    = ReadFile( file, dest, fileSize, &bytesRead, 0 );
            if( ( status ) && ( fileSize == bytesRead ) ) {
                result.contents = dest;
                result.size     = fileSize;
			} else {
				_popSize( memory, result.size );
                
                char debug[ 128 ] {};
                sprintf( debug, "ERROR! File: %s : unable to read contents!\n", filename );
                OutputDebugString( debug );
			}
			CloseHandle( file );
		} else {
            char debug[ 128 ] = {};
            sprintf( debug, "ERROR! File: %s : does not contain any data!\n", filename );
            OutputDebugString( debug );
		}
	} else {
        char debug[ 128 ] = {};
        sprintf( debug, "ERROR! File: %s : unable to open!\n", filename );
        OutputDebugString( debug );
	}
	
	return result;
}

internal boo32
win32_writeFile( char * SaveDir, char * filename, char * extension, void * data, uint32 size ) {
	boo32 result = false;
	
	char filePath[ 2048 ] = {};
    genFilePath( filePath, SaveDir, filename, extension );
    
	HANDLE fileHandle = CreateFile( filePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0 );
	if( fileHandle != INVALID_HANDLE_VALUE ) {
		DWORD bytesWritten;
		if( WriteFile( fileHandle, data, size, &bytesWritten, 0 ) ) {
			result = ( bytesWritten == size );
		}
        
		CloseHandle( fileHandle );
	}
    
	return result;
}

internal boo32
win32_doesFileExist( char * SaveDir, char * filename, char * extension ) {
	char filePath[ 2048 ] = {};
    genFilePath( filePath, SaveDir, filename, extension );
    
    boo32 result = PathFileExists( filePath );
    return result;
}

internal boo32
win32_deleteFile( char * SaveDir, char * filename, char * extension ) {
	char filePath[ 2048 ] = {};
    genFilePath( filePath, SaveDir, filename, extension );
    
    boo32 result = DeleteFile( filePath );
    return result;
}

uint32 frame_counter = 0;
internal void
win32_outputMessage( char * header, char * message ) {
    char str[ 64 ] = {};
    sprintf( str, "%10u : %8s : %s\n", frame_counter, header, message );
    OutputDebugString( str );
}

internal void
win32_logMessage( uint32 message, boo32 fromWndProc ) {
    char * header0 = "WndProc";
    char * header1 = "my";
    
    char * header = ( fromWndProc ? header0 : header1 );
    
    switch( message ) {
        case WM_ACTIVATE: {
            // Sent to both the window being activated and the window being deactivated. If the windows use the same input queue, the message is sent synchronously, first to the window procedure of the Top-level window being deactivated, then to the window procedure of the Top-level window being activated. If the windows use different input queues, the message is sent asynchronously, so the window is activated immediately.
            win32_outputMessage( header, "WM_ACTIVATE" );
        } break;
        
        case WM_ACTIVATEAPP: {
            // Sent when the active window changes. Sent to both the activated window and the deactivated window.
            win32_outputMessage( header, "WM_ACTIVATEAPP" );
        } break;
        
        case WM_CAPTURECHANGED: {
            // Sent to the window that is losing the Mouse capture.
            win32_outputMessage( header, "WM_CAPTURECHANGED" );
        } break;
        
        case WM_CLOSE: {
            // Sent as a signal that a window or an application should terminate.
            win32_outputMessage( header, "WM_CLOSE" );
        } break;
        
        case WM_DESTROY: {
            // Sent when a window is being destroyed. It is sent to the window procedure of the window being destroyed after the window is removed from the screen.
            win32_outputMessage( header, "WM_DESTROY" );
        } break;
        
        case WM_GETMINMAXINFO: {
            // Sent to a window when its size or Position is about to change.
            win32_outputMessage( header, "WM_GETMINMAXINFO" );
        } break;
        
        case WM_GETICON: {
            // Sent to a window to retrieve the handle to the large or small icon associated with the window.
            win32_outputMessage( header, "WM_GETICON" );
        } break;
        
        case WM_IME_SETCONTEXT: {
            // Sent to an application when a window is activated. A window receives this message through its WindowProc function.
            win32_outputMessage( header, "WM_IME_SETCONTEXT" );
        } break;
        
        case WM_IME_NOTIFY: {
            // Sent to an application to notify it of changes to the IME window. A window receives this message through its WindowProc function.
            win32_outputMessage( header, "WM_IME_NOTIFY" );
        } break;
        
        case WM_KILLFOCUS: {
            // Sent to a window immediately before it loses the Keyboard focus.
            win32_outputMessage( header, "WM_KILLFOCUS" );
        } break;
        
        case WM_SETFOCUS: {
            // Sent to a window after it has gained the Keyboard focus.
            win32_outputMessage( header, "WM_KILLFOCUS" );
        } break;
        
        case WM_MOUSEMOVE: {
            // Posted to a window when the cursor moves. If the Mouse is not captured, the message is posted to the window that contains the cursor. Otherwise, the message is posted to the window that has captured the Mouse.
            // win32_outputMessage( header, "WM_MOUSEMOVE" );
        } break;
        
        case WM_NCACTIVATE: {
            // Sent to a window when its nonclient area needs to be changed to indicate an active or inactive State.
            win32_outputMessage( header, "WM_NCACTIVATE" );
        } break;
        
        case WM_NCCREATE: {
            // Sent when a window is first created. Sent prior to the WM_CREATE message.
            win32_outputMessage( header, "WM_NCCREATE" );
        } break;
        
        case WM_NCCALCSIZE: {
            // Sent when the size and Position of a window's client area must be calculated.
            win32_outputMessage( header, "WM_NCCALCSIZE" );
        } break;
        
        case WM_NCDESTROY: {
            // Notifies a window that its nonclient area is being destroyed. The DestroyWindow function sends the WM_NCDESTROY message to the window following the WM_DESTROY message.WM_DESTROY is used to free the allocated memory object associated with the window. The WM_NCDESTROY message is sent after the child windows have been destroyed. In contrast, WM_DESTROY is sent before the child windows are destroyed. A window receives this message through its WindowProc function.
            win32_outputMessage( header, "WM_NCDESTROY" );
        } break;
        
        case WM_NCHITTEST: {
            // Sent to a window to determine what part of the window corresponds to a particular screen coordinate. Sent, for example, in response to Mouse cursor movement or when a Mouse button is pressed/released.
            // win32_outputMessage( header, "WM_NCHITTEST" );
        } break;
        
        case WM_NCLBUTTONDOWN: {
            // Sent to a window when the user presses the Left Mouse button while the cursor is within the nonclient area of the window.
            win32_outputMessage( header, "WM_NCLBUTTONDOWN" );
        } break;
        
        case WM_NCMOUSEMOVE: {
            // Posted to a window when the cursor is moved within the nonclient area of the window. This message is posted to the window that contains the cursor. If a window has captured the Mouse, this message is not posted.
            // win32_outputMessage( header, "WM_NCMOUSEMOVE" );
        } break;
        
        case WM_PAINT: {
            // Sent when the system or another application makes a request to paint a portion of the application's window.
            win32_outputMessage( header, "WM_PAINT" );
        } break;
        
        case WM_QUIT: {
            // Indicates a request to terminate an application, and is generated when the application calls the PostQuitMessage function.
            win32_outputMessage( header, "WM_QUIT" );
        } break;
        
        case WM_SETCURSOR: {
            // Sent if the Mouse cursor moves, but the input is not captured by the window.
            //win32_outputMessage( header, "WM_SETCURSOR" );
        } break;
        
        case WM_SYSCOMMAND: {
            // A window receives this message when the user chooses a command from the Window menu (formerly known as the system or control menu) or when the user chooses the maximize button, minimize button, restore button, or close button.
            win32_outputMessage( header, "WM_SYSCOMMAND" );
        } break;
		
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            win32_outputMessage( header, "Windows Key Message" );
            // ERROR! Keyboard message was Passed directly to WindowProc! All Keyboard messages should be handled directly by the win32_processPendingMessages()!
        } break;
        
        case WM_TIMER: {
            // Posted to the installing thread's message queue when a timer expires. The message is posted by the GetMessage or PeekMessage function.
            // win32_outputMessage( header, "WM_TIMER" );
        } break;
        
        case WM_WINDOWPOSCHANGED: {
            // Sent to a window whose size, Position, or place in the Z order has changed as a result of a call to the SetWindowPos function or another window-management function.
            win32_outputMessage( header, "WM_WINDOWPOSCHANGED" );
        } break;
        
        case WM_WINDOWPOSCHANGING: {
            // Sent to a window whose size, Position, or place in the Z order is about to change as a result of a call to the SetWindowPos function or another window-management function.
            win32_outputMessage( header, "WM_WINDOWPOSCHANGING" );
        } break;
        
        default: {
            char str[ 8 ] = {};
            sprintf( str, "0x%04X", ( uint32 )message );
            win32_outputMessage( header, str );
        } break;
    }
}

global_variable boo32 global_restoreFocus = false;

LRESULT CALLBACK
win32_WindowProc( HWND window, uint32 message, WPARAM wParam, LPARAM lParam ) {
    //win32_logMessage( message, true );
	LRESULT result = 0;
    switch( message ) {
        case WM_ACTIVATEAPP: {
            global_restoreFocus = true;
        } break;
        
        case WM_DESTROY: {
            PostQuitMessage( 0 );
        } break;
        
        default: {
            result = DefWindowProc( window, message, wParam, lParam );
        } break;
    }
    return result;
}

internal void
win32_processPendingMessages( PLATFORM * Platform, HWND windowHandle ) {
    KEYBOARD_STATE * Keyboard = &Platform->Keyboard;
    Keyboard->nEvents = 0;
    
    MOUSE_STATE * Mouse = &Platform->Mouse;
    MOUSE_endOfFrame( Mouse );
    
	MSG message = {};
    while( PeekMessage( &message, 0, 0, 0, PM_REMOVE ) ) {
        //win32_logMessage( message.message, false );
        
        switch( message.message ) {
            case WM_QUIT: {
                Platform->isRunning = false;
            } break;
            
			case WM_MOUSEWHEEL: {
				int16 wParam_hi = ( ( message.wParam >> 16 ) & 0xFFFF );
				int32 wheelClicks = wParam_hi / 120;
				Mouse->wheelClicks = wheelClicks;
			} break;
			
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP: {
				addButtonEvent( Mouse, MouseButton_Left, ( message.wParam & MK_LBUTTON ), Keyboard->flags );
			} break;
			
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP: {
				addButtonEvent( Mouse, MouseButton_middle, ( message.wParam & MK_MBUTTON ), Keyboard->flags );
			} break;
			
			case WM_RBUTTONUP:
			case WM_RBUTTONDOWN: {
				addButtonEvent( Mouse, MouseButton_Right, ( message.wParam & MK_RBUTTON ), Keyboard->flags );
			} break;
			
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP: {
                boo32 wasDown = ( ( message.lParam & ( 1 << 30 ) ) != 0 );
				boo32 isDown  = ( ( message.lParam & ( 1 << 31 ) ) == 0 );
				if( isDown != wasDown ) {
                    switch( message.wParam ) {
						case VK_BACK: { addKeyEvent( Keyboard, KeyCode_backspace, isDown, Keyboard->flags ); } break;
						case VK_TAB: 		{ addKeyEvent( Keyboard, KeyCode_tab, isDown, Keyboard->flags ); } break;
						case VK_RETURN:	{ addKeyEvent( Keyboard, KeyCode_Enter, isDown, Keyboard->flags ); } break;
						case VK_SHIFT: 	{
                            if( isDown ) {
                                Keyboard->flags |=  KEYBOARD_FLAGS__SHIFT;
                            } else {
                                Keyboard->flags &= ~KEYBOARD_FLAGS__SHIFT;
                            }
							addKeyEvent( Keyboard, KeyCode_shift, isDown, Keyboard->flags );
						} break;
						case VK_CONTROL: {
                            if( isDown ) {
                                Keyboard->flags |=  KEYBOARD_FLAGS__CONTROL;
                            } else {
                                Keyboard->flags &= ~KEYBOARD_FLAGS__CONTROL;
                            }
							addKeyEvent( Keyboard, KeyCode_control, isDown, Keyboard->flags );
						} break;
						case VK_MENU: 		{
                            if( isDown ) {
                                Keyboard->flags |=  KEYBOARD_FLAGS__ALT;
                            } else {
                                Keyboard->flags &= ~KEYBOARD_FLAGS__ALT;
                            }
                            addKeyEvent( Keyboard, KeyCode_alt, isDown, Keyboard->flags );
                        } break;
						case VK_CAPITAL: 	{ addKeyEvent( Keyboard, KeyCode_capsLock, 	isDown, Keyboard->flags ); } break;
						case VK_ESCAPE: 	{ addKeyEvent( Keyboard, KeyCode_Escape, 		isDown, Keyboard->flags ); } break;
						case VK_SPACE: 	{ addKeyEvent( Keyboard, KeyCode_space, 		isDown, Keyboard->flags ); } break;
						case VK_PRIOR: 	{ addKeyEvent( Keyboard, KeyCode_pageUp, 		isDown, Keyboard->flags ); } break;
						case VK_NEXT: 		{ addKeyEvent( Keyboard, KeyCode_pageDown, 	isDown, Keyboard->flags ); } break;
						case VK_END: 		{ addKeyEvent( Keyboard, KeyCode_end, 			isDown, Keyboard->flags ); } break;
						case VK_HOME: 		{ addKeyEvent( Keyboard, KeyCode_home, 		isDown, Keyboard->flags ); } break;
						case VK_LEFT: 		{ addKeyEvent( Keyboard, KeyCode_Left, 		isDown, Keyboard->flags ); } break;
						case VK_RIGHT: 	{ addKeyEvent( Keyboard, KeyCode_Right, 		isDown, Keyboard->flags ); } break;
						case VK_UP: 		{ addKeyEvent( Keyboard, KeyCode_up, 			isDown, Keyboard->flags ); } break;
						case VK_DOWN: 		{ addKeyEvent( Keyboard, KeyCode_down,       isDown, Keyboard->flags ); } break;
						case VK_INSERT: 	{ addKeyEvent( Keyboard, KeyCode_insert, 		isDown, Keyboard->flags ); } break;
						case VK_DELETE: 	{ addKeyEvent( Keyboard, KeyCode_delete, 		isDown, Keyboard->flags ); } break;
						case 0x30: { addKeyEvent( Keyboard, KeyCode_0, isDown, Keyboard->flags ); } break;
						case 0x31: { addKeyEvent( Keyboard, KeyCode_1, isDown, Keyboard->flags ); } break;
						case 0x32: { addKeyEvent( Keyboard, KeyCode_2, isDown, Keyboard->flags ); } break;
						case 0x33: { addKeyEvent( Keyboard, KeyCode_3, isDown, Keyboard->flags ); } break;
						case 0x34: { addKeyEvent( Keyboard, KeyCode_4, isDown, Keyboard->flags ); } break;
						case 0x35: { addKeyEvent( Keyboard, KeyCode_5, isDown, Keyboard->flags ); } break;
						case 0x36: { addKeyEvent( Keyboard, KeyCode_6, isDown, Keyboard->flags ); } break;
						case 0x37: { addKeyEvent( Keyboard, KeyCode_7, isDown, Keyboard->flags ); } break;
						case 0x38: { addKeyEvent( Keyboard, KeyCode_8, isDown, Keyboard->flags ); } break;
						case 0x39: { addKeyEvent( Keyboard, KeyCode_9, isDown, Keyboard->flags ); } break;
						case 0x41: { addKeyEvent( Keyboard, KeyCode_a, isDown, Keyboard->flags ); } break;
						case 0x42: { addKeyEvent( Keyboard, KeyCode_b, isDown, Keyboard->flags ); } break;
						case 0x43: { addKeyEvent( Keyboard, KeyCode_c, isDown, Keyboard->flags ); } break;
						case 0x44: { addKeyEvent( Keyboard, KeyCode_d, isDown, Keyboard->flags ); } break;
						case 0x45: { addKeyEvent( Keyboard, KeyCode_e, isDown, Keyboard->flags ); } break;
						case 0x46: { addKeyEvent( Keyboard, KeyCode_f, isDown, Keyboard->flags ); } break;
						case 0x47: { addKeyEvent( Keyboard, KeyCode_g, isDown, Keyboard->flags ); } break;
						case 0x48: { addKeyEvent( Keyboard, KeyCode_h, isDown, Keyboard->flags ); } break;
						case 0x49: { addKeyEvent( Keyboard, KeyCode_i, isDown, Keyboard->flags ); } break;
						case 0x4A: { addKeyEvent( Keyboard, KeyCode_j, isDown, Keyboard->flags ); } break;
						case 0x4B: { addKeyEvent( Keyboard, KeyCode_k, isDown, Keyboard->flags ); } break;
						case 0x4C: { addKeyEvent( Keyboard, KeyCode_l, isDown, Keyboard->flags ); } break;
						case 0x4D: { addKeyEvent( Keyboard, KeyCode_m, isDown, Keyboard->flags ); } break;
						case 0x4E: { addKeyEvent( Keyboard, KeyCode_n, isDown, Keyboard->flags ); } break;
						case 0x4F: { addKeyEvent( Keyboard, KeyCode_o, isDown, Keyboard->flags ); } break;
						case 0x50: { addKeyEvent( Keyboard, KeyCode_p, isDown, Keyboard->flags ); } break;
						case 0x51: { addKeyEvent( Keyboard, KeyCode_q, isDown, Keyboard->flags ); } break;
						case 0x52: { addKeyEvent( Keyboard, KeyCode_r, isDown, Keyboard->flags ); } break;
						case 0x53: { addKeyEvent( Keyboard, KeyCode_s, isDown, Keyboard->flags ); } break;
						case 0x54: { addKeyEvent( Keyboard, KeyCode_t, isDown, Keyboard->flags ); } break;
						case 0x55: { addKeyEvent( Keyboard, KeyCode_u, isDown, Keyboard->flags ); } break;
						case 0x56: { addKeyEvent( Keyboard, KeyCode_v, isDown, Keyboard->flags ); } break;
						case 0x57: { addKeyEvent( Keyboard, KeyCode_w, isDown, Keyboard->flags ); } break;
						case 0x58: { addKeyEvent( Keyboard, KeyCode_x, isDown, Keyboard->flags ); } break;
						case 0x59: { addKeyEvent( Keyboard, KeyCode_y, isDown, Keyboard->flags ); } break;
						case 0x5A: { addKeyEvent( Keyboard, KeyCode_z, isDown, Keyboard->flags ); } break;
						case VK_OEM_1: { addKeyEvent( Keyboard, KeyCode_semicolon, isDown, Keyboard->flags ); } break;
						case VK_OEM_PLUS: { addKeyEvent( Keyboard, KeyCode_equal, isDown, Keyboard->flags ); } break;
						case VK_OEM_COMMA: { addKeyEvent( Keyboard, KeyCode_comma, isDown, Keyboard->flags ); } break;
						case VK_OEM_MINUS: { addKeyEvent( Keyboard, KeyCode_dash, isDown, Keyboard->flags ); } break;
						case VK_OEM_PERIOD: { addKeyEvent( Keyboard, KeyCode_period, isDown, Keyboard->flags ); } break;
						case VK_OEM_2: { addKeyEvent( Keyboard, KeyCode_forwardSlash, isDown, Keyboard->flags ); } break;
						case VK_OEM_3: { addKeyEvent( Keyboard, KeyCode_tilde, isDown, Keyboard->flags ); } break;
						case VK_OEM_4: { addKeyEvent( Keyboard, KeyCode_openBracket, isDown, Keyboard->flags ); } break;
						case VK_OEM_5: { addKeyEvent( Keyboard, KeyCode_backSlash, isDown, Keyboard->flags ); } break;
						case VK_OEM_6: { addKeyEvent( Keyboard, KeyCode_closeBracket, isDown, Keyboard->flags ); } break;
						case VK_OEM_7: { addKeyEvent( Keyboard, KeyCode_quote, isDown, Keyboard->flags ); } break;
						case VK_NUMPAD0: { addKeyEvent( Keyboard, KeyCode_num0, isDown, Keyboard->flags ); } break;
						case VK_NUMPAD1: { addKeyEvent( Keyboard, KeyCode_num1, isDown, Keyboard->flags ); } break;
						case VK_NUMPAD2: { addKeyEvent( Keyboard, KeyCode_num2, isDown, Keyboard->flags ); } break;
						case VK_NUMPAD3: { addKeyEvent( Keyboard, KeyCode_num3, isDown, Keyboard->flags ); } break;
						case VK_NUMPAD4: {addKeyEvent( Keyboard, KeyCode_num4, isDown, Keyboard->flags ); } break;
						case VK_NUMPAD5: { addKeyEvent( Keyboard, KeyCode_num5, isDown, Keyboard->flags ); } break;
						case VK_NUMPAD6: { addKeyEvent( Keyboard, KeyCode_num6, isDown, Keyboard->flags ); } break;
						case VK_NUMPAD7: { addKeyEvent( Keyboard, KeyCode_num7, isDown, Keyboard->flags ); } break;
						case VK_NUMPAD8: { addKeyEvent( Keyboard, KeyCode_num8, isDown, Keyboard->flags ); } break;
						case VK_NUMPAD9: { addKeyEvent( Keyboard, KeyCode_num9, isDown, Keyboard->flags ); } break;
						case VK_MULTIPLY: { addKeyEvent( Keyboard, KeyCode_numMul, isDown, Keyboard->flags ); } break;
						case VK_ADD: { addKeyEvent( Keyboard, KeyCode_numAdd, isDown, Keyboard->flags ); } break;
						case VK_DECIMAL: { addKeyEvent( Keyboard, KeyCode_numPeriod, isDown, Keyboard->flags ); } break;
						case VK_SUBTRACT: { addKeyEvent( Keyboard, KeyCode_numSub, isDown, Keyboard->flags ); } break;
						case VK_DIVIDE: { addKeyEvent( Keyboard, KeyCode_numDiv, isDown, Keyboard->flags ); } break;
						case VK_F1:      { addKeyEvent( Keyboard, KeyCode_F1,   isDown, Keyboard->flags ); } break;
						case VK_F2:      { addKeyEvent( Keyboard, KeyCode_F2,   isDown, Keyboard->flags ); } break;
						case VK_F3:      { addKeyEvent( Keyboard, KeyCode_F3,   isDown, Keyboard->flags ); } break;
						case VK_F4:      { addKeyEvent( Keyboard, KeyCode_F4,   isDown, Keyboard->flags ); } break;
						case VK_F5:      { addKeyEvent( Keyboard, KeyCode_F5,   isDown, Keyboard->flags ); } break;
						case VK_F6:      { addKeyEvent( Keyboard, KeyCode_F6,   isDown, Keyboard->flags ); } break;
						case VK_F7:      { addKeyEvent( Keyboard, KeyCode_F7,   isDown, Keyboard->flags ); } break;
						case VK_F8:      { addKeyEvent( Keyboard, KeyCode_F8,   isDown, Keyboard->flags ); } break;
						case VK_F9:      { addKeyEvent( Keyboard, KeyCode_F9,   isDown, Keyboard->flags ); } break;
						case VK_F10:     { addKeyEvent( Keyboard, KeyCode_F10,  isDown, Keyboard->flags ); } break;
						case VK_F11:     { addKeyEvent( Keyboard, KeyCode_F11,  isDown, Keyboard->flags ); } break;
						case VK_F12:     { addKeyEvent( Keyboard, KeyCode_F12,  isDown, Keyboard->flags ); } break;
						default: {
							char string[ 128 ] = {};
							sprintf( string, "Key message received, but not processed: %lu %s\n", (uint32)message.wParam, ( isDown ? "PRESSED" : "RELEASED" ) );
							OutputDebugString( string );
						} break;
					};
				}
			} break;
            
            default: {
                TranslateMessage( &message );
                DispatchMessage ( &message );
            } break;
        }
	}
}

internal void
initPass( RENDERER * Renderer, RENDER_PASS * Pass, MEMORY * memory, uint32 nTri, uint32 nLine, uint32 nText ) {
    Pass->triObjectList.maxObjects  = nTri;
    Pass->lineObjectList.maxObjects = nLine;
    Pass->textList.maxObjects       = nText;
    
    Pass->triObjectList.object  = _pushArray( memory,      RENDER_OBJECT, nTri  );
    Pass->lineObjectList.object = _pushArray( memory,      RENDER_OBJECT, nLine );
    Pass->textList.object       = _pushArray( memory, TEXT_RENDER_OBJECT, nText + DEBUG_SYSTEM__SAFETY_OBJECT_COUNT );
    
    Pass->ModelBuffer = &Renderer->ModelBuffer;
    Pass->textBuffer  = &Renderer->textBuffer;
}

typedef DWORD XINPUT_GET_STATE( DWORD controllerIndex, XINPUT_STATE     * controllerState     );
typedef DWORD XINPUT_SET_STATE( DWORD controllerIndex, XINPUT_VIBRATION * controllerVibration );

internal void
renderPass( RENDERER * Renderer, RENDER_PASS * Pass, mat4 Camera_transform ) {
    ID3D12GraphicsCommandList * commandList = Renderer->commandList;
    
    { // Draw Models
        VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
        commandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
        commandList->IASetIndexBuffer  ( &buffer->indexView );
        
        commandList->SetGraphicsRoot32BitConstants( 0, 16, Camera_transform.elem, 0 );
        
        { // triangle Models
            RENDER_OBJECT_LIST * objectList = &Pass->triObjectList;
            
            commandList->SetPipelineState( Renderer->PSS_tri_noDepth_blend );
            commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
            for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                RENDER_OBJECT object    = objectList->object[ iObject ];
                MODEL_DATA    ModelData = Renderer->ModelData[ object.ModelID ];
                
                commandList->SetGraphicsRoot32BitConstants( 0, 16, object.transform.elem, 16 );
                
                TEXTURE_ID TextureID = TextureID_defaultTexture;
                if( Renderer->Texture_isLoaded[ object.TextureID ] ) {
                    TextureID = object.TextureID;
                }
                D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( Renderer->device, Renderer->SRVHeap, TextureID );
                commandList->SetGraphicsRootDescriptorTable( 2, handle );
                
                commandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
                
                commandList->DrawIndexedInstanced( ModelData.nIndex, 1, ModelData.bIndex, ModelData.bVertex, 0 );
            }
            objectList->nObjects = 0;
        } // END triangle Models
        
        { // line Models
            RENDER_OBJECT_LIST * objectList = &Pass->lineObjectList;
            
            commandList->SetPipelineState( Renderer->PSS_line_noDepth_blend );
            commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_LINESTRIP );
            for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                RENDER_OBJECT object    = objectList->object[ iObject ];
                MODEL_DATA    ModelData = Renderer->ModelData[ object.ModelID ];
                
                commandList->SetGraphicsRoot32BitConstants( 0, 16, object.transform.elem, 16 );
                
                D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( Renderer->device, Renderer->SRVHeap, object.TextureID );
                commandList->SetGraphicsRootDescriptorTable( 2, handle );
                
                commandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
                
                commandList->DrawIndexedInstanced( ModelData.nIndex, 1, ModelData.bIndex, ModelData.bVertex, 0 );
            }
            objectList->nObjects = 0;
        } // END line Models
    }
    
    { // Draw overlay text
        commandList->SetPipelineState( Renderer->PSS_tri_noDepth_blend );
        
        VERTEX1_BUFFER * buffer = &Renderer->textBuffer;
        buffer->vertexView.SizeInBytes = sizeof( VERTEX1 ) * buffer->nVertex;
        
        commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
        commandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
        
        D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( Renderer->device, Renderer->SRVHeap, TextureID_font );
        commandList->SetGraphicsRootDescriptorTable( 2, handle );
        
        //mat4 Camera_transform = mat4_orthographic( App_Bound );
        commandList->SetGraphicsRoot32BitConstants( 0, 16, Camera_transform.elem, 0 );
        
        mat4 Model_transform = mat4_idEntity();
        commandList->SetGraphicsRoot32BitConstants( 0, 16, Model_transform.elem, 16 );
        
        TEXT_RENDER_OBJECT_LIST * objectList = &Pass->textList;
        for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
            TEXT_RENDER_OBJECT object = objectList->object[ iObject ];
            
            commandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
            
            commandList->DrawInstanced( object.nVertex, 1, object.bVertex, 0 );
        }
        objectList->nObjects = 0;
    }
}

internal int64
Win32_GetPerfCount() {
	LARGE_INTEGER PerfCounter = {};
	QueryPerformanceCounter( &PerfCounter );
	
	int64 Result = *( ( int64 * )&PerfCounter );
	return Result;
}


internal flo32
Win32_GetMSElapsed( int64 StartCounter, int64 EndCounter, int64 PerfFrequency ) {
	LARGE_INTEGER CounterA  = *( ( LARGE_INTEGER * )&StartCounter  );
	LARGE_INTEGER CounterB  = *( ( LARGE_INTEGER * )&EndCounter    );
	LARGE_INTEGER Frequency = *( ( LARGE_INTEGER * )&PerfFrequency );
	flo32 Result = 1000.0f * ( ( flo32 )( CounterB.QuadPart - CounterA.QuadPart ) / ( flo32 )( Frequency.QuadPart ) );
	return Result;
}

int32 CALLBACK
WinMain( HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int32 windowShowCode ) {
    srand( ( uint32 )time( 0 ) );
    
	boo32 SleepFreqIsHiRes = ( timeBeginPeriod( 1 ) == TIMERR_NOERROR );
    Assert( SleepFreqIsHiRes );
    
    LARGE_INTEGER PerfFrequency;
    QueryPerformanceFrequency( &PerfFrequency );
    
    uint32 memory_size = 0;
    memory_size += sizeof( APP_STATE );
    memory_size += sizeof( PLATFORM  );
    memory_size += APP_permMemorySize;
    memory_size += APP_TempMemorySize;
    
    MEMORY PlatformMemory = {};
    PlatformMemory.size = memory_size;
    PlatformMemory.base = VirtualAlloc( 0, memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
    
    APP_STATE * AppState = _pushType( &PlatformMemory, APP_STATE );
    PLATFORM  * Platform = _pushType( &PlatformMemory, PLATFORM  );
    Platform->targetSec  = 1.0f / APP_targetFPS;
    
    Platform->permMemory = subArena( &PlatformMemory, APP_permMemorySize );
    Platform->TempMemory = subArena( &PlatformMemory, APP_TempMemorySize );
    Platform->readFile      = win32_readFile;
    Platform->writeFile     = win32_writeFile;
    Platform->doesFileExist = win32_doesFileExist;
    Platform->deleteFile    = win32_deleteFile;
    
    D3D12_EnableDebug();
    DIRECTX_12_DISPLAY_SETTINGS displaySettings = getDisplay( &Platform->TempMemory );
    
    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc   = win32_WindowProc;
    windowClass.hInstance     = instance;
    windowClass.lpszClassName = "WindowClass";
    windowClass.hCursor       = LoadCursor( 0, IDC_ARROW );
    
    if( RegisterClass( &windowClass ) ) {
        // uint32 window_flags = WS_OVERLAPPEDWINDOW; // Windowed
        uint32 window_flags = ( WS_VISIBLE | WS_EX_TOPMOST | WS_POPUP ); // Fullscreen
        HWND window = CreateWindowEx( 0, "WindowClass", "Caption", window_flags, 0, 0, displaySettings.width, displaySettings.Height, 0, 0, instance, 0 );
        
        if( window ) {
            Platform->Renderer = initDirectX12( displaySettings, window );
            
            RENDERER     * Renderer    = &Platform->Renderer;
            MEMORY       * permMemory  = &Platform->permMemory;
            MEMORY       * TempMemory  = &Platform->TempMemory;
            //AUDIO_SYSTEM * audioSystem = &Platform->audioSystem;
            
            createShader( Renderer, displaySettings );
            
            ShowWindow( window, SW_SHOW );
            
            Platform->isRunning = true;
            
            FONT font = loadFont( 16.0f );
            
            resetCommandList( Renderer );
            
            RENDER_PASS * Pass_Game = &Renderer->Pass_Game;
            RENDER_PASS * Pass_UI   = &Renderer->Pass_UI;
            
            initPass( Renderer, Pass_Game, permMemory, 32768, 4096, 512 );
            initPass( Renderer, Pass_UI,   permMemory, 4096,  4096, 512 );
            Renderer->ModelData[ ModelID_line          ] = genLine         ( Renderer );
            Renderer->ModelData[ ModelID_rect          ] = genRect         ( Renderer );
            Renderer->ModelData[ ModelID_rectOutline   ] = genRectOutline  ( Renderer );
            Renderer->ModelData[ ModelID_circle        ] = genCircle       ( Renderer );
            Renderer->ModelData[ ModelID_circleOutline ] = genCircleOutline( Renderer );
            Renderer->ModelData[ ModelID_box           ] = genBox          ( Renderer );
            
            Renderer->ModelData[ ModelID_Player_wallSlide ] = genModel_PlayerWallSlide( Renderer );
            Renderer->ModelData[ ModelID_Jumper_stunned   ] = genModel_JumperStunned  ( Renderer );
            
            { // create and Upload white Texture
                uint32 whiteTexture = 0xFFFFFFFF;
                UploadTexture( Renderer, TextureID_whiteTexture, 1, 1, &whiteTexture );
            }
            { // create and Upload default debug Texture
                uint32 defaultTexture[ 256 * 256 ] = {};
                
                uint32 width  = 256;
                uint32 Height = 256;
                uint32 nTexel = width * Height;
                for( uint32 iter = 0; iter < nTexel; iter++ ) {
                    uint32 iRow = ( iter / 8192 );
                    uint32 iCol = ( iter / 32 ) % 8;
                    
                    if( ( ( ( iRow + iCol ) % 2 ) == 0 ) ) {
                        defaultTexture[ iter ] = 0xFFFF00FF;
                    } else {
                        defaultTexture[ iter ] = 0xFF444444;
                    }
                }
                UploadTexture( Renderer, TextureID_defaultTexture, width, Height, &defaultTexture );
            }
            
            UploadTexture( Renderer, TextureID_font, font.Texture_width, font.Texture_Height, font.Texture_data );
            
            GenTexture_ExitTypeS_Walk_EnterLeft( Renderer, TempMemory, TextureID_ExitTypeS_Walk_EnterLeft );
            UploadBMPFromFile( Platform, TextureID_ExitTypeS_Grate_Duck, ART_SAVE_DIRECTORY, "ExitTypeS_Grate_Duck" );
            UploadBMPFromFile( Platform, TextureID_ControlIcon_Use, ART_SAVE_DIRECTORY, "ControlIcon_Use" );
            
            executeCommandList( Renderer );
            
            //WIN32_AUDIO win32_audio = initAudio( window, audioSystem, permMemory );
            
            vec2 app_dim     = Vec2( ( flo32 )displaySettings.width, ( flo32 )displaySettings.Height );
            vec2 app_halfDim = app_dim * 0.5f;
            rect App_Bound   = rectBLD( Vec2( 0.0f, 0.0f ), app_dim );
            
            AppState->app_dim     = app_dim;
            AppState->app_halfDim = app_halfDim;
            AppState->App_Bound   = App_Bound;
            AppState->font        = &font;
            
            vec2 debugSystem_basePos = getTL( App_Bound ) + Vec2( 10.0f, -14.0f );
            globalVar_debugSystem.Pass_Game     = Pass_Game;
            globalVar_debugSystem.Pass_UI       = Pass_UI;
            globalVar_debugSystem.font          = &font;
            globalVar_debugSystem.advanceHeight = font.advanceHeight;
            frame_counter++;
            
            MOUSE_STATE      * Mouse      = &Platform->Mouse;
            KEYBOARD_STATE   * Keyboard   = &Platform->Keyboard;
            CONTROLLER_STATE * controller = &Platform->controller;
            Mouse->basePos = app_halfDim;
            
            HMODULE XInputLib = LoadLibraryA( "xinput1_3.dll" );
            XINPUT_GET_STATE * XInputGetState = 0;
            XINPUT_SET_STATE * XInputSetState = 0;
            if( XInputLib ) {
                XInputGetState = ( XINPUT_GET_STATE * )GetProcAddress( XInputLib, "XInputGetState" );
                XInputSetState = ( XINPUT_SET_STATE * )GetProcAddress( XInputLib, "XInputSetState" );
                
                controller->Left.dead  = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
                controller->Right.dead = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
            } else {
                InvalidCodePath;
            }
            
            ShowCursor( true );
            
#if   START_IN_EDITOR
            AppState->Mode = appMode_Editor;
#elif START_IN_LAYOUT
            AppState->Mode = appMode_layout;
#endif
            AppState->Collision_memory  = subArena( permMemory, _MB( 1 ) );
            AppState->Event_memory      = subArena( permMemory, _MB( 1 ) );
            AppState->Particle.Particle = _pushArray( permMemory, PARTICLE, PARTICLE_MAX_COUNT );
            
            while( Platform->isRunning ) {
                int64  PerfCounter_Start  = Win32_GetPerfCount();
                uint64 CycleCounter_Start = __rdtsc();
                
                globalVar_debugSystem.atPos = getBL( AppState->App_Bound ) + Vec2( 10.0f, 14.0f );
                
                win32_processPendingMessages( Platform, window );
                if( global_restoreFocus ) {
                    global_restoreFocus = false;
                    memset( &Platform->Mouse,    0, sizeof( MOUSE_STATE    ) );
                    memset( &Platform->Keyboard, 0, sizeof( KEYBOARD_STATE ) );
                    Mouse->basePos = app_halfDim;
                }
                
                resetCommandList( Renderer );
                if( wasPressed( Keyboard, KeyCode_F4, ( KEYBOARD_FLAGS__ALT ) ) ) {
                    PostQuitMessage( 0 );
                }
                
                POINT Mouse_Position = {};
                GetCursorPos( &Mouse_Position );
                flo32 MouseX = ( flo32 )Mouse_Position.x;
                flo32 MouseY = ( flo32 )( displaySettings.Height - 1 - Mouse_Position.y );
                
                Mouse->prevPosition = Mouse->Position;
                Mouse->Position     = Vec2( MouseX, MouseY );
                Mouse->dPos         = Mouse->Position - Mouse->prevPosition;
                
                { // update controller input
                    XINPUT_STATE controller_State  = {};
                    DWORD        controller_status = XInputGetState( 0, &controller_State );
                    boo32        controller_buttonDown[ 14 ] = {};
                    for( uint32 controllerButton_index = 0; controllerButton_index < controllerButton_count; controllerButton_index++ ) {
                        controller->button[ controllerButton_index ].halfTransitionCount = 0;
                    }
                    
                    controller->Left.xy  = Int32Pair( 0, 0 );
                    controller->Right.xy = Int32Pair( 0, 0 );
                    
                    if( controller_status == ERROR_SUCCESS ) {
                        updateButton( controller, controllerButton_dPad_up,        ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP        ) );
                        updateButton( controller, controllerButton_dPad_down,      ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN      ) );
                        updateButton( controller, controllerButton_dPad_Left,      ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT      ) );
                        updateButton( controller, controllerButton_dPad_Right,     ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT     ) );
                        updateButton( controller, controllerButton_start,          ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_START          ) );
                        updateButton( controller, controllerButton_back,           ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_BACK           ) );
                        updateButton( controller, controllerButton_thumb_Left,     ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB     ) );
                        updateButton( controller, controllerButton_thumb_Right,    ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB    ) );
                        updateButton( controller, controllerButton_shoulder_Left,  ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER  ) );
                        updateButton( controller, controllerButton_shoulder_Right, ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ) );
                        updateButton( controller, controllerButton_A,              ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_A              ) );
                        updateButton( controller, controllerButton_B,              ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_B              ) );
                        updateButton( controller, controllerButton_X,              ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_X              ) );
                        updateButton( controller, controllerButton_Y,              ( controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_Y              ) );
                        uint8 LeftTrigger  = ( uint8 )controller_State.Gamepad.bLeftTrigger;
                        uint8 RightTrigger = ( uint8 )controller_State.Gamepad.bRightTrigger;
                        updateButton( controller, controllerButton_trigger_Left,  LeftTrigger  > 0 );
                        updateButton( controller, controllerButton_trigger_Right, RightTrigger > 0 );
                        
                        controller->Left.xy.x  = ( int32 )controller_State.Gamepad.sThumbLX;
                        controller->Left.xy.y  = ( int32 )controller_State.Gamepad.sThumbLY;
                        controller->Right.xy.x = ( int32 )controller_State.Gamepad.sThumbRX;
                        controller->Right.xy.y = ( int32 )controller_State.Gamepad.sThumbRY;
                    }
                }
                
                APP_updateAndRender( AppState, Platform );
                //updateAudio( &win32_audio, audioSystem );
                
                if( Mouse->resetPos ) {
                    int32 X = ( int32 )Mouse->basePos.x;
                    int32 Y = displaySettings.Height - 1 - ( int32 )Mouse->basePos.y;
                    SetCursorPos( X, Y );
                    
                    Mouse->Position = Mouse->basePos;
                }
                
                if( globalVar_debugSystem.errorOccurred ) {
                    DrawString_noErrorCheck( globalVar_debugSystem.Pass_UI, globalVar_debugSystem.font, "ERROR OCCURRED!! Check console for details!", globalVar_debugSystem.atPos, Vec2( 1.0f, 1.0f ), COLOR_RED );
                    globalVar_debugSystem.atPos.y -= globalVar_debugSystem.advanceHeight;
                }
                
                uint64 CycleCounter_App = __rdtsc();
                
                ID3D12GraphicsCommandList * commandList = Renderer->commandList;
                
                ID3D12Resource * backBuffer = Renderer->backBuffers[ Renderer->currentBackBufferIndex ];
                D3D12_CPU_DESCRIPTOR_HANDLE RTV = CD3DX12_CPU_DESCRIPTOR_HANDLE( Renderer->RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), Renderer->currentBackBufferIndex, Renderer->RTVDescriptorSize );
                D3D12_CPU_DESCRIPTOR_HANDLE DSV = Renderer->DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
                
                D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET );
                commandList->ResourceBarrier( 1, &barrier );
                
                flo32 Color[ 4 ] = { 0.02f, 0.02f, 0.02f, 1.0f };
                commandList->ClearRenderTargetView( RTV, Color, 0, 0 );
                commandList->ClearDepthStencilView( DSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, 0 );
                
                commandList->SetPipelineState        ( Renderer->PSS_tri_depth_noBlend );
                commandList->SetGraphicsRootSignature( Renderer->rootSignature );
                
                commandList->SetDescriptorHeaps( 1, &Renderer->SRVHeap );
                
                flo32 width  = ( flo32 )displaySettings.width;
                flo32 Height = ( flo32 )displaySettings.Height;
                D3D12_VIEWPORT viewport = { 0.0f, 0.0f, width, Height, 0.0f, 1.0f };
                D3D12_RECT     scissor  = { 0, 0, ( LONG )width, ( LONG )Height };
                
                commandList->RSSetViewports   ( 1, &viewport );
                commandList->RSSetScissorRects( 1, &scissor  );
                
                commandList->OMSetRenderTargets( 1, &RTV, 0, &DSV );
                
                mat4 ui_transform   = mat4_orthographic( App_Bound );
                
                CAMERA_STATE * Camera = &AppState->Camera;
                if( AppState->Mode == appMode_Editor ) {
                    Camera = &AppState->Editor.Camera;
                }
                if( AppState->Mode == appMode_layout ) {
                    Camera = &AppState->layout.Camera;
                }
                rect Camera_Bound   = rectCD( Camera->pos, Camera->dim );
                mat4 game_transform = mat4_orthographic( Camera_Bound );
                
                renderPass( Renderer, Pass_Game, game_transform );
                renderPass( Renderer, Pass_UI,   ui_transform   );
                
                Renderer->textBuffer.nVertex = 0;
                
                int64  PerfCounter_End  = Win32_GetPerfCount();
                uint64 CycleCounter_End = __rdtsc();
                
                char   PerfString[ 128 ] = {};
                uint64 CycleElapsed_App  = CycleCounter_App - CycleCounter_Start;
                { // PROFILE MAIN LOOP
                    flo32  MSElapsed = Win32_GetMSElapsed( PerfCounter_Start, PerfCounter_End, PerfFrequency.QuadPart );
                    uint64 CycleElapsed = CycleCounter_End - CycleCounter_Start;
                    
                    sprintf( PerfString, "FRAME PERF: %6.02fms, %10lluMC", MSElapsed, CycleElapsed );
                    DISPLAY_STRING( PerfString );
                    
                    flo32 Percent = ( flo32 )( CycleElapsed_App ) / ( flo32 )( CycleElapsed ) * 100.0f;
                    sprintf( PerfString, "FRAME PERF: APP: %10llu, %.02f%%, DRAW: %.02f%%", CycleElapsed_App, Percent, 100.0f - Percent );
                    DISPLAY_STRING( PerfString );
                }
                
                { // OTHER PROFILING
                    PROFILE_STATE * State = &AppState->Profile;
                    for( uint32 iProfileBlock = 0; iProfileBlock < State->nProfileBlock; iProfileBlock++ ) {
                        PROFILE_BLOCK Profile = State->ProfileBlock[ iProfileBlock ];
                        
                        uint64 CycleElapsed = Profile.CycleCounter_End - Profile.CycleCounter_Start;
                        flo32  Percent = ( flo32 )CycleElapsed / ( flo32 )CycleElapsed_App * 100.0f;
                        
                        sprintf( PerfString, "%s: %10llu, %.02f%%", Profile.Label, CycleElapsed, Percent );
                        DISPLAY_STRING( PerfString );
                    }
                    State->nProfileBlock = 0;
                }
                
                present( Renderer );
            }
        }
    }
    
    return 0;
}