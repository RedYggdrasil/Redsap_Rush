#include "App/GPUCompute/RSRComputeTextureTask.h"
#include "App/GPUCompute/RSRComputePipeline.h"
#include "App/GPUCompute/RSRComputeTextureBuffer.h"
#include "App/Game/RSRProgramInstance.h"
#include "App/Data/Textures/RSRTexture2D.h"
#include "App/Tools/Shader.h"
#include "App/Tools/ImageLoader.h"
#include "App/Tools/RSRLog.h"
#include <string>

using namespace RSRush;
using namespace DirectX;



RSRush::RSRComputeTextureTask::RSRComputeTextureTask(const std::string_view InRootSingatureShaderName, const std::string_view InComputeShaderName, uint16_t InSizeX, uint16_t InSizeY, uint16_t InBPP, uint16_t InCC, DXGI_FORMAT InDXGIFormat)
    : RSRComputeTextureTask(InSizeX, InSizeY, InBPP, InCC, InDXGIFormat)
{
    m_rootSingatureShaderName = std::string(InRootSingatureShaderName);
    m_computeShaderName = InComputeShaderName;
}

RSRush::RSRComputeTextureTask::RSRComputeTextureTask(std::shared_ptr<Shader> InRootSingatureShader, const std::shared_ptr<Shader> InComputeShader, uint16_t InSizeX, uint16_t InSizeY, uint16_t InBPP, uint16_t InCC, DXGI_FORMAT InDXGIFormat)
    : RSRComputeTextureTask(InSizeX, InSizeY, InBPP, InCC, InDXGIFormat)
{
    m_rootSingatureShader = InRootSingatureShader;
    m_computeShader = InComputeShader;
}

RSRComputeTextureTask::RSRComputeTextureTask(uint16_t InSizeX, uint16_t InSizeY, uint16_t InBPP, uint16_t InCC, DXGI_FORMAT InDXGIFormat)
    : m_texture(), m_greeblePipeline(), m_sizeX(InSizeX), m_sizeY(InSizeY),
    m_bPP(InBPP), m_cC(InCC), m_dXGIFormat(InDXGIFormat),
    m_rootSingatureShaderName(), m_rootSingatureShader(), m_computeShaderName(), m_computeShader()
{
    if ((m_sizeX % 8) != 0)
    {
        RSRLog::Log(LOG_ERROR, TEXT("Only multiple of 8 are supported for texture size, X does not match requirements !"));
    }
    if ((m_sizeY % 8) != 0)
    {
        RSRLog::Log(LOG_ERROR, TEXT("Only multiple of 8 are supported for texture size, Y does not match requirements !"));
    }
}

void RSRComputeTextureTask::CreateResources(ID3D12Device10* InDevice)
{
    m_texture = std::make_unique<RSRComputeTextureBuffer>(m_sizeX, m_sizeY, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
    m_texture->AllocateDataOnGPU(InDevice);
    m_texture->AllocateDescriptorTable(InDevice);

    if (!m_rootSingatureShader)
    {
        m_rootSingatureShader = std::make_shared<Shader>(m_rootSingatureShaderName);
    }
    if (!m_computeShader)
    {
        m_computeShader = std::make_shared<Shader>(m_computeShaderName);
    }
    m_greeblePipeline = std::make_unique<RSRComputePipeline>
        (
            m_rootSingatureShader,
            m_computeShader
        );
    m_greeblePipeline->Init(InDevice);
}

void RSRComputeTextureTask::Compute(ID3D12GraphicsCommandList7* InCommandList)
{
    InCommandList->SetComputeRootSignature(m_greeblePipeline->GetRootSig());
    InCommandList->SetPipelineState(m_greeblePipeline->GetPSO());

    InCommandList->SetDescriptorHeaps(1, m_texture->GetUAVHeapPtr());

    InCommandList->SetComputeRootDescriptorTable(0, m_texture->GetUAVHeap()->GetGPUDescriptorHandleForHeapStart());

    //a draw

    //Context b0
    //Mesh b1 depend per mesh

    unsigned int dispatchX = m_sizeX / 8;
    unsigned int dispatchY = m_sizeY / 8;

    InCommandList->Dispatch(dispatchX, dispatchY, 1);


    m_texture->TransitionToState(InCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

Microsoft::WRL::ComPtr<ID3D12Resource2> RSRComputeTextureTask::GetResultBuffer() const
{
    return m_texture->GetBuffer();
}

std::shared_ptr<RSRTexture2D> RSRComputeTextureTask::ResultBufferToTexture2D() const
{
    Microsoft::WRL::ComPtr<ID3D12Resource2> loadedGreebleTexture;
    loadedGreebleTexture = GetResultBuffer();

    ImageLoader::ImageData data
    {
        .data = std::vector<char>(),
        .width = m_sizeX,
        .height = m_sizeY,
        .bpp = m_bPP,
        .cc = m_cC,
        .wicPixelFormat = GUID(),
        .giPixelFormat = m_dXGIFormat
    };
    return std::make_shared<RSRTexture2D>(GetResultBuffer(), data);
}

RSRComputeTextureTask::~RSRComputeTextureTask()
{

}