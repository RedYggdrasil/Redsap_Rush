#include "App/Interfaces/Listeners/IInputListener.h"

static constexpr  DirectX::XMFLOAT2 DEFAULT_AXIS_VALUE = { 0.f, 0.f };
DirectX::XMFLOAT2 RSRush::IInputListener::GetAxis2DSafe(const WPARAM InAxis, bool& bOutExist)  const noexcept
{
	std::unordered_map<WPARAM, DirectX::XMFLOAT2>::const_iterator iter = m_currentAxis.find(InAxis);

	if (iter != m_currentAxis.end())
	{
		bOutExist = true;
		return iter->second;
	}
	bOutExist = false;
	return DEFAULT_AXIS_VALUE;
}

bool RSRush::IInputListener::ReceiveKey(bool bIsDown, WPARAM InKey)
{
	if (bIsDown)
	{
		if (!m_currentlyHeldKeys.contains(InKey))
		{
			m_currentlyHeldKeys.emplace(InKey);
		}
	}
	else
	{
		if (m_currentlyHeldKeys.contains(InKey))
		{
			m_currentlyHeldKeys.erase(InKey);
		}
	}
	return true;
}

bool RSRush::IInputListener::ReceiveAxis2D(WPARAM InAxis, DirectX::XMFLOAT2 InLocation)
{
	if (!m_currentAxis.contains(InAxis))
	{
		m_currentAxis.emplace(InAxis, InLocation);
	}
	else
	{
		m_currentAxis[InAxis] = InLocation;
	}
	return true;
}
