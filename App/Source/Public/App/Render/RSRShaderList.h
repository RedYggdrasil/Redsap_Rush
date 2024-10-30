#pragma once
#include "Gen_App/Config/AppConfig.h"
#include <string>

namespace RSRush
{
	namespace EShaderName
	{
		static inline const std::string BasicRootSignature2D("BasicRootSignature2D.cso");
		static inline const std::string BasicRootSignature3D("BasicRootSignature3D.cso");
		static inline const std::string BasicRootSignatureInstanced3D("BasicRootSignature3DInstanced.cso");
#if DEBUG_PHYSIC
		static inline const std::string BasicRootSignatureDebugPhysic("BasicRootSignatureDebugPhysic.cso");
#endif
		static inline const std::string basicVS2DShader("Basic2DVS.cso");
		static inline const std::string basicPS2DShader("Basic2DPS.cso");

		static inline const std::string basicVS3DShader("Basic3DVS.cso");
		static inline const std::string basicPS3DShader("Basic3DPS.cso");

		static inline const std::string basicVS3DInstancedShader("Basic3DInstancedVS.cso");
		static inline const std::string basicPS3DInstancedShader("Basic3DInstancedPS.cso");

#if DEBUG_PHYSIC
		static inline const std::string basicVSDebugPhysicShader("BasicDebugPhysicVS.cso");
		static inline const std::string basicPSDebugPhysicShader("BasicDebugPhysicPS.cso");
#endif
	};
}