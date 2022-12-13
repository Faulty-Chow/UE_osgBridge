#include "osgBridgeMeshActor.h"
#include "osgBridgeView.h"
#include "Database/osgBridgeMeshSection.h"
#include "Database/osgBridgeNode.h"
#include "Database/osgBridgePagedLOD.h"
#include "Database/osgBridgeGeometry.h"
#include "Components/RuntimeMeshComponentStatic.h"
#include "osgBridgeEngineSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"

AosgBridgeMeshActor::AosgBridgeMeshActor() :
	_nextSectionID(0), _nextMaterialSlot(0)
{
	PrimaryActorTick.bCanEverTick = true;

	_pView = nullptr;
	_pRuntimeMeshComponentStatic = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("RuntimeMeshComponentStatic"));
	_pDefaultMaterial = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
		TEXT("Material'/UE_osgBridge/UE_osgBridge_DefaultMaterial_4_27.UE_osgBridge_DefaultMaterial_4_27'")));
	check(_pDefaultMaterial);
}

int32 AosgBridgeMeshActor::SetupMaterialSlot(UMaterialInstanceDynamic* material)
{
	int32 materialSlot = GetMaterialSlotFromPool();
	SetupMaterialSlot(materialSlot, material);
	return materialSlot;
}

void AosgBridgeMeshActor::SetupMaterialSlot(int32 materialSlot, UMaterialInterface* material)
{
	check(IsInGameThread());
	FString slotName = "Material_" + FString::FromInt(materialSlot);
	_pRuntimeMeshComponentStatic->SetupMaterialSlot(materialSlot, FName(slotName), material);
}

void AosgBridgeMeshActor::ViewUpdateRequest(std::vector<class osgBridgeMeshSection*>& addToViewMeshSections, std::vector<int32>& removeFromViewMeshSections)
{
	std::unique_lock<std::mutex> lock(_viewUpdateMutex);
	for (auto& meshSection : addToViewMeshSections)
	{
		if (meshSection == nullptr)
			continue;
		check(meshSection->IsValid());
		check(meshSection->_materialSlotIndex != INDEX_NONE);
		if (meshSection->_sectionID == INDEX_NONE)
			meshSection->_sectionID = GetSectionIDFromPool();
		if (_visibleMeshSections.find(meshSection->_sectionID) == _visibleMeshSections.end())
			_meshSectionsToRender.emplace_back(meshSection);
	}
	int64 frameNumber = UKismetSystemLibrary::GetFrameCount();
	for (auto& sectionID : removeFromViewMeshSections)
	{
		if (sectionID != INDEX_NONE && _visibleMeshSections.find(sectionID) != _visibleMeshSections.end())
			_meshSectionsToRemove.emplace_back(std::pair<int64, int32>(frameNumber, sectionID));
	}
}

int32 AosgBridgeMeshActor::CreateSectionGameThread(osgBridgeMeshSection* meshSection)
{
	check(IsInGameThread());
	if (meshSection && meshSection->IsValid())
	{
		if (meshSection->_materialSlotIndex == INDEX_NONE)
			meshSection->_materialSlotIndex = SetupMaterialSlot(meshSection->_pMaterial.Get());
		if (meshSection->_sectionID == INDEX_NONE)
			meshSection->_sectionID = GetSectionIDFromPool();
		std::unique_lock<std::mutex> lock(_viewUpdateMutex);
		if (_visibleMeshSections.find(meshSection->_sectionID) == _visibleMeshSections.end())
		{
			CreateSectionFromComponents(meshSection);
			_visibleMeshSections.insert(meshSection->_sectionID);
		}
		return meshSection->_sectionID;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Try to create an invalid MeshSection."));
		return INDEX_NONE;
	}
}

void AosgBridgeMeshActor::CreateSectionFromComponents(osgBridgeMeshSection* meshSection)
{
	// UE_LOG(LogTemp, Log, TEXT("CreateSection %d"), meshSection->_sectionID);
	_pRuntimeMeshComponentStatic->CreateSectionFromComponents(
		0, meshSection->_sectionID, meshSection->_materialSlotIndex,
		meshSection->_vertices, meshSection->_triangles, meshSection->_normals,
		meshSection->_UVs, meshSection->_vertexColors, meshSection->_tangents,
		ERuntimeMeshUpdateFrequency::Frequent, false);
}

void AosgBridgeMeshActor::RemoveSectionGameThread(int32 meshSectionID)
{
	check(IsInGameThread());
	std::unique_lock<std::mutex> lock(_viewUpdateMutex);
	if (_visibleMeshSections.find(meshSectionID) != _visibleMeshSections.end())
	{
		RemoveSection(meshSectionID);
		_visibleMeshSections.erase(meshSectionID);
	}
}

void AosgBridgeMeshActor::RemoveSection(int32 meshSectionID)
{
	// UE_LOG(LogTemp, Log, TEXT("RemoveSection %d"), meshSectionID);
	_pRuntimeMeshComponentStatic->RemoveSection(0, meshSectionID);
}

void AosgBridgeMeshActor::MarkMaterialPendingKill(int32 materialSlot)
{
	if (materialSlot == INDEX_NONE)
		return;
	std::unique_lock<std::mutex> lock(_pendingKillMaterialsMutex);
	_pendingKillMaterials.emplace_back(std::pair<int64, int32>(UKismetSystemLibrary::GetFrameCount(), materialSlot));
}

bool AosgBridgeMeshActor::IsMeshSectionVisible(int32 sectionID)
{
	std::unique_lock<std::mutex> lock(_viewUpdateMutex);
	if (_visibleMeshSections.find(sectionID) == _visibleMeshSections.end())
		return false;
	else
		return false;
}

osgBridgeView* AosgBridgeMeshActor::GetViewInfo()
{
	return _pView;
}

void AosgBridgeMeshActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	
	int64 frameNumber = UKismetSystemLibrary::GetFrameCount();
	{
		std::unique_lock<std::mutex> lock(_viewUpdateMutex);
		for (auto& meshSection : _meshSectionsToRender)
		{
			CreateSectionFromComponents(meshSection);
			_visibleMeshSections.insert(meshSection->_sectionID);
		}
		_meshSectionsToRender.clear();
		
		for (auto itr = _meshSectionsToRemove.begin(); itr != _meshSectionsToRemove.end();)
		{
			if (frameNumber - itr->first > UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetFrameBuffer())
			{
				RemoveSection(itr->second);
				_visibleMeshSections.erase(itr->second);
				itr = _meshSectionsToRemove.erase(itr);
			}
			else
				break;
		}
	}

	{
		std::unique_lock<std::mutex> lock(_pendingKillMaterialsMutex);
		for (auto itr = _pendingKillMaterials.begin(); itr != _pendingKillMaterials.end();)
		{
			if (frameNumber - itr->first > UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetFrameBuffer())
			{
				SetupMaterialSlot(itr->second, _pDefaultMaterial);
				ReturnMaterialSlotToPool(itr->second);
				itr = _pendingKillMaterials.erase(itr);
			}
			else
				break;
		}
	}

	GEngine->ForceGarbageCollection(true);
}

int32 AosgBridgeMeshActor::GetSectionIDFromPool()
{
	int32 sectionID = INDEX_NONE;
	std::unique_lock<std::mutex> lock(_meshSectionIDPoolMutex);
	if (_meshSectionIDPool.Num() == 0)
		sectionID = _nextSectionID++;
	else
	{
		sectionID = _meshSectionIDPool[0];
		_meshSectionIDPool.RemoveAt(0);
	}
	return sectionID;
}

void AosgBridgeMeshActor::ReturnSectionIDToPool(int32& sectionID)
{
	if (sectionID < 0)
		return;
	std::unique_lock<std::mutex> lock(_meshSectionIDPoolMutex);
	_meshSectionIDPool.Add(sectionID);
	sectionID = INDEX_NONE;
}

int32 AosgBridgeMeshActor::GetMaterialSlotFromPool()
{
	int32 materialSlot = INDEX_NONE;
	std::unique_lock<std::mutex> lock(_materialSlotPoolMutex);
	if (_materialSlotPool.Num() == 0)
		materialSlot = _nextMaterialSlot++;
	else
	{
		materialSlot = _materialSlotPool[0];
		_materialSlotPool.RemoveAt(0);
	}
	return materialSlot;
}

void AosgBridgeMeshActor::ReturnMaterialSlotToPool(int32 materialSlot)
{
	if (materialSlot == INDEX_NONE)
		return;
	std::unique_lock<std::mutex> lock(_materialSlotPoolMutex);
	_materialSlotPool.Add(materialSlot);
	materialSlot = INDEX_NONE;
}