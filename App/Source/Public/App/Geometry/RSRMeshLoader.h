#pragma once
#include "MDS/Tools/System/IRProgramMemNode.h"
#include <string_view>
#include <vector>

namespace RSRush
{
	class RSRProgramInstance;
	class RSRMesh3D;
	class RSRTextureLibrary;
	class RSRMeshLoader : public mds::IRProgramMemElem
	{
	public:
		static RSRMeshLoader* Get(const mds::IRProgramMemElem* InProgramMemElem);
		static RSRMeshLoader* Get(RSRProgramInstance* InProgramInstance);

	public:
		RSRMeshLoader(RSRProgramInstance* InProgramInstance);
		R_VIRTUAL_IMPLICIT ~RSRMeshLoader() R_OVERRIDE_IMPLICIT;

		void Init();
		void ShutDown();

	public:
		std::shared_ptr<RSRMesh3D> LoadMesh3DFromPath(const std::string_view InMeshAssetPath, RSRTextureLibrary* InTextureLibrary);
		std::vector<std::shared_ptr<RSRMesh3D>> LoadMeshes3DFromPath(const std::string_view InMeshAssetPath, RSRTextureLibrary* InTextureLibrary);

	};
};