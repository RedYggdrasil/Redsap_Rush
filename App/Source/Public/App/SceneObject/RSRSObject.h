#pragma once
#include "App/Data/Meshes/RSRMesh3D.h"
#include "MDS/Tools/Assets/RAsset.h"
#include "MDS/Tools/RResourceStateType.h"
#include "App/Data/RSRTransform.h"


namespace RSRush
{
	class RSROScene;
	class RSRSObject
	{
	protected:
		bool m_bIsHandledAsSObject;
		bool m_bIsDrawnAsInstance;
		RSRush::RSRTransformMatrix m_mainTransform;
		mds::RAssetAuthority m_mainMeshAuthority;
		RSRush::RSRSharedMesh3DPtr m_mainMesh;
	public:
		inline bool GetIsHandledAsSObject() const { return m_bIsHandledAsSObject; }
		inline bool GetIsDrawnAsInstance() const { return m_bIsDrawnAsInstance; }
		inline const RSRush::RSRTransformMatrix& GetTrsMat() const { return m_mainTransform; }
		RSRSObject(const mds::RAssetAuthority InMainMeshAuthority);
		RSRSObject(const mds::RAssetAuthority InMainMeshAuthority, const bool bInIsHandledAsSObject);
		RSRSObject(const mds::RAssetAuthority InMainMeshAuthority, const bool bInIsHandledAsSObject, const bool bInIsDrawnAsInstance);
		virtual ~RSRSObject();
		/// <summary>
		/// Not executed on main tread, Compute new input and request new position
		/// new application state is to be validated by Physic Pass
		/// </summary>
		virtual bool PrePassTick(const double InGameTime, const double InDeltaTime) { return true; };
		/// <summary>
		/// Read the result of PrepassTick and Physic Pass, and apply them to object before rendering
		/// </summary>
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) { return true; };


		virtual bool UploadResources(struct ID3D12Device10* InDevice, struct ID3D12GraphicsCommandList7* InUploadCommandList);
		virtual bool FreeUploadBuffers();
		virtual bool FreeResourceBuffers();
		virtual bool DrawGeometry(ID3D12GraphicsCommandList7* InDraw3DCommandList) const;
		
		virtual void OnAddedToScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene);
		virtual void OnRemovedFromScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene);
	public:
		inline bool HasMesh() const { return (bool)m_mainMesh; }
		mds::RResourceStateType GetResourceState() const;
	};
};