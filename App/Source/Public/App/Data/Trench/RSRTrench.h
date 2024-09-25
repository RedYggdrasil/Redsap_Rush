#pragma once
#include "App/Data/Meshes/RSRMeshLODs.h"
#include "App/Data/Meshes/RSRMesh3D.h"
#include "MDS/Tools/RResourceStateType.h"
#include "App/Data/Trench/RSRTrenchDefs.h"
#include "App/Data/Trench/RSRVoxalGrid.h"
#include "App/Data/Physic/RSRPhysicBody.h"
#include "App/Data/Physic/RSRIPhysicalEntity.h"


#include <vector>

namespace RSRush
{

	class RSRPlayerPath;
	class RSRTrench : public RSRIPhysicalEntity
	{
	protected:
		static UINT64 s_nextInstanceID;

		bool m_bThreadedInitializationCompleted = false;
		std::weak_ptr<RSRush::RSRPlayerPath> m_pPlayerPath;

		double m_startProgression;
		DirectX::XMFLOAT3 m_startLocation;
		DirectX::XMFLOAT3 m_startDirection;

		double m_endProgression;
		DirectX::XMFLOAT3 m_endLocation;
		DirectX::XMFLOAT3 m_endDirection;

		//This exist only to store WIP colliders before registering the object in PhysicManager;
		RSRCollidersBody m_localColliders = RSRCollidersBody(RSRush::WORLD_STATIC);
		//RSRCollidersBodyLWPair m_physicBody = RSRCollidersBodyLWPair(RSRush::WORLD_STATIC);

	protected:
		RSRMeshLODs m_mainMesh;
		RSRush::RSRTransformMatrix m_mainTransform = RSRush::RSRTransformMatrix(mds::TRS_IDENTITY);
		UINT64 m_instanceID;

		bool mb_meshGenerated = false;
		//std::thread m_meshGenerationThread;
	public:
		virtual RSRPhysicBody GeneratePhysicBody();
		inline bool HasMesh() const { return m_mainMesh.LODSize() > 0; }
		inline bool HasMesh(const size_t InLODIndex) const { return m_mainMesh.LODSize() > InLODIndex; }
		mds::RResourceStateType GetResourceState() const { return m_mainMesh.GetMinResourceState(); }
		mds::RResourceStateType GetResourceState(const size_t InLODIndex) const;
		inline UINT64 GetInstanceID() const { return m_instanceID; }

		virtual void StartGeneratingMesh() = 0;
		bool IsGenerationCompleted() const { return m_bThreadedInitializationCompleted; }
		virtual bool UploadResources(struct ID3D12Device10* InDevice, struct ID3D12GraphicsCommandList7* InUploadCommandList);
		virtual bool FreeUploadBuffers();
		virtual bool FreeResourceBuffers();
		virtual bool XM_CALLCONV DrawTrench(DirectX::FXMVECTOR InCameraPosition, ID3D12GraphicsCommandList7* InDrawCommandList);
		/*virtual*/ ~RSRTrench()/*override*/;

		virtual void SetTransform(const RSRush::RSRTransformMatrix& InTransform);
		virtual void SetPositon(const DirectX::XMFLOAT3& InPosition);
		virtual void SetRotation(const DirectX::XMFLOAT3& InRotation);
		virtual void SetScale(const DirectX::XMFLOAT3& InScale);

		double GetStartProgression() const { return m_startProgression; }
		const DirectX::XMFLOAT3& GetStartLocation() const  { return m_startLocation; }
		const DirectX::XMFLOAT3& GetStartDirection() const { return m_startDirection; }

		double GetEndProgression() const { return m_endProgression; }
		const DirectX::XMFLOAT3& GetEndLocation() const  { return m_endLocation; }
		const DirectX::XMFLOAT3& GetEndDirection() const { return m_endDirection; }
		
		void SetStartPositionData(double InStartProgression, const DirectX::XMFLOAT3& InStartLocation, const DirectX::XMFLOAT3& InStartDirection, std::weak_ptr<RSRush::RSRPlayerPath> InPlayerPath);
	protected:
		RSRTrench();
		void SetEndPositionData(double InEndProgression, const DirectX::XMFLOAT3& InEndLocation, const DirectX::XMFLOAT3& InEndDirection);

		virtual void PositionSelf() = 0;
	
	public:
		//Trenches do not move for now, so neither does their colliders
		virtual void OnPhysicalPrePass(double InDeltaTime) override { RSRIPhysicalEntity::OnPhysicalPrePass(InDeltaTime); }
	

		static constexpr RSRVoxelRectangle BASIC_UNDER_VOLUME = 
		{
			.Min = DirectX::XMINT3(-RSRTrenchDefs::HALF_TRENCH_LENGHT_VXL, -RSRTrenchDefs::HALF_TOTAL_WIDTH_VXL, RSRTrenchDefs::UNDER_MESH_VXL   ),
			.Max = DirectX::XMINT3(+RSRTrenchDefs::HALF_TRENCH_LENGHT_VXL, +RSRTrenchDefs::HALF_TOTAL_WIDTH_VXL, RSRTrenchDefs::GROUND_HEIGHT_VXL)
		};

		static constexpr RSRVoxelRectangle BASIC_LEFT_TRENCH =
		{
			.Min = DirectX::XMINT3(-RSRTrenchDefs::HALF_TRENCH_LENGHT_VXL, -RSRTrenchDefs::HALF_TOTAL_WIDTH_VXL , RSRTrenchDefs::GROUND_HEIGHT_VXL),
			.Max = DirectX::XMINT3(+RSRTrenchDefs::HALF_TRENCH_LENGHT_VXL, -RSRTrenchDefs::HALF_TRENCH_WIDTH_VXL, RSRTrenchDefs::ROOF_HEIGHT_VXL  )
		};

		static constexpr RSRVoxelRectangle BASIC_RIGHT_TRENCH =
		{
			.Min = DirectX::XMINT3(-RSRTrenchDefs::HALF_TRENCH_LENGHT_VXL, +RSRTrenchDefs::HALF_TRENCH_WIDTH_VXL, RSRTrenchDefs::GROUND_HEIGHT_VXL),
			.Max = DirectX::XMINT3(+RSRTrenchDefs::HALF_TRENCH_LENGHT_VXL, +RSRTrenchDefs::HALF_TOTAL_WIDTH_VXL , RSRTrenchDefs::ROOF_HEIGHT_VXL  )
		};
	};
}