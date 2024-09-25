#pragma once
#include "App/Data/Physic/RSRIPhysicalEntity.h"

#include <DirectXMath.h>


namespace RSRush
{

	class RSRPPConnection : public RSRIPhysicalEntity
	{
	public:
		static const RSRPPConnection INVALID_CONNECTION;
	public :
		uint16_t NextNodeIndex;

		
		RSRPPConnection();
		RSRPPConnection(const RSRPPConnection& InOther) = delete;
		RSRPPConnection& operator=(const RSRPPConnection& InOther) = delete;
	public:
		bool IsValid() const;


	public:
		//Trenches do not move for now, so neither does their colliders
		virtual void OnPhysicalPrePass(double InDeltaTime) override { RSRIPhysicalEntity::OnPhysicalPrePass(InDeltaTime); }

	private : 
		static std::shared_ptr<RSRPPConnection> Create(class RSRPlayerPath& OwningPath, const uint16_t InNextNodeIndex, const DirectX::XMFLOAT3& InStartLocation, const DirectX::XMFLOAT3& InEndLocation);
		static void Clean(std::shared_ptr<RSRPPConnection> InConnection);
		friend class RSRPlayerPath;
	};
};