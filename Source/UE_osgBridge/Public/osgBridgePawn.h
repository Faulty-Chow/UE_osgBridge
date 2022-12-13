#pragma once
#include "CoreMinimal.h"
#include "osgBridgeView.h"

/*
	可以获取运行时 Pawn的视角信息
*/
class UE_OSGBRIDGE_API osgBridgePawn :public osgBridgeView
{
public:
	osgBridgePawn() = default;
	osgBridgePawn(const osgBridgePawn& other);

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return true; }
};