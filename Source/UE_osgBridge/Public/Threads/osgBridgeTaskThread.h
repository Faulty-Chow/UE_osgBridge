#pragma once
#include "osgBridgeBaseThread.h"

/*
	任务线程
*/
class UE_OSGBRIDGE_API osgBridgeTaskThread :public osgBridgeBaseThread
{
public:
	osgBridgeTaskThread(class osgBridgeThreadPoolBase* pThreadPool, FString threadName);

	void StartThread() = delete;
	void StopThread(bool bShouldWait = true);

	FORCEINLINE bool SetTask(class osgBridgeAsyncTask* task);
	FORCEINLINE class osgBridgeAsyncTask* GetCurrentTask();

	void Wakeup();

	template<typename Lambda>
	static void DoInGameThread(Lambda&& task);

protected:
	virtual uint32 Run() override;

private:
	class osgBridgeThreadPoolBase* _pThreadPool;

	std::atomic_bool _bTaskFinish;
	class osgBridgeAsyncTask* _pTask;

	std::atomic_bool _bTimeToDie;
};