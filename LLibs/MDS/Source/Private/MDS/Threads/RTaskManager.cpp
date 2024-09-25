#include "MDS/Threads/RTaskManager.h"

void mds::RTaskManager::EnqueueTask(std::shared_ptr<RTask> InTask)
{
	std::unique_lock<std::mutex>lock(m_taskLock);
	m_tasks.push(InTask);
	lock.unlock();
	m_cvHasTask.notify_one();
}

void mds::RTaskManager::EnqueueTasks(std::shared_ptr<RTask>* InTasks, const size_t InCount)
{
	if (InCount < 1) { return; }
	std::unique_lock<std::mutex>lock(m_taskLock);
	for (size_t i = 0; i < InCount; ++i)
	{
		m_tasks.push(InTasks[i]);
	}
	lock.unlock();
	if (InCount >= m_workers.size())
	{
		m_cvHasTask.notify_all();
	}
	else
	{
		for (size_t i = 0; i < InCount; ++i)
		{
			m_cvHasTask.notify_one();
		}
	}
}

void mds::RTaskManager::CallMainThreadCallbacks()
{
	if (m_waitingCallbackTasks.size() > 0)
	{
		std::unique_lock<std::mutex>lockCallback(m_taskCallbackLock);
		size_t c = m_waitingCallbackTasks.size();
		for (size_t i = 0; i < c; ++i)
		{
			auto task = std::move(m_waitingCallbackTasks.front());
			m_waitingCallbackTasks.pop();
			task->ExecuteCallback(task);
		}
		lockCallback.unlock();
	}
}

bool mds::RTaskManager::Init(const size_t InNbThread)
{
	auto ThreadFunc = [this]() -> void
		{
			do
			{
				std::unique_lock<std::mutex> lock(m_taskLock);
				m_cvHasTask.wait(lock, [this] { return m_bIsShuttingDown || !m_tasks.empty(); });
				if (m_bIsShuttingDown && m_tasks.empty())
				{
					return;
				}
				auto task = std::move(m_tasks.front());
				m_tasks.pop();
				lock.unlock();
				task->operator()();

				if (task->HasMainThreadCallback())
				{
					std::lock_guard<std::mutex>lockCallback(m_taskCallbackLock);
					m_waitingCallbackTasks.push(task);
				}
			} while (true);
		};
	//m_workers.
	m_workers.reserve(InNbThread);
	for (size_t i = 0; i < InNbThread; ++i)
	{
		m_workers.push_back(std::thread(ThreadFunc));
	}
	m_bIsInitialized = true;
	return true;
}

void mds::RTaskManager::Shutdown()
{
	if (m_bIsInitialized)
	{
		if (m_workers.size() > 0)
		{
			m_taskLock.lock();
			m_bIsShuttingDown = true;
			m_taskLock.unlock();
			m_cvHasTask.notify_all();
			for (std::thread& worker : m_workers)
			{
				worker.join();
			}
		}
	}
	m_bIsInitialized = m_bIsShuttingDown = false;
}

mds::RTaskManager::~RTaskManager()
{
	if (m_bIsInitialized && !m_bIsShuttingDown)
	{
		Shutdown();
	}
}
