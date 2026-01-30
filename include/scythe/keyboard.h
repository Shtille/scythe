#ifndef __SCYTHE_KEYBOARD_H__
#define __SCYTHE_KEYBOARD_H__

#include <scythe/platform.h>
#include <scythe/flags.h>

namespace scythe {

	/**
	 * @brief      Defines a keyboard key modifier.
	 */
	enum class KeyboardKeyModifier : int {
		kShift     = 0x01,
		kControl   = 0x02,
		kAlt       = 0x04,
		kSuper     = 0x08,
	};
	
	/**
	 * @brief      This class describes a keyboard key.
	 */
	enum class KeyboardKey : int {
		kUnknown,
		
		k0,
		k1,
		k2,
		k3,
		k4,
		k5,
		k6,
		k7,
		k8,
		k9,
		
		kA,
		kB,
		kC,
		kD,
		kE,
		kF,
		kG,
		kH,
		kI,
		kJ,
		kK,
		kL,
		kM,
		kN,
		kO,
		kP,
		kQ,
		kR,
		kS,
		kT,
		kU,
		kV,
		kW,
		kX,
		kY,
		kZ,
		
		kF1,
		kF2,
		kF3,
		kF4,
		kF5,
		kF6,
		kF7,
		kF8,
		kF9,
		kF10,
		kF11,
		kF12,
		kF13,
		kF14,
		kF15,
		kF16,
		kF17,
		kF18,
		kF19,
		kF20,
		
		kApostrophe,
		kSlash,
		kBackslash,
		kComma,
		kEqual,
		kMinus,
		kPeriod,
		kSemicolon,
		kLeftBracket,
		kRightBracket,
		kGraveAccent,
		kWorld1,
		kWorld2,
		
		kBackspace,
		kCapsLock,
		kInsert,
		kDelete,
		kHome,
		kEnd,
		kEnter,
		kEscape,
		kMenu,
		kNumLock,
		kScrollLock,
		kPause,
		kPageUp,
		kPageDown,
		kSpace,
		kTab,
		
		kUp,
		kDown,
		kLeft,
		kRight,
		
		kLeftAlt,
		kLeftControl,
		kLeftShift,
		kLeftSuper,
		kRightAlt,
		kRightControl,
		kRightShift,
		kRightSuper,
		
		kKp0,
		kKp1,
		kKp2,
		kKp3,
		kKp4,
		kKp5,
		kKp6,
		kKp7,
		kKp8,
		kKp9,
		kKpAdd,
		kKpSubstract,
		kKpMultiply,
		kKpDivide,
		kKpDecimal,
		kKpEqual,
		kKpEnter,
		kPrintScreen,
		
		kCount
	};

	typedef unsigned short KeyCode;
	typedef Flags<KeyboardKeyModifier, int> KeyModifiers;

	/**
	 * @brief      This class describes a keyboard key state.
	 */
	struct alignas(1) KeyboardKeyState
	{
		bool down;		//!< key down flag (key down if true and key up if false)
	};

	/**
	 * @brief      This class describes a keyboard state.
	 */
	struct KeyboardState {
		KeyboardState();

		/// Returns key by its code
		KeyboardKey operator[](KeyCode code) const;
		KeyboardKey GetKeyByCode(KeyCode code) const;

		/// Returns key state by key
		KeyboardKeyState& GetState(KeyboardKey key);

		bool IsKeyDown(KeyboardKey key) const;
		bool IsKeyUp(KeyboardKey key) const;

		void SetKeyDown(KeyboardKey key, bool value);
		void SetKeyUp(KeyboardKey key, bool value);

		/**
		 * @brief      Determines if key has been pressed.
		 *
		 * @param[in]  down_key  The key that becomes pressed
		 * @param[in]  key       The key to check
		 *
		 * @return     True if key pressed, False otherwise.
		 */
		bool IsKeyPressed(KeyboardKey down_key, KeyboardKey key);

		static constexpr KeyCode kTableSize = 
#if defined(SCYTHE_TARGET_WINDOWS)
		512;
#else
		256;
#endif
		static constexpr int kStatesSize = static_cast<int>(KeyboardKey::kCount);

		KeyboardKey table[kTableSize]; 			//!< key code to key table
		KeyModifiers modifiers;					//!< key modifiers bitfield
		KeyboardKeyState states[kStatesSize];	//!< key to key state table
	};

	/**
	 * @brief      Determines whether the specified code is good character.
	 *
	 * @param[in]  code  The code
	 *
	 * @return     True if the specified code is good character, False otherwise.
	 */
	bool IsGoodChar(KeyCode code);

} // namespace scythe

#endif