#include <fstream>
#include <string>
#include <sstream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "../Logger/Log.hpp"

#include "Shader.hpp"

using namespace VoxSmith;

Shader::Shader(const char* sPath)
{
	enum class ShaderMode {
		NONE,
		VERTEX,
		FRAGMENT,
		GEOMETRY
	};

	std::ifstream sFile(sPath);

	if (!sFile.is_open())
	{
		LOG_CORE_ERROR("Could not open {}", sPath);
	}

	std::stringstream sStream;
	sStream << sFile.rdbuf();

	std::string vStr = "";
	std::string fStr = "";

	std::string line;
	ShaderMode mode = ShaderMode::NONE;
	while (std::getline(sStream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("VERTEX") != std::string::npos)
			{
				mode = ShaderMode::VERTEX;
			}
			else if (line.find("FRAGMENT") != std::string::npos)
			{
				mode = ShaderMode::FRAGMENT;
			}
		}
		else
		{
			switch (mode)
			{
			case ShaderMode::VERTEX:
				vStr += line += '\n';
				break;

			case ShaderMode::FRAGMENT:
				fStr += line += '\n';
				break;
			}
		}
	}

	const char* vSrc = vStr.c_str();
	const char* fSrc = fStr.c_str();
	int32_t success;
	char infolog[512];

	uint32_t vID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vID, 1, &vSrc, nullptr);
	glCompileShader(vID);
	glGetShaderiv(vID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vID, 512, NULL, infolog);
		LOG_ERROR("ERROR::VERTEX_SHADER::COMPILATION_FAILED:\n{}", infolog);
	}

	uint32_t fID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fID, 1, &fSrc, nullptr);
	glCompileShader(fID);
	glGetShaderiv(fID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fID, 512, NULL, infolog);
		LOG_ERROR("ERROR::FRAGMENT_SHADER::COMPILATION_FAILED:\n{}", infolog);
	}

	ID = glCreateProgram();
	glAttachShader(ID, vID);
	glAttachShader(ID, fID);
	glLinkProgram(ID);
	glGetShaderiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		LOG_ERROR("ERROR::SHADER_PROGRAM::LINK_FAILED:\n{}", infolog);
	}

	glDeleteShader(vID);
	glDeleteShader(fID);
}

Shader::~Shader()
{

}

void Shader::use()
{
	glUseProgram(ID);
}

int32_t Shader::isUniform(const char* uniform)
{
	int32_t location = glGetUniformLocation(ID, uniform);

	if (location == -1)
	{
		LOG_ERROR("SHADER_UNIFORM::ERROR::No such uniform: {}", uniform);
	}

	return location;
}

void Shader::setUniform1i(const char* uniform, const int32_t value)
{
	use();
	glUniform1i(isUniform(uniform), value);
}

void Shader::setUniform1b(const char* uniform, const bool value)
{
	use();
	glUniform1i(isUniform(uniform), value);
}

void Shader::setUniform1f(const char* uniform, const float value)
{
	use();
	glUniform1d(isUniform(uniform), value);
}

void Shader::setUniform4m(const char* uniform, const glm::mat4& value)
{
	use();
	glUniformMatrix3fv(isUniform(uniform), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform3fv(const char* uniform, const glm::vec3& value)
{
	use();
	glUniform3fv(isUniform(uniform), 1, glm::value_ptr(value));
}
