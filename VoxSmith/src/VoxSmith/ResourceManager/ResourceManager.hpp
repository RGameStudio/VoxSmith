#pragma once 

#include <unordered_map>
#include <string>

#include "../Core.hpp"

namespace VoxSmith
{
	class Texture;
	class Shader;
	class ComputeShader;

	class VOX_SMITH_API ResourceManager final
	{
	public:
		static ResourceManager& getInstance();
	
		void setShader(const std::string& sName, Shader& shader);
		void setCShader(const std::string& cName, ComputeShader& shader);
		void setTexture(const std::string& tName, Texture& texture);
		
		Shader& getShader(const std::string& sName);
		ComputeShader& getCShader(const std::string& cName);
		Texture& getTexture(const std::string& tName);

		ResourceManager(const ResourceManager&) = delete; 
		ResourceManager& operator=(const ResourceManager&) = delete;
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager& operator=(ResourceManager&&) = delete;

	private:
		ResourceManager() = default;
		~ResourceManager() = default;

		static ResourceManager* s_manager;

		static std::unordered_map<std::string, Shader> s_shaders;
		static std::unordered_map<std::string, ComputeShader> s_computeShaders;
		static std::unordered_map<std::string, Texture> s_textures;
	};
}
