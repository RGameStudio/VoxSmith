#include "Timestep.hpp"

using namespace VoxSmith;

Timestep::Timestep(const float time)
	: m_time(time)
{
}

Timestep::Timestep(const float start, const float end)
	: m_time(end - start)
{
}
