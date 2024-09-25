#pragma once
#include "App/Libs/WinInclude.h"
#include <string_view>
#include <filesystem>

class Shader
{
public:
	Shader(std::string_view InShaderName);
	~Shader();

	inline const void* GetBuffer() const { return m_data; }
	inline const size_t GetSize() const { return m_size; }
private:
	void* m_data = nullptr;
	size_t m_size = 0;
};