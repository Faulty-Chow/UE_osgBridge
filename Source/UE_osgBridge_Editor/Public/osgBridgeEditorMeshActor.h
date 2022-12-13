/*
 * @Author: Sherry-Chow 874318263@qq.com
 * @Date: 2022-12-12 14:14:11
 * @LastEditors: Sherry-Chow 874318263@qq.com
 * @LastEditTime: 2022-12-12 14:16:57
 * @FilePath: \Source\UE_osgBridge_Editor\Public\osgBridgeEditorMeshActor.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include "CoreMinimal.h"
#include "osgBridgeMeshActor.h"
#include "osgBridgeEditorMeshActor.generated.h"


UCLASS()
class UE_OSGBRIDGE_EDITOR_API AosgBridgeEditorMeshActor : public AosgBridgeMeshActor
{
	GENERATED_BODY()
public:
	AosgBridgeEditorMeshActor();

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
};