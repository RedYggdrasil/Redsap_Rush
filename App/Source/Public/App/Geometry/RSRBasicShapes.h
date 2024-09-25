#pragma once

#include "App/Data/Shaders/ShaderStructures.h"
#include "MDS/Tools/Templates/Singleton.h"
#include "App/Data/Meshes/RSRMesh3D.h"
#include "App/Data/Meshes/RSRMesh2D.h"
#include <string>
#include <string_view>
namespace RSRush
{
	class RSRBasicShapes : public mds::Singleton<RSRBasicShapes>
	{
	public:
		static const std::string DEFAULT_SQUARE_NAME;
		static const std::string DEFAULT_PLANE_NAME;
		static const std::string DEFAULT_SPHERE_NAME;

		static const std::string DEFAULT_PLANE_NAME_2D;
		static const std::string DEFAULT_TRIANGLE_NAME_2D;
	protected:
		RSRush::RSRSharedMesh3DPtr m_defaultSquare = nullptr;
		RSRush::RSRSharedMesh3DPtr m_defaultPlane = nullptr;
		RSRush::RSRSharedMesh3DPtr m_defaultSphere = nullptr;
		RSRush::RSRSharedMesh2DPtr m_defaultPlane2D = nullptr;
		RSRush::RSRSharedMesh2DPtr m_defaultTriangle2D = nullptr;

	public:
		bool Init(class RSRAssetManager* InAssetManager);
		bool UploadResources(struct ID3D12Device10* InDevice, struct ID3D12GraphicsCommandList7* InUploadCommandList);
		bool FreeUploadBuffers();
		void Shutdown();
public:
	RSRBasicShapes(const RSRBasicShapes&) = delete;
	RSRBasicShapes& operator=(const RSRBasicShapes&) = delete;

	public:
		const RSRush::RSRSharedMesh3DPtr& GetDefSquare() const { return m_defaultSquare; };
		const RSRush::RSRSharedMesh3DPtr& GetDefPlane() const { return m_defaultPlane; };
		const RSRush::RSRSharedMesh3DPtr& GetDefSphere() const { return m_defaultSphere; };
		const RSRush::RSRSharedMesh2DPtr& GetDefPlane2D() const { return m_defaultPlane2D; };
		const RSRush::RSRSharedMesh2DPtr& GetDefTriangle2D() const { return m_defaultTriangle2D; };
	public:
	RSRush::RSRSharedMesh3DPtr GetRegisterNewSquare(const std::string_view InShapeName, const DirectX::XMFLOAT3& InVertexColor, RSRAssetManager* InAssetManager = nullptr);
	RSRush::RSRSharedMesh3DPtr GetRegisterNewPlane(const std::string_view InShapeName, const DirectX::XMFLOAT3& InVertexColor, RSRAssetManager* InAssetManager = nullptr);

	RSRush::RSRSharedMesh2DPtr GetRegisterNewPlane2D(const std::string_view InShapeName, const DirectX::XMFLOAT4& InVertexColor, const DirectX::XMFLOAT2& Pivot = { 0.f, 0.f }, const DirectX::XMFLOAT2& InTopLeft = { -1.f, -1.f }, const DirectX::XMFLOAT2& InBottomRight = { 1.f, 1.f }, uint32_t InTextureIndex = 0, RSRAssetManager* InAssetManager = nullptr);
	RSRush::RSRSharedMesh2DPtr GetRegisterNewTriangle2D(const std::string_view InShapeName, const DirectX::XMFLOAT4& InVertexColor, const DirectX::XMFLOAT2& Pivot = { 0.f, 0.f }, const DirectX::XMFLOAT2& InTopLeft = { -1.f, -1.f }, const DirectX::XMFLOAT2& InBottomRight = { 1.f, 1.f }, uint32_t InTextureIndex = 0, RSRAssetManager* InAssetManager = nullptr);

	RSRush::RSRSharedMesh3DPtr GetRegisterNewSphere(const std::string_view InShapeName, const DirectX::XMFLOAT3& InVertexColor, uint32_t InTextureIndex, RSRAssetManager* InAssetManager = nullptr);
	protected:
	RSRBasicShapes();
	public:
		friend class mds::Singleton<RSRBasicShapes>;
	};
}