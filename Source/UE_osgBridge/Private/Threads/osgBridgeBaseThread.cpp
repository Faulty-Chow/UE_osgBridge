#include "Threads/osgBridgeBaseThread.h"

osgBridgeBaseThread::osgBridgeBaseThread(FString threadName) :
	_threadName(threadName), _bDead(false), _bActive(false),
	_pThread(nullptr), _pStartEvent(nullptr)
{
	_bAllowLifecycleLog = true;
}

osgBridgeBaseThread::~osgBridgeBaseThread()
{
	_pStartEvent = nullptr;
	delete _pThread;
	_pThread = nullptr;
}

bool osgBridgeBaseThread::Create(FEvent* pStartEvent, uint32 stackSize /*= (32 * 1024)*/, EThreadPriority threadPriority /*= TPri_Normal*/)
{
	_pStartEvent = pStartEvent;
	_pThread = FRunnableThread::Create(this, *_threadName, stackSize, threadPriority);
	if (_pThread)
		return true;
	return false;
}

bool osgBridgeBaseThread::Init()
{
	ThreadLifecycleLog("Init.");
	return FRunnable::Init();
}

void osgBridgeBaseThread::StartThread()
{
	if (_pStartEvent)
	{
		if (_pThread)
		{
			_pStartEvent->Trigger();
			_pStartEvent = nullptr;
		}
	}
}

uint32 osgBridgeBaseThread::Run()
{
	if (_pStartEvent)
		_pStartEvent->Wait();
	ThreadLifecycleLog("Running.");
	return 0;
}

void osgBridgeBaseThread::Stop()
{
	_bActive.store(false);
	ThreadLifecycleLog("Stop.");
}

void osgBridgeBaseThread::Exit()
{
	_bDead.store(true);
	ThreadLifecycleLog("Exit.");
}