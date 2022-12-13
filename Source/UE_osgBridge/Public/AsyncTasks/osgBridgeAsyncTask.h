#pragma once
#include "CoreMinimal.h"

/*
	AsyncTask base class
*/

class UE_OSGBRIDGE_API osgBridgeAsyncTask
{
public:
	virtual void Execute() = 0;
	virtual void Abandon() = 0;
	int32 GetPriority(bool bUpdate = false);
	/* If you add the task to ordered container, this function may cause undefined behavior. */
	virtual void UpdatePriority() = 0;

	virtual bool operator<(const osgBridgeAsyncTask& rhs) const;

protected:
	int32 _priority;
};