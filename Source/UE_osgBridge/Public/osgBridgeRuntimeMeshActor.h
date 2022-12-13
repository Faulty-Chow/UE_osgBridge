/*
 * @Author: Sherry-Chow 874318263@qq.com
 * @Date: 2022-12-12 14:13:33
 * @LastEditors: Sherry-Chow 874318263@qq.com
 * @LastEditTime: 2022-12-12 14:15:07
 * @FilePath: \Source\UE_osgBridge\Public\osgBridgeRuntimeMeshActor.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include "CoreMinimal.h"
#include "osgBridgeMeshActor.h"
#include "osgBridgeRuntimeMeshActor.generated.h"

/*
	Runtime模式 用于显示模型的Actor
*/
UCLASS()
class UE_OSGBRIDGE_API AosgBridgeRuntimeMeshActor : public AosgBridgeMeshActor
{
	GENERATED_BODY()
public:
	AosgBridgeRuntimeMeshActor();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float deltaTime) override;
};