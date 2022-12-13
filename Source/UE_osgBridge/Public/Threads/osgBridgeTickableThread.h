#pragma once
#include "osgBridgeBaseThread.h"
#include <mutex>
#include <condition_variable>

/*
	��������Ϸ�̲߳���Tick���̻߳���
	ͨ��Tick��WaitForTick������̬��������Ϸ�߳�ͬ��
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