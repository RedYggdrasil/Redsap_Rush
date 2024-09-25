#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include "MDS/Defines/MDS.h"

namespace RSRush
{
	struct RSRPaths
	{
	public:
		STATIC_STRUCT_R5(RSRPaths);
	private:
		static std::filesystem::path m_runnablePath;
		static std::string m_runnablePath_str;
		static std::string_view m_runnablePath_view;
		static std::filesystem::path m_assetsDiskPath;
		static std::string m_assetsDiskPath_str;
		static std::string_view m_assetsDiskPath_view;
		static std::filesystem::path m_shadersDiskPath;
		static std::string m_shadersDiskPath_str;
		static std::string_view m_shadersDiskPath_view;
	public:
		inline static const std::filesystem::path& RunnablePath() { return m_runnablePath; }
		inline static const std::string& RunnablePathSTR() { return m_runnablePath_str; }
		inline static std::string_view RunnablePathVIEW() { return m_runnablePath_view; }
		inline static const std::filesystem::path& AssetsDiskPath() { return m_assetsDiskPath; }
		inline static const std::string& AssetsDiskPathSTR() { return m_assetsDiskPath_str; }
		inline static std::string_view AssetsDiskPathVIEW() { return m_assetsDiskPath_view; }
		inline static const std::filesystem::path& ShadersDiskPath() { return m_shadersDiskPath; }
		inline static const std::string& ShadersDiskPathSTR() { return m_shadersDiskPath_str; }
		inline static std::string_view ShadersDiskPathView() { return m_shadersDiskPath_view; }
	private:
		friend class RSRProgramInstance;
	};
};