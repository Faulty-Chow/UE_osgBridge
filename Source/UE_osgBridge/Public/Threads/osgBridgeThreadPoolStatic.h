#pragma once
#include "osgBridgeThreadPoolBase.h"
#include "AsyncTasks/osgBridgeAsyncTaskQueue.h"
#include <mutex>

class UE_OSGBRIDGE_API osgBridgeThreadPoolStatic :public osgBridgeThreadPoolBase
{
public:
	osgBridgeThreadPoolStatic(int32 numTaskThread);
	~osgBridgeThreadPoolStatic();

public:
	virtual bool Create() override;
	virtual void Destroy() override;
	virtual bool ReturnToPool(class osgBridgeTaskThread* pThread) override;

public:
	virtual void Tick(float DeltaTime) override {}
	bool IsTickable() const { return false; }
	bool IsTickableInEditor() const { return false; }

	void Pause();
	void Resume();

	/*CoreThread & GameThread Only.*/
	FORCEINLINE void RequestExecuteTask(class osgBridgeAsyncTask* task);

private:
	int32 _numTaskThreads;
	std::mutex _threadPoolMutex;
	TArray<class osgBridgeTaskThread*> _freeTaskThreadPool;
	TArray<class osgBridgeTaskThread*> _taskThreadPool;

	FEvent* _pTaskThreadsStartEvent;

	// osgBridgeAsyncTaskPriorityQueue _taskQueue;
	TArray<class osgBridgeAsyncTask*> _taskQueue;
	TArray<class osgBridgeAsyncTask*> _executingTaskList;
};