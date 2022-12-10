#include "Threads/osgBridgeTaskThread.h"
#include "Threads/osgBridgeThreadPoolBase.h"
#include "AsyncTasks/osgBridgeAsyncTask.h"

osgBridgeTaskThread::osgBridgeTaskThread(osgBridgeThreadPoolBase* pThreadPool, FString threadName) :
	osgBridgeBaseThread(threadName), _pThreadPool(pThreadPool), _bTaskFinish(true), _bTimeToDie(false)
{
	_pTask = nullptr;
}

template<typename Lambda>
void osgBridgeTaskThread::DoInGameThread(Lambda&& task)
{
	if (IsInGameThread())
		task();
	else
	{
		FFunctionGraphTask::CreateAndDispatchWhenReady(
			[task]()
			{
				task();
			},
			TStatId(), nullptr, ENamedThreads::GameThread);
	}
}

void osgBridgeTaskThread::StopThread(bool bShouldWait /*= true*/)
{
	_bTimeToDie.store(true);
	_pThread->Suspend(false);
	_pThread->Kill(bShouldWait);
}

void osgBridgeTaskThread::Wakeup()
{
	if (_bTimeToDie.load() || _bActive.load() || _bDead.load())
		return;
	_pThread->Suspend(false);
}

bool osgBridgeTaskThread::SetTask(osgBridgeAsyncTask* task)
{
	if (_bTaskFinish.load())
	{
		_pTask = task;
		return true;
	}
	return false;
}

osgBridgeAsyncTask* osgBridgeTaskThread::GetCurrentTask()
{
	if (_bActive.load())
		return _pTask;
	return nullptr;
}

uint32 osgBridgeTaskThread::Run()
{
	if (_pStartEvent)
		_pStartEvent->Wait();
	_pStartEvent = nullptr;
	_pTask = nullptr;
	while(!_bTimeToDie.load())
	{
		_bActive.store(!_pThreadPool->ReturnToPool(this));
		if (_bActive.load())
		{
			check(_pTask);
			_bTaskFinish.store(false);
			_pTask->Execute();
			_bTaskFinish.store(true);
		}
		else
		{
			_pThread->Suspend(true);
		}
	}
	return 0;
}