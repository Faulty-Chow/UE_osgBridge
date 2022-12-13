#include "Threads/osgBridgeTickableThread.h"

int32 osgBridgeTickableThread::NumTickableThreads = 0;
int32 osgBridgeTickableThread::TickDoneCount = 0;

std::mutex osgBridgeTickableThread::TickableThreadMutex;
std::condition_variable osgBridgeTickableThread::TickableThreadCondition;

FEvent* osgBridgeTickableThread::TickDoneEvent = FPlatformProcess::GetSynchEventFromPool(true);

void osgBridgeTickableThread::Tick(float DeltaTime)
{
	std::unique_lock<std::mutex> lock(TickableThreadMutex);
	check(TickDoneCount == NumTickableThreads);
	TickDoneCount = 0;
	TickDoneEvent->Trigger();
}

void osgBridgeTickableThread::WaitForTickDone()
{
	std::unique_lock<std::mutex> lock(TickableThreadMutex);
	while (TickDoneCount != NumTickableThreads)
		TickableThreadCondition.wait(lock);
	check(TickDoneCount == NumTickableThreads);
}

osgBridgeTickableThread::osgBridgeTickableThread(FString threadName) :
	osgBridgeBaseThread(threadName), _bTimeToDie(false)
{
	std::unique_lock<std::mutex> lock(TickableThreadMutex);
	++NumTickableThreads;
}

osgBridgeTickableThread::~osgBridgeTickableThread()
{
	std::unique_lock<std::mutex> lock(TickableThreadMutex);
	--NumTickableThreads;
}

void osgBridgeTickableThread::TimeToDie(bool bShouldWait /*= true*/)
{
	_bTimeToDie.store(true);
	_pThread->Suspend(false);
	_pThread->Kill(bShouldWait);
}

uint32 osgBridgeTickableThread::Run()
{
	_pStartEvent->Wait();
	while (!_bTimeToDie.load())
	{
		Tick();
		std::unique_lock<std::mutex> lock(TickableThreadMutex);
		++TickDoneCount;
		if (TickDoneCount == NumTickableThreads)
			TickableThreadCondition.notify_all();
		else
			check(TickDoneEvent->Wait() == false);
	}
	return 0;
}