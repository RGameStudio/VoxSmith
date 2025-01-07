#include <glad/glad.h>

#include "stb_image_loader.hpp"

#include "VoxSmith/Logger/Log.hpp"

#include "Texture.hpp"

using namespace VoxSmith;

constexpr uint32_t g_width = 64;
constexpr uint32_t g_height = 64;

Texture::Texture(const char* path)
	: m_type(TextureType::BASE)
	, ID(0)
{

}

Texture::Texture(std::vector<std::string> paths)
	: m_type(TextureType::ARRAY)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, ID);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 7, GL_RGBA8, g_width, g_height, paths.size());
	for (uint8_t iPath = 0; iPath < paths.size(); iPath++)
	{
		int32_t width, height, nrChannels;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(paths[iPath].c_str(), &width, &height, &nrChannels, 0);

		if (!data)
		{
			LOG_CORE_ERROR("TEXTURE::ARRAY::CONSTRUCTION::Could not upload data. {0}", paths[iPath]);
		}

		uint32_t transparency;
		if (std::string(paths[iPath]).find(".jpg") != std::string::npos && data)
		{
			transparency = GL_RGB;
		}
		else
		{
			transparency = GL_RGBA;
		}

		glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY,
			0,
			0,
			0,
			iPath,
			width, height,
			1,
			transparency,
			GL_UNSIGNED_BYTE,
			data);


		stbi_image_free(data);
	}
	
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
}

Texture::Texture(const uint32_t width, const uint32_t height)
	: m_type(TextureType::PIXEL)
{
	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA32F, 
		width, height, 
		0, 
		GL_RGBA, 
		GL_FLOAT, 
		nullptr);
	glBindImageTexture(0, ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

Texture::~Texture()
{

}

void Texture::use() const
{
	switch (m_type)
	{
	case TextureType::BASE:
		glBindTexture(GL_TEXTURE_2D, ID);
		break;

	case TextureType::ARRAY:
		glBindTexture(GL_TEXTURE_3D, ID);
		break;

	case TextureType::PIXEL:
		glBindTexture(GL_TEXTURE_2D, ID);
		break;
	}
}
