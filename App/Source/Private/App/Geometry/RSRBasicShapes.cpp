#include "App/Geometry/RSRBasicShapes.h"
#include "App/Data/Meshes/RSRMesh3D.h"
#include "App/Managers/RSRAssetManager.h"
#include "MDS/Tools/RCoordinate.h"
#include "App/Tools/RSRLog.h"

using namespace DirectX;
using namespace RSRush;
static XMFLOAT3 FACE_COLOR_DEFAULT = XMFLOAT3(1.f, 1.f, 1.f);
static XMFLOAT4 FACE_COLOR_DEFAULT_ALPHA = XMFLOAT4(1.f, 1.f, 1.f, 1.f);

//.rsr is a Dot Red Sap Realtime (as oppose to read from disk (.rs Red Sap ?) )
const std::string RSRBasicShapes::DEFAULT_SQUARE_NAME = TEXT("SquareMeshDefault.rsrmesh");
const std::string RSRBasicShapes::DEFAULT_PLANE_NAME = TEXT("FaceMeshDefault.rsrmesh");
const std::string RSRBasicShapes::DEFAULT_SPHERE_NAME = TEXT("SphereMeshDefault.rsrmesh");
const std::string RSRBasicShapes::DEFAULT_PLANE_NAME_2D = TEXT("FaceMeshDefault2D.rsrmesh");
const std::string RSRBasicShapes::DEFAULT_TRIANGLE_NAME_2D = TEXT("TriangleMeshDefault2D.rsrmesh");

bool RSRBasicShapes::Init(RSRAssetManager* InAssetManager)
{
    if (!InAssetManager)
    {
        RSRLog::LogError(TEXT("Cannot initialize BasicShapes without a valid RSRAssetManager Ptr !"));
        return false;
    }
    bool allSucess = true;
    m_defaultSquare = GetRegisterNewSquare(DEFAULT_SQUARE_NAME, FACE_COLOR_DEFAULT, 0Ui32, InAssetManager);
    if (!m_defaultSquare) 
    {
        RSRLog::LogError(TEXT("Cannot initialize default Square BasicShapes !"));
        return false; 
    }
    m_defaultPlane = GetRegisterNewPlane(DEFAULT_PLANE_NAME, FACE_COLOR_DEFAULT, 0Ui32, InAssetManager);
    if (!m_defaultPlane)
    {
        RSRLog::LogError(TEXT("Cannot initialize default Plane BasicShapes !"));
        return false;
    }
    m_defaultSphere = GetRegisterNewSphere(DEFAULT_SPHERE_NAME, FACE_COLOR_DEFAULT, 0, InAssetManager);
    if (!m_defaultSphere)
    {
        RSRLog::LogError(TEXT("Cannot initialize default Sphere BasicShapes !"));
        return false;
    }
    m_defaultPlane2D = GetRegisterNewPlane2D(DEFAULT_PLANE_NAME_2D, FACE_COLOR_DEFAULT_ALPHA);
    if (!m_defaultPlane2D)
    {
        RSRLog::LogError(TEXT("Cannot initialize default Plane2D BasicShapes !"));
        return false;
    }
    m_defaultTriangle2D = GetRegisterNewTriangle2D(DEFAULT_TRIANGLE_NAME_2D, FACE_COLOR_DEFAULT_ALPHA);
    if (!m_defaultTriangle2D)
    {
        RSRLog::LogError(TEXT("Cannot initialize default Triangle2D BasicShapes !"));
        return false;
    }
	return true;
}

bool RSRBasicShapes::UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
    bool allSucessfull = true;
    allSucessfull = m_defaultSquare->UploadResources(InDevice, InUploadCommandList) && allSucessfull;
    allSucessfull = m_defaultPlane->UploadResources(InDevice, InUploadCommandList) && allSucessfull;
    allSucessfull = m_defaultSphere->UploadResources(InDevice, InUploadCommandList) && allSucessfull;
    allSucessfull = m_defaultPlane2D->UploadResources(InDevice, InUploadCommandList) && allSucessfull;
    allSucessfull = m_defaultTriangle2D->UploadResources(InDevice, InUploadCommandList) && allSucessfull;
    return allSucessfull;
}

bool RSRBasicShapes::FreeUploadBuffers()
{
    bool allSucessfull = true;
    allSucessfull = m_defaultSquare->FreeUploadBuffer() && allSucessfull;
    allSucessfull = m_defaultPlane->FreeUploadBuffer() && allSucessfull;
    allSucessfull = m_defaultSphere->FreeUploadBuffer() && allSucessfull;
    allSucessfull = m_defaultPlane2D->FreeUploadBuffer() && allSucessfull;
    allSucessfull = m_defaultTriangle2D->FreeUploadBuffer() && allSucessfull;
    return allSucessfull;
}

void RSRBasicShapes::Shutdown()
{
    if (m_defaultTriangle2D) { m_defaultTriangle2D->FreeAllBuffers(); }
    if (m_defaultPlane2D) { m_defaultPlane2D->FreeAllBuffers(); }
    if (m_defaultSphere) { m_defaultSphere->FreeAllBuffers(); }
    if (m_defaultPlane) { m_defaultPlane->FreeAllBuffers(); }
    if (m_defaultSquare) { m_defaultSquare->FreeAllBuffers(); }
    m_defaultTriangle2D.reset();
    m_defaultPlane2D.reset();
    m_defaultSphere.reset();
    m_defaultPlane.reset();
    m_defaultSquare.reset();
}

RSRSharedMesh3DPtr RSRBasicShapes::GetRegisterNewSquare(const std::string_view InShapeName, const XMFLOAT3& InVertexColor, uint32_t InTextureIndex, RSRAssetManager* InAssetManager)
{
    if (!InAssetManager)
    {
        InAssetManager = &RSRAssetManager::Get();
    }
    return
        InAssetManager->AddAsset<RSRMesh3D>
        (
            mds::NameDynamicAsset(mds::RAssetType::Mesh, InShapeName),
            false,
            std::vector<VertexPositionUVColor>
    {
        // Front face
        { .pos = XFP3(0.5f, -0.5f, 0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, 0.5f, 0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, -0.5f, 0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, 0.5f, 0.5f),    .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, 0.5f, 0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, -0.5f, 0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 1.f), .textureIndex = InTextureIndex },
            // Back face
        { .pos = XFP3(-0.5f, 0.5f, -0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, -0.5f, -0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, -0.5f, -0.5f), .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, 0.5f, -0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, -0.5f, -0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, 0.5f, -0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 0.f), .textureIndex = InTextureIndex },
            // Left face
        { .pos = XFP3(-0.5f, -0.5f, 0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, 0.5f, -0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, -0.5f, -0.5f), .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, 0.5f, 0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, 0.5f, -0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, -0.5f, 0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 1.f), .textureIndex = InTextureIndex },
            // Right face
        { .pos = XFP3(0.5f, 0.5f, -0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, -0.5f, 0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, -0.5f, -0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, 0.5f, 0.5f),    .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, -0.5f, 0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, 0.5f, -0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 0.f), .textureIndex = InTextureIndex },
            // Top face
        { .pos = XFP3(-0.5f, 0.5f, 0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, 0.5f, -0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, 0.5f, -0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, 0.5f, 0.5f),    .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, 0.5f, -0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, 0.5f, 0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 0.f), .textureIndex = InTextureIndex },
            // Bottom face
        { .pos = XFP3(0.5f, -0.5f, -0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, -0.5f, 0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, -0.5f, -0.5f), .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 0.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, -0.5f, 0.5f),   .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(-0.5f, -0.5f, 0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(0.f, 1.f), .textureIndex = InTextureIndex },
        { .pos = XFP3(0.5f, -0.5f, -0.5f),  .color = InVertexColor, .uv0 = XMFLOAT2(1.f, 0.f), .textureIndex = InTextureIndex }
    }
    //std::vector<unsigned short>{ //Triangle index
    //    0, 1, 2,
    //    3, 4, 5
    //}
    );
}

RSRSharedMesh3DPtr RSRBasicShapes::GetRegisterNewPlane(const std::string_view InShapeName, const XMFLOAT3& InVertexColor, uint32_t InTextureIndex, RSRAssetManager* InAssetManager)
{
    return GetRegisterNewPlane(InShapeName, InVertexColor, InTextureIndex, XMFLOAT2{ 0.f, 0.f }, XMFLOAT2{ 1.f, 1.f }, InAssetManager);
}

RSRush::RSRSharedMesh3DPtr RSRush::RSRBasicShapes::GetRegisterNewPlane(const std::string_view InShapeName, const DirectX::XMFLOAT3& InVertexColor, uint32_t InTextureIndex, const DirectX::XMFLOAT2& InUVScale, RSRAssetManager* InAssetManager)
{
     return GetRegisterNewPlane(InShapeName, InVertexColor, InTextureIndex, XMFLOAT2{ 0.f, 0.f }, InUVScale, InAssetManager);
}

RSRush::RSRSharedMesh3DPtr RSRush::RSRBasicShapes::GetRegisterNewPlane(const std::string_view InShapeName, const DirectX::XMFLOAT3& InVertexColor, uint32_t InTextureIndex, const DirectX::XMFLOAT2& InMinUV, const DirectX::XMFLOAT2& InMaxUV, RSRAssetManager* InAssetManager)
{
    return RSRAssetManager::Get().AddAsset<RSRMesh3D>
        (
            mds::NameDynamicAsset(mds::RAssetType::Mesh, InShapeName),
            false,
            std::vector<VertexPositionUVColor>
    {
        //First triangle
        {
            .pos = XMFLOAT3(-0.5f, -0.5f, 0.f),
                .color = InVertexColor,
                .uv0 = XMFLOAT2(InMinUV.x, InMaxUV.y),
                .textureIndex = InTextureIndex
        },
                {
                    .pos = XMFLOAT3(0.5f, -0.5f, 0.f),
                    .color = InVertexColor,
                    .uv0 = XMFLOAT2(InMaxUV.x, InMaxUV.y),
                    .textureIndex = InTextureIndex
                },
                {
                    .pos = XMFLOAT3(-0.5f, 0.5f, 0.f),
                    .color = InVertexColor,
                    .uv0 = XMFLOAT2(InMinUV.x, InMinUV.y),
                    .textureIndex = InTextureIndex
                },
            //Second triangle
                {
                    .pos = XMFLOAT3(0.5f, 0.5f, 0.f),
                    .color = InVertexColor,
                    .uv0 = XMFLOAT2(InMaxUV.x, InMinUV.y),
                    .textureIndex = InTextureIndex
                }
    },
            std::vector<unsigned short>{
        0, 2, 1,
            1, 2, 3
    }
        );
}

RSRSharedMesh2DPtr RSRBasicShapes::GetRegisterNewPlane2D(const std::string_view InShapeName, const DirectX::XMFLOAT4& InVertexColor, const XMFLOAT2& Pivot, const XMFLOAT2& InTopLeft, const XMFLOAT2& InBottomRight, uint32_t InTextureIndex, RSRAssetManager* InAssetManager)
{
    if (!InAssetManager)
    {
        InAssetManager = &RSRAssetManager::Get();
    }
    return InAssetManager->AddAsset<RSRMesh2D>
        (
            mds::NameDynamicAsset(mds::RAssetType::Mesh, InShapeName),
            false,
            std::vector<VertexPosition2DUV>
            {
                {//TopLeft
                    .pos = XMFLOAT4(Pivot.x, Pivot.y, InTopLeft.x, InTopLeft.y),
                    .color = InVertexColor,
                    .uv0 = XMFLOAT2(0.f, 0.f),
                    .textureIndex = InTextureIndex
                },
                {//TopRight
                    .pos = XMFLOAT4(Pivot.x, Pivot.y, InBottomRight.x, InTopLeft.y),
                    .color = InVertexColor,
                    .uv0 = XMFLOAT2(1.0f, 0.0f),
                    .textureIndex = InTextureIndex
                },
                {//BottomLeft
                    .pos = XMFLOAT4(Pivot.x, Pivot.y, InTopLeft.x, InBottomRight.y),
                    .color = InVertexColor,
                    .uv0 = XMFLOAT2(0.0f, 1.0f),
                    .textureIndex = InTextureIndex
                },
                //BottomRight
                {
                    .pos = XMFLOAT4(Pivot.x, Pivot.y, InBottomRight.x, InBottomRight.y),
                    .color = InVertexColor,
                    .uv0 = XMFLOAT2(1.0f, 1.0f),
                    .textureIndex = InTextureIndex
                }
            },
            std::vector<unsigned short>{ //Triangle index
            0, 1, 2,
            2, 1, 3
    }
    );
}

RSRush::RSRSharedMesh2DPtr RSRush::RSRBasicShapes::GetRegisterNewTriangle2D(const std::string_view InShapeName, const DirectX::XMFLOAT4& InVertexColor, const DirectX::XMFLOAT2& Pivot, const DirectX::XMFLOAT2& InTopLeft, const DirectX::XMFLOAT2& InBottomRight, uint32_t InTextureIndex, RSRAssetManager* InAssetManager)
{
    if (!InAssetManager)
    {
        InAssetManager = &RSRAssetManager::Get();
    }
    return InAssetManager->AddAsset<RSRMesh2D>
        (
            mds::NameDynamicAsset(mds::RAssetType::Mesh, InShapeName),
            false,
            std::vector<VertexPosition2DUV>
            {
                {//TopLeft
                    .pos = XMFLOAT4(Pivot.x, Pivot.y, (InTopLeft.x + InBottomRight.x) * 2.f, InTopLeft.y),
                        .color = InVertexColor,
                        .uv0 = XMFLOAT2(0.5f, 0.f),
                        .textureIndex = InTextureIndex
                },
                //BottomRight
                {
                    .pos = XMFLOAT4(Pivot.x, Pivot.y, InBottomRight.x, InBottomRight.y),
                    .color = InVertexColor,
                    .uv0 = XMFLOAT2(1.0f, 1.0f),
                    .textureIndex = InTextureIndex
                },
                {//BottomLeft
                    .pos = XMFLOAT4(Pivot.x, Pivot.y, InTopLeft.x, InBottomRight.y),
                    .color = InVertexColor,
                    .uv0 = XMFLOAT2(0.0f, 1.0f),
                    .textureIndex = InTextureIndex
                }
            }
        );
}

static bool CreateSphereVertexData(float InRadius, int32_t InNbLatitude, int32_t InNbLongitude, const DirectX::XMFLOAT3& InVertexColor, uint32_t InTextureIndex, std::vector<VertexPositionUVColor>& OutVertexArray);
static bool CreateSphereTriangleArray(int32_t InNbLatitude, int32_t InNbLongitude, std::vector<uint16_t>& OutTriangleArray);

RSRush::RSRSharedMesh3DPtr RSRush::RSRBasicShapes::GetRegisterNewSphere(const std::string_view InShapeName, const DirectX::XMFLOAT3& InVertexColor, uint32_t InTextureIndex, RSRAssetManager* InAssetManager)
{
    if (!InAssetManager)
    {
        InAssetManager = &RSRAssetManager::Get();
    }
    std::vector<VertexPositionUVColor> verices;
    std::vector<uint16_t> triangles;

    bool AllSucessfull = CreateSphereVertexData(0.5f, 24, 24, InVertexColor, InTextureIndex, /*Out*/verices);
    AllSucessfull = CreateSphereTriangleArray(24, 24, /*Out*/triangles) && AllSucessfull;
    return InAssetManager->AddAsset<RSRMesh3D>
        (
            mds::NameDynamicAsset(mds::RAssetType::Mesh, InShapeName),
            false,
            verices,
            triangles
        );
}
//Longitude is +1 because last vertex Longitude is first vertex Longitude but with different UVs
constexpr int32_t SphereVertexArrayLenght(const int32_t InNbLatitude, const int32_t InNbLongitude) { return InNbLatitude * (InNbLongitude + 1); }

bool CreateSphereVertexData(float InRadius, int32_t InNbLatitude, int32_t InNbLongitude, const DirectX::XMFLOAT3& InVertexColor, uint32_t InTextureIndex, std::vector<VertexPositionUVColor>& OutVertexArray)
{
    if (InNbLatitude < 3 || InNbLongitude < 3)
    {
        return false;
    }
    //                                   UV [0, 0]           UV [1, 0]
    // U = 0, 1                                _________________
    //   |  __ <------- V = 0				   |               |
    //   | / |\								   |               |
    //   |/  | \							   |               |
    //   |_ _| _| <--- Latitude = X = U		   |               |
    //   |   |  |							   |               |
    //    \  |</----- Longitude = Y = V		   |               |
    //     \__/ <------- V = 1                 |_______________|
    //                                   UV [0, 1]           UV [1, 1]
    //
    
    const int32_t cVertex = SphereVertexArrayLenght(InNbLatitude, InNbLongitude);
    OutVertexArray.assign(cVertex, VertexPositionUVColor());

    const int32_t cV = InNbLatitude;
    const int32_t cU = InNbLongitude + 1;
    for (int32_t vIndex = 0; vIndex < cV; ++vIndex)
    {
        //[O -> 1]
        float V = (float)vIndex / (float)(cV - 1);

        //Goest from +Z Radius to -Z Radius in [O -> 1]
        float height = InRadius - (InRadius * V * 2.f);

        float width = (fabs(height) < std::numeric_limits<float>::epsilon()) ? 
            InRadius
            :
            sqrtf(std::abs((height * height) - (InRadius * InRadius)))
            ;
        //float scaleRadius = cosf(XM_PI * V);//0.5f - (cosf(XM_2PI * V) * 0.5f);
        for (int32_t uIndex = 0; uIndex < cU; ++uIndex)
        {
            int32_t index = (vIndex * cU) + uIndex;

            //[O -> 1]
            float U = (float)uIndex / (float)(cU - 1);

            VertexPositionUVColor& vertex = OutVertexArray[index];
            XMVECTOR lPos = XMVECTOR
            {
                width * sinf(XM_2PI * U),
                width * cosf(XM_2PI * U),
                height,
                1.f
            };
            XMStoreFloat3(&vertex.pos, lPos);
            XMStoreFloat3(&vertex.normal, XMVector3Normalize(lPos));
            vertex.color = InVertexColor;
            vertex.uv0 = XMFLOAT2(U, V);
            vertex.textureIndex = InTextureIndex;
        }
    }
    return true;
}

bool CreateSphereTriangleArray(int32_t InNbLatitude, int32_t InNbLongitude, std::vector<uint16_t>& OutTriangleArray)
{
    if (InNbLatitude < 3 || InNbLongitude < 3)
    {
        return false;
    }

    const int32_t SquareLenght = 2 * 3;

    const int32_t cVertex = SphereVertexArrayLenght(InNbLatitude, InNbLongitude);

    const int32_t cV = InNbLatitude;
    //Last longitude is first with UV (U Axis) = 1 instead of 0
    const int32_t cU = InNbLongitude + 1;

    //Square Count for U
    const int32_t cSqrU = cU - 1;
    //Square Count for V
    const int32_t cSqrV = cV - 1;

    int32_t t_lastIDX = 0;
    OutTriangleArray.assign(cSqrU * cSqrV * SquareLenght, 0);

    for (int32_t vIndex = 0; vIndex < cSqrV; ++vIndex)
    {
        for (int32_t uIndex = 0; uIndex < cSqrU; ++uIndex)
        {
            int32_t UVIndex = (vIndex * cU) + uIndex;

            int32_t squareIndex = (vIndex * cSqrU) + uIndex;

            int32_t sqrIdxInArr = squareIndex * SquareLenght;
            //  0|3     1
            //   _______
            //   |\    |
            //   |  \  |
            //   |____\|
            //  5      2|4
            // 
            if (true /*uIndex < cSqrU - 1*/)
            {
                //Triangle 1
                OutTriangleArray[sqrIdxInArr] =		/*sqrIdxInArr		; //*/UVIndex;				//0
                OutTriangleArray[sqrIdxInArr + 1] =	/*sqrIdxInArr + 1	; //*/UVIndex + 1;			//1
                OutTriangleArray[sqrIdxInArr + 2] =	/*sqrIdxInArr + 2	; //*/UVIndex + cU + 1;		//2
                /*					; //*/
//Triangle 2							/*					; //*/
                OutTriangleArray[sqrIdxInArr + 3] =	/*sqrIdxInArr + 3	; //*/UVIndex;				//3
                OutTriangleArray[sqrIdxInArr + 4] =	/*sqrIdxInArr + 4	; //*/UVIndex + cU + 1  ;	//4
                OutTriangleArray[sqrIdxInArr + 5] =	/*sqrIdxInArr + 5	; //*/UVIndex + cU;	        //5
            }
            t_lastIDX = std::max(t_lastIDX, sqrIdxInArr + 5);
        }
    }

    RSRLog::Log(LOG_DISPLAY, TEXT("array size : '{}', last index : '{}'"), OutTriangleArray.size(), t_lastIDX);
    return true;
}


RSRBasicShapes::RSRBasicShapes()
:mds::Singleton<RSRBasicShapes>()
{

}
