#pragma once

#include <cstdint>

#include <glm/glm.hpp>

#include "../Core.hpp"

namespace VoxSmith
{
	class VOX_SMITH_API Shader
	{
	public:
		Shader(const char* sPath);
		~Shader() noexcept;
		
		void use();

		void setUniform1i(const char* uniform, const int32_t value);
		void setUniform1b(const char* uniform, const bool value);
		void setUniform1f(const char* uniform, const float value);
		void setUniform4m(const char* uniform, const glm::mat4& value);
		void setUniform3fv(const char* uniform, const glm::vec3& value);

	private:
		uint32_t ID;

		int32_t isUniform(const char* uniform);
	};

	class VOX_SMITH_API ComputShader
	{
		ComputShader();
		~ComputShader() noexcept = default;
	};
}