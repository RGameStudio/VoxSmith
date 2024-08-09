#pragma once

#include <cstdint>
#include <string>

#include <glm/glm.hpp>

#include "../Core.hpp"

namespace VoxSmith
{
	class VOX_SMITH_API Shader final
	{
	public:
		Shader(const std::string& cPath);
		~Shader() noexcept;
		
		void use() const;

		void setUniform1i(const char* uniform, const int32_t value) const;
		void setUniform1b(const char* uniform, const bool value) const;
		void setUniform1f(const char* uniform, const float value) const;
		void setUniform4m(const char* uniform, const glm::mat4& value) const;
		void setUniform3fv(const char* uniform, const glm::vec3& value) const;

		Shader() = default;

	private:
		uint32_t ID;

		int32_t isUniform(const char* uniform) const;
	};

	class VOX_SMITH_API ComputeShader final
	{
	public:
		ComputeShader(const char* cPath);
		~ComputeShader() noexcept;

		void use() const;
		void launchWorkGroups(const glm::ivec3& dim) const;

		void setUniform1i(const char* uniform, const int32_t value) const;
		void setUniform1b(const char* uniform, const bool value) const;
		void setUniform1f(const char* uniform, const float value) const;
		void setUniform4m(const char* uniform, const glm::mat4& value) const;
		void setUniform3fv(const char* uniform, const glm::vec3& value) const;

		ComputeShader() = default;
	
	private:
		uint32_t ID;

		int32_t isUniform(const char* uniform) const;
	};
}