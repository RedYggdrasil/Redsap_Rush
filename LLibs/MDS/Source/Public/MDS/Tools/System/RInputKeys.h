#pragma once
#include "MDS/Tools/REnum.h"
#include "mds/Tools/System/RKeyCodes.h"

//Iostream included for MDS_KEY_CONSTEXPR_CONV logging
#include <iostream>

enum class EMDSInputKeyType : uint32_t
{
	////////////////MOUSE////////////////
	MDS_IK_LBUTTON,					//Right mouse button
	MDS_IK_RBUTTON,					//Control-break processing
	MDS_IK_CANCEL,					//Control-break processing
	MDS_IK_MBUTTON,					//Middle mouse button
	MDS_IK_XBUTTON1,				//X1 mouse button (button4)
	MDS_IK_XBUTTON2,				//X2 mouse button (button5)
	//Reserved
	////////////////KEYBOARD////////////////
	MDS_IK_BACK,					//BACKSPACE key
	MDS_IK_TAB,						//TAB key
	//Reserved
	MDS_IK_CLEAR,					//CLEAR key
	MDS_IK_RETURN,					//ENTER key
	//Unassigned
	MDS_IK_SHIFT,					//SHIFT key
	MDS_IK_CONTROL,					//CTRL key
	MDS_IK_MENU,					//ALT key
	MDS_IK_PAUSE,					//PAUSE key
	MDS_IK_CAPITAL,					//CAPS LOCK key
	MDS_IK_KANA_HANGUL,				//IME Kana/Hangul mode

	MDS_IK_IME_ON,					//IME On
	MDS_IK_JUNJA,					//IME Junja mode
	MDS_IK_FINAL,					//IME final mode
	MDS_IK_HANJA_KANJI,				//IME Hanja Kanji mode
	MDS_IK_IME_OFF,					//IME Off
	MDS_IK_ESCAPE,					//ESC key
	MDS_IK_CONVERT,					//IME convert
	MDS_IK_NONCONVERT,				//IME nonconvert
	MDS_IK_ACCEPT,					//IME accept
	MDS_IK_MODECHANGE,				//IME mode change request
	MDS_IK_SPACE,					//SPACEBAR
	MDS_IK_PRIOR,					//PAGE UP key
	MDS_IK_NEXT,					//PAGE DOWN key
	MDS_IK_END,						//END key
	MDS_IK_HOME,					//HOME key
	MDS_IK_LEFT,					//LEFT ARROW key
	MDS_IK_UP,						//UP ARROW key
	MDS_IK_RIGHT,					//RIGHT ARROW key
	MDS_IK_DOWN	,					//DOWN ARROW key
	MDS_IK_SELECT,					//SELECT key
	MDS_IK_PRINT,					//PRINT key
	MDS_IK_EXECUTE,					//EXECUTE key
	MDS_IK_SNAPSHOT,				//PRINT SCREEN key
	MDS_IK_INSERT,					//INS key
	MDS_IK_DELETE,					//DEL key
	MDS_IK_HELP	,					//HELP key
	MDS_IK_0,						//0 key
	MDS_IK_1,						//1 key
	MDS_IK_2,						//2 key
	MDS_IK_3,						//3 key
	MDS_IK_4,						//4 key
	MDS_IK_5,						//5 key
	MDS_IK_6,						//6 key
	MDS_IK_7,						//7 key
	MDS_IK_8,						//8 key
	MDS_IK_9,						//9 key
	//Undefined

	//Letters
	MDS_IK_A,						//A key
	MDS_IK_B,						//B key
	MDS_IK_C,						//C key
	MDS_IK_D,						//D key
	MDS_IK_E,						//E key
	MDS_IK_F,						//F key
	MDS_IK_G,						//G key
	MDS_IK_H,						//H key
	MDS_IK_I,						//I key
	MDS_IK_J,						//J key
	MDS_IK_K,						//K key
	MDS_IK_L,						//L key
	MDS_IK_M,						//M key
	MDS_IK_N,						//N key
	MDS_IK_O,						//O key
	MDS_IK_P,						//P key
	MDS_IK_Q,						//Q key
	MDS_IK_R,						//R key
	MDS_IK_S,						//S key
	MDS_IK_T,						//T key
	MDS_IK_U,						//U key
	MDS_IK_V,						//V key
	MDS_IK_W,						//W key
	MDS_IK_X,						//X key
	MDS_IK_Y,						//Y key
	MDS_IK_Z,						//Z key
	// Windows keys
	MDS_IK_LWIN,					//Left Windows key (Natural keyboard)
	MDS_IK_RWIN,					//Right Windows key (Natural keyboard)
	MDS_IK_APPS,					//Applications key (Natural keyboard)

//Reserved


	MDS_IK_SLEEP,					//Computer Sleep key


	MDS_IK_NUMPAD0,					//Numeric keypad 0 key
	MDS_IK_NUMPAD1,					//Numeric keypad 1 key
	MDS_IK_NUMPAD2,					//Numeric keypad 2 key
	MDS_IK_NUMPAD3,					//Numeric keypad 3 key
	MDS_IK_NUMPAD4,					//Numeric keypad 4 key
	MDS_IK_NUMPAD5,					//Numeric keypad 5 key
	MDS_IK_NUMPAD6,					//Numeric keypad 6 key
	MDS_IK_NUMPAD7,					//Numeric keypad 7 key
	MDS_IK_NUMPAD8,					//Numeric keypad 8 key
	MDS_IK_NUMPAD9,					//Numeric keypad 9 key
	MDS_IK_MULTIPLY,				//Multiply key
	MDS_IK_ADD,						//Add key
	MDS_IK_SEPARATOR,				//Separator key
	MDS_IK_SUBTRACT,				//Subtract key
	MDS_IK_DECIMAL,					//Decimal key
	MDS_IK_DIVIDE,					//Divide key

	// Function keys
	MDS_IK_F1,						//F1 key
	MDS_IK_F2,						//F2 key
	MDS_IK_F3,						//F3 key
	MDS_IK_F4,						//F4 key
	MDS_IK_F5,						//F5 key
	MDS_IK_F6,						//F6 key
	MDS_IK_F7,						//F7 key
	MDS_IK_F8,						//F8 key
	MDS_IK_F9,						//F9 key
	MDS_IK_F10,						//F10 key
	MDS_IK_F11,						//F11 key
	MDS_IK_F12,						//F12 key
	MDS_IK_F13,						//F13 key
	MDS_IK_F14,						//F14 key
	MDS_IK_F15,						//F15 key
	MDS_IK_F16,						//F16 key
	MDS_IK_F17,						//F17 key
	MDS_IK_F18,						//F18 key
	MDS_IK_F19,						//F19 key
	MDS_IK_F20,						//F20 key
	MDS_IK_F21,						//F21 key
	MDS_IK_F22,						//F22 key
	MDS_IK_F23,						//F23 key
	MDS_IK_F24,						//F24 key

	//Unassigned


	MDS_IK_NUMLOCK,					//NUM LOCK key
	MDS_IK_SCROLL,					//SCROLL LOCK key
	MDS_IK_OEM_NEC_EQUAL,			//Equal sign on numeric keypad
	//MDS_IK_OEM_FJ_JISHO,			//'Dictionary' key
	MDS_IK_OEM_FJ_MASSHOU,			//'Unregister word' key
	MDS_IK_OEM_FJ_TOUROKU,			//'Register word' key
	MDS_IK_OEM_FJ_LOYA,				//'Left OYAYUBI' key
	MDS_IK_OEM_FJ_ROYA,				//'Right OYAYUBI' key
	//Unassigned
	MDS_IK_LSHIFT,					//Left SHIFT key
	MDS_IK_RSHIFT,					//Right SHIFT key
	MDS_IK_LCONTROL,				//Left CONTROL key
	MDS_IK_RCONTROL,				//Right CONTROL key
	MDS_IK_LMENU,					//Left MENU key
	MDS_IK_RMENU,					//Right MENU key
	MDS_IK_BROWSER_BACK,			//Browser Back key
	MDS_IK_BROWSER_FORWARD,			//Browser Forward key
	MDS_IK_BROWSER_REFRESH,			//Browser Refresh key
	MDS_IK_BROWSER_STOP,			//Browser Stop key
	MDS_IK_BROWSER_SEARCH,			//Browser Search key
	MDS_IK_BROWSER_FAVORITES,		//Browser Favorites key
	MDS_IK_BROWSER_HOME,			//Browser Start and Home key
	MDS_IK_VOLUME_MUTE,				//Volume Mute key
	MDS_IK_VOLUME_DOWN,				//Volume Down key
	MDS_IK_VOLUME_UP,				//Volume Up key
	MDS_IK_MEDIA_NEXT_TRACK,		//Next Track key
	MDS_IK_MEDIA_PREV_TRACK,		//Previous Track key
	MDS_IK_MEDIA_STOP,				//Stop Media key
	MDS_IK_MEDIA_PLAY_PAUSE,		//Play/Pause Media key
	MDS_IK_LAUNCH_MAIL,				//Start Mail key
	MDS_IK_LAUNCH_MEDIA_SELECT,		//Select Media key
	MDS_IK_LAUNCH_APP1,				//Start Application 1 key
	MDS_IK_LAUNCH_APP2,				//Start Application 2 key
	//Reserved
	MDS_IK_OEM_1,					//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key
	MDS_IK_OEM_PLUS,				//For any country/region, the '+' key
	MDS_IK_OEM_COMMA,				//For any country/region, the ',' key
	MDS_IK_OEM_MINUS,				//For any country/region, the '-' key
	MDS_IK_OEM_PERIOD,				//For any country/region, the '.' key
	MDS_IK_OEM_2,					//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
	MDS_IK_OEM_3,					//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
	//Reserved
	//Unassigned
	MDS_IK_OEM_4,					//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
	MDS_IK_OEM_5,					//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\\|' key
	MDS_IK_OEM_6,					//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
	MDS_IK_OEM_7,					//Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
	MDS_IK_OEM_8,					//Used for miscellaneous characters; it can vary by keyboard.
	//Reserved
	MDS_IK_OEM_AX,					//'AX' key on Japanese AX kbd
	MDS_IK_OEM_102,					//The <> keys on the US standard keyboard, or the \\| key on the non-US 102-key keyboard
	//OEM specific
	MDS_IK_PROCESSKEY,				//IME PROCESS key
	//OEM specific
	MDS_IK_PACKET,					//Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
	//Unassigned
	//OEM specific
	MDS_IK_ATTN,					//Attn key
	MDS_IK_CRSEL,					//CrSel key
	MDS_IK_EXSEL,					//ExSel key
	MDS_IK_EREOF,					//Erase EOF key
	MDS_IK_PLAY,					//Play key
	MDS_IK_ZOOM,					//Zoom key
	MDS_IK_NONAME,					//Reserved
	MDS_IK_PA1,						//PA1 key
	MDS_IK_OEM_CLEAR,				//Clear key

	////////////////GAMEPAD////////////////
	MDS_IK_GMPD_MENU,
	MDS_IK_GMPD_VIEW,
	MDS_IK_GMPD_A,
	MDS_IK_GMPD_B,
	MDS_IK_GMPD_X,
	MDS_IK_GMPD_Y,
	MDS_IK_GMPD_DPAD_UP,
	MDS_IK_GMPD_DPAD_DOWN,
	MDS_IK_GMPD_DPAD_LEFT,
	MDS_IK_GMPD_DPAD_RIGHT,
	MDS_IK_GMPD_DPAD_LEFT_SHOULD,
	MDS_IK_GMPD_DPAD_RIGHT_SHOULD,
	MDS_IK_GMPD_DPAD_LEFT_THUMBSTICK,
	MDS_IK_GMPD_DPAD_RIGHT_THUMBSTICK,

	RENUM_COUNT
};

inline constexpr bool IsMouseKey(const EMDSInputKeyType EInKeyType)
{
	return
	EInKeyType <= EMDSInputKeyType::MDS_IK_XBUTTON2
	&&
	EInKeyType >= EMDSInputKeyType::MDS_IK_LBUTTON
	&&
	EInKeyType != EMDSInputKeyType::MDS_IK_CANCEL;
}
inline constexpr bool IsKeyBoardKey(const EMDSInputKeyType EInKeyType)
{
	return
		EInKeyType >= EMDSInputKeyType::MDS_IK_BACK
		&&
		EInKeyType <= EMDSInputKeyType::MDS_IK_OEM_CLEAR;
}
inline constexpr bool IsGamepadKey(const EMDSInputKeyType EInKeyType)
{
	return
		EInKeyType >= EMDSInputKeyType::MDS_IK_GMPD_MENU
		&&
		EInKeyType <= EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT_THUMBSTICK;
}

enum class EMDSAxis1DType : uint32_t
{
	// mouse
	MDS_A1D_MS_SCROLL,
	// gamepad
	MDS_A1D_GMPD_LEFT_TRIGGER,
	MDS_A1D_GMPD_RIGHT_TRIGGER,

	RENUM_COUNT
};


enum class EMDSAxis2DType : uint32_t
{
	// mouse
	MDS_A2D_MS_WND_NRML,
	MDS_A2D_MS_FRAME,
	MDS_A2D_MS_ACC,
	// gamepad
	MDS_A2D_GMPD_LEFT_STICK,
	MDS_A2D_GMPD_RIGHT_STICK,

	RENUM_COUNT
};

#pragma region MDS_KEY_CONSTEXPR_CONV
constexpr MDS_KEY ToMDSKey(EMDSInputKeyType EInInputKey)
{
	switch (EInInputKey)
	{
	case EMDSInputKeyType::MDS_IK_LBUTTON: return MDS_KEY_LBUTTON;
	case EMDSInputKeyType::MDS_IK_RBUTTON: return MDS_KEY_RBUTTON;
	case EMDSInputKeyType::MDS_IK_CANCEL: return MDS_KEY_CANCEL;
	case EMDSInputKeyType::MDS_IK_MBUTTON: return MDS_KEY_MBUTTON;
	case EMDSInputKeyType::MDS_IK_XBUTTON1: return MDS_KEY_XBUTTON1;
	case EMDSInputKeyType::MDS_IK_XBUTTON2: return MDS_KEY_XBUTTON2;
	case EMDSInputKeyType::MDS_IK_BACK: return MDS_KEY_BACK;
	case EMDSInputKeyType::MDS_IK_TAB: return MDS_KEY_TAB;
	case EMDSInputKeyType::MDS_IK_CLEAR: return MDS_KEY_CLEAR;
	case EMDSInputKeyType::MDS_IK_RETURN: return MDS_KEY_RETURN;
	case EMDSInputKeyType::MDS_IK_SHIFT: return MDS_KEY_SHIFT;
	case EMDSInputKeyType::MDS_IK_CONTROL: return MDS_KEY_CONTROL;
	case EMDSInputKeyType::MDS_IK_MENU: return MDS_KEY_MENU;
	case EMDSInputKeyType::MDS_IK_PAUSE: return MDS_KEY_PAUSE;
	case EMDSInputKeyType::MDS_IK_CAPITAL: return MDS_KEY_CAPITAL;
	case EMDSInputKeyType::MDS_IK_KANA_HANGUL: return MDS_KEY_KANA_HANGUL;
	case EMDSInputKeyType::MDS_IK_IME_ON: return MDS_KEY_IME_ON;
	case EMDSInputKeyType::MDS_IK_JUNJA: return MDS_KEY_JUNJA;
	case EMDSInputKeyType::MDS_IK_FINAL: return MDS_KEY_FINAL;
	case EMDSInputKeyType::MDS_IK_HANJA_KANJI: return MDS_KEY_HANJA_KANJI;
	case EMDSInputKeyType::MDS_IK_IME_OFF: return MDS_KEY_IME_OFF;
	case EMDSInputKeyType::MDS_IK_ESCAPE: return MDS_KEY_ESCAPE;
	case EMDSInputKeyType::MDS_IK_CONVERT: return MDS_KEY_CONVERT;
	case EMDSInputKeyType::MDS_IK_NONCONVERT: return MDS_KEY_NONCONVERT;
	case EMDSInputKeyType::MDS_IK_ACCEPT: return MDS_KEY_ACCEPT;
	case EMDSInputKeyType::MDS_IK_MODECHANGE: return MDS_KEY_MODECHANGE;
	case EMDSInputKeyType::MDS_IK_SPACE: return MDS_KEY_SPACE;
	case EMDSInputKeyType::MDS_IK_PRIOR: return MDS_KEY_PRIOR;
	case EMDSInputKeyType::MDS_IK_NEXT: return MDS_KEY_NEXT;
	case EMDSInputKeyType::MDS_IK_END: return MDS_KEY_END;
	case EMDSInputKeyType::MDS_IK_HOME: return MDS_KEY_HOME;
	case EMDSInputKeyType::MDS_IK_LEFT: return MDS_KEY_LEFT;
	case EMDSInputKeyType::MDS_IK_UP: return MDS_KEY_UP;
	case EMDSInputKeyType::MDS_IK_RIGHT: return MDS_KEY_RIGHT;
	case EMDSInputKeyType::MDS_IK_DOWN: return MDS_KEY_DOWN;
	case EMDSInputKeyType::MDS_IK_SELECT: return MDS_KEY_SELECT;
	case EMDSInputKeyType::MDS_IK_PRINT: return MDS_KEY_PRINT;
	case EMDSInputKeyType::MDS_IK_EXECUTE: return MDS_KEY_EXECUTE;
	case EMDSInputKeyType::MDS_IK_SNAPSHOT: return MDS_KEY_SNAPSHOT;
	case EMDSInputKeyType::MDS_IK_INSERT: return MDS_KEY_INSERT;
	case EMDSInputKeyType::MDS_IK_DELETE: return MDS_KEY_DELETE;
	case EMDSInputKeyType::MDS_IK_HELP: return MDS_KEY_HELP;
	case EMDSInputKeyType::MDS_IK_0: return MDS_KEY_0;
	case EMDSInputKeyType::MDS_IK_1: return MDS_KEY_1;
	case EMDSInputKeyType::MDS_IK_2: return MDS_KEY_2;
	case EMDSInputKeyType::MDS_IK_3: return MDS_KEY_3;
	case EMDSInputKeyType::MDS_IK_4: return MDS_KEY_4;
	case EMDSInputKeyType::MDS_IK_5: return MDS_KEY_5;
	case EMDSInputKeyType::MDS_IK_6: return MDS_KEY_6;
	case EMDSInputKeyType::MDS_IK_7: return MDS_KEY_7;
	case EMDSInputKeyType::MDS_IK_8: return MDS_KEY_8;
	case EMDSInputKeyType::MDS_IK_9: return MDS_KEY_9;
	case EMDSInputKeyType::MDS_IK_A: return MDS_KEY_A;
	case EMDSInputKeyType::MDS_IK_B: return MDS_KEY_B;
	case EMDSInputKeyType::MDS_IK_C: return MDS_KEY_C;
	case EMDSInputKeyType::MDS_IK_D: return MDS_KEY_D;
	case EMDSInputKeyType::MDS_IK_E: return MDS_KEY_E;
	case EMDSInputKeyType::MDS_IK_F: return MDS_KEY_F;
	case EMDSInputKeyType::MDS_IK_G: return MDS_KEY_G;
	case EMDSInputKeyType::MDS_IK_H: return MDS_KEY_H;
	case EMDSInputKeyType::MDS_IK_I: return MDS_KEY_I;
	case EMDSInputKeyType::MDS_IK_J: return MDS_KEY_J;
	case EMDSInputKeyType::MDS_IK_K: return MDS_KEY_K;
	case EMDSInputKeyType::MDS_IK_L: return MDS_KEY_L;
	case EMDSInputKeyType::MDS_IK_M: return MDS_KEY_M;
	case EMDSInputKeyType::MDS_IK_N: return MDS_KEY_N;
	case EMDSInputKeyType::MDS_IK_O: return MDS_KEY_O;
	case EMDSInputKeyType::MDS_IK_P: return MDS_KEY_P;
	case EMDSInputKeyType::MDS_IK_Q: return MDS_KEY_Q;
	case EMDSInputKeyType::MDS_IK_R: return MDS_KEY_R;
	case EMDSInputKeyType::MDS_IK_S: return MDS_KEY_S;
	case EMDSInputKeyType::MDS_IK_T: return MDS_KEY_T;
	case EMDSInputKeyType::MDS_IK_U: return MDS_KEY_U;
	case EMDSInputKeyType::MDS_IK_V: return MDS_KEY_V;
	case EMDSInputKeyType::MDS_IK_W: return MDS_KEY_W;
	case EMDSInputKeyType::MDS_IK_X: return MDS_KEY_X;
	case EMDSInputKeyType::MDS_IK_Y: return MDS_KEY_Y;
	case EMDSInputKeyType::MDS_IK_Z: return MDS_KEY_Z;
	case EMDSInputKeyType::MDS_IK_LWIN: return MDS_KEY_LWIN;
	case EMDSInputKeyType::MDS_IK_RWIN: return MDS_KEY_RWIN;
	case EMDSInputKeyType::MDS_IK_APPS: return MDS_KEY_APPS;
	case EMDSInputKeyType::MDS_IK_SLEEP: return MDS_KEY_SLEEP;
	case EMDSInputKeyType::MDS_IK_NUMPAD0: return MDS_KEY_NUMPAD0;
	case EMDSInputKeyType::MDS_IK_NUMPAD1: return MDS_KEY_NUMPAD1;
	case EMDSInputKeyType::MDS_IK_NUMPAD2: return MDS_KEY_NUMPAD2;
	case EMDSInputKeyType::MDS_IK_NUMPAD3: return MDS_KEY_NUMPAD3;
	case EMDSInputKeyType::MDS_IK_NUMPAD4: return MDS_KEY_NUMPAD4;
	case EMDSInputKeyType::MDS_IK_NUMPAD5: return MDS_KEY_NUMPAD5;
	case EMDSInputKeyType::MDS_IK_NUMPAD6: return MDS_KEY_NUMPAD6;
	case EMDSInputKeyType::MDS_IK_NUMPAD7: return MDS_KEY_NUMPAD7;
	case EMDSInputKeyType::MDS_IK_NUMPAD8: return MDS_KEY_NUMPAD8;
	case EMDSInputKeyType::MDS_IK_NUMPAD9: return MDS_KEY_NUMPAD9;
	case EMDSInputKeyType::MDS_IK_MULTIPLY: return MDS_KEY_MULTIPLY;
	case EMDSInputKeyType::MDS_IK_ADD: return MDS_KEY_ADD;
	case EMDSInputKeyType::MDS_IK_SEPARATOR: return MDS_KEY_SEPARATOR;
	case EMDSInputKeyType::MDS_IK_SUBTRACT: return MDS_KEY_SUBTRACT;
	case EMDSInputKeyType::MDS_IK_DECIMAL: return MDS_KEY_DECIMAL;
	case EMDSInputKeyType::MDS_IK_DIVIDE: return MDS_KEY_DIVIDE;
	case EMDSInputKeyType::MDS_IK_F1: return MDS_KEY_F1;
	case EMDSInputKeyType::MDS_IK_F2: return MDS_KEY_F2;
	case EMDSInputKeyType::MDS_IK_F3: return MDS_KEY_F3;
	case EMDSInputKeyType::MDS_IK_F4: return MDS_KEY_F4;
	case EMDSInputKeyType::MDS_IK_F5: return MDS_KEY_F5;
	case EMDSInputKeyType::MDS_IK_F6: return MDS_KEY_F6;
	case EMDSInputKeyType::MDS_IK_F7: return MDS_KEY_F7;
	case EMDSInputKeyType::MDS_IK_F8: return MDS_KEY_F8;
	case EMDSInputKeyType::MDS_IK_F9: return MDS_KEY_F9;
	case EMDSInputKeyType::MDS_IK_F10: return MDS_KEY_F10;
	case EMDSInputKeyType::MDS_IK_F11: return MDS_KEY_F11;
	case EMDSInputKeyType::MDS_IK_F12: return MDS_KEY_F12;
	case EMDSInputKeyType::MDS_IK_F13: return MDS_KEY_F13;
	case EMDSInputKeyType::MDS_IK_F14: return MDS_KEY_F14;
	case EMDSInputKeyType::MDS_IK_F15: return MDS_KEY_F15;
	case EMDSInputKeyType::MDS_IK_F16: return MDS_KEY_F16;
	case EMDSInputKeyType::MDS_IK_F17: return MDS_KEY_F17;
	case EMDSInputKeyType::MDS_IK_F18: return MDS_KEY_F18;
	case EMDSInputKeyType::MDS_IK_F19: return MDS_KEY_F19;
	case EMDSInputKeyType::MDS_IK_F20: return MDS_KEY_F20;
	case EMDSInputKeyType::MDS_IK_F21: return MDS_KEY_F21;
	case EMDSInputKeyType::MDS_IK_F22: return MDS_KEY_F22;
	case EMDSInputKeyType::MDS_IK_F23: return MDS_KEY_F23;
	case EMDSInputKeyType::MDS_IK_F24: return MDS_KEY_F24;
	case EMDSInputKeyType::MDS_IK_NUMLOCK: return MDS_KEY_NUMLOCK;
	case EMDSInputKeyType::MDS_IK_SCROLL: return MDS_KEY_SCROLL;
	case EMDSInputKeyType::MDS_IK_OEM_NEC_EQUAL: return MDS_KEY_OEM_NEC_EQUAL;
		//case EMDSInputKeyType::MDS_IK_OEM_FJ_JISHO: return MDS_KEY_OEM_FJ_JISHO;
	case EMDSInputKeyType::MDS_IK_OEM_FJ_MASSHOU: return MDS_KEY_OEM_FJ_MASSHOU;
	case EMDSInputKeyType::MDS_IK_OEM_FJ_TOUROKU: return MDS_KEY_OEM_FJ_TOUROKU;
	case EMDSInputKeyType::MDS_IK_OEM_FJ_LOYA: return MDS_KEY_OEM_FJ_LOYA;
	case EMDSInputKeyType::MDS_IK_OEM_FJ_ROYA: return MDS_KEY_OEM_FJ_ROYA;
	case EMDSInputKeyType::MDS_IK_LSHIFT: return MDS_KEY_LSHIFT;
	case EMDSInputKeyType::MDS_IK_RSHIFT: return MDS_KEY_RSHIFT;
	case EMDSInputKeyType::MDS_IK_LCONTROL: return MDS_KEY_LCONTROL;
	case EMDSInputKeyType::MDS_IK_RCONTROL: return MDS_KEY_RCONTROL;
	case EMDSInputKeyType::MDS_IK_LMENU: return MDS_KEY_LMENU;
	case EMDSInputKeyType::MDS_IK_RMENU: return MDS_KEY_RMENU;
	case EMDSInputKeyType::MDS_IK_BROWSER_BACK: return MDS_KEY_BROWSER_BACK;
	case EMDSInputKeyType::MDS_IK_BROWSER_FORWARD: return MDS_KEY_BROWSER_FORWARD;
	case EMDSInputKeyType::MDS_IK_BROWSER_REFRESH: return MDS_KEY_BROWSER_REFRESH;
	case EMDSInputKeyType::MDS_IK_BROWSER_STOP: return MDS_KEY_BROWSER_STOP;
	case EMDSInputKeyType::MDS_IK_BROWSER_SEARCH: return MDS_KEY_BROWSER_SEARCH;
	case EMDSInputKeyType::MDS_IK_BROWSER_FAVORITES: return MDS_KEY_BROWSER_FAVORITES;
	case EMDSInputKeyType::MDS_IK_BROWSER_HOME: return MDS_KEY_BROWSER_HOME;
	case EMDSInputKeyType::MDS_IK_VOLUME_MUTE: return MDS_KEY_VOLUME_MUTE;
	case EMDSInputKeyType::MDS_IK_VOLUME_DOWN: return MDS_KEY_VOLUME_DOWN;
	case EMDSInputKeyType::MDS_IK_VOLUME_UP: return MDS_KEY_VOLUME_UP;
	case EMDSInputKeyType::MDS_IK_MEDIA_NEXT_TRACK: return MDS_KEY_MEDIA_NEXT_TRACK;
	case EMDSInputKeyType::MDS_IK_MEDIA_PREV_TRACK: return MDS_KEY_MEDIA_PREV_TRACK;
	case EMDSInputKeyType::MDS_IK_MEDIA_STOP: return MDS_KEY_MEDIA_STOP;
	case EMDSInputKeyType::MDS_IK_MEDIA_PLAY_PAUSE: return MDS_KEY_MEDIA_PLAY_PAUSE;
	case EMDSInputKeyType::MDS_IK_LAUNCH_MAIL: return MDS_KEY_LAUNCH_MAIL;
	case EMDSInputKeyType::MDS_IK_LAUNCH_MEDIA_SELECT: return MDS_KEY_LAUNCH_MEDIA_SELECT;
	case EMDSInputKeyType::MDS_IK_LAUNCH_APP1: return MDS_KEY_LAUNCH_APP1;
	case EMDSInputKeyType::MDS_IK_LAUNCH_APP2: return MDS_KEY_LAUNCH_APP2;
	case EMDSInputKeyType::MDS_IK_OEM_1: return MDS_KEY_OEM_1;
	case EMDSInputKeyType::MDS_IK_OEM_PLUS: return MDS_KEY_OEM_PLUS;
	case EMDSInputKeyType::MDS_IK_OEM_COMMA: return MDS_KEY_OEM_COMMA;
	case EMDSInputKeyType::MDS_IK_OEM_MINUS: return MDS_KEY_OEM_MINUS;
	case EMDSInputKeyType::MDS_IK_OEM_PERIOD: return MDS_KEY_OEM_PERIOD;
	case EMDSInputKeyType::MDS_IK_OEM_2: return MDS_KEY_OEM_2;
	case EMDSInputKeyType::MDS_IK_OEM_3: return MDS_KEY_OEM_3;
	case EMDSInputKeyType::MDS_IK_OEM_4: return MDS_KEY_OEM_4;
	case EMDSInputKeyType::MDS_IK_OEM_5: return MDS_KEY_OEM_5;
	case EMDSInputKeyType::MDS_IK_OEM_6: return MDS_KEY_OEM_6;
	case EMDSInputKeyType::MDS_IK_OEM_7: return MDS_KEY_OEM_7;
	case EMDSInputKeyType::MDS_IK_OEM_8: return MDS_KEY_OEM_8;
	case EMDSInputKeyType::MDS_IK_OEM_AX: return MDS_KEY_OEM_AX;
	case EMDSInputKeyType::MDS_IK_OEM_102: return MDS_KEY_OEM_102;
	case EMDSInputKeyType::MDS_IK_PROCESSKEY: return MDS_KEY_PROCESSKEY;
	case EMDSInputKeyType::MDS_IK_PACKET: return MDS_KEY_PACKET;
	case EMDSInputKeyType::MDS_IK_ATTN: return MDS_KEY_ATTN;
	case EMDSInputKeyType::MDS_IK_CRSEL: return MDS_KEY_CRSEL;
	case EMDSInputKeyType::MDS_IK_EXSEL: return MDS_KEY_EXSEL;
	case EMDSInputKeyType::MDS_IK_EREOF: return MDS_KEY_EREOF;
	case EMDSInputKeyType::MDS_IK_PLAY: return MDS_KEY_PLAY;
	case EMDSInputKeyType::MDS_IK_ZOOM: return MDS_KEY_ZOOM;
	case EMDSInputKeyType::MDS_IK_NONAME: return MDS_KEY_NONAME;
	case EMDSInputKeyType::MDS_IK_PA1: return MDS_KEY_PA1;
	case EMDSInputKeyType::MDS_IK_OEM_CLEAR: return MDS_KEY_OEM_CLEAR;

		////////////////GAMEPAD////////////////
	case EMDSInputKeyType::MDS_IK_GMPD_MENU: return MDS_KEY_GMPD_MENU;
	case EMDSInputKeyType::MDS_IK_GMPD_VIEW: return MDS_KEY_GMPD_VIEW;
	case EMDSInputKeyType::MDS_IK_GMPD_A: return MDS_KEY_GMPD_A;
	case EMDSInputKeyType::MDS_IK_GMPD_B: return MDS_KEY_GMPD_B;
	case EMDSInputKeyType::MDS_IK_GMPD_X: return MDS_KEY_GMPD_X;
	case EMDSInputKeyType::MDS_IK_GMPD_Y: return MDS_KEY_GMPD_Y;
	case EMDSInputKeyType::MDS_IK_GMPD_DPAD_UP: return MDS_KEY_GMPD_DPAD_UP;
	case EMDSInputKeyType::MDS_IK_GMPD_DPAD_DOWN: return MDS_KEY_GMPD_DPAD_DOWN;
	case EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT: return MDS_KEY_GMPD_DPAD_LEFT;
	case EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT: return MDS_KEY_GMPD_DPAD_RIGHT;
	case EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT_SHOULD: return MDS_KEY_GMPD_DPAD_LEFT_SHOULD;
	case EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT_SHOULD: return MDS_KEY_GMPD_DPAD_RIGHT_SHOULD;
	case EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT_THUMBSTICK: return MDS_KEY_GMPD_DPAD_LEFT_THUMBSTICK;
	case EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT_THUMBSTICK: return MDS_KEY_GMPD_DPAD_RIGHT_THUMBSTICK;

	case EMDSInputKeyType::NONE: return 0;
	default:
		std::cout << "[ERROR] : MDS_KEY not implemented" << (uint32_t)EInInputKey << std::endl;
		return MDS_KEY();
	}
}

constexpr EMDSInputKeyType ToMDSInputKeyType(MDS_KEY InMDS_KEY)
{
	switch (InMDS_KEY)
	{
	case (MDS_KEY)0: return EMDSInputKeyType::NONE;
	case MDS_KEY_LBUTTON: return EMDSInputKeyType::MDS_IK_LBUTTON;
	case MDS_KEY_RBUTTON: return EMDSInputKeyType::MDS_IK_RBUTTON;
	case MDS_KEY_CANCEL: return EMDSInputKeyType::MDS_IK_CANCEL;
	case MDS_KEY_MBUTTON: return EMDSInputKeyType::MDS_IK_MBUTTON;
	case MDS_KEY_XBUTTON1: return EMDSInputKeyType::MDS_IK_XBUTTON1;
	case MDS_KEY_XBUTTON2: return EMDSInputKeyType::MDS_IK_XBUTTON2;
	case MDS_KEY_BACK: return EMDSInputKeyType::MDS_IK_BACK;
	case MDS_KEY_TAB: return EMDSInputKeyType::MDS_IK_TAB;
	case MDS_KEY_CLEAR: return EMDSInputKeyType::MDS_IK_CLEAR;
	case MDS_KEY_RETURN: return EMDSInputKeyType::MDS_IK_RETURN;
	case MDS_KEY_SHIFT: return EMDSInputKeyType::MDS_IK_SHIFT;
	case MDS_KEY_CONTROL: return EMDSInputKeyType::MDS_IK_CONTROL;
	case MDS_KEY_MENU: return EMDSInputKeyType::MDS_IK_MENU;
	case MDS_KEY_PAUSE: return EMDSInputKeyType::MDS_IK_PAUSE;
	case MDS_KEY_CAPITAL: return EMDSInputKeyType::MDS_IK_CAPITAL;
	case MDS_KEY_KANA_HANGUL: return EMDSInputKeyType::MDS_IK_KANA_HANGUL;
	case MDS_KEY_IME_ON: return EMDSInputKeyType::MDS_IK_IME_ON;
	case MDS_KEY_JUNJA: return EMDSInputKeyType::MDS_IK_JUNJA;
	case MDS_KEY_FINAL: return EMDSInputKeyType::MDS_IK_FINAL;
	case MDS_KEY_HANJA_KANJI: return EMDSInputKeyType::MDS_IK_HANJA_KANJI;
	case MDS_KEY_IME_OFF: return EMDSInputKeyType::MDS_IK_IME_OFF;
	case MDS_KEY_ESCAPE: return EMDSInputKeyType::MDS_IK_ESCAPE;
	case MDS_KEY_CONVERT: return EMDSInputKeyType::MDS_IK_CONVERT;
	case MDS_KEY_NONCONVERT: return EMDSInputKeyType::MDS_IK_NONCONVERT;
	case MDS_KEY_ACCEPT: return EMDSInputKeyType::MDS_IK_ACCEPT;
	case MDS_KEY_MODECHANGE: return EMDSInputKeyType::MDS_IK_MODECHANGE;
	case MDS_KEY_SPACE: return EMDSInputKeyType::MDS_IK_SPACE;
	case MDS_KEY_PRIOR: return EMDSInputKeyType::MDS_IK_PRIOR;
	case MDS_KEY_NEXT: return EMDSInputKeyType::MDS_IK_NEXT;
	case MDS_KEY_END: return EMDSInputKeyType::MDS_IK_END;
	case MDS_KEY_HOME: return EMDSInputKeyType::MDS_IK_HOME;
	case MDS_KEY_LEFT: return EMDSInputKeyType::MDS_IK_LEFT;
	case MDS_KEY_UP: return EMDSInputKeyType::MDS_IK_UP;
	case MDS_KEY_RIGHT: return EMDSInputKeyType::MDS_IK_RIGHT;
	case MDS_KEY_DOWN: return EMDSInputKeyType::MDS_IK_DOWN;
	case MDS_KEY_SELECT: return EMDSInputKeyType::MDS_IK_SELECT;
	case MDS_KEY_PRINT: return EMDSInputKeyType::MDS_IK_PRINT;
	case MDS_KEY_EXECUTE: return EMDSInputKeyType::MDS_IK_EXECUTE;
	case MDS_KEY_SNAPSHOT: return EMDSInputKeyType::MDS_IK_SNAPSHOT;
	case MDS_KEY_INSERT: return EMDSInputKeyType::MDS_IK_INSERT;
	case MDS_KEY_DELETE: return EMDSInputKeyType::MDS_IK_DELETE;
	case MDS_KEY_HELP: return EMDSInputKeyType::MDS_IK_HELP;
	case MDS_KEY_0: return EMDSInputKeyType::MDS_IK_0;
	case MDS_KEY_1: return EMDSInputKeyType::MDS_IK_1;
	case MDS_KEY_2: return EMDSInputKeyType::MDS_IK_2;
	case MDS_KEY_3: return EMDSInputKeyType::MDS_IK_3;
	case MDS_KEY_4: return EMDSInputKeyType::MDS_IK_4;
	case MDS_KEY_5: return EMDSInputKeyType::MDS_IK_5;
	case MDS_KEY_6: return EMDSInputKeyType::MDS_IK_6;
	case MDS_KEY_7: return EMDSInputKeyType::MDS_IK_7;
	case MDS_KEY_8: return EMDSInputKeyType::MDS_IK_8;
	case MDS_KEY_9: return EMDSInputKeyType::MDS_IK_9;
	case MDS_KEY_A: return EMDSInputKeyType::MDS_IK_A;
	case MDS_KEY_B: return EMDSInputKeyType::MDS_IK_B;
	case MDS_KEY_C: return EMDSInputKeyType::MDS_IK_C;
	case MDS_KEY_D: return EMDSInputKeyType::MDS_IK_D;
	case MDS_KEY_E: return EMDSInputKeyType::MDS_IK_E;
	case MDS_KEY_F: return EMDSInputKeyType::MDS_IK_F;
	case MDS_KEY_G: return EMDSInputKeyType::MDS_IK_G;
	case MDS_KEY_H: return EMDSInputKeyType::MDS_IK_H;
	case MDS_KEY_I: return EMDSInputKeyType::MDS_IK_I;
	case MDS_KEY_J: return EMDSInputKeyType::MDS_IK_J;
	case MDS_KEY_K: return EMDSInputKeyType::MDS_IK_K;
	case MDS_KEY_L: return EMDSInputKeyType::MDS_IK_L;
	case MDS_KEY_M: return EMDSInputKeyType::MDS_IK_M;
	case MDS_KEY_N: return EMDSInputKeyType::MDS_IK_N;
	case MDS_KEY_O: return EMDSInputKeyType::MDS_IK_O;
	case MDS_KEY_P: return EMDSInputKeyType::MDS_IK_P;
	case MDS_KEY_Q: return EMDSInputKeyType::MDS_IK_Q;
	case MDS_KEY_R: return EMDSInputKeyType::MDS_IK_R;
	case MDS_KEY_S: return EMDSInputKeyType::MDS_IK_S;
	case MDS_KEY_T: return EMDSInputKeyType::MDS_IK_T;
	case MDS_KEY_U: return EMDSInputKeyType::MDS_IK_U;
	case MDS_KEY_V: return EMDSInputKeyType::MDS_IK_V;
	case MDS_KEY_W: return EMDSInputKeyType::MDS_IK_W;
	case MDS_KEY_X: return EMDSInputKeyType::MDS_IK_X;
	case MDS_KEY_Y: return EMDSInputKeyType::MDS_IK_Y;
	case MDS_KEY_Z: return EMDSInputKeyType::MDS_IK_Z;
	case MDS_KEY_LWIN: return EMDSInputKeyType::MDS_IK_LWIN;
	case MDS_KEY_RWIN: return EMDSInputKeyType::MDS_IK_RWIN;
	case MDS_KEY_APPS: return EMDSInputKeyType::MDS_IK_APPS;
	case MDS_KEY_SLEEP: return EMDSInputKeyType::MDS_IK_SLEEP;
	case MDS_KEY_NUMPAD0: return EMDSInputKeyType::MDS_IK_NUMPAD0;
	case MDS_KEY_NUMPAD1: return EMDSInputKeyType::MDS_IK_NUMPAD1;
	case MDS_KEY_NUMPAD2: return EMDSInputKeyType::MDS_IK_NUMPAD2;
	case MDS_KEY_NUMPAD3: return EMDSInputKeyType::MDS_IK_NUMPAD3;
	case MDS_KEY_NUMPAD4: return EMDSInputKeyType::MDS_IK_NUMPAD4;
	case MDS_KEY_NUMPAD5: return EMDSInputKeyType::MDS_IK_NUMPAD5;
	case MDS_KEY_NUMPAD6: return EMDSInputKeyType::MDS_IK_NUMPAD6;
	case MDS_KEY_NUMPAD7: return EMDSInputKeyType::MDS_IK_NUMPAD7;
	case MDS_KEY_NUMPAD8: return EMDSInputKeyType::MDS_IK_NUMPAD8;
	case MDS_KEY_NUMPAD9: return EMDSInputKeyType::MDS_IK_NUMPAD9;
	case MDS_KEY_MULTIPLY: return EMDSInputKeyType::MDS_IK_MULTIPLY;
	case MDS_KEY_ADD: return EMDSInputKeyType::MDS_IK_ADD;
	case MDS_KEY_SEPARATOR: return EMDSInputKeyType::MDS_IK_SEPARATOR;
	case MDS_KEY_SUBTRACT: return EMDSInputKeyType::MDS_IK_SUBTRACT;
	case MDS_KEY_DECIMAL: return EMDSInputKeyType::MDS_IK_DECIMAL;
	case MDS_KEY_DIVIDE: return EMDSInputKeyType::MDS_IK_DIVIDE;
	case MDS_KEY_F1: return EMDSInputKeyType::MDS_IK_F1;
	case MDS_KEY_F2: return EMDSInputKeyType::MDS_IK_F2;
	case MDS_KEY_F3: return EMDSInputKeyType::MDS_IK_F3;
	case MDS_KEY_F4: return EMDSInputKeyType::MDS_IK_F4;
	case MDS_KEY_F5: return EMDSInputKeyType::MDS_IK_F5;
	case MDS_KEY_F6: return EMDSInputKeyType::MDS_IK_F6;
	case MDS_KEY_F7: return EMDSInputKeyType::MDS_IK_F7;
	case MDS_KEY_F8: return EMDSInputKeyType::MDS_IK_F8;
	case MDS_KEY_F9: return EMDSInputKeyType::MDS_IK_F9;
	case MDS_KEY_F10: return EMDSInputKeyType::MDS_IK_F10;
	case MDS_KEY_F11: return EMDSInputKeyType::MDS_IK_F11;
	case MDS_KEY_F12: return EMDSInputKeyType::MDS_IK_F12;
	case MDS_KEY_F13: return EMDSInputKeyType::MDS_IK_F13;
	case MDS_KEY_F14: return EMDSInputKeyType::MDS_IK_F14;
	case MDS_KEY_F15: return EMDSInputKeyType::MDS_IK_F15;
	case MDS_KEY_F16: return EMDSInputKeyType::MDS_IK_F16;
	case MDS_KEY_F17: return EMDSInputKeyType::MDS_IK_F17;
	case MDS_KEY_F18: return EMDSInputKeyType::MDS_IK_F18;
	case MDS_KEY_F19: return EMDSInputKeyType::MDS_IK_F19;
	case MDS_KEY_F20: return EMDSInputKeyType::MDS_IK_F20;
	case MDS_KEY_F21: return EMDSInputKeyType::MDS_IK_F21;
	case MDS_KEY_F22: return EMDSInputKeyType::MDS_IK_F22;
	case MDS_KEY_F23: return EMDSInputKeyType::MDS_IK_F23;
	case MDS_KEY_F24: return EMDSInputKeyType::MDS_IK_F24;
	case MDS_KEY_NUMLOCK: return EMDSInputKeyType::MDS_IK_NUMLOCK;
	case MDS_KEY_SCROLL: return EMDSInputKeyType::MDS_IK_SCROLL;
	case MDS_KEY_OEM_NEC_EQUAL: return EMDSInputKeyType::MDS_IK_OEM_NEC_EQUAL;
		//case MDS_KEY_OEM_FJ_JISHO				: return EMDSInputKeyType::MDS_IK_OEM_FJ_JISHO;
	case MDS_KEY_OEM_FJ_MASSHOU: return EMDSInputKeyType::MDS_IK_OEM_FJ_MASSHOU;
	case MDS_KEY_OEM_FJ_TOUROKU: return EMDSInputKeyType::MDS_IK_OEM_FJ_TOUROKU;
	case MDS_KEY_OEM_FJ_LOYA: return EMDSInputKeyType::MDS_IK_OEM_FJ_LOYA;
	case MDS_KEY_OEM_FJ_ROYA: return EMDSInputKeyType::MDS_IK_OEM_FJ_ROYA;
	case MDS_KEY_LSHIFT: return EMDSInputKeyType::MDS_IK_LSHIFT;
	case MDS_KEY_RSHIFT: return EMDSInputKeyType::MDS_IK_RSHIFT;
	case MDS_KEY_LCONTROL: return EMDSInputKeyType::MDS_IK_LCONTROL;
	case MDS_KEY_RCONTROL: return EMDSInputKeyType::MDS_IK_RCONTROL;
	case MDS_KEY_LMENU: return EMDSInputKeyType::MDS_IK_LMENU;
	case MDS_KEY_RMENU: return EMDSInputKeyType::MDS_IK_RMENU;
	case MDS_KEY_BROWSER_BACK: return EMDSInputKeyType::MDS_IK_BROWSER_BACK;
	case MDS_KEY_BROWSER_FORWARD: return EMDSInputKeyType::MDS_IK_BROWSER_FORWARD;
	case MDS_KEY_BROWSER_REFRESH: return EMDSInputKeyType::MDS_IK_BROWSER_REFRESH;
	case MDS_KEY_BROWSER_STOP: return EMDSInputKeyType::MDS_IK_BROWSER_STOP;
	case MDS_KEY_BROWSER_SEARCH: return EMDSInputKeyType::MDS_IK_BROWSER_SEARCH;
	case MDS_KEY_BROWSER_FAVORITES: return EMDSInputKeyType::MDS_IK_BROWSER_FAVORITES;
	case MDS_KEY_BROWSER_HOME: return EMDSInputKeyType::MDS_IK_BROWSER_HOME;
	case MDS_KEY_VOLUME_MUTE: return EMDSInputKeyType::MDS_IK_VOLUME_MUTE;
	case MDS_KEY_VOLUME_DOWN: return EMDSInputKeyType::MDS_IK_VOLUME_DOWN;
	case MDS_KEY_VOLUME_UP: return EMDSInputKeyType::MDS_IK_VOLUME_UP;
	case MDS_KEY_MEDIA_NEXT_TRACK: return EMDSInputKeyType::MDS_IK_MEDIA_NEXT_TRACK;
	case MDS_KEY_MEDIA_PREV_TRACK: return EMDSInputKeyType::MDS_IK_MEDIA_PREV_TRACK;
	case MDS_KEY_MEDIA_STOP: return EMDSInputKeyType::MDS_IK_MEDIA_STOP;
	case MDS_KEY_MEDIA_PLAY_PAUSE: return EMDSInputKeyType::MDS_IK_MEDIA_PLAY_PAUSE;
	case MDS_KEY_LAUNCH_MAIL: return EMDSInputKeyType::MDS_IK_LAUNCH_MAIL;
	case MDS_KEY_LAUNCH_MEDIA_SELECT: return EMDSInputKeyType::MDS_IK_LAUNCH_MEDIA_SELECT;
	case MDS_KEY_LAUNCH_APP1: return EMDSInputKeyType::MDS_IK_LAUNCH_APP1;
	case MDS_KEY_LAUNCH_APP2: return EMDSInputKeyType::MDS_IK_LAUNCH_APP2;
	case MDS_KEY_OEM_1: return EMDSInputKeyType::MDS_IK_OEM_1;
	case MDS_KEY_OEM_PLUS: return EMDSInputKeyType::MDS_IK_OEM_PLUS;
	case MDS_KEY_OEM_COMMA: return EMDSInputKeyType::MDS_IK_OEM_COMMA;
	case MDS_KEY_OEM_MINUS: return EMDSInputKeyType::MDS_IK_OEM_MINUS;
	case MDS_KEY_OEM_PERIOD: return EMDSInputKeyType::MDS_IK_OEM_PERIOD;
	case MDS_KEY_OEM_2: return EMDSInputKeyType::MDS_IK_OEM_2;
	case MDS_KEY_OEM_3: return EMDSInputKeyType::MDS_IK_OEM_3;
	case MDS_KEY_OEM_4: return EMDSInputKeyType::MDS_IK_OEM_4;
	case MDS_KEY_OEM_5: return EMDSInputKeyType::MDS_IK_OEM_5;
	case MDS_KEY_OEM_6: return EMDSInputKeyType::MDS_IK_OEM_6;
	case MDS_KEY_OEM_7: return EMDSInputKeyType::MDS_IK_OEM_7;
	case MDS_KEY_OEM_8: return EMDSInputKeyType::MDS_IK_OEM_8;
	case MDS_KEY_OEM_AX: return EMDSInputKeyType::MDS_IK_OEM_AX;
	case MDS_KEY_OEM_102: return EMDSInputKeyType::MDS_IK_OEM_102;
	case MDS_KEY_PROCESSKEY: return EMDSInputKeyType::MDS_IK_PROCESSKEY;
	case MDS_KEY_PACKET: return EMDSInputKeyType::MDS_IK_PACKET;
	case MDS_KEY_ATTN: return EMDSInputKeyType::MDS_IK_ATTN;
	case MDS_KEY_CRSEL: return EMDSInputKeyType::MDS_IK_CRSEL;
	case MDS_KEY_EXSEL: return EMDSInputKeyType::MDS_IK_EXSEL;
	case MDS_KEY_EREOF: return EMDSInputKeyType::MDS_IK_EREOF;
	case MDS_KEY_PLAY: return EMDSInputKeyType::MDS_IK_PLAY;
	case MDS_KEY_ZOOM: return EMDSInputKeyType::MDS_IK_ZOOM;
	case MDS_KEY_NONAME: return EMDSInputKeyType::MDS_IK_NONAME;
	case MDS_KEY_PA1: return EMDSInputKeyType::MDS_IK_PA1;
	case MDS_KEY_OEM_CLEAR: return EMDSInputKeyType::MDS_IK_OEM_CLEAR;

		////////////////GAMEPAD////////////////
	case MDS_KEY_GMPD_MENU: return EMDSInputKeyType::MDS_IK_GMPD_MENU;
	case MDS_KEY_GMPD_VIEW: return EMDSInputKeyType::MDS_IK_GMPD_VIEW;
	case MDS_KEY_GMPD_A: return EMDSInputKeyType::MDS_IK_GMPD_A;
	case MDS_KEY_GMPD_B: return EMDSInputKeyType::MDS_IK_GMPD_B;
	case MDS_KEY_GMPD_X: return EMDSInputKeyType::MDS_IK_GMPD_X;
	case MDS_KEY_GMPD_Y: return EMDSInputKeyType::MDS_IK_GMPD_Y;
	case MDS_KEY_GMPD_DPAD_UP: return EMDSInputKeyType::MDS_IK_GMPD_DPAD_UP;
	case MDS_KEY_GMPD_DPAD_DOWN: return EMDSInputKeyType::MDS_IK_GMPD_DPAD_DOWN;
	case MDS_KEY_GMPD_DPAD_LEFT: return EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT;
	case MDS_KEY_GMPD_DPAD_RIGHT: return EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT;
	case MDS_KEY_GMPD_DPAD_LEFT_SHOULD: return EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT_SHOULD;
	case MDS_KEY_GMPD_DPAD_RIGHT_SHOULD: return EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT_SHOULD;
	case MDS_KEY_GMPD_DPAD_LEFT_THUMBSTICK: return EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT_THUMBSTICK;
	case MDS_KEY_GMPD_DPAD_RIGHT_THUMBSTICK: return EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT_THUMBSTICK;
	default:
		std::cout << "[ERROR] : EMDSInputKeyType not implemented" << (uint32_t)InMDS_KEY << std::endl;
		return EMDSInputKeyType::NONE;
	}
}

#pragma endregion MDS_KEY_CONSTEXPR_CONV