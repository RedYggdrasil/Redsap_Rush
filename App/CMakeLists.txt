# CMakeList.txt : CMake project for App, include source and define
# project specific logic here.
#
#project("App" VERSION 1.0)

set(APP_NAME "App")

set (APP_PUBLIC_SRC "Source/Public/${APP_NAME}/")
set (APP_PRIVTE_SRC "Source/Private/${APP_NAME}/")
set (APP_CMAKE_SRC "CMake_Source/Public/Gen_${APP_NAME}/")

# List of included CMake sources
set(APP_SOURCES_CMake
  ${APP_CMAKE_SRC}Config/${APP_NAME}Config.h.in
)

# List of included sources
set(APP_SOURCES_CPP
  ${APP_PUBLIC_SRC}Libs/WinInclude.h
  ${APP_PUBLIC_SRC}Tools/ComPointer.hpp
  ${APP_PUBLIC_SRC}Tools/RSRLog.h							${APP_PRIVTE_SRC}Tools/RSRLog.cpp
  ${APP_PUBLIC_SRC}Tools/DebugLayer.h						${APP_PRIVTE_SRC}Tools/DebugLayer.cpp
  ${APP_PUBLIC_SRC}Tools/ImageLoader.h						${APP_PRIVTE_SRC}Tools/ImageLoader.cpp
  ${APP_PUBLIC_SRC}Tools/Shader.h							${APP_PRIVTE_SRC}Tools/Shader.cpp
  ${APP_PUBLIC_SRC}Tools/Window.h							${APP_PRIVTE_SRC}Tools/Window.cpp
  
  ${APP_PUBLIC_SRC}System/RSRRandomizer.h					${APP_PRIVTE_SRC}System/RSRRandomizer.cpp
  
  ${APP_PUBLIC_SRC}GPUCompute/RSRComputeTextureTask.h		${APP_PRIVTE_SRC}GPUCompute/RSRComputeTextureTask.cpp
  ${APP_PUBLIC_SRC}GPUCompute/Texture/RSRComputeGreebleTexture.h		${APP_PRIVTE_SRC}GPUCompute/Texture/RSRComputeGreebleTexture.cpp
  ${APP_PUBLIC_SRC}GPUCompute/Texture/RSRComputeUVTexture.h				${APP_PRIVTE_SRC}GPUCompute/Texture/RSRComputeUVTexture.cpp
  ${APP_PUBLIC_SRC}GPUCompute/RSRComputeTextureBuffer.h		${APP_PRIVTE_SRC}GPUCompute/RSRComputeTextureBuffer.cpp
  ${APP_PUBLIC_SRC}GPUCompute/RSRComputePipeline.h			${APP_PRIVTE_SRC}GPUCompute/RSRComputePipeline.cpp

  ${APP_PUBLIC_SRC}Tools/Inputs/RSRUserInput.h				${APP_PRIVTE_SRC}Tools/Inputs/RSRUserInput.cpp
  ${APP_PUBLIC_SRC}Tools/Inputs/RSRGameInput.h				${APP_PRIVTE_SRC}Tools/Inputs/RSRGameInput.cpp
															${APP_PRIVTE_SRC}Tools/Inputs/RSRGameInput_devices.cpp
  ${APP_PUBLIC_SRC}Tools/Inputs/RSRGameInput_XInput.h		${APP_PRIVTE_SRC}Tools/Inputs/RSRGameInput_XInput.cpp

  ${APP_PUBLIC_SRC}D3D/DXContext.h							${APP_PRIVTE_SRC}D3D/DXContext.cpp
  ${APP_PUBLIC_SRC}Game/RSRPaths.h							${APP_PRIVTE_SRC}Game/RSRPaths.cpp
  ${APP_PUBLIC_SRC}Game/RSRProgramInstance.h				${APP_PRIVTE_SRC}Game/RSRProgramInstance.cpp
  ${APP_PUBLIC_SRC}Render/RSRPipelines.h					${APP_PRIVTE_SRC}Render/RSRPipelines.cpp
  ${APP_PUBLIC_SRC}Render/RSRPipelineStateObject.h			${APP_PRIVTE_SRC}Render/RSRPipelineStateObject.cpp
  ${APP_PUBLIC_SRC}Render/RSR2DPSO.h						${APP_PRIVTE_SRC}Render/RSR2DPSO.cpp
  ${APP_PUBLIC_SRC}Render/RSR3DPSO.h						${APP_PRIVTE_SRC}Render/RSR3DPSO.cpp
  ${APP_PUBLIC_SRC}Render/RSR3DInstancedPSO.h				${APP_PRIVTE_SRC}Render/RSR3DInstancedPSO.cpp
  ${APP_PUBLIC_SRC}Render/RSR3DDebugPSO.h					${APP_PRIVTE_SRC}Render/RSR3DDebugPSO.cpp
  ${APP_PUBLIC_SRC}Render/RSRShaderList.h					${APP_PRIVTE_SRC}Render/RSRShaderList.cpp
  
  ${APP_PUBLIC_SRC}Gameplay/RSRScene.h							${APP_PRIVTE_SRC}Gameplay/RSRScene.cpp
  ${APP_PUBLIC_SRC}Gameplay/Scene/RSRSceneInstancedMeshes.h		${APP_PRIVTE_SRC}Gameplay/Scene/RSRSceneInstancedMeshes.cpp
  ${APP_PUBLIC_SRC}Gameplay/Scene/RSRIInstancedMesheHolder.h	${APP_PRIVTE_SRC}Gameplay/Scene/RSRIInstancedMesheHolder.cpp
  ${APP_PUBLIC_SRC}Gameplay/GameManager.h						${APP_PRIVTE_SRC}Gameplay/GameManager.cpp
  ${APP_PUBLIC_SRC}Gameplay/PlayerController.h					${APP_PRIVTE_SRC}Gameplay/PlayerController.cpp
  ${APP_PUBLIC_SRC}Gameplay/Pawn.h								${APP_PRIVTE_SRC}Gameplay/Pawn.cpp
  
  ${APP_PUBLIC_SRC}TheTrench/RSRTTScene.h						${APP_PRIVTE_SRC}TheTrench/RSRTTScene.cpp
  ${APP_PUBLIC_SRC}TheTrench/Gameplay/RSRTTGameManager.h		${APP_PRIVTE_SRC}TheTrench/Gameplay/RSRTTGameManager.cpp
  ${APP_PUBLIC_SRC}TheTrench/Gameplay/RSRTTPlayerController.h	${APP_PRIVTE_SRC}TheTrench/Gameplay/RSRTTPlayerController.cpp
  ${APP_PUBLIC_SRC}TheTrench/Gameplay/RSRTT404Pawn.h			${APP_PRIVTE_SRC}TheTrench/Gameplay/RSRTT404Pawn.cpp
  
  ${APP_PUBLIC_SRC}TheTrench/RSRODrawableBackground.h			${APP_PRIVTE_SRC}TheTrench/RSRODrawableBackground.cpp
  ${APP_PUBLIC_SRC}TheTrench/RSRODrawableLightSource.h			${APP_PRIVTE_SRC}TheTrench/RSRODrawableLightSource.cpp
  
  ${APP_PUBLIC_SRC}Physic/LD/RSRLDType.h							${APP_PRIVTE_SRC}Physic/LD/RSRLDType.cpp
  ${APP_PUBLIC_SRC}Physic/LD/RSRLDElem.h							${APP_PRIVTE_SRC}Physic/LD/RSRLDElem.cpp
  ${APP_PUBLIC_SRC}Physic/LD/RSRLDReader.h							${APP_PRIVTE_SRC}Physic/LD/RSRLDReader.cpp
  ${APP_PUBLIC_SRC}Physic/Demo01/RSRPD1Scene.h						${APP_PRIVTE_SRC}Physic/Demo01/RSRPD1Scene.cpp
  ${APP_PUBLIC_SRC}Physic/Demo01/Gameplay/RSRPD1GameManager.h		${APP_PRIVTE_SRC}Physic/Demo01/Gameplay/RSRPD1GameManager.cpp
  ${APP_PUBLIC_SRC}Physic/Demo01/Gameplay/RSRPD1PlayerController.h	${APP_PRIVTE_SRC}Physic/Demo01/Gameplay/RSRPD1PlayerController.cpp
  ${APP_PUBLIC_SRC}Physic/Demo01/Gameplay/RSRPD1Pawn.h				${APP_PRIVTE_SRC}Physic/Demo01/Gameplay/RSRPD1Pawn.cpp
  ${APP_PUBLIC_SRC}Physic/Demo01/LD/LDSlides.h						${APP_PRIVTE_SRC}Physic/Demo01/LD/LDSlides.cpp

  ${APP_PUBLIC_SRC}Physic/Solvers/RSRSolver.h						${APP_PRIVTE_SRC}Physic/Solvers/RSRSolver.cpp
  ${APP_PUBLIC_SRC}Physic/Solvers/RSRImpulseSolver.h				${APP_PRIVTE_SRC}Physic/Solvers/RSRImpulseSolver.cpp
  ${APP_PUBLIC_SRC}Physic/Solvers/RSRPositionSolver.h				${APP_PRIVTE_SRC}Physic/Solvers/RSRPositionSolver.cpp
  
  ${APP_PUBLIC_SRC}Physic/Demo01/SObject/RSRPD1StaticSObject.h		${APP_PRIVTE_SRC}Physic/Demo01/SObject/RSRPD1StaticSObject.cpp
  ${APP_PUBLIC_SRC}Physic/Demo01/SObject/RSRPD1SphereSObject.h		${APP_PRIVTE_SRC}Physic/Demo01/SObject/RSRPD1SphereSObject.cpp

  ${APP_PUBLIC_SRC}Game/RSRPlayerPath.h						${APP_PRIVTE_SRC}Game/RSRPlayerPath.cpp
  
  ${APP_PUBLIC_SRC}SceneObject/RSROScene.h					${APP_PRIVTE_SRC}SceneObject/RSROScene.cpp
  ${APP_PUBLIC_SRC}SceneObject/RSRSObject.h					${APP_PRIVTE_SRC}SceneObject/RSRSObject.cpp
  ${APP_PUBLIC_SRC}SceneObject/RSRSObjectInstance.h			${APP_PRIVTE_SRC}SceneObject/RSRSObjectInstance.cpp
  

  ${APP_PUBLIC_SRC}Managers/RSRAssetManager.h				${APP_PRIVTE_SRC}Managers/RSRAssetManager.cpp
  ${APP_PUBLIC_SRC}Managers/RSRTrenchManager.h				${APP_PRIVTE_SRC}Managers/RSRTrenchManager.cpp
  ${APP_PUBLIC_SRC}Managers/RSRPhysicManager.h				${APP_PRIVTE_SRC}Managers/RSRPhysicManager.cpp
															${APP_PRIVTE_SRC}Managers/RSRPhysicManager_Dynamics.hpp
															${APP_PRIVTE_SRC}Managers/RSRPhysicManager_Collisions.hpp
															

  ${APP_PUBLIC_SRC}Interfaces/Listeners/IInputListener.h	${APP_PRIVTE_SRC}Interfaces/Listeners/IInputListener.cpp  

  ${APP_PUBLIC_SRC}Data/Meshes/RSRMesh.h					${APP_PRIVTE_SRC}Data/Meshes/RSRMesh.cpp
  ${APP_PUBLIC_SRC}Data/Meshes/RSRMesh2D.h					${APP_PRIVTE_SRC}Data/Meshes/RSRMesh2D.cpp
  ${APP_PUBLIC_SRC}Data/Meshes/RSRMesh3D.h					${APP_PRIVTE_SRC}Data/Meshes/RSRMesh3D.cpp
  ${APP_PUBLIC_SRC}Data/Meshes/RSRMeshLODs.h				${APP_PRIVTE_SRC}Data/Meshes/RSRMeshLODs.cpp
  
  ${APP_PUBLIC_SRC}Data/Meshes/RSRMeshInstances.h			${APP_PRIVTE_SRC}Data/Meshes/RSRMeshInstances.cpp

  ${APP_PUBLIC_SRC}Data/Textures/RSRTexture2D.h				${APP_PRIVTE_SRC}Data/Textures/RSRTexture2D.cpp
  
  ${APP_PUBLIC_SRC}Data/Physic/RSRCollider.h				${APP_PRIVTE_SRC}Data/Physic/RSRCollider.cpp
  ${APP_PUBLIC_SRC}Data/Physic/RSRCollisionType.h			${APP_PRIVTE_SRC}Data/Physic/RSRCollisionType.cpp
  ${APP_PUBLIC_SRC}Data/Physic/RSRCollisionResult.h			${APP_PRIVTE_SRC}Data/Physic/RSRCollisionResult.cpp
															${APP_PRIVTE_SRC}Data/Physic/RSRCollisionFuncs.hpp
  ${APP_PUBLIC_SRC}Data/Physic/RSRColMask.h					${APP_PRIVTE_SRC}Data/Physic/RSRColMask.cpp
  ${APP_PUBLIC_SRC}Data/Physic/RSRCollidersBody.h			${APP_PRIVTE_SRC}Data/Physic/RSRCollidersBody.cpp
  ${APP_PUBLIC_SRC}Data/Physic/RSRPhysicBody.h				${APP_PRIVTE_SRC}Data/Physic/RSRPhysicBody.cpp
  ${APP_PUBLIC_SRC}Data/Physic/RSRPhysicContext.h			${APP_PRIVTE_SRC}Data/Physic/RSRPhysicContext.cpp
  ${APP_PUBLIC_SRC}Data/Physic/RSRIPhysicalEntity.h			${APP_PRIVTE_SRC}Data/Physic/RSRIPhysicalEntity.cpp
  ${APP_PUBLIC_SRC}Data/Physic/RSRPPConnection.h			${APP_PRIVTE_SRC}Data/Physic/RSRPPConnection.cpp

  ${APP_PUBLIC_SRC}Data/RSRCameraData.h						${APP_PRIVTE_SRC}Data/RSRCameraData.cpp
  ${APP_PUBLIC_SRC}Data/RSRTransform.h						${APP_PRIVTE_SRC}Data/RSRTransform.cpp
  ${APP_PUBLIC_SRC}Data/Shaders/ShaderStructures.h			${APP_PRIVTE_SRC}Data/Shaders/ShaderStructures.cpp

  ${APP_PUBLIC_SRC}Data/Materials/RSRMaterial.h				${APP_PRIVTE_SRC}Data/Materials/RSRMaterial.cpp
  ${APP_PUBLIC_SRC}Data/Materials/RSRMaterialParts.h		${APP_PRIVTE_SRC}Data/Materials/RSRMaterialParts.cpp
  ${APP_PUBLIC_SRC}Data/Materials/RSR3DMaterial.h			${APP_PRIVTE_SRC}Data/Materials/RSR3DMaterial.cpp
  
  ${APP_PUBLIC_SRC}Data/Trench/RSRTrenchDefs.h				${APP_PRIVTE_SRC}Data/Trench/RSRTrenchDefs.cpp
  ${APP_PUBLIC_SRC}Data/Trench/RSRTrench.h					${APP_PRIVTE_SRC}Data/Trench/RSRTrench.cpp
  ${APP_PUBLIC_SRC}Data/Trench/RSRBasicTrench.h				${APP_PRIVTE_SRC}Data/Trench/RSRBasicTrench.cpp
  ${APP_PUBLIC_SRC}Data/Trench/RSRVoxalTrench.h				${APP_PRIVTE_SRC}Data/Trench/RSRVoxalTrench.cpp
  ${APP_PUBLIC_SRC}Data/Trench/RSRVoxalGrid.h				${APP_PRIVTE_SRC}Data/Trench/RSRVoxalGrid.cpp
  ${APP_PUBLIC_SRC}Data/Trench/RSRVoxalPlane.h				${APP_PRIVTE_SRC}Data/Trench/RSRVoxalPlane.cpp
  ${APP_PUBLIC_SRC}Data/Trench/Tools/RSRVoxelTools.h		${APP_PRIVTE_SRC}Data/Trench/Tools/RSRVoxelTools.cpp

  ${APP_PUBLIC_SRC}Data/Trench/Obstacles/RSRVoxelObstacle.h				${APP_PRIVTE_SRC}Data/Trench/Obstacles/RSRVoxelObstacle.cpp
  ${APP_PUBLIC_SRC}Data/Trench/Obstacles/RSRVoxelRectangularObstacle.h	${APP_PRIVTE_SRC}Data/Trench/Obstacles/RSRVoxelRectangularObstacle.cpp

  ${APP_PUBLIC_SRC}Geometry/RSRBasicShapes.h				${APP_PRIVTE_SRC}Geometry/RSRBasicShapes.cpp

  ${APP_PRIVTE_SRC}main.cpp
)

set(APP_SOURCES_SHADER_PIPELINE
  ${APP_PRIVTE_SRC}Shaders/SimplexNoise2D.hlsli

  ${APP_PRIVTE_SRC}Shaders/Compute/ComputePipeline.hlsli
  ${APP_PRIVTE_SRC}Shaders/BasicPipeline2D.hlsli
  ${APP_PRIVTE_SRC}Shaders/BasicPipeline3D.hlsli
  ${APP_PRIVTE_SRC}Shaders/BasicPipeline3DInstanced.hlsli
  ${APP_PRIVTE_SRC}Shaders/BasicPipelineDebugPhysic.hlsli
  ${APP_PRIVTE_SRC}Shaders/BasicPipelineDebugPhysic.hlsli
)

set(APP_SOURCES_SHADER_ROOTSIG
  ${APP_PRIVTE_SRC}Shaders/Compute/ComputeRootSignature.hlsl
  ${APP_PRIVTE_SRC}Shaders/BasicRootSignature2D.hlsl
  ${APP_PRIVTE_SRC}Shaders/BasicRootSignature3D.hlsl
  ${APP_PRIVTE_SRC}Shaders/BasicRootSignature3DInstanced.hlsl
  ${APP_PRIVTE_SRC}Shaders/BasicRootSignatureDebugPhysic.hlsl
)

# List of included shaders
set(APP_SOURCES_SHADER_VERTEX
  ${APP_PRIVTE_SRC}Shaders/Basic2DVS.hlsl
  ${APP_PRIVTE_SRC}Shaders/Basic3DVS.hlsl
  ${APP_PRIVTE_SRC}Shaders/Basic3DInstancedVS.hlsl
  ${APP_PRIVTE_SRC}Shaders/BasicDebugPhysicVS.hlsl
)
# List of included shaders
set(APP_SOURCES_SHADER_PIXEL
  ${APP_PRIVTE_SRC}Shaders/Basic2DPS.hlsl
  ${APP_PRIVTE_SRC}Shaders/Basic3DPS.hlsl
  ${APP_PRIVTE_SRC}Shaders/Basic3DInstancedPS.hlsl
  ${APP_PRIVTE_SRC}Shaders/BasicDebugPhysicPS.hlsl
)
# List of included shaders
set(APP_SOURCES_SHADER_COMPUTE
  ${APP_PRIVTE_SRC}Shaders/Compute/ComputeGreebleTexture.hlsl
  ${APP_PRIVTE_SRC}Shaders/Compute/ComputeUVTexture.hlsl
)

set(APP_SOURCES_SHADER_ALL ${APP_SOURCES_SHADER_PIPELINE} ${APP_SOURCES_SHADER_ROOTSIG} ${APP_SOURCES_SHADER_VERTEX} ${APP_SOURCES_SHADER_PIXEL} ${APP_SOURCES_SHADER_COMPUTE})


create_non_existing("${APP_SOURCES_CPP}" "${CMAKE_CURRENT_SOURCE_DIR}" ${APP_NAME})

create_non_existing("${APP_SOURCES_SHADER_ALL}" "${CMAKE_CURRENT_SOURCE_DIR}" ${APP_NAME})

# Add source to this project's executable.
add_executable ( ${APP_NAME} ${APP_SOURCES_CMake} ${APP_SOURCES_CPP} ${APP_SOURCES_SHADER_ALL} )

# WIP on asset handling.
# add_custom_target(copy_assets
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Assets ${CMAKE_CURRENT_BINARY_DIR}/Assets
# )
# add_dependencies(${APP_NAME} copy_assets)


#  VS_SHADER_ENTRYPOINT main_vert # To Set Entry function name
set_source_files_properties(${APP_SOURCES_SHADER_ROOTSIG} PROPERTIES VS_SHADER_MODEL rootsig_1.1 VS_SHADER_TYPE RootSignature)
set_source_files_properties(${APP_PRIVTE_SRC}Shaders/Compute/ComputeRootSignature.hlsl PROPERTIES VS_SHADER_ENTRYPOINT ROOTSIGCOMPUTE)
set_source_files_properties(${APP_PRIVTE_SRC}Shaders/BasicRootSignature2D.hlsl PROPERTIES VS_SHADER_ENTRYPOINT ROOTSIG2D)
set_source_files_properties(${APP_PRIVTE_SRC}Shaders/BasicRootSignature3D.hlsl PROPERTIES VS_SHADER_ENTRYPOINT ROOTSIG3D)
set_source_files_properties(${APP_PRIVTE_SRC}Shaders/BasicRootSignature3DInstanced.hlsl PROPERTIES VS_SHADER_ENTRYPOINT ROOTSIG3DINSTANCED)
set_source_files_properties(${APP_PRIVTE_SRC}Shaders/BasicRootSignatureDebugPhysic.hlsl PROPERTIES VS_SHADER_ENTRYPOINT ROOTSIGDP)

set_source_files_properties(${APP_SOURCES_SHADER_PIPELINE} PROPERTIES VS_SHADER_MODEL 6.0)
set_source_files_properties(${APP_SOURCES_SHADER_VERTEX} PROPERTIES VS_SHADER_MODEL 6.0 VS_SHADER_TYPE Vertex)
set_source_files_properties(${APP_SOURCES_SHADER_PIXEL} PROPERTIES VS_SHADER_MODEL 6.0 VS_SHADER_TYPE Pixel)
set_source_files_properties(${APP_SOURCES_SHADER_COMPUTE} PROPERTIES VS_SHADER_MODEL 6.0 VS_SHADER_TYPE Compute)


# 'source_group' Ensure than Visual Studio Solution respect File Hierarchy in filters
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${APP_SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${APP_SOURCES_SHADER_ALL})

configure_file("CMake_Source/Public/Gen_App/Config/AppConfig.h.in" "Gen_SRC/Public/Gen_App/Config/AppConfig.h")

# target_link_libraries ( ${APP_NAME}
#   PRIVATE
#     libs::lib1
# )

#add_compile_definitions("SOME_DEFINITION" 0)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 20)
endif()

#Load GameInput Library from GameInput external library
get_property(GAMEINPUT_LIB_PATH GLOBAL PROPERTY GAMEINPUT_LIB_PATH)
get_property(GAMEINPUT_INC_PATH GLOBAL PROPERTY GAMEINPUT_INC_PATH)
get_property(GAMEINPUT_LIB GLOBAL PROPERTY GAMEINPUT_LIB)


get_property(TRACY_INC_PATH GLOBAL PROPERTY TRACY_INC_PATH)



target_link_libraries(${APP_NAME} PUBLIC "MDS" "d3d12.lib" "dxgi.lib" "dxguid.lib" ${GAMEINPUT_LIB} Tracy::TracyClient)

target_include_directories(${APP_NAME} 
 PUBLIC
  "${PROJECT_BINARY_DIR}/${APP_NAME}/Gen_SRC/Public/"
  "${CMAKE_CURRENT_SOURCE_DIR}/Source/Public/"
  ${GAMEINPUT_INC_PATH}
  ${TRACY_INC_PATH}
 PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Source/Private/"
)