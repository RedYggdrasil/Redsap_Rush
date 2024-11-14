#include "App/Data/Trench/RSRBasicTrench.h"

#include "MDS/Threads/RTask.h"
#include "MDS/Threads/RTaskFunction.h"
#include "MDS/Threads/RTaskManager.h"

#include "App/Game/RSRPlayerPath.h"
#include "App/Managers/RSRAssetManager.h"
#include "App/Managers/RSRTrenchManager.h"
#include "App/System/RSRRandomizer.h"
#include "App/Tools/RSRLog.h"

#include <format>

#include <array>
#include <algorithm>
#include <utility>
#include <DirectXMath.h>
#include <random>

using namespace DirectX;
using namespace RSRush;
using namespace RSRush::RSRTrenchDefs;

//
//    public:
//        virtual ~RTask() {};
//    public:
//
//        virtual void operator()() R_PURE;
//    protected:
namespace RSRush
{
    struct RSRTaskBasicTrenchGeometry : public RSRTaskTrenchGeometry
    {
    private:
        RSRBasicTrench* m_owningTrench = nullptr;
    public:
        RSRTaskBasicTrenchGeometry() = delete;
        RSRTaskBasicTrenchGeometry(RSRBasicTrench* InOwningTrench)
        :m_owningTrench(InOwningTrench)
        {
            
        };
        ~RSRTaskBasicTrenchGeometry()
        {
        };
    public:
        virtual void StartTask() R_PURE_OVERRIDE
        {
            if (m_owningTrench)
            {
                std::shared_ptr<mds::RTaskFunction> task = std::make_shared<mds::RTaskFunction>
                    (
                        [this]()->void {m_owningTrench->GenerateGeometry(); },
                        [this](std::shared_ptr<mds::RTask> InTask)->void 
                        { 
                            m_owningTrench->ReportGeometryGenerated();
                        }
                    );
                mds::RTaskManager::Get().EnqueueTask(task);
            }
        };
    };
};

RSRBasicTrench::RSRBasicTrench(RSRTrenchManager* InTrenchManager)
:RSRTrench(InTrenchManager)
{
}

RSRBasicTrench::~RSRBasicTrench()
{
}

void RSRBasicTrench::StartGeneratingMesh()
{
    GeneratingMeshInitialSetup();
    GeneratingMeshThreadSensitive();
    StartGeneratingMeshThreadInsensitive();
}

void RSRBasicTrench::PositionSelf()
{
    //TODO : Take Start and End Dir into account (should be the same dir, but not necessarly toward +X
    XMVECTOR startLoc = XMLoadFloat3(&m_startLocation);
    XMVECTOR endLoc = XMLoadFloat3(&m_endLocation);
    XMVECTOR startDir = XMLoadFloat3(&m_startDirection);
    XMVECTOR endDir = XMLoadFloat3(&m_endDirection);
    XMVECTOR midLocation = (startLoc + endLoc) * 0.5f;
    XMFLOAT3 computedMidLoc;

    XMStoreFloat3(&computedMidLoc, midLocation);
    m_mainTransform.SetPositon(computedMidLoc);
}

void RSRBasicTrench::GeneratingMeshInitialSetup()
{
    GeneratePositionData();
    this->PositionSelf();
    GeneratePath();
}

void RSRBasicTrench::GeneratingMeshThreadSensitive()
{
    GeneratePhysicColliders();
}

std::shared_ptr<RSRTaskTrenchGeometry> RSRush::RSRBasicTrench::CreateGeometryTask()
{
    return std::make_shared<RSRTaskBasicTrenchGeometry>(this);
}

void RSRush::RSRBasicTrench::StartGeneratingMeshThreadInsensitive()
{
    m_GeometryTask = this->CreateGeometryTask();
    m_GeometryTask->StartTask();
}

void RSRush::RSRBasicTrench::ReportGeometryGenerated()
{
    m_bThreadedInitializationCompleted = true;
    m_GeometryTask.reset();
}


void ConnectAllNodePointsToNewNode(RSRPlayerPath* pPlayerPath, RSRPPNode* InStartNode, RSRPPNode* InEndNode)
{

    for (RSRPPNode::RSRPPNodePoint& nodePoint : InStartNode->PathPoints)
    {
        //RSRLog::Log(LOG_DISPLAY, TEXT("[{}] Is Valid nodePoint {} ?"), ((nodePoint.IsValid())? TEXT("TRUE_"):TEXT("FALSE")), mds::_tostring(nodePoint.Location));
        if (nodePoint.IsValid())
        {
            /*bool*/pPlayerPath->ConnectNodePoints(nodePoint, *InEndNode, 0);
        }
    }
}
void RSRBasicTrench::GeneratePath()
{
    std::mt19937_64 gen = RSRRandomizer::Get().GetTrenchGenerator(m_instanceID);

    static std::uniform_real_distribution<float> dist(-1.0, 1.0);

    std::shared_ptr<RSRPlayerPath> pPlayerPath = ((m_pPlayerPath.expired()) ? nullptr : m_pPlayerPath.lock());
    if (pPlayerPath)
    {
        double currentProgression = pPlayerPath->GetMaxProgression();
        RSRPPNode* startNode = pPlayerPath->GetLastNode();
        RSRPPNode* newNode = nullptr;
        if (currentProgression + DBL_EPSILON < m_startProgression)
        {
            //RSRLog::Log(LOG_DISPLAY, TEXT("Create Start Node at : {}"), mds::_tostring(m_startLocation));
            //Add a point for start location
            //pPlayerPath->PathCountAt
            newNode = pPlayerPath->CreateGetBackNode();
            *newNode = RSRPPNode::Create
            (
                m_startProgression,
                //Path Point 0
                RSRPPNode::RSRPPNodePoint(this->m_startLocation)
            );
            if (startNode && startNode->IsValid())
            {
                ConnectAllNodePointsToNewNode(pPlayerPath.get(), startNode, newNode);
            }
            startNode = newNode;
            newNode = nullptr;
        }
        if (currentProgression + DBL_EPSILON < m_endProgression)
        {
            XMFLOAT3 posPoint = m_endLocation;
            posPoint.y = m_endLocation.y +  dist(gen) * HALF_TRENCH_WIDTH;
            posPoint.z = m_endLocation.z + (dist(gen) * HALF_HEIGHT);
            //RSRLog::Log(LOG_DISPLAY, TEXT("Create End Node at : {}"), mds::_tostring(posPoint));
            //Add a point for end
            newNode = pPlayerPath->CreateGetBackNode();
            *newNode = RSRPPNode::Create
            (
                m_endProgression,
                //Path Point 0
                RSRPPNode::RSRPPNodePoint(posPoint)
            );
            ConnectAllNodePointsToNewNode(pPlayerPath.get(), startNode, newNode);
        }
    }
}

void RSRBasicTrench::GenerateGeometry()
{
    XMFLOAT3 DEFAULT_COLOR = XMFLOAT3(1.f, 1.f, 1.f);

    //                         X
    //                         ^
    //  HALF_TRENCH_LENGHT --> |  01&09_____05&10___________06&13_____02&14
    //                         |       |   |                     |   |
    //                         |       |   |         F0          |   |
    //                   0 --> |     F1|F2 |F3       F4        F5|F6 |F7
    //                         |       |   |                     |   |
    // -HALF_TRENCH_LENGHT --> |  00&08|___|04&11___________07&12|___|03&15
    //                         |----------------------------------------->Y
    //                        /        |   |          |
    //                       /     -TRENCH_WIDTH/2    0
    //                      Z          |
    //                         -TOTAL_WIDTH/2

    //                   Z                                                                    Z
    //                   ^                                                                    ^
    //                   |                                                                    |    
    //   ROOF_HEIGHT --> |  08&09_____11&10           12&13_____15&14         ROOF_HEIGHT --> |    09&10_____________F2F6______________08&11
    //                   |       |F2 |                     |F6 |                              |    13&14|                             |12&15
    //                   |       |   |                     |   |                              |         |             F1              |
    //                   |       |   | F3                  |   | F7                           |         |             F3              |
    //             0 --> |     F1|   |                   F5|   |                        0 --> |         |             F5              |
    //                   |  X    |   |                     |   |                              |  Y      |             F7              |
    //                   |  ^    |   |                     |   |                              |  ^      |                             |
    // GROUND_HEIGHT --> | /     |   |04&05____F4_____07&06|   |            GROUND_HEIGHT --> | /  05&06|_____________F4______________|04&07
    //  UNDER_MESH ----> |/ 00&01|_____________F0______________|03&02        UNDER_MESH ----> |/   01&02|_____________F0______________|00&03
    //                   |----------------------------------->Y                               |X<------------------------------------------ -X
    //                           |   |          |                                                       |              |              |
    //                       -TRENCH_WIDTH/2    0                                            +HALF_TRENCH_LENGHT       0      -HALF_TRENCH_LENGHT
    //                           |
    //                   -TOTAL_WIDTH/2


    std::mt19937_64 gen = RSRRandomizer::Get().GetTrenchGenerator(m_instanceID);
    static std::uniform_int_distribution<> dist(0, 3);
    uint16_t UseTextureIndex = dist(gen);

    std::array VPos = std::to_array<XMFLOAT3>(
    {
        {   //00
            -HALF_TRENCH_LENGHT, -HALF_TOTAL_WIDTH, UNDER_MESH
        },
        {   //01
            +HALF_TRENCH_LENGHT, -HALF_TOTAL_WIDTH, UNDER_MESH
        },
        {   //02
            +HALF_TRENCH_LENGHT, +HALF_TOTAL_WIDTH, UNDER_MESH
        },
        {   //03
            -HALF_TRENCH_LENGHT, +HALF_TOTAL_WIDTH, UNDER_MESH
        },

        {   //04
            -HALF_TRENCH_LENGHT, -HALF_TRENCH_WIDTH, GROUND_HEIGHT
        },
        {   //05
            +HALF_TRENCH_LENGHT, -HALF_TRENCH_WIDTH, GROUND_HEIGHT
        },
        {   //06
            +HALF_TRENCH_LENGHT, +HALF_TRENCH_WIDTH, GROUND_HEIGHT
        },
        {   //07
            -HALF_TRENCH_LENGHT, +HALF_TRENCH_WIDTH, GROUND_HEIGHT
        },

        {   //08
            -HALF_TRENCH_LENGHT, -HALF_TOTAL_WIDTH, ROOF_HEIGHT
        },
        {   //09
            +HALF_TRENCH_LENGHT, -HALF_TOTAL_WIDTH, ROOF_HEIGHT
        },
        {   //10
            +HALF_TRENCH_LENGHT, -HALF_TRENCH_WIDTH, ROOF_HEIGHT
        },
        {   //11
            -HALF_TRENCH_LENGHT, -HALF_TRENCH_WIDTH, ROOF_HEIGHT
        },

        {   //12
            -HALF_TRENCH_LENGHT, +HALF_TRENCH_WIDTH, ROOF_HEIGHT
        },
        {   //13
            +HALF_TRENCH_LENGHT, +HALF_TRENCH_WIDTH, ROOF_HEIGHT
        },
        {   //14
            +HALF_TRENCH_LENGHT, +HALF_TOTAL_WIDTH, ROOF_HEIGHT
        },
        {   //15
            -HALF_TRENCH_LENGHT, +HALF_TOTAL_WIDTH, ROOF_HEIGHT
        }
    });
    std::array FNrm = std::to_array<XMFLOAT3>(
        {
            //F00
            N_DOWN,
            //F01
            N_LEFT,
            //F02
            N_UP,
            //F03
            N_RIGHT,
            //F04
            N_UP,
            //F05
            N_LEFT,
            //F06
            N_UP,
            //F07
            N_RIGHT
        });
    std::array FUv0 = std::to_array<XMFLOAT2>(
        {
            //F00
            {0.f, 0.f},
            //F01
            {1.f, 0.f},
            //F02
            {1.f, 1.f},
            //F03
            { 0.f, 1.f }
        });
    auto FV = [] (unsigned short InFaceIndex, unsigned short InVextexIndex) constexpr { return ((InFaceIndex * 4) + InVextexIndex); };
    
    
    RSRSharedMesh3DPtr simpleTrenchMesh = RSRAssetManager::Get(m_trenchManager->GetProgramInstance())->AddAsset<RSRMesh3D>
        (
            mds::NameDynamicAsset(mds::RAssetType::Mesh, std::format("Trench{}_Basic", m_instanceID)),
            false,
            std::vector<VertexPositionUVColor>
            {
                ////////////F0////////////
                {   //00
                    .pos = VPos[1],
                    .normal = FNrm[0],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[0],
                    .textureIndex = UseTextureIndex
                },
                {   //01
                    .pos = VPos[0],
                    .normal = FNrm[0],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[1],
                    .textureIndex = UseTextureIndex
                },
                {   //02
                    .pos = VPos[3],
                    .normal = FNrm[0],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[2],
                    .textureIndex = UseTextureIndex
                },
                {   //03
                    .pos = VPos[2],
                    .normal = FNrm[0],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[3],
                    .textureIndex = UseTextureIndex
                },
                ////////////F1////////////
                {   //F1V0
                    .pos = VPos[9],
                    .normal = FNrm[1],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[0],
                    .textureIndex = UseTextureIndex
                },
                {   //F1V1
                    .pos = VPos[8],
                    .normal = FNrm[1],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[1],
                    .textureIndex = UseTextureIndex
                },
                {   //F1V2
                    .pos = VPos[0],
                    .normal = FNrm[1],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[2],
                    .textureIndex = UseTextureIndex
                },
                {   //F1V3
                    .pos = VPos[1],
                    .normal = FNrm[1],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[3],
                    .textureIndex = UseTextureIndex
                },
                ////////////F2////////////
                {   //F2V0
                    .pos = VPos[8],
                    .normal = FNrm[2],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[0],
                    .textureIndex = UseTextureIndex
                },
                {   //F2V1
                    .pos = VPos[9],
                    .normal = FNrm[2],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[1],
                    .textureIndex = UseTextureIndex
                },
                {   //F2V2
                    .pos = VPos[10],
                    .normal = FNrm[2],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[2],
                    .textureIndex = UseTextureIndex
                },
                {   //F2V3
                    .pos = VPos[11],
                    .normal = FNrm[2],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[3],
                    .textureIndex = UseTextureIndex
                },
                ////////////F3////////////
                {   //F3V0
                    .pos = VPos[11],
                    .normal = FNrm[3],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[0],
                    .textureIndex = UseTextureIndex
                },
                {   //F3V1
                    .pos = VPos[10],
                    .normal = FNrm[3],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[1],
                    .textureIndex = UseTextureIndex
                },
                {   //F3V2
                    .pos = VPos[5],
                    .normal = FNrm[3],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[2],
                    .textureIndex = UseTextureIndex
                },
                {   //F3V3
                    .pos = VPos[4],
                    .normal = FNrm[3],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[3],
                    .textureIndex = UseTextureIndex
                },
                    ////////////F4////////////
                {   //F4V0
                    .pos = VPos[04],
                    .normal = FNrm[4],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[0],
                    .textureIndex = UseTextureIndex
                },
                {   //F4V1
                    .pos = VPos[05],
                    .normal = FNrm[4],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[1],
                    .textureIndex = UseTextureIndex
                },
                {   //F4V2
                    .pos = VPos[06],
                    .normal = FNrm[4],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[2],
                    .textureIndex = UseTextureIndex
                },
                {   //F4V3
                    .pos = VPos[07],
                    .normal = FNrm[4],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[3],
                    .textureIndex = UseTextureIndex
                },
                ////////////F5////////////
                {   //F5V0
                    .pos = VPos[13],
                    .normal = FNrm[5],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[0],
                    .textureIndex = UseTextureIndex
                },
                {   //F5V1
                    .pos = VPos[12],
                    .normal = FNrm[5],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[1],
                    .textureIndex = UseTextureIndex
                },
                {   //F5V2
                    .pos = VPos[07],
                    .normal = FNrm[5],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[2],
                    .textureIndex = UseTextureIndex
                },
                {   //F5V3
                    .pos = VPos[06],
                    .normal = FNrm[5],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[3],
                    .textureIndex = UseTextureIndex
                },
                ////////////F6////////////
                {   //F6V0
                    .pos = VPos[12],
                    .normal = FNrm[6],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[0],
                    .textureIndex = UseTextureIndex
                },
                {   //F6V1
                    .pos = VPos[13],
                    .normal = FNrm[6],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[1],
                    .textureIndex = UseTextureIndex
                },
                {   //F6V2
                    .pos = VPos[14],
                    .normal = FNrm[6],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[2],
                    .textureIndex = UseTextureIndex
                },
                {   //F6V3
                    .pos = VPos[15],
                    .normal = FNrm[6],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[3],
                    .textureIndex = UseTextureIndex
                },
                    ////////////F7////////////
                {   //F7V0
                    .pos = VPos[15],
                    .normal = FNrm[7],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[0],
                    .textureIndex = UseTextureIndex
                },
                {   //F7V1
                    .pos = VPos[14],
                    .normal = FNrm[7],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[1],
                    .textureIndex = UseTextureIndex
                },
                {   //F7V2
                    .pos = VPos[02],
                    .normal = FNrm[7],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[2],
                    .textureIndex = UseTextureIndex
                },
                {   //F7V3
                    .pos = VPos[03],
                    .normal = FNrm[7],
                    .color = DEFAULT_COLOR,
                    .uv0 = FUv0[3],
                    .textureIndex = UseTextureIndex
                }
            },
            std::vector<unsigned short>
            {
                //F0
                FV(0, 0), FV(0, 1), FV(0, 3),
                FV(0, 1), FV(0, 2), FV(0, 3),
                //F1
                FV(1, 0), FV(1, 1), FV(1, 3),
                FV(1, 1), FV(1, 2), FV(1, 3),
                //F2
                FV(2, 0), FV(2, 1), FV(2, 3),
                FV(2, 1), FV(2, 2), FV(2, 3),
                //F3
                FV(3, 0), FV(3, 1), FV(3, 3),
                FV(3, 1), FV(3, 2), FV(3, 3),
                //F4
                FV(4, 0), FV(4, 1), FV(4, 3),
                FV(4, 1), FV(4, 2), FV(4, 3),
                //F5
                FV(5, 0), FV(5, 1), FV(5, 3),
                FV(5, 1), FV(5, 2), FV(5, 3),
                //F6
                FV(6, 0), FV(6, 1), FV(6, 3),
                FV(6, 1), FV(6, 2), FV(6, 3),
                //F7
                FV(7, 0), FV(7, 1), FV(7, 3),
                FV(7, 1), FV(7, 2), FV(7, 3),
            }
            );
    m_mainMesh.Insert(simpleTrenchMesh, FLT_MAX);
}

void RSRBasicTrench::GeneratePositionData()
{
    XMVECTOR startLoc = XMLoadFloat3(&m_startLocation);
    XMVECTOR startDir = XMLoadFloat3(&m_startDirection);

    XMVECTOR endLoc = startLoc + (startDir * BASIC_TRENCH_X_LENGHT);
    XMVECTOR endDir = startDir;

    XMVECTOR length = XMVector3Length(endLoc - startLoc);
    XMFLOAT3 computedEndLoc, computedEndDirection;
    XMStoreFloat3(&computedEndLoc, endLoc); XMStoreFloat3(&computedEndDirection, endDir);
    this->SetEndPositionData(m_startProgression + length.m128_f32[0], computedEndLoc, computedEndDirection);

}

void RSRBasicTrench::GeneratePhysicColliders()
{
    //                   Z                                                
    //                   ^                                                
    //                   |                                                
    //   ROOF_HEIGHT --> |       _____                     _____     
    //                   |       |   |                     |   |          
    //                   |       |   |                     |   |          
    //                   |       |   |                     |   |       
    //             0 --> |       |[1]|                     |[2]|          
    //                   |  X    |   |                     |   |          
    //                   |  ^    |   |                     |   |          
    // GROUND_HEIGHT --> | /     |   |_____________________|   |
    //  UNDER_MESH ----> |/      |_____________[0]_____________|    
    //                   |----------------------------------->Y           
    //                           |   |          |                         
    //                       -TRENCH_WIDTH/2    0                         
    //                           |
    //                   -TOTAL_WIDTH/2

    RSRCollidersBody& localBody = this->m_localColliders;
    size_t startIDX = localBody.Colliders.size();
    localBody.Colliders.resize(startIDX + 3);
    RSRCollider* bCol   = &localBody.Colliders[startIDX + 0];
    RSRCollider* lCol   = &localBody.Colliders[startIDX + 1];
    RSRCollider* rCol   = &localBody.Colliders[startIDX + 2];
    
    XMFLOAT4 QuatIdentity = { 0.f, 0.f, 0.f, 1.f };
    //XMMatrixTransformation
    bCol->Type = lCol->Type = rCol->Type = RSRColliderType::RectCollider;
    bCol->RectData =
    {
        /*.Center = */      {0.f, 0.f, (GROUND_HEIGHT + UNDER_MESH) * 0.5f},
        /*.Extents = */     { HALF_TRENCH_LENGHT, HALF_TRENCH_WIDTH, (GROUND_HEIGHT - UNDER_MESH) * 0.5f },
        /*.Orientation = */ QuatIdentity
    };

    static constexpr float sideYCenter = ((TRENCH_WIDTH + TOTAL_WIDTH) * 0.5f) * 0.5f;
    static constexpr float sideYHalfWidth = ((TOTAL_WIDTH - TRENCH_WIDTH) * 0.5f) * 0.5f;
    static constexpr float halfTotalHeight = (ROOF_HEIGHT - UNDER_MESH) * 0.5f;
    static constexpr float totalHeightCenter = (ROOF_HEIGHT + UNDER_MESH) * 0.5f;
    lCol->RectData =
    {
        /*.Center = */      { 0.f, -sideYCenter, totalHeightCenter },
        /*.Extents = */     { HALF_TRENCH_LENGHT, sideYHalfWidth, halfTotalHeight},
        /*.Orientation = */ QuatIdentity
    };

    rCol->RectData =
    {
        /*.Center = */      { 0.f, sideYCenter, totalHeightCenter },
        /*.Extents = */     lCol->RectData.Extents,
        /*.Orientation = */ QuatIdentity
    };

    localBody.ComputeAABB();
}
