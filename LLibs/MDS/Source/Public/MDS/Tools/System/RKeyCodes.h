#pragma once
//https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
#include <stdint.h>

typedef uint32_t            MDS_KEY;

////////////////MOUSE////////////////
#define MDS_KEY_LBUTTON					0x01 		//Left mouse button
#define MDS_KEY_RBUTTON					0x02 		//Right mouse button
#define MDS_KEY_CANCEL					0x03 		//Control-break processing
#define MDS_KEY_MBUTTON					0x04 		//Middle mouse button
#define MDS_KEY_XBUTTON1				0x05 		//X1 mouse button (button4)
#define MDS_KEY_XBUTTON2				0x06 		//X2 mouse button (button5)
//#define MDS_KEY_-						0x07 		//Reserved
////////////////KEYBOARD////////////////
#define MDS_KEY_BACK					0x08 		//BACKSPACE key
#define MDS_KEY_TAB						0x09 		//TAB key
//#define MDS_KEY_-						0x0A-0B 	//Reserved
#define MDS_KEY_CLEAR					0x0C 		//CLEAR key
#define MDS_KEY_RETURN					0x0D 		//ENTER key
//#define MDS_KEY_-						0x0E-0F 	//Unassigned
#define MDS_KEY_SHIFT					0x10 		//SHIFT key
#define MDS_KEY_CONTROL					0x11 		//CTRL key
#define MDS_KEY_MENU					0x12 		//ALT key
#define MDS_KEY_PAUSE					0x13 		//PAUSE key
#define MDS_KEY_CAPITAL					0x14 		//CAPS LOCK key
#define MDS_KEY_KANA_HANGUL				0x15 		//IME Kana/Hangul mode

#define MDS_KEY_IME_ON					0x16 		//IME On
#define MDS_KEY_JUNJA					0x17 		//IME Junja mode
#define MDS_KEY_FINAL					0x18 		//IME final mode
#define MDS_KEY_HANJA_KANJI				0x19 		//IME Hanja Kanji mode
#define MDS_KEY_IME_OFF					0x1A 		//IME Off
#define MDS_KEY_ESCAPE					0x1B 		//ESC key
#define MDS_KEY_CONVERT					0x1C 		//IME convert
#define MDS_KEY_NONCONVERT				0x1D 		//IME nonconvert
#define MDS_KEY_ACCEPT					0x1E 		//IME accept
#define MDS_KEY_MODECHANGE				0x1F 		//IME mode change request
#define MDS_KEY_SPACE					0x20 		//SPACEBAR
#define MDS_KEY_PRIOR					0x21 		//PAGE UP key
#define MDS_KEY_NEXT					0x22 		//PAGE DOWN key
#define MDS_KEY_END						0x23 		//END key
#define MDS_KEY_HOME					0x24 		//HOME key
#define MDS_KEY_LEFT					0x25 		//LEFT ARROW key
#define MDS_KEY_UP						0x26 		//UP ARROW key
#define MDS_KEY_RIGHT					0x27 		//RIGHT ARROW key
#define MDS_KEY_DOWN					0x28 		//DOWN ARROW key
#define MDS_KEY_SELECT					0x29 		//SELECT key
#define MDS_KEY_PRINT					0x2A 		//PRINT key
#define MDS_KEY_EXECUTE					0x2B 		//EXECUTE key
#define MDS_KEY_SNAPSHOT				0x2C 		//PRINT SCREEN key
#define MDS_KEY_INSERT					0x2D 		//INS key
#define MDS_KEY_DELETE					0x2E 		//DEL key
#define MDS_KEY_HELP					0x2F 		//HELP key
#define MDS_KEY_0						0x30 		//0 key
#define MDS_KEY_1						0x31 		//1 key
#define MDS_KEY_2						0x32 		//2 key
#define MDS_KEY_3						0x33 		//3 key
#define MDS_KEY_4						0x34 		//4 key
#define MDS_KEY_5						0x35 		//5 key
#define MDS_KEY_6						0x36 		//6 key
#define MDS_KEY_7						0x37 		//7 key
#define MDS_KEY_8						0x38 		//8 key
#define MDS_KEY_9						0x39 		//9 key
//#define MDS_KEY_-						0x3A-40 	//Undefined

//Letters	
#define MDS_KEY_A						0x41 		//A key
#define MDS_KEY_B						0x42 		//B key
#define MDS_KEY_C						0x43 		//C key
#define MDS_KEY_D						0x44 		//D key
#define MDS_KEY_E						0x45 		//E key
#define MDS_KEY_F						0x46 		//F key
#define MDS_KEY_G						0x47 		//G key
#define MDS_KEY_H						0x48 		//H key
#define MDS_KEY_I						0x49 		//I key
#define MDS_KEY_J						0x4A 		//J key
#define MDS_KEY_K						0x4B 		//K key
#define MDS_KEY_L						0x4C 		//L key
#define MDS_KEY_M						0x4D 		//M key
#define MDS_KEY_N						0x4E 		//N key
#define MDS_KEY_O						0x4F 		//O key
#define MDS_KEY_P						0x50 		//P key
#define MDS_KEY_Q						0x51 		//Q key
#define MDS_KEY_R						0x52 		//R key
#define MDS_KEY_S						0x53 		//S key
#define MDS_KEY_T						0x54 		//T key
#define MDS_KEY_U						0x55 		//U key
#define MDS_KEY_V						0x56 		//V key
#define MDS_KEY_W						0x57 		//W key
#define MDS_KEY_X						0x58 		//X key
#define MDS_KEY_Y						0x59 		//Y key
#define MDS_KEY_Z						0x5A 		//Z key
// Windows keys
#define MDS_KEY_LWIN					0x5B 		//Left Windows key (Natural keyboard)
#define MDS_KEY_RWIN					0x5C 		//Right Windows key (Natural keyboard)
#define MDS_KEY_APPS					0x5D 		//Applications key (Natural keyboard)

//#define MDS_KEY_-						0x5E 		//Reserved

// Sleep key
#define MDS_KEY_SLEEP					0x5F 		//Computer Sleep key

// Numpad keys
#define MDS_KEY_NUMPAD0					0x60 		//Numeric keypad 0 key
#define MDS_KEY_NUMPAD1					0x61 		//Numeric keypad 1 key
#define MDS_KEY_NUMPAD2					0x62 		//Numeric keypad 2 key
#define MDS_KEY_NUMPAD3					0x63 		//Numeric keypad 3 key
#define MDS_KEY_NUMPAD4					0x64 		//Numeric keypad 4 key
#define MDS_KEY_NUMPAD5					0x65 		//Numeric keypad 5 key
#define MDS_KEY_NUMPAD6					0x66 		//Numeric keypad 6 key
#define MDS_KEY_NUMPAD7					0x67 		//Numeric keypad 7 key
#define MDS_KEY_NUMPAD8					0x68 		//Numeric keypad 8 key
#define MDS_KEY_NUMPAD9					0x69 		//Numeric keypad 9 key
#define MDS_KEY_MULTIPLY				0x6A 		//Multiply key
#define MDS_KEY_ADD						0x6B 		//Add key
#define MDS_KEY_SEPARATOR				0x6C 		//Separator key
#define MDS_KEY_SUBTRACT				0x6D 		//Subtract key
#define MDS_KEY_DECIMAL					0x6E 		//Decimal key
#define MDS_KEY_DIVIDE					0x6F 		//Divide key

// Function keys
#define MDS_KEY_F1						0x70 		//F1 key
#define MDS_KEY_F2						0x71 		//F2 key
#define MDS_KEY_F3						0x72 		//F3 key
#define MDS_KEY_F4						0x73 		//F4 key
#define MDS_KEY_F5						0x74 		//F5 key
#define MDS_KEY_F6						0x75 		//F6 key
#define MDS_KEY_F7						0x76 		//F7 key
#define MDS_KEY_F8						0x77 		//F8 key
#define MDS_KEY_F9						0x78 		//F9 key
#define MDS_KEY_F10						0x79 		//F10 key
#define MDS_KEY_F11						0x7A 		//F11 key
#define MDS_KEY_F12						0x7B 		//F12 key
#define MDS_KEY_F13						0x7C 		//F13 key
#define MDS_KEY_F14						0x7D 		//F14 key
#define MDS_KEY_F15						0x7E 		//F15 key
#define MDS_KEY_F16						0x7F 		//F16 key
#define MDS_KEY_F17						0x80 		//F17 key
#define MDS_KEY_F18						0x81 		//F18 key
#define MDS_KEY_F19						0x82 		//F19 key
#define MDS_KEY_F20						0x83 		//F20 key
#define MDS_KEY_F21						0x84 		//F21 key
#define MDS_KEY_F22						0x85 		//F22 key
#define MDS_KEY_F23						0x86 		//F23 key
#define MDS_KEY_F24						0x87 		//F24 key

//#define MDS_KEY_-						0x88-8F 	//Unassigned

// OEM keys
#define MDS_KEY_NUMLOCK					0x90 		//NUM LOCK key
#define MDS_KEY_SCROLL					0x91 		//SCROLL LOCK key
#define MDS_KEY_OEM_NEC_EQUAL			0x92 		//Equal sign on numeric keypad
//#define MDS_KEY_OEM_FJ_JISHO			0x92 		//'Dictionary' key
#define MDS_KEY_OEM_FJ_MASSHOU			0x93 		//'Unregister word' key
#define MDS_KEY_OEM_FJ_TOUROKU			0x94 		//'Register word' key
#define MDS_KEY_OEM_FJ_LOYA				0x95 		//'Left OYAYUBI' key
#define MDS_KEY_OEM_FJ_ROYA				0x96 		//'Right OYAYUBI' key
//#define MDS_KEY_-						0x97-9F 	//Unassigned
#define MDS_KEY_LSHIFT					0xA0 		//Left SHIFT key
#define MDS_KEY_RSHIFT					0xA1 		//Right SHIFT key
#define MDS_KEY_LCONTROL				0xA2 		//Left CONTROL key
#define MDS_KEY_RCONTROL				0xA3 		//Right CONTROL key
#define MDS_KEY_LMENU					0xA4 		//Left MENU key
#define MDS_KEY_RMENU					0xA5 		//Right MENU key
#define MDS_KEY_BROWSER_BACK			0xA6 		//Browser Back key
#define MDS_KEY_BROWSER_FORWARD			0xA7 		//Browser Forward key
#define MDS_KEY_BROWSER_REFRESH			0xA8 		//Browser Refresh key
#define MDS_KEY_BROWSER_STOP			0xA9 		//Browser Stop key
#define MDS_KEY_BROWSER_SEARCH			0xAA 		//Browser Search key
#define MDS_KEY_BROWSER_FAVORITES		0xAB 	//Browser Favorites key
#define MDS_KEY_BROWSER_HOME			0xAC 		//Browser Start and Home key
#define MDS_KEY_VOLUME_MUTE				0xAD 		//Volume Mute key
#define MDS_KEY_VOLUME_DOWN				0xAE 		//Volume Down key
#define MDS_KEY_VOLUME_UP				0xAF 		//Volume Up key
#define MDS_KEY_MEDIA_NEXT_TRACK 		0xB0 		//Next Track key
#define MDS_KEY_MEDIA_PREV_TRACK 		0xB1 		//Previous Track key
#define MDS_KEY_MEDIA_STOP				0xB2 		//Stop Media key
#define MDS_KEY_MEDIA_PLAY_PAUSE		0xB3 		//Play/Pause Media key
#define MDS_KEY_LAUNCH_MAIL				0xB4 		//Start Mail key
#define MDS_KEY_LAUNCH_MEDIA_SELECT		0xB5 	//Select Media key
#define MDS_KEY_LAUNCH_APP1				0xB6 		//Start Application 1 key
#define MDS_KEY_LAUNCH_APP2				0xB7 		//Start Application 2 key
//#define MDS_KEY_-						0xB8-B9 	//Reserved
#define MDS_KEY_OEM_1					0xBA 		//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key
#define MDS_KEY_OEM_PLUS				0xBB 		//For any country/region, the '+' key
#define MDS_KEY_OEM_COMMA				0xBC 		//For any country/region, the ',' key
#define MDS_KEY_OEM_MINUS				0xBD 		//For any country/region, the '-' key
#define MDS_KEY_OEM_PERIOD				0xBE 		//For any country/region, the '.' key
#define MDS_KEY_OEM_2					0xBF 		//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
#define MDS_KEY_OEM_3					0xC0 		//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
//#define MDS_KEY_-						0xC1-D7 	//Reserved
//#define MDS_KEY_-						0xD8-DA 	//Unassigned
#define MDS_KEY_OEM_4					0xDB 		//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
#define MDS_KEY_OEM_5					0xDC 		//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\\|' key
#define MDS_KEY_OEM_6					0xDD 		//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
#define MDS_KEY_OEM_7					0xDE 		//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
#define MDS_KEY_OEM_8					0xDF 		//Used for miscellaneous characters; it can vary by keyboard.
//#define MDS_KEY_-						0xE0 		//Reserved
#define MDS_KEY_OEM_AX					0xE1 		//'AX' key on Japanese AX kbd
#define MDS_KEY_OEM_102					0xE2		//The <> keys on the US standard keyboard, or the \\| key on the non-US 102-key keyboard
//#define MDS_KEY_- 					0xE3-E4 	//OEM specific
#define MDS_KEY_PROCESSKEY 				0xE5 		//IME PROCESS key
//#define MDS_KEY_- 					0xE6 		//OEM specific
#define MDS_KEY_PACKET 					0xE7 		//Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
//#define MDS_KEY_- 					0xE8 		//Unassigned
//#define MDS_KEY_- 					0xE9-F5 	//OEM specific
#define MDS_KEY_ATTN 					0xF6 		//Attn key
#define MDS_KEY_CRSEL 					0xF7 		//CrSel key
#define MDS_KEY_EXSEL 					0xF8 		//ExSel key
#define MDS_KEY_EREOF 					0xF9 		//Erase EOF key
#define MDS_KEY_PLAY 					0xFA 		//Play key
#define MDS_KEY_ZOOM 					0xFB 		//Zoom key
#define MDS_KEY_NONAME 					0xFC 		//Reserved
#define MDS_KEY_PA1						0xFD 		//PA1 key
#define MDS_KEY_OEM_CLEAR 				0xFE 		//Clear key

////////////////GAMEPAD////////////////
#define MDS_KEY_GMPD_MENU 					0x101 		//GameInputGamepadMenu
#define MDS_KEY_GMPD_VIEW 					0x102 		//GameInputGamepadView
#define MDS_KEY_GMPD_A 						0x103 		//GameInputGamepadA              
#define MDS_KEY_GMPD_B 						0x104 		//GameInputGamepadB              
#define MDS_KEY_GMPD_X						0x105 		//GameInputGamepadX              
#define MDS_KEY_GMPD_Y	 					0x106 		//GameInputGamepadY              
#define MDS_KEY_GMPD_DPAD_UP				0x107 		//GameInputGamepadDPadUp         
#define MDS_KEY_GMPD_DPAD_DOWN				0x108 		//GameInputGamepadDPadDown       
#define MDS_KEY_GMPD_DPAD_LEFT				0x109 		//GameInputGamepadDPadLeft       
#define MDS_KEY_GMPD_DPAD_RIGHT				0x10A 		//GameInputGamepadDPadRight      
#define MDS_KEY_GMPD_DPAD_LEFT_SHOULD		0x10B 		//GameInputGamepadLeftShoulder   
#define MDS_KEY_GMPD_DPAD_RIGHT_SHOULD		0x10C 		//GameInputGamepadRightShoulder  
#define MDS_KEY_GMPD_DPAD_LEFT_THUMBSTICK	0x10D 		//GameInputGamepadLeftThumbstick 
#define MDS_KEY_GMPD_DPAD_RIGHT_THUMBSTICK	0x10E 		//GameInputGamepadRightThumbstick

#define MDS_AXIS_MOUSE					0x0200 		//Right mouse button