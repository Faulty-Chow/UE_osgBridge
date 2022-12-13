/*
 * @Author: Sherry-Chow 874318263@qq.com
 * @Date: 2022-12-12 14:14:24
 * @LastEditors: Sherry-Chow 874318263@qq.com
 * @LastEditTime: 2022-12-12 14:17:23
 * @FilePath: \Source\UE_osgBridge_Editor\Private\osgBridgeEditorMeshActor.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "osgBridgeEditorMeshActor.h"
#include "osgBridgeViewEditor.h"
#include "osgBridgeEngineSubsystem.h"

AosgBridgeEditorMeshActor::AosgBridgeEditorMeshActor():
	AosgBridgeMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;

	_pView = new osgBridgeViewEditor;
}

void AosgBridgeEditorMeshActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->SetMeshActor(this);
	UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->LoadNodeFiles("F:\\FaultyChow\\terra_osgbs(2)\\terra_osgbs");
}

void AosgBridgeEditorMeshActor::BeginPlay()
{
	AosgBridgeMeshActor::BeginPlay();
	PrimaryActorTick.bCanEverTick = false;
	dynamic_cast<osgBridgeViewEditor*>(_pView)->SetTickable(false);
}

void AosgBridgeEditorMeshActor::Tick(float deltaTime)
{
	AosgBridgeMeshActor::Tick(deltaTime);
}