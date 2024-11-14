#pragma once

#define NOMINMAX
#include "MDS/Tools/REnum.h"
#include "MDS/Tools/System/IRProgramMemNode.h"



#include <GameInput.h>
#include <wrl/client.h>

#include <unordered_set>
#include <unordered_map>
struct IGameInput;
namespace RSRush
{
	class RSRProgramInstance;
	namespace NSGameInput
	{
		class RSRXInputOverrideData
		{

		};
		enum class RSRDeviceHandleOverrideType : uint32_t
		{
			NO_OVERRIDE = 0,
			XINPUT = 1,
			RENUM_COUNT
		};
		struct RSRDeviceOverrideData
		{
			RSRDeviceHandleOverrideType Type = RSRDeviceHandleOverrideType::NO_OVERRIDE;
			union
			{
				RSRXInputOverrideData* XInputOverride = nullptr;
			};
		public:
			constexpr bool IsGameInput() const { return this->Type == RSRDeviceHandleOverrideType::NO_OVERRIDE; }
		};
		// Hash function for APP_LOCAL_DEVICE_ID
		struct APP_LOCAL_DEVICE_ID_Hash { std::size_t operator()(const APP_LOCAL_DEVICE_ID& id) const; };
		// Equality comparison function for APP_LOCAL_DEVICE_ID
		struct APP_LOCAL_DEVICE_ID_Equal { inline bool operator()(const APP_LOCAL_DEVICE_ID& id1, const APP_LOCAL_DEVICE_ID& id2) const { /*Bit compare*/ return std::memcmp(id1.value, id2.value, sizeof(APP_LOCAL_DEVICE_ID)) == 0; } };
		typedef uint8_t RSR_USER_ID;
		static constexpr RSR_USER_ID USER_ID_PLAYER_0 = 0ui8;
		static constexpr RSR_USER_ID USER_ID_NONE = UINT8_MAX;

		struct RSRUserDeviceInfos
		{
		public :
			Microsoft::WRL::ComPtr<IGameInputDevice> Device;
			uint8_t UserIndex = USER_ID_NONE;
			RSRDeviceOverrideData DeviceOverride = RSRDeviceOverrideData();
		};
	}
	class RSRGameInput : public mds::IRProgramMemElem
	{
	private:
		class IRSRUserInputProvider* m_userProvider = nullptr;
	private:
		static uintptr_t ms_nextInputcallbackLookupKey;
		static std::unordered_map<GameInputCallbackToken, RSRGameInput*> sm_dxGameInputLookup;

		Microsoft::WRL::ComPtr<IGameInput> m_GameInput;
		uintptr_t m_inputcallbackLookupKey;
		GameInputCallbackToken m_inputCallbackToken;

		bool m_bGameInputAvalable = false;

		std::unordered_set<WPARAM> m_currentlyHeldKeys;
		//std::unordered_map<WPARAM, DirectX::XMFLOAT2> m_currentAxis;
		std::unordered_map<APP_LOCAL_DEVICE_ID, NSGameInput::RSRUserDeviceInfos, NSGameInput::APP_LOCAL_DEVICE_ID_Hash, NSGameInput::APP_LOCAL_DEVICE_ID_Equal> m_devices;
	
	public:
		RSRGameInput(const RSRGameInput&) = delete;
		RSRGameInput& operator=(const RSRGameInput&) = delete;
	public:
		bool Init(RSRProgramInstance* InProgramInstance, IRSRUserInputProvider* InUserProvider = nullptr);
		inline void SetUserProvider(IRSRUserInputProvider* InUserProvider) { m_userProvider = InUserProvider; };
		void Shutdown();
		void Update(const double InDeltaTime);

		static void CALLBACK OnDeviceEnumerated(
			_In_ GameInputCallbackToken callbackToken,
			_In_ void* context,
			_In_ IGameInputDevice* device,
			_In_ uint64_t timestamp,
			_In_ GameInputDeviceStatus currentStatus,
			_In_ GameInputDeviceStatus previousStatus
		);
	public:
		RSRGameInput();
	protected:
		bool HandleGameInputReading(IGameInputReading* InGameInputReading, class RSRUserInput* InUserInput, const double InDeltaTime);

		bool HandleGameInputMouse(IGameInputReading* InGameInputReading, RSRUserInput* InUserInput, const double InDeltaTime);
		bool HandleGameInputKeyboard(IGameInputReading* InGameInputReading, RSRUserInput* InUserInput, const double InDeltaTime);
		bool HandleGameInputGamePad(IGameInputReading* InGameInputReading, RSRUserInput* InUserInput, const double InDeltaTime, bool& bOutHasInput);
	
		bool HandleInputGameInputByWindowEvent(RSRUserInput* InUserInput);
	};
};