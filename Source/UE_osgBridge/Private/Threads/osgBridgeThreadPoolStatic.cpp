#include "Threads/osgBridgeThreadPoolStatic.h"
#include "AsyncTasks/osgBridgeAsyncTask.h"
#include "Threads/osgBridgeTaskThread.h"
#include "Database/osgBridgeNode.h"

#include <cstdlib>

osgBridgeThreadPoolStatic::osgBridgeThreadPoolStatic(int32 numTaskThread):
	_numTaskThreads(numTaskThread)
{
	_pTaskThreadsStartEvent = FPlatformProcess::GetSynchEventFromPool(true);
}

osgBridgeThreadPoolStatic::~osgBridgeThreadPoolStatic()
{
	FPlatformProcess::ReturnSynchEventToPool(_pTaskThreadsStartEvent);
	_pTaskThreadsStartEvent = nullptr;
}

bool osgBridgeThreadPoolStatic::Create()
{
	for (int32 i = 0; i < _numTaskThreads; i++)
	{
		FString threadName = "osgBridgeTaskThread_" + FString::FromInt(i);
		osgBridgeTaskThread* pTaskThread = new osgBridgeTaskThread(this, threadName);
		check(pTaskThread->Create(_pTaskThreadsStartEvent));
		_taskThreadPool.Add(pTaskThread);
	}
	_pTaskThreadsStartEvent->Trigger();
	return true;
}

void osgBridgeThreadPoolStatic::Destroy()
{
	_pTaskThreadsStartEvent->Trigger();
	for (auto& pTaskThread: _taskThreadPool)
	{
		pTaskThread->StopThread();
		delete pTaskThread;
		pTaskThread = nullptr;
	}
	_taskThreadPool.Empty();
}

bool osgBridgeThreadPoolStatic::ReturnToPool(osgBridgeTaskThread* pThread)
{
	check(pThread);
	std::unique_lock<std::mutex> lock(_threadPoolMutex);
	if (pThread->GetCurrentTask())
		check(_executingTaskList.Remove(pThread->GetCurrentTask()) == 1);
	if (_taskQueue.Num() == 0)
	{
		if (!_freeTaskThreadPool.Contains(pThread))
			_freeTaskThreadPool.Add(pThread);
		return true;
	}
	else
	{
		osgBridgeAsyncTask* task = _taskQueue[0];
		check(pThread->SetTask(task));
		_taskQueue.RemoveAt(0);
		_executingTaskList.Emplace(task);
		_freeTaskThreadPool.Remove(pThread);
		return false;
	}
}

void osgBridgeThreadPoolStatic::RequestExecuteTask(osgBridgeAsyncTask* task)
{
	if (task)
	{
		std::unique_lock<std::mutex> lock(_threadPoolMutex);
		if (_taskQueue.Find(task) == INDEX_NONE && _executingTaskList.Find(task) == INDEX_NONE)
			_taskQueue.Emplace(task);
		if (_freeTaskThreadPool.Num() > 0)
		{
			int32 index = rand() % _freeTaskThreadPool.Num();
			_freeTaskThreadPool[index]->Wakeup();
		}
	}
}
