#include <future>
#include <vector>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	template <typename T, typename U>
	class VOX_SMITH_API Task
	{
	public:
		Task(std::future<T>&& future);
		Task(std::future<T>& future);

		bool ready() const;
		bool isBufferEmpty() const;
		void updateBuffer(U& data);
		T get();

		Task(Task&&) = default;
		Task& operator=(Task&&) = default;

	private:
		std::future<T> m_future;
		std::vector<U> m_buffer;
	};

	template <typename T, typename U>
	Task<T, U>::Task(std::future<T>& future)
		: m_future(std::move(future))
	{}

	template <typename T, typename U>
	Task<T, U>::Task(std::future<T>&& future)
		: m_future(future)
	{}

	template <typename T, typename U>
	bool Task<T, U>::ready() const
	{
		return m_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
	}

	template <typename T, typename U>
	bool Task<T, U>::isBufferEmpty() const
	{
		return m_buffer.empty();
	}

	template <typename T, typename U>
	void Task<T, U>::updateBuffer(U& data)
	{
		m_buffer.push_back(data);
	}

	template <typename T, typename U>
	T Task<T, U>::get()
	{
		return m_future.get();
	}

	template <typename T = void>
	class VOX_SMITH_API SimpleTask
	{
	public:
		SimpleTask(std::future<T>&& future);
		SimpleTask(std::future<T>& future);

		bool ready() const;
		T get();
		
		SimpleTask(SimpleTask&&) = default;
		SimpleTask& operator=(SimpleTask&&) = default;

	private:
		std::future<T> m_future;
	};
	
	template <typename T>
	SimpleTask<T>::SimpleTask(std::future<T>& future)
		: m_future(std::move(future))
	{}

	template <typename T>
	SimpleTask<T>::SimpleTask(std::future<T>&& future)
		: m_future(future)
	{}

	template <typename T>
	bool SimpleTask<T>::ready() const
	{
		return m_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
	}

	template <typename T>
	T SimpleTask<T>::get()
	{
		return m_future.get();
	}
}
