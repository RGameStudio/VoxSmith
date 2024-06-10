#include <glad/glad.h>

#include "stb_image_loader.hpp"

#include "Texture.hpp"

using namespace VoxSmith;

Texture::Texture(const char* path)
	: m_type(TextureType::BASE)
{

}

Texture::Texture(std::vector<const char*> paths)
	: m_type(TextureType::ARRAY)
{

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
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
