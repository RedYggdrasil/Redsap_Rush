#pragma once 
#include "MDS/Threads/RTask.h"


namespace mds
{
	struct RTaskFunction : public RTask
	{
	protected:
		std::function<void()> m_funcTask;
	public:
		R_VIRTUAL_IMPLICIT ~RTaskFunction() R_OVERRIDE_IMPLICIT {};
	public:

		virtual void operator()() override { m_funcTask(); TaskCompleted(true); }
	public:
		template <typename TFunc>
		RTaskFunction(TFunc&& InFunctionDecl) : RTask(), m_funcTask(std::forward<TFunc>(InFunctionDecl)) {}
		template <typename TFunc, typename TFuncCallback>
		RTaskFunction(TFunc&& InFunctionDecl, TFuncCallback&& InCallbackDecl) : RTask(std::forward<TFuncCallback>(InCallbackDecl)), m_funcTask(std::forward<TFunc>(InFunctionDecl)) {}


		RTaskFunction(std::function<void()>&& InFunction) : RTask(), m_funcTask(std::move(InFunction)) {}
		RTaskFunction(std::function<void()>&& InFunction, std::function<void(std::shared_ptr<RTask>)>&& InCallback) : RTask(std::move(InCallback)), m_funcTask(std::move(InFunction)) {}
	};
};