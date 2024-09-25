#pragma once

#define NOMINMAX
#include <wtypes.h>
#include <unordered_set>
#include <unordered_map>
#include <DirectXMath.h>
namespace RSRush
{
	class IInputListener
	{
	protected:
		std::unordered_set<WPARAM> m_currentlyHeldKeys;
		std::unordered_map<WPARAM, DirectX::XMFLOAT2> m_currentAxis;
		DirectX::XMFLOAT2 GetAxis2DSafe(const WPARAM InAxis, bool& bOutExist) const noexcept;
		inline DirectX::XMFLOAT2 GetAxis2DSafe(const WPARAM InAxis) const noexcept { bool _; return GetAxis2DSafe(InAxis, _); }
	public:
		virtual bool ReceiveKey(bool bIsDown, WPARAM InKey);
		virtual bool ReceiveAxis2D(WPARAM InAxis, DirectX::XMFLOAT2 InLocation);
	};
}