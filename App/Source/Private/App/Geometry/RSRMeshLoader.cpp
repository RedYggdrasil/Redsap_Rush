#include "App/Geometry/RSRMeshLoader.h"
#include "App/Data/Meshes/RSRMesh3D.h"
#include "App/Data/Textures/RSRTexture2D.h"
#include "App/Data/Textures/RSRTextureLibrary.h"
#include "App/Game/RSRProgramInstance.h"
#include "App/Tools/RSRLog.h"
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>

#include <vector>
#include <sstream>


using namespace RSRush;
using namespace DirectX;

//AI Code Start
static std::vector<std::string> SplitString(const std::string& str, char delimiter) 
{ 
	std::vector<std::string> tokens; 
	std::stringstream ss(str); 
	std::string token; 
	while (std::getline(ss, token, delimiter)) 
	{ 
		tokens.push_back(token); 
	} 
	return tokens; 
}
static std::vector<std::string> SplitString(const std::string& str, const std::string& delimiter)
{ 
	std::vector<std::string> tokens; 
	size_t pos = 0; 
	size_t delimiter_length = delimiter.length(); 
	size_t start = 0; 
	while ((pos = str.find(delimiter, start)) != std::string::npos) 
	{ 
		tokens.push_back(str.substr(start, pos - start)); 
		start = pos + delimiter_length; 
	} tokens.push_back(str.substr(start)); 
	return tokens; 
}
//AI Code End

static inline constexpr XMFLOAT3 ToFLT3(const aiVector3D& InAIVector3D)
{
	return XMFLOAT3{ (float)InAIVector3D.x, (float)InAIVector3D.y, (float)InAIVector3D.z };
}
static inline constexpr XMFLOAT3 ToFLT3(const aiColor4D& InAIColor4D)
{
	return XMFLOAT3{ (float)InAIColor4D.r, (float)InAIColor4D.g, (float)InAIColor4D.b };
}
static inline constexpr XMFLOAT2 ToFLT2(const aiVector3D& InAIVector3D)
{
	return XMFLOAT2{ (float)InAIVector3D.x, (float)InAIVector3D.y };
}

RSRMeshLoader* RSRush::RSRMeshLoader::Get(const mds::IRProgramMemElem* InProgramMemElem)
{
	return InProgramMemElem->GetRoot<RSRProgramInstance>()->GetMeshLoader();
}

RSRMeshLoader* RSRush::RSRMeshLoader::Get(RSRProgramInstance* InProgramInstance)
{
	return InProgramInstance->GetMeshLoader();
}

RSRMeshLoader::RSRMeshLoader(RSRProgramInstance* InProgramInstance)
: mds::IRProgramMemElem(InProgramInstance)
{
}

RSRMeshLoader::~RSRMeshLoader()
{
}

void RSRMeshLoader::Init()
{
}

void RSRMeshLoader::ShutDown()
{
}

bool LoadAIMesh(std::vector<VertexPositionUVColor>& InOutVertexes, std::vector<unsigned short>& InOutIndexes, const aiMesh* InAiMesh, const size_t InMeshIndex, const aiScene* InScene, const std::vector<uint16_t>& InMaterialIndexToTextureIDX, size_t& OutSceneMaterialIndex);

std::shared_ptr<RSRMesh3D> RSRush::RSRMeshLoader::LoadMesh3DFromPath(const std::string_view InMeshAssetPath, RSRTextureLibrary* InTextureLibrary)
{
	auto vec = LoadMeshes3DFromPath(InMeshAssetPath, InTextureLibrary);
	return vec.size() > 0 ? vec[0] : nullptr;
}

std::vector<std::shared_ptr<RSRMesh3D>> RSRush::RSRMeshLoader::LoadMeshes3DFromPath(const std::string_view InMeshAssetPath, RSRTextureLibrary* InTextureLibrary)
{
	std::vector<std::shared_ptr<RSRMesh3D>> loadedMeshes;
	std::vector<size_t> loadedMeshesMaterialIndexes;

	std::vector<uint16_t> materialToTextureIDXs;

	Assimp::Importer Importer;
	const char* meshPath = InMeshAssetPath.data();
	//Export as Y Forward Z Up in blender
	const aiScene* pScene = Importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices /* | aiProcess_MakeLeftHanded*/);
	if (!pScene)
	{
		RSRLog::Log(LOG_ERROR, TEXT("RSRMeshLoader : Failed to read 3D mesh at '{}'."), InMeshAssetPath);
		return loadedMeshes;
	}
	if (pScene->mNumMeshes < 1)
	{
		RSRLog::Log(LOG_ERROR, TEXT("RSRMeshLoader : Failed to read 3D mesh, less than 1 mesh present ({})."), pScene->mNumMeshes);
		return loadedMeshes;
	}

	if (pScene->HasMaterials())
	{
		for (size_t i = 0; i < pScene->mNumMaterials; ++i)
		{
			uint16_t textureIDX = 0;
			const aiMaterial* mat = pScene->mMaterials[i];
			if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString texturePath;
				aiReturn ret = mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
				std::filesystem::path diffusePath = texturePath.C_Str();
				diffusePath = diffusePath.lexically_normal();
				std::string diffusePathSTR = diffusePath.string();
				std::vector<std::string> splittedStrings = SplitString(diffusePathSTR, RSRPaths::AassetFolderAndSeparatorSTR());
				if (splittedStrings.size() > 0 && splittedStrings.back().size() > 0)
				{
					std::string remainingPath = splittedStrings.back();
					std::shared_ptr<RSRTexture2D> texture2D = InTextureLibrary->AddGetByPath(splittedStrings.back());
					if (texture2D)
					{
						textureIDX = texture2D->GetTextureIndex();
					}
				}

			}

			materialToTextureIDXs.push_back(textureIDX);

			//std::string filepathString = filepath.lexically_normal().string();

			RSRLog::Log(LOG_DISPLAY, TEXT("mat {} : '{}' '{}'"), mat->GetName().C_Str(), mat->mNumAllocated, mat->mNumProperties);
			if (i == 0) { continue; }
			for (size_t j = 0; j < mat->mNumProperties; ++j)
			{
				const aiMaterialProperty* matprop = mat->mProperties[j];
				//aiPropertyTypeInfo
				RSRLog::Log(LOG_DISPLAY, TEXT("matProp {}"), matprop->mKey.C_Str());

			}
		}
	}
	loadedMeshes.reserve(1);
	loadedMeshesMaterialIndexes.reserve(pScene->mNumMeshes);

	std::vector<VertexPositionUVColor>vertexes;
	std::vector<unsigned short>indexes;

	for (size_t i = 0; i < pScene->mNumMeshes; ++i)
	{
		loadedMeshesMaterialIndexes.push_back(0);
		bool bSucess = LoadAIMesh(/*InOut*/vertexes, /*InOut*/indexes, pScene->mMeshes[i], i, pScene, materialToTextureIDXs, loadedMeshesMaterialIndexes.back());
	}
	loadedMeshes.push_back(std::make_shared<RSRMesh3D>(std::move(vertexes), std::move(indexes)));
	

	return loadedMeshes;
}


bool LoadAIMesh(std::vector<VertexPositionUVColor>& InOutVertexes, std::vector<unsigned short>& InOutIndexes, const aiMesh* InAiMesh, const size_t InMeshIndex, const aiScene* InScene, const std::vector<uint16_t>& InMaterialIndexToTextureIDX, size_t& OutSceneMaterialIndex)
{
	//std::vector<VertexPositionUVColor> vertexes;
	//std::vector<unsigned short> indexes;
	const size_t vertexStartIndex = InOutVertexes.size();
	const size_t indexStartIndex = InOutIndexes.size();

	std::vector<unsigned short> materialIndexes;

	const size_t cMeshVertices = InAiMesh->mNumVertices;
	const size_t cMeshFaces = InAiMesh->mNumFaces;

	OutSceneMaterialIndex = InAiMesh->mMaterialIndex;

	InOutVertexes.reserve(vertexStartIndex + cMeshVertices);

	const bool bImportNormal	= true && InAiMesh->HasNormals();
	const bool bImportColor		= true && InAiMesh->HasVertexColors(0);
	const bool bImportTexCoord	= true && InAiMesh->HasTextureCoords(0);

	static constexpr XMFLOAT3 DEFAULT_NORMAL = XMFLOAT3(1.f, 1.f, 1.f);
	static constexpr XMFLOAT3 DEFAULT_COLOR = XMFLOAT3(1.f, 1.f, 1.f);
	static constexpr XMFLOAT2 DEFAULT_UV = XMFLOAT2(0.f, 0.f);

	uint16_t textureIDX = ((InMaterialIndexToTextureIDX.size() > OutSceneMaterialIndex) ? InMaterialIndexToTextureIDX[OutSceneMaterialIndex] : 0);
	for (size_t i = 0; i < cMeshVertices; ++i)
	{
		InOutVertexes.push_back
		(
			VertexPositionUVColor
			{
				.pos	= ToFLT3(InAiMesh->mVertices[i]),
				.normal = bImportNormal	? ToFLT3(InAiMesh->mNormals[i]) : DEFAULT_NORMAL,
				.color	= bImportColor	? ToFLT3(InAiMesh->mColors[0][i]) : DEFAULT_COLOR,
				.uv0	= bImportTexCoord ? ToFLT2(InAiMesh->mTextureCoords[0][i]) : DEFAULT_UV,
				.textureIndex = textureIDX
			}
		);
	}


	int iSizeBefore = 0;//vboModelData.GetCurrentSize();
	//iMeshStartIndices.push_back(iSizeBefore / iVertexTotalSize);

	//Supposing faces are triangulated, 3*face is the minimal size of the indexes array
	InOutIndexes.reserve(indexStartIndex + (cMeshFaces * 3));
	for (size_t i = 0; i < cMeshFaces; ++i)
	{
		const aiFace& face = InAiMesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; ++j)
		{
			InOutIndexes.push_back((uint16_t)vertexStartIndex + (uint16_t)face.mIndices[j]);
		}

	}
	InOutVertexes.shrink_to_fit();
	InOutIndexes.shrink_to_fit();

	return true;
}
//		face.mNumIndices
//		FOR(k, 3)
//		{
//			aiVector3D pos = InAiMesh->mVertices[face.mIndices[k]];
//			aiVector3D uv = InAiMesh->mTextureCoords[0][face.mIndices[k]];
//			aiVector3D normal = InAiMesh->HasNormals() ? InAiMesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
//			vboModelData.AddData(&pos, sizeof(aiVector3D));
//			vboModelData.AddData(&uv, sizeof(aiVector2D));
//			vboModelData.AddData(&normal, sizeof(aiVector3D));
//		}
//	}
//	int iMeshVertices = mesh->mNumVertices;
//	iTotalVertices += iMeshVertices;
//	iMeshSizes.push_back((vboModelData.GetCurrentSize() - iSizeBefore) / iVertexTotalSize);
//}
//iNumMaterials = InScene->mNumMaterials;
//
//	return nullptr;
//}