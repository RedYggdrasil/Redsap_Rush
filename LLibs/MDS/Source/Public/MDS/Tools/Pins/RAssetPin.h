#pragma once

#include "MDS/Tools/RPin.h"
#include "MDS/Tools/Assets/RAsset.h"
#include <memory>

namespace mds
{
	class RAssetPin : public RPin
	{
	protected:
		std::shared_ptr<RAsset> m_asset;
	public:
		std::shared_ptr<RAsset>& Get() { return m_asset; }
		const std::shared_ptr<RAsset>& Get() const { return m_asset; }
	public:
		virtual bool HasResource() override { return m_asset.get(); /*Pointer != 0 bool casting*/ };
		virtual RPin* Clone() override;
		virtual bool Release() override;

		RAssetPin() : RAssetPin(nullptr) {};
		RAssetPin(RAsset* InAsset);

		RAssetPin(const RAssetPin& InOther);
		RAssetPin(RAssetPin&& RVOther) noexcept;
		RAssetPin& operator=(const RAssetPin& InOther);
		RAssetPin& operator=(RAssetPin&& RVOther) noexcept;

		/*virtual*/~RAssetPin() /*override*/;
	private:
		//Release ThisClass-level Resources
		bool InternalRelease();
	};
};