#pragma once
#include "osgBridgeThreadPoolBase.h"
#include "AsyncTasks/osgBridgeAsyncTaskQueue.h"
#include <mutex>

/*
	全局单例(因为托管与EngineSubsystem)线程池 主要处理3类异步任务：场景更新，IO读取，内存拷贝
*/
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