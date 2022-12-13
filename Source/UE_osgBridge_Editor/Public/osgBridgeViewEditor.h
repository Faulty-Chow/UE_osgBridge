#pragma once
#include "CoreMinimal.h"
#include "osgBridgeView.h"

class UE_OSGBRIDGE_EDITOR_API osgBridgeViewEditor :public osgBridgeView
{
public:
	osgBridgeViewEditor() :_bTickable(true) {}

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return _bTickable; }
	virtual bool IsTickableInEditor() const override { return true; }

	void SetTickable(bool bTickable) { check(IsInGameThread()); _bTickable = bTickable; }

private:
	bool _bTickable;
};