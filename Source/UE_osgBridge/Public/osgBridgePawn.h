#pragma once
#include "CoreMinimal.h"
#include "osgBridgeView.h"

class UE_OSGBRIDGE_API osgBridgePawn :public osgBridgeView
{
public:
	osgBridgePawn() = default;
	osgBridgePawn(const osgBridgePawn& other);

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return true; }
	virtual bool IsTickableInEditor() const override { return false; }
};