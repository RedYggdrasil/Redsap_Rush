#include "App/Tools/Shader.h"
#include "App/Game/RSRPaths.h"
#include <cstdlib>
#include <fstream>

Shader::Shader(std::string_view InShaderName)
{

	std::ifstream shaderIn(RSRush::RSRPaths::ShadersDiskPath() / InShaderName, std::ios::binary);
	if (shaderIn.is_open())
	{
		shaderIn.seekg(0, std::ios::end);
		m_size = shaderIn.tellg();
		shaderIn.seekg(0, std::ios::beg);
		m_data = malloc(m_size);
		if (m_data)
		{
			shaderIn.read((char*)m_data, m_size);
		}
	}
}

Shader::~Shader()
{
	free(m_data);
}
