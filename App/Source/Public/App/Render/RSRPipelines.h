#pragma once
#include "App/Render/RSRPipelineStateObject.h"
#include "App/Render/RSR2DPSO.h"
#include "App/Render/RSR3DPSO.h"
#include "App/Render/RSR3DInstancedPSO.h"
#if DEBUG_PHYSIC
#include "App/Render/RSR3DDebugPSO.h"
#endif

namespace RSRush
{
	class RSRProgramInstance;
	class RSRPipelines
	{
	protected:
		bool m_bIsCorrectlyLoaded;
		RSRProgramInstance* m_programInstance;

		RSR2DPSO m_pipelineStateDesc2D;
		RSR3DPSO m_pipelineStateDesc3D;
		RSR3DInstancedPSO m_pipelineStateDesc3DInstanced;

#if DEBUG_PHYSIC
		RSR3DDebugPSO m_pipelineStateDescDebugPhysic;
#endif

	public:
		bool GetIsCorrectlyLoaded() const { return m_bIsCorrectlyLoaded; }
		inline const RSRPipelineStateObject& GetRSRPSO2D() const { return m_pipelineStateDesc2D; }
		inline const RSRPipelineStateObject& GetRSRPSO3D() const { return m_pipelineStateDesc3D; }
		inline const RSRPipelineStateObject& GetRSRPSO3DInstanced() const { return m_pipelineStateDesc3DInstanced; }
#if DEBUG_PHYSIC
		inline const RSRPipelineStateObject& GetRSRPSO3DDebugPhysic() const { return m_pipelineStateDescDebugPhysic; }
#endif
		inline const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetStateDesc2D() const { return m_pipelineStateDesc2D.GetStateDesc(); }
		inline const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetStateDesc3D() const { return m_pipelineStateDesc3D.GetStateDesc(); }
		inline const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetStateDesc3DInstanced() const { return m_pipelineStateDesc3DInstanced.GetStateDesc(); }
#if DEBUG_PHYSIC
		inline const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetStateDescDebugPhysic() const { return m_pipelineStateDescDebugPhysic.GetStateDesc(); }
#endif
	public:
		RSRPipelines(RSRProgramInstance* InProgramInstance);
		~RSRPipelines();
	};
};
