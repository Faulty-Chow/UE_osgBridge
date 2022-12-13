#pragma once
#include <atomic>

#define ThreadLifecycleLog(Lifecycle) if(_bAllowLifecycleLog)UE_LOG(LogTemp,Warning,TEXT("%s : %s"),*_threadName,*FString(Lifecycle))

/*
	线程基类
*/
class UE_OSGBRIDGE_API osgBridgeBaseThread : public FRunnable
{
public:
	osgBridgeBaseThread(FString threadName);
	virtual ~osgBridgeBaseThread();

	FORCEINLINE void AllowThreadLifecycleLog(bool bAllow) { _bAllowLifecycleLog = bAllow; }

	bool Create(FEvent* pStartEvent, uint32 stackSize = (32 * 1024), EThreadPriority threadPriority = TPri_Normal);
	void StartThread();

public:
	__forceinline bool IsDone() { return _bDead.load(); }
	__forceinline bool IsActive() { return _bActive.load(); }

protected:
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

protected:
	FString _threadName;
	FRunnableThread* _pThread;

	FEvent* _pStartEvent;

	std::atomic_bool _bActive;
	std::atomic_bool _bDead;

	bool _bAllowLifecycleLog;
};