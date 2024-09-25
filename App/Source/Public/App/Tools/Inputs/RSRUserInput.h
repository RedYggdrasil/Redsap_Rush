#pragma once



#include <wrl/client.h>
#include <unordered_set>
#include <DirectXMath.h>

#include "MDS/RTool.h"
#include "MDS/Tools/System/RInputKeys.h"


//struct IGameInputReading;
namespace RSRush
{
	class RSRUserInput
	{
		
	protected:
		//Duplicate logic found in DX12 mini engine, higher end data like hold for X second can be implemented in child classes
		bool  m_keyState [2][mds::UT_cast(EMDSInputKeyType::COUNT)];
		float m_axis1D	 [2][mds::UT_cast(EMDSAxis1DType::COUNT)];
		DirectX::XMFLOAT2 m_axis2D[2][mds::UT_cast(EMDSAxis2DType::COUNT)];

	public:
		virtual void OnUserInputStart(const double InDeltaTime);
		virtual void OnUserInputEnd();
		//virtual void HandleGameInputReading(_In_ IGameInputReading* gameInputReading, const double InDeltaTime);
	private:
		//handle 
		friend class RSRGameInput;
		friend class RSRGameInput_XInput;
	};

	class IRSRUserInputProvider
	{
	public:
		virtual RSRUserInput* GetUserInput(const size_t InIndex) const PURE;
		virtual size_t GetUserInputCount() const PURE;
		void NotifyUserInputStart(const double InDeltaTime) const;
		void NotifyUserInputEnd() const;
	};
};