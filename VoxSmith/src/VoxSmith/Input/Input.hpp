#pragma once

#include <cstdint>

#include "../Core.hpp"

#define VOX_KEY_SPACE              32
#define VOX_KEY_APOSTROPHE         39  /* ' */
#define VOX_KEY_COMMA              44  /* , */
#define VOX_KEY_MINUS              45  /* - */
#define VOX_KEY_PERIOD             46  /* . */
#define VOX_KEY_SLASH              47  /* / */
#define VOX_KEY_0                  48
#define VOX_KEY_1                  49
#define VOX_KEY_2                  50
#define VOX_KEY_3                  51
#define VOX_KEY_4                  52
#define VOX_KEY_5                  53
#define VOX_KEY_6                  54
#define VOX_KEY_7                  55
#define VOX_KEY_8                  56
#define VOX_KEY_9                  57
#define VOX_KEY_SEMICOLON          59  /* ; */
#define VOX_KEY_EQUAL              61  /* = */
#define VOX_KEY_A                  65
#define VOX_KEY_B                  66
#define VOX_KEY_C                  67
#define VOX_KEY_D                  68
#define VOX_KEY_E                  69
#define VOX_KEY_F                  70
#define VOX_KEY_G                  71
#define VOX_KEY_H                  72
#define VOX_KEY_I                  73
#define VOX_KEY_J                  74
#define VOX_KEY_K                  75
#define VOX_KEY_L                  76
#define VOX_KEY_M                  77
#define VOX_KEY_N                  78
#define VOX_KEY_O                  79
#define VOX_KEY_P                  80
#define VOX_KEY_Q                  81
#define VOX_KEY_R                  82
#define VOX_KEY_S                  83
#define VOX_KEY_T                  84
#define VOX_KEY_U                  85
#define VOX_KEY_V                  86
#define VOX_KEY_W                  87
#define VOX_KEY_X                  88
#define VOX_KEY_Y                  89
#define VOX_KEY_Z                  90
#define VOX_KEY_LEFT_BRACKET       91 
#define VOX_KEY_BACKSLASH          92 
#define VOX_KEY_RIGHT_BRACKET      93 
#define VOX_KEY_GRAVE_ACCENT       96 
#define VOX_KEY_WORLD_1            161
#define VOX_KEY_WORLD_2            162

#define VOX_KEY_ESCAPE             256
#define VOX_KEY_ENTER              257
#define VOX_KEY_TAB                258
#define VOX_KEY_BACKSPACE          259
#define VOX_KEY_INSERT             260
#define VOX_KEY_DELETE             261
#define VOX_KEY_RIGHT              262
#define VOX_KEY_LEFT               263
#define VOX_KEY_DOWN               264
#define VOX_KEY_UP                 265
#define VOX_KEY_PAGE_UP            266
#define VOX_KEY_PAGE_DOWN          267
#define VOX_KEY_HOME               268
#define VOX_KEY_END                269
#define VOX_KEY_CAPS_LOCK          280
#define VOX_KEY_SCROLL_LOCK        281
#define VOX_KEY_NUM_LOCK           282
#define VOX_KEY_PRINT_SCREEN       283
#define VOX_KEY_PAUSE              284
#define VOX_KEY_F1                 290
#define VOX_KEY_F2                 291
#define VOX_KEY_F3                 292
#define VOX_KEY_F4                 293
#define VOX_KEY_F5                 294
#define VOX_KEY_F6                 295
#define VOX_KEY_F7                 296
#define VOX_KEY_F8                 297
#define VOX_KEY_F9                 298
#define VOX_KEY_F10                299
#define VOX_KEY_F11                300
#define VOX_KEY_F12                301
#define VOX_KEY_F13                302
#define VOX_KEY_F14                303
#define VOX_KEY_F15                304
#define VOX_KEY_F16                305
#define VOX_KEY_F17                306
#define VOX_KEY_F18                307
#define VOX_KEY_F19                308
#define VOX_KEY_F20                309
#define VOX_KEY_F21                310
#define VOX_KEY_F22                311
#define VOX_KEY_F23                312
#define VOX_KEY_F24                313
#define VOX_KEY_F25                314
#define VOX_KEY_KP_0               320
#define VOX_KEY_KP_1               321
#define VOX_KEY_KP_2               322
#define VOX_KEY_KP_3               323
#define VOX_KEY_KP_4               324
#define VOX_KEY_KP_5               325
#define VOX_KEY_KP_6               326
#define VOX_KEY_KP_7               327
#define VOX_KEY_KP_8               328
#define VOX_KEY_KP_9               329
#define VOX_KEY_KP_DECIMAL         330
#define VOX_KEY_KP_DIVIDE          331
#define VOX_KEY_KP_MULTIPLY        332
#define VOX_KEY_KP_SUBTRACT        333
#define VOX_KEY_KP_ADD             334
#define VOX_KEY_KP_ENTER           335
#define VOX_KEY_KP_EQUAL           336
#define VOX_KEY_LEFT_SHIFT         340
#define VOX_KEY_LEFT_CONTROL       341
#define VOX_KEY_LEFT_ALT           342
#define VOX_KEY_LEFT_SUPER         343
#define VOX_KEY_RIGHT_SHIFT        344
#define VOX_KEY_RIGHT_CONTROL      345
#define VOX_KEY_RIGHT_ALT          346
#define VOX_KEY_RIGHT_SUPER        347
#define VOX_KEY_MENU               348

namespace VoxSmith
{
	enum VOX_SMITH_API KeyStatus : int8_t 
	{
		RELEASE = 0,
		PRESS = 1,
		REPEAT = 2,
	};

	class VOX_SMITH_API Keyboard
	{
	public:
		static Keyboard& getInstance();

		void setKeyStatus(const uint32_t key, KeyStatus status);
		KeyStatus getKeyStatus(const uint32_t key) const;

		bool isKeyActive(const uint32_t key) const;

	private:
		Keyboard();
		~Keyboard();
		
		static Keyboard* s_keyboard;

		KeyStatus m_keys[1024];
	};
}
