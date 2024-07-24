#include "../Renderer/Renderer.hpp"
#include "../Shader/Shader.hpp"

#include "VoxSmith/Chunk/Voxel.hpp"

#include "Mesh.hpp"

using namespace VoxSmith;

#define GREEDY_2D 0
#ifndef GREEDY_2D
void Mesh::bakeGreedy(const std::vector<Voxel>& voxels, const float cSize)
	std::vector<bool> mask;
	mask.reserve(cSize.x * cSize.y);

	for (int32_t y = 0; y < cSize.y; y++)
	{
		for (int32_t x = 0; x < cSize.x; x++)
		{
			if (voxels.at(getId({ x, y, 0 }, cSize)).type == VoxelType::Opaque)
			{
				mask.push_back(true);
			}
		}
	}

	int32_t n = 0;
	for (int32_t y = 0; y < cSize.y; y++)
	{
		for (int32_t x = 0; x < cSize.x; x)
		{
			if (mask.at(n))
			{
				int32_t width;
				for (width = 1;
					x + width < cSize.x && voxels.at(getId({ x + width, y, 0 }, cSize)).type != VoxelType::Empty;
					width++)
				{
				}

				bool done = false;
				int32_t height;
				for (height = 1; y + height < cSize.y; height++)
				{
					for (int32_t w = 0; w < width; w++)
					{
						int32_t id = n + w + height * cSize.x;
						if (!mask.at(id))
						{
							done = true;
							break;
						}
					}
					if (done)
					{
						break;
					}
				}

				addQuadFace({ x, y, 0 }, { width, 0, 0 }, { 0, height, 0 });

				for (int32_t h = 0; h < height; h++)
				{
					for (int32_t w = 0; w < width; w++)
					{
						auto maskId = n + w + h * cSize.x;
						mask.at(maskId) = false;
					}
				}

				x += width;
				n += width;
			}
			else
			{
				x++;
				n++;
			}
		}
	}
#endif

void Mesh::draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader) const
{
	if (renderer == nullptr)
	{
		return;
	}

	renderer->draw(m_buffer, shader, m_vertexCount);
}

void Mesh::loadToBuffer(const std::vector<Vertex>& vertices)
{
	m_vertexCount = vertices.size();
	m_buffer.init(vertices);
}
