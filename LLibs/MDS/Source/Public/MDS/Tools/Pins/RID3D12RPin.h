#pragma once
#include "MDS/Tools/RPin.h"
#include <wrl/client.h>
#include <d3d12.h>

namespace mds
{
	class RID3D12RPin : public RPin
	{
	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource2> m_d3d12Resource;
	public:
		Microsoft::WRL::ComPtr<ID3D12Resource2>& Get() { return m_d3d12Resource; }
		const Microsoft::WRL::ComPtr<ID3D12Resource2>& Get() const { return m_d3d12Resource; }
	public:
		virtual bool HasResource() override { return m_d3d12Resource.Get(); /*Pointer != 0 bool casting*/ };
		virtual RPin* Clone() override;
		virtual bool Release() override;

		RID3D12RPin() : RID3D12RPin(nullptr) {};
		RID3D12RPin(ID3D12Resource2* InResource);

		RID3D12RPin(const RID3D12RPin& InOther);
		RID3D12RPin(RID3D12RPin&& RVOther) noexcept;
		RID3D12RPin& operator=(const RID3D12RPin& InOther);
		RID3D12RPin& operator=(RID3D12RPin&& RVOther) noexcept;

		/*virtual*/~RID3D12RPin() /*override*/;
	private:
		//Release ThisClass-level Resources
		bool InternalRelease();
	};
};