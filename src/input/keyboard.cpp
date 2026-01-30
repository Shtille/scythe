#include <scythe/keyboard.h>

#include <cstring>

namespace scythe {

	static void FillTable(KeyboardKey table[])
	{
		std::memset(table, (int)KeyboardKey::kUnknown, KeyboardState::kTableSize * sizeof(KeyboardKey));
#if defined(SCYTHE_TARGET_WINDOWS)
		table[0x00B] = KeyboardKey::k0;
		table[0x002] = KeyboardKey::k1;
		table[0x003] = KeyboardKey::k2;
		table[0x004] = KeyboardKey::k3;
		table[0x005] = KeyboardKey::k4;
		table[0x006] = KeyboardKey::k5;
		table[0x007] = KeyboardKey::k6;
		table[0x008] = KeyboardKey::k7;
		table[0x009] = KeyboardKey::k8;
		table[0x00A] = KeyboardKey::k9;
		table[0x01E] = KeyboardKey::kA;
		table[0x030] = KeyboardKey::kB;
		table[0x02E] = KeyboardKey::kC;
		table[0x020] = KeyboardKey::kD;
		table[0x012] = KeyboardKey::kE;
		table[0x021] = KeyboardKey::kF;
		table[0x022] = KeyboardKey::kG;
		table[0x023] = KeyboardKey::kH;
		table[0x017] = KeyboardKey::kI;
		table[0x024] = KeyboardKey::kJ;
		table[0x025] = KeyboardKey::kK;
		table[0x026] = KeyboardKey::kL;
		table[0x032] = KeyboardKey::kM;
		table[0x031] = KeyboardKey::kN;
		table[0x018] = KeyboardKey::kO;
		table[0x019] = KeyboardKey::kP;
		table[0x010] = KeyboardKey::kQ;
		table[0x013] = KeyboardKey::kR;
		table[0x01F] = KeyboardKey::kS;
		table[0x014] = KeyboardKey::kT;
		table[0x016] = KeyboardKey::kU;
		table[0x02F] = KeyboardKey::kV;
		table[0x011] = KeyboardKey::kW;
		table[0x02D] = KeyboardKey::kX;
		table[0x015] = KeyboardKey::kY;
		table[0x02C] = KeyboardKey::kZ;

		table[0x028] = KeyboardKey::kApostrophe;
		table[0x02B] = KeyboardKey::kBackslash;
		table[0x033] = KeyboardKey::kComma;
		table[0x00D] = KeyboardKey::kEqual;
		table[0x029] = KeyboardKey::kGraveAccent;
		table[0x01A] = KeyboardKey::kLeftBracket;
		table[0x00C] = KeyboardKey::kMinus;
		table[0x034] = KeyboardKey::kPeriod;
		table[0x01B] = KeyboardKey::kRightBracket;
		table[0x027] = KeyboardKey::kSemicolon;
		table[0x035] = KeyboardKey::kSlash;
		table[0x056] = KeyboardKey::kWorld2;

		table[0x00E] = KeyboardKey::kBackspace;
		table[0x153] = KeyboardKey::kDelete;
		table[0x14F] = KeyboardKey::kEnd;
		table[0x01C] = KeyboardKey::kEnter;
		table[0x001] = KeyboardKey::kEscape;
		table[0x147] = KeyboardKey::kHome;
		table[0x152] = KeyboardKey::kInsert;
		table[0x15D] = KeyboardKey::kMenu;
		table[0x151] = KeyboardKey::kPageDown;
		table[0x149] = KeyboardKey::kPageUp;
		table[0x045] = KeyboardKey::kPause;
		table[0x039] = KeyboardKey::kSpace;
		table[0x00F] = KeyboardKey::kTab;
		table[0x03A] = KeyboardKey::kCapsLock;
		table[0x145] = KeyboardKey::kNumLock;
		table[0x046] = KeyboardKey::kScrollLock;
		table[0x03B] = KeyboardKey::kF1;
		table[0x03C] = KeyboardKey::kF2;
		table[0x03D] = KeyboardKey::kF3;
		table[0x03E] = KeyboardKey::kF4;
		table[0x03F] = KeyboardKey::kF5;
		table[0x040] = KeyboardKey::kF6;
		table[0x041] = KeyboardKey::kF7;
		table[0x042] = KeyboardKey::kF8;
		table[0x043] = KeyboardKey::kF9;
		table[0x044] = KeyboardKey::kF10;
		table[0x057] = KeyboardKey::kF11;
		table[0x058] = KeyboardKey::kF12;
		table[0x064] = KeyboardKey::kF13;
		table[0x065] = KeyboardKey::kF14;
		table[0x066] = KeyboardKey::kF15;
		table[0x067] = KeyboardKey::kF16;
		table[0x068] = KeyboardKey::kF17;
		table[0x069] = KeyboardKey::kF18;
		table[0x06A] = KeyboardKey::kF19;
		table[0x06B] = KeyboardKey::kF20;
	//     table[0x06C] = KeyboardKey::kF21;
	//     table[0x06D] = KeyboardKey::kF22;
	//     table[0x06E] = KeyboardKey::kF23;
	//     table[0x076] = KeyboardKey::kF24;
		table[0x038] = KeyboardKey::kLeftAlt;
		table[0x01D] = KeyboardKey::kLeftControl;
		table[0x02A] = KeyboardKey::kLeftShift;
		table[0x15B] = KeyboardKey::kLeftSuper;
		table[0x137] = KeyboardKey::kPrintScreen;
		table[0x138] = KeyboardKey::kRightAlt;
		table[0x11D] = KeyboardKey::kRightControl;
		table[0x036] = KeyboardKey::kRightShift;
		table[0x15C] = KeyboardKey::kRightSuper;
		table[0x150] = KeyboardKey::kDown;
		table[0x14B] = KeyboardKey::kLeft;
		table[0x14D] = KeyboardKey::kRight;
		table[0x148] = KeyboardKey::kUp;

		table[0x052] = KeyboardKey::kKp0;
		table[0x04F] = KeyboardKey::kKp1;
		table[0x050] = KeyboardKey::kKp2;
		table[0x051] = KeyboardKey::kKp3;
		table[0x04B] = KeyboardKey::kKp4;
		table[0x04C] = KeyboardKey::kKp5;
		table[0x04D] = KeyboardKey::kKp6;
		table[0x047] = KeyboardKey::kKp7;
		table[0x048] = KeyboardKey::kKp8;
		table[0x049] = KeyboardKey::kKp9;
		table[0x04E] = KeyboardKey::kKpAdd;
		table[0x053] = KeyboardKey::kKpDecimal;
		table[0x135] = KeyboardKey::kKpDivide;
		table[0x11C] = KeyboardKey::kKpEnter;
		table[0x037] = KeyboardKey::kKpMultiply;
		table[0x04A] = KeyboardKey::kKpSubstract;
#elif defined(SCYTHE_TARGET_UNIX)

		// TODO:

#elif defined(SCYTHE_TARGET_MAC)
		table[0x1D] = KeyboardKey::k0;
		table[0x12] = KeyboardKey::k1;
		table[0x13] = KeyboardKey::k2;
		table[0x14] = KeyboardKey::k3;
		table[0x15] = KeyboardKey::k4;
		table[0x17] = KeyboardKey::k5;
		table[0x16] = KeyboardKey::k6;
		table[0x1A] = KeyboardKey::k7;
		table[0x1C] = KeyboardKey::k8;
		table[0x19] = KeyboardKey::k9;
		table[0x00] = KeyboardKey::kA;
		table[0x0B] = KeyboardKey::kB;
		table[0x08] = KeyboardKey::kC;
		table[0x02] = KeyboardKey::kD;
		table[0x0E] = KeyboardKey::kE;
		table[0x03] = KeyboardKey::kF;
		table[0x05] = KeyboardKey::kG;
		table[0x04] = KeyboardKey::kH;
		table[0x22] = KeyboardKey::kI;
		table[0x26] = KeyboardKey::kJ;
		table[0x28] = KeyboardKey::kK;
		table[0x25] = KeyboardKey::kL;
		table[0x2E] = KeyboardKey::kM;
		table[0x2D] = KeyboardKey::kN;
		table[0x1F] = KeyboardKey::kO;
		table[0x23] = KeyboardKey::kP;
		table[0x0C] = KeyboardKey::kQ;
		table[0x0F] = KeyboardKey::kR;
		table[0x01] = KeyboardKey::kS;
		table[0x11] = KeyboardKey::kT;
		table[0x20] = KeyboardKey::kU;
		table[0x09] = KeyboardKey::kV;
		table[0x0D] = KeyboardKey::kW;
		table[0x07] = KeyboardKey::kX;
		table[0x10] = KeyboardKey::kY;
		table[0x06] = KeyboardKey::kZ;
		
		table[0x27] = KeyboardKey::kApostrophe;
		table[0x2A] = KeyboardKey::kBackslash;
		table[0x2B] = KeyboardKey::kComma;
		table[0x18] = KeyboardKey::kEqual;
		table[0x32] = KeyboardKey::kGraveAccent;
		table[0x21] = KeyboardKey::kLeftBracket;
		table[0x1B] = KeyboardKey::kMinus;
		table[0x2F] = KeyboardKey::kPeriod;
		table[0x1E] = KeyboardKey::kRightBracket;
		table[0x29] = KeyboardKey::kSemicolon;
		table[0x2C] = KeyboardKey::kSlash;
		table[0x0A] = KeyboardKey::kWorld1;
		
		table[0x33] = KeyboardKey::kBackspace;
		table[0x39] = KeyboardKey::kCapsLock;
		table[0x75] = KeyboardKey::kDelete;
		table[0x7D] = KeyboardKey::kDown;
		table[0x77] = KeyboardKey::kEnd;
		table[0x24] = KeyboardKey::kEnter;
		table[0x35] = KeyboardKey::kEscape;
		table[0x7A] = KeyboardKey::kF1;
		table[0x78] = KeyboardKey::kF2;
		table[0x63] = KeyboardKey::kF3;
		table[0x76] = KeyboardKey::kF4;
		table[0x60] = KeyboardKey::kF5;
		table[0x61] = KeyboardKey::kF6;
		table[0x62] = KeyboardKey::kF7;
		table[0x64] = KeyboardKey::kF8;
		table[0x65] = KeyboardKey::kF9;
		table[0x6D] = KeyboardKey::kF10;
		table[0x67] = KeyboardKey::kF11;
		table[0x6F] = KeyboardKey::kF12;
		table[0x69] = KeyboardKey::kF13;
		table[0x6B] = KeyboardKey::kF14;
		table[0x71] = KeyboardKey::kF15;
		table[0x6A] = KeyboardKey::kF16;
		table[0x40] = KeyboardKey::kF17;
		table[0x4F] = KeyboardKey::kF18;
		table[0x50] = KeyboardKey::kF19;
		table[0x5A] = KeyboardKey::kF20;
		table[0x73] = KeyboardKey::kHome;
		table[0x72] = KeyboardKey::kInsert;
		table[0x7B] = KeyboardKey::kLeft;
		table[0x3A] = KeyboardKey::kLeftAlt;
		table[0x3B] = KeyboardKey::kLeftControl;
		table[0x38] = KeyboardKey::kLeftShift;
		table[0x37] = KeyboardKey::kLeftSuper;
		table[0x6E] = KeyboardKey::kMenu;
		table[0x47] = KeyboardKey::kNumLock;
		table[0x79] = KeyboardKey::kPageDown;
		table[0x74] = KeyboardKey::kPageUp;
		table[0x7C] = KeyboardKey::kRight;
		table[0x3D] = KeyboardKey::kRightAlt;
		table[0x3E] = KeyboardKey::kRightControl;
		table[0x3C] = KeyboardKey::kRightShift;
		table[0x36] = KeyboardKey::kRightSuper;
		table[0x31] = KeyboardKey::kSpace;
		table[0x30] = KeyboardKey::kTab;
		table[0x7E] = KeyboardKey::kUp;
		
		table[0x52] = KeyboardKey::kKp0;
		table[0x53] = KeyboardKey::kKp1;
		table[0x54] = KeyboardKey::kKp2;
		table[0x55] = KeyboardKey::kKp3;
		table[0x56] = KeyboardKey::kKp4;
		table[0x57] = KeyboardKey::kKp5;
		table[0x58] = KeyboardKey::kKp6;
		table[0x59] = KeyboardKey::kKp7;
		table[0x5B] = KeyboardKey::kKp8;
		table[0x5C] = KeyboardKey::kKp9;
		table[0x45] = KeyboardKey::kKpAdd;
		table[0x41] = KeyboardKey::kKpDecimal;
		table[0x4B] = KeyboardKey::kKpDivide;
		table[0x4C] = KeyboardKey::kKpEnter;
		table[0x51] = KeyboardKey::kKpEqual;
		table[0x43] = KeyboardKey::kKpMultiply;
		table[0x4E] = KeyboardKey::kKpSubstract;
#else
		static_assert(false, "KeyboardKey::Fill not implemented");
#endif
	}

	KeyboardState::KeyboardState()
	{
		FillTable(table);
		// Fill states table
		std::memset(states, 0, kStatesSize * sizeof(KeyboardKeyState));
	}
	KeyboardKey KeyboardState::operator[](KeyCode code) const
	{
		return GetKeyByCode(code);
	}
	KeyboardKey KeyboardState::GetKeyByCode(KeyCode code) const
	{
#ifdef SCYTHE_DEBUG
		if (code >= kTableSize)
			return KeyboardKey::kUnknown;
#endif
		return table[code];
	}
	KeyboardKeyState& KeyboardState::GetState(KeyboardKey key)
	{
		return states[static_cast<int>(key)];
	}
	bool KeyboardState::IsKeyDown(KeyboardKey key) const
	{
		return states[static_cast<int>(key)].down;
	}
	bool KeyboardState::IsKeyUp(KeyboardKey key) const
	{
		return !IsKeyDown(key);
	}
	void KeyboardState::SetKeyDown(KeyboardKey key, bool value)
	{
		states[static_cast<int>(key)].down = value;
	}
	void KeyboardState::SetKeyUp(KeyboardKey key, bool value)
	{
		states[static_cast<int>(key)].down = !value;
	}
	bool KeyboardState::IsKeyPressed(KeyboardKey down_key, KeyboardKey key)
	{
		return down_key == key && !IsKeyDown(key);
	}

	bool IsGoodChar(KeyCode code)
	{
		return (code >= 32 && code <= 126) || code >= 160;
	}

} // namespace scythe