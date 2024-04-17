#include <fstream>
#include <string>
#include <sstream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "../Logger/Log.hpp"

#include "Shader.hpp"

using namespace VoxSmith;

enum class ShaderMode {
	NONE,
	VERTEX,
	FRAGMENT,
	GEOMETRY,
	COMPUTE,
};

Shader::Shader(const char* sPath)
{
	std::ifstream sFile(sPath);

	if (!sFile.is_open())
	{
		LOG_CORE_ERROR("ERROR::SHADER_SOURCE::Could not open {}", sPath);
	}

	std::stringstream sStream;
	sStream << sFile.rdbuf();

	std::string vStr = "";
	std::string gStr = "";
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
			else if (line.find("GEOMETRY") != std::string::npos)
			{
				mode = ShaderMode::GEOMETRY;
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

			case ShaderMode::GEOMETRY:
				gStr += line += '\n';
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

	uint32_t gID = 0;
	if (!gStr.empty())
	{
		const char* gSrc = gStr.c_str();
		glShaderSource(gID, 1, &gSrc, nullptr);
		glCompileShader(gID);
		glGetShaderiv(gID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vID, 512, NULL, infolog);
			LOG_ERROR("ERROR::VERTEX_SHADER::COMPILATION_FAILED:\n{}", infolog);
		}
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
	if (gID)
	{
		glAttachShader(ID, gID);
	}
	glLinkProgram(ID);
	glGetShaderiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		LOG_ERROR("ERROR::SHADER_PROGRAM::LINK_FAILED:{}", infolog);
	}

	glDeleteShader(vID);
	if (gID)
	{
		glDeleteShader(gID);
	}
	glDeleteShader(fID);
}

Shader::~Shader()
{

}

void Shader::use() const
{
	glUseProgram(ID);
}

int32_t Shader::isUniform(const char* uniform) const
{
	int32_t location = glGetUniformLocation(ID, uniform);

	if (location == -1)
	{
		LOG_ERROR("SHADER_UNIFORM::ERROR::No such uniform: {}", uniform);
	}

	return location;
}

void Shader::setUniform1i(const char* uniform, const int32_t value) const
{
	use();
	glUniform1i(isUniform(uniform), value);
}

void Shader::setUniform1b(const char* uniform, const bool value) const
{
	use();
	glUniform1i(isUniform(uniform), value);
}

void Shader::setUniform1f(const char* uniform, const float value) const
{
	use();
	glUniform1d(isUniform(uniform), value);
}

void Shader::setUniform4m(const char* uniform, const glm::mat4& value) const
{
	use();
	glUniformMatrix3fv(isUniform(uniform), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform3fv(const char* uniform, const glm::vec3& value) const
{
	use();
	glUniform3fv(isUniform(uniform), 1, glm::value_ptr(value));
}

ComputeShader::ComputeShader(const char* cPath)
{
	std::ifstream cFile(cPath);

	if (!cFile.is_open())
	{
		LOG_CORE_ERROR("ERROR:COMPUTE_SHADER_SOURCE::Could not open {}", cPath);
	}

	std::stringstream sStream;
	sStream << cFile.rdbuf();

	std::string cStr;

	std::string line;
	while (std::getline(sStream, line))
	{
		cStr += line += '\n';
	}

	int32_t success;
	char infolog[512];
	const char* cSrc = cStr.c_str();

	uint32_t cID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(cID, 1, &cSrc, nullptr);
	glCompileShader(cID);
	glGetShaderiv(cID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(cID, 512, NULL, infolog);
		LOG_ERROR("ERROR::COMPUTE_SHADER::COMPILATION_FAILED:\n{}", infolog);
	}

	ID = glCreateProgram();
	glAttachShader(ID, cID);
	glLinkProgram(ID);
	 	glGetShaderiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ID, 512, nullptr, infolog);
		LOG_ERROR("ERROR::COMPUTE_SHADER::LINK_FAILED:{}", infolog);
	}

	glDeleteShader(cID);
}

ComputeShader::~ComputeShader()
{

}

void ComputeShader::use() const
{
	glUseProgram(ID);
}

void ComputeShader::launchWorkGroups(const glm::ivec3& dim) const
{
	use();
	glDispatchCompute(dim.x, dim.y, dim.z);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

int32_t ComputeShader::isUniform(const char* uniform) const
{
	int32_t location = glGetUniformLocation(ID, uniform);

	if (location == -1)
	{
		LOG_ERROR("SHADER_UNIFORM::ERROR::No such uniform: {}", uniform);
	}

	return location;
}

void ComputeShader::setUniform1i(const char* uniform, const int32_t value) const
{
	use();
	glUniform1i(isUniform(uniform), value);
}

void ComputeShader::setUniform1b(const char* uniform, const bool value) const
{
	use();
	glUniform1i(isUniform(uniform), value);
}

void ComputeShader::setUniform1f(const char* uniform, const float value) const
{
	use();
	glUniform1f(isUniform(uniform), value);
}

void ComputeShader::setUniform4m(const char* uniform, const glm::mat4& value) const
{
	use();
	glUniformMatrix3fv(isUniform(uniform), 1, GL_FALSE, glm::value_ptr(value));
}

void ComputeShader::setUniform3fv(const char* uniform, const glm::vec3& value) const
{
	use();
	glUniform3fv(isUniform(uniform), 1, glm::value_ptr(value));
}
