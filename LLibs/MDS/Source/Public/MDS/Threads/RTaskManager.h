#pragma once

#include "MDS/Tools/Templates/Singleton.h"
#include "MDS/Threads/RTask.h"

#include <queue>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace mds
{
	class RTaskManager : public Singleton<RTaskManager>
	{
	protected:
		bool m_bIsShuttingDown = false;
		bool m_bIsInitialized = false;
		std::vector<std::thread> m_workers;
		std::queue<std::shared_ptr<RTask>> m_tasks;
		std::queue<std::shared_ptr<RTask>> m_waitingCallbackTasks;
		std::mutex m_taskLock;
		std::mutex m_taskCallbackLock;
		std::condition_variable m_cvHasTask;
	public:

		void EnqueueTask(std::shared_ptr<RTask> InTask);
		inline void EnqueueTasks(std::vector<std::shared_ptr<RTask>> InTasks)
		{ if (InTasks.size() > 0) { EnqueueTasks(&InTasks[0], InTasks.size()); } };
		void EnqueueTasks(std::shared_ptr<RTask>* InTasks, const size_t InCount);


		void CallMainThreadCallbacks();
		bool Init(const size_t InNbThread);
		void Shutdown();

		virtual ~RTaskManager();
	protected:
		friend class Singleton<RTaskManager>;
	};
};