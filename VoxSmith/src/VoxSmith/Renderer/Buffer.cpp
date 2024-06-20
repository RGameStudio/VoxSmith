#include "Mesh.hpp"

#include "Buffer.hpp"

using namespace VoxSmith;

void Buffer::bind() const
{
	glBindVertexArray(VAO);
}
