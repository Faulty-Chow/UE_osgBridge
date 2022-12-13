#pragma once
#include "CoreMinimal.h"
#include "Threads/osgBridgeTaskThread.h"

/*
	线程池基类
*/
class UE_OSGBRIDGE_API osgBridgeThreadPoolBase
{
public:
	virtual bool Create() = 0;

	virtual void Destroy() = 0;

	virtual void Tick(float DeltaTime) = 0;

	virtual bool ReturnToPool(class osgBridgeTaskThread* pThread);
};