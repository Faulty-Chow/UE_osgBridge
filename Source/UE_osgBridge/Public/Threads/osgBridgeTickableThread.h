#pragma once
#include "osgBridgeBaseThread.h"
#include <mutex>
#include <condition_variable>

/*
	可以与游戏线程并行Tick的线程基类
	通过Tick和WaitForTick两个静态函数与游戏线程同步
*/
class UE_OSGBRIDGE_API osgBridgeTickableThread :public osgBridgeBaseThread
{
public:
	osgBridgeTickableThread(FString threadName);
	~osgBridgeTickableThread();

	void TimeToDie(bool bShouldWait = true);

	static void Tick(float DeltaTime);
	static void WaitForTickDone();

protected:
	virtual uint32 Run() override;
	virtual void Tick() = 0;

private:
	std::atomic_bool _bTimeToDie;

private:
	static int32 NumTickableThreads;
	static int32 TickDoneCount;

	static std::mutex TickableThreadMutex;
	static std::condition_variable TickableThreadCondition;

	static FEvent* TickDoneEvent;
};