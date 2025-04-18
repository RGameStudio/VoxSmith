#include "VoxSmith/Shader/Shader.hpp"
#include "VoxSmith/Texture/Texture.hpp"

#include "VoxSmith/Logger/Log.hpp"

#include "ResourcesLists.hpp"
#include "ResourceManager.hpp"

using namespace VoxSmith;

ResourceManager* ResourceManager::s_manager = nullptr;

std::unordered_map<std::string, Shader> ResourceManager::s_shaders = {};
std::unordered_map<std::string, ComputeShader> ResourceManager::s_computeShaders = {};
std::unordered_map<std::string, Texture> ResourceManager::s_textures = {};

ResourceManager::~ResourceManager()
{

}

void ResourceManager::initShaders()
{
	for (const auto& [key, value] : s_shaderPaths)
	{
		setShader(key, Shader(value));
	}
}

void ResourceManager::initTextureList()
{
	setTexture(s_voxelTextureList, Texture(s_texturePaths));
}

ResourceManager& ResourceManager::getInstance()
{
	if (s_manager == nullptr)
	{
		s_manager = new ResourceManager();
	}

	return *s_manager;
}

void ResourceManager::setShader(const std::string& sName, Shader& shader)
{
	if (s_shaders.find(sName) != s_shaders.end())
	{
		LOG_CORE_ERROR("RESOURCE_MANAGER::SHADERS::ERROR::Shader name '{}' is already occupied", sName);
		return;
	}

	s_shaders[sName] = shader;
}

void ResourceManager::setCShader(const std::string& cName, ComputeShader& shader)
{
	if (s_computeShaders.find(cName) != s_computeShaders.end())
	{
		LOG_CORE_ERROR("RESOURCE_MANAGER::COMPUTE_SHADERS::ERROR::Shader name '{}' is already occupied", cName);
		return;
	}

	s_computeShaders[cName] = shader;
}

void ResourceManager::setTexture(const std::string& tName, Texture& texture)
{
	if (s_textures.find(tName) != s_textures.end())
	{
		LOG_CORE_ERROR("RESOURCE_MANAGER::TEXTURES::ERROR::Texture name '{}' is already occupied", tName);
		return;
	}

	s_textures[tName] = texture;
}

Shader& ResourceManager::getShader(const std::string& sName) const
{
	if (s_shaders.find(sName) == s_shaders.end())
	{
		LOG_CORE_ERROR("RESOURCE_MANAGER::SHADERS:ERROR::No such shader '{}'", sName);
	}

	return s_shaders[sName];
}

Texture& ResourceManager::getTexture(const std::string& tName) const
{
	if (s_textures.find(tName) == s_textures.end())
	{
		LOG_CORE_ERROR("RESOURCE_MANAGER::TEXTURES:ERROR::No such texture '{}'", tName);
	}

	return s_textures[tName];
}

ComputeShader& ResourceManager::getCShader(const std::string& cName) const
{
	if (s_computeShaders.find(cName) == s_computeShaders.end())
	{
		LOG_CORE_ERROR("RESOURCE_MANAGER::COMPUTE_SHADERS::ERROR::No such shader '{}'", cName);
	}

	return s_computeShaders[cName];
}
