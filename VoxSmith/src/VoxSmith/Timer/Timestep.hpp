#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	class VOX_SMITH_API Timestep
	{
	public:
		Timestep(const float time);
		Timestep(const float start, const float end);
		~Timestep() = default;

		inline float getMilliseconds() const { return m_time; }
		inline float getNanoseconds() const { return m_time / 1e+6; }

	private:
		float m_time;
	};
}