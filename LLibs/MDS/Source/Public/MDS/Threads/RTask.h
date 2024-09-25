#pragma once

#include "MDS/RTool.h"
#include <memory>
#include <functional>
namespace mds
{
	struct RTask
	{
	private:
		bool m_bIsCompleted;
		bool m_bIsTaskSucessfull;

		std::function<void(std::shared_ptr<RTask>)> m_funcMainThreadCallback;
	public:
		virtual ~RTask() {};
	public:

		virtual void operator()() R_PURE;
		inline bool HasMainThreadCallback() const { return (bool)m_funcMainThreadCallback; };
		inline void ExecuteCallback(std::shared_ptr<RTask> InThisTaskSPtr) { m_funcMainThreadCallback(InThisTaskSPtr); };
	protected:
		RTask() : m_bIsCompleted(false), m_bIsTaskSucessfull(false), m_funcMainThreadCallback(nullptr) {};
		RTask(std::function<void(std::shared_ptr<RTask>)>&& InCallback) : m_bIsCompleted(false), m_bIsTaskSucessfull(false), m_funcMainThreadCallback(std::move(InCallback)) {};
		template <typename TFuncCallback>
		RTask(TFuncCallback&& InCallbackDecl) : m_bIsCompleted(false), m_bIsTaskSucessfull(false), m_funcMainThreadCallback(std::forward<TFuncCallback>(InCallbackDecl)) {};

	protected:
		void TaskCompleted(const bool bInTaskSucessfull = true) { m_bIsTaskSucessfull = bInTaskSucessfull; m_bIsCompleted = true; }
	};
};