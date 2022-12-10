#pragma once
#include "CoreMinimal.h"
#include <mutex>

#undef  PI
#define PI 					(3.1415926535897932f)

class UE_OSGBRIDGE_API osgBridgeView
{
public:
	virtual void Tick(float DeltaTime) = 0;
	virtual bool IsTickable() const { return false; };
	virtual bool IsTickableInEditor() const { return false; }

	float GetSphereBoundsPixelSizeInView(const FBoxSphereBounds& sphereBounds, bool bABS = true);
	bool IsSphereBoundsInView(const FBoxSphereBounds& sphereBounds);

protected:
	std::mutex _rwMutex;
	bool _bValid;

	FDateTime _lastTickTimeStamp;
	int64 _lastTickFrameNumber;

	FVector _viewOrigin;
	FMatrix _projectionMatrix;
	FMatrix _viewProjectionMatrix;
	FIntRect _viewSize;
};