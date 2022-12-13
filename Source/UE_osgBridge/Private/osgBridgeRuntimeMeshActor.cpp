/*
 * @Author: Sherry-Chow 874318263@qq.com
 * @Date: 2022-12-12 14:13:52
 * @LastEditors: Sherry-Chow 874318263@qq.com
 * @LastEditTime: 2022-12-12 14:16:24
 * @FilePath: \Source\UE_osgBridge\Private\osgBridgeRuntimeMeshActor.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "osgBridgeRuntimeMeshActor.h"
#include "osgBridgePawn.h"
#include "osgBridgeEngineSubsystem.h"

AosgBridgeRuntimeMeshActor::AosgBridgeRuntimeMeshActor():
	AosgBridgeMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;
	_pView = new osgBridgePawn;
}

void AosgBridgeRuntimeMeshActor::BeginPlay()
{
	AosgBridgeMeshActor::BeginPlay();
	UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->SetMeshActor(this);
	UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->LoadNodeFiles("F:\\meicheng\\osgb\\Data");
}

void AosgBridgeRuntimeMeshActor::Tick(float deltaTime)
{
	AosgBridgeMeshActor::Tick(deltaTime);
	UE_LOG(LogTemp, Warning, TEXT("AosgBridgeRuntimeMeshActor::Tick( %f )"), deltaTime);
}