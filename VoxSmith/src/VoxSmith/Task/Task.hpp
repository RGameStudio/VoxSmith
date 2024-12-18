#include <future>
#include <vector>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	template <typename T = void>
	class Task
	{
	public:
		Task(std::future<T>&& future);
		Task(std::future<T>& future);

		bool ready() const;
		bool launched() const;
		T get();

		Task() = default;
		Task(Task&&) = default;
		Task& operator=(Task&&) = default;

	private:
		std::future<T> m_future;

		bool m_launched = false;
	};

	template <typename T>
	Task<T>::Task(std::future<T>& future)
		: m_future(std::move(future))
		, m_launched(true)
	{}

	template <typename T>
	Task<T>::Task(std::future<T>&& future)
		: m_future(std::move(future))
		, m_launched(true)
	{}

	template <typename T>
	bool Task<T>::ready() const
	{
		if (!m_launched)
		{
			LOG_CORE_WARN("Task is not even launched yet!");
			return m_launched;
		}

		return m_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
	}

	template <typename T>
	bool Task<T>::launched() const
	{
		return m_launched;
	}

	template <typename T>
	T Task<T>::get()
	{
		return m_future.get();
		m_launched = false;
	}
}

