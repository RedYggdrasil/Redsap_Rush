#include "App/Game/RSRPaths.h"

using namespace RSRush;

std::filesystem::path RSRPaths::m_runnablePath;
std::string RSRPaths::m_runnablePath_str;
std::string_view RSRPaths::m_runnablePath_view;

std::filesystem::path RSRPaths::m_assetsDiskPath;
std::string RSRPaths::m_assetsDiskPath_str;
std::string_view RSRPaths::m_assetsDiskPath_view;

std::filesystem::path RSRPaths::m_shadersDiskPath;
std::string RSRPaths::m_shadersDiskPath_str;
std::string_view RSRPaths::m_shadersDiskPath_view;