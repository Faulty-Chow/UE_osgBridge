#pragma once
#include <mutex>
#include <vector>
#include <set>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "osgBridgeMeshActor.generated.h"

UCLASS()
class UE_OSGBRIDGE_API AosgBridgeMeshActor : public AActor
{
	GENERATED_BODY()
public:
	AosgBridgeMeshActor();
	~AosgBridgeMeshActor() = default;

	virtual void Tick(float deltaTime) override;

public:
	FORCEINLINE UMaterialInterface* GetDefaultMaterial() { return _pDefaultMaterial; }
	int32 SetupMaterialSlot(UMaterialInstanceDynamic* material);
	void SetupMaterialSlot(int32 materialSlot, UMaterialInterface* material);

	int32 GetSectionIDFromPool();
	void ReturnSectionIDToPool(int32& sectionID);

	void ViewUpdateRequest(std::vector<class osgBridgeMeshSection*>& addToViewMeshSections, std::vector<int32>& removeFromViewMeshSections);

	int32 CreateSectionGameThread(class osgBridgeMeshSection* meshSection);
	void RemoveSectionGameThread(int32 meshSectionID);

	void MarkMaterialPendingKill(int32 materialSlot);
	bool IsMeshSectionVisible(int32 sectionID);

	FORCEINLINE class osgBridgeView* GetViewInfo();

protected:
	void CreateSectionFromComponents(class osgBridgeMeshSection* meshSection);
	void RemoveSection(int32 meshSectionID);

	int32 GetMaterialSlotFromPool();
	void ReturnMaterialSlotToPool(int32 materialSlot);

protected:
	UPROPERTY(EditAnywhere)
		UMaterialInterface* _pDefaultMaterial;

	std::mutex _viewUpdateMutex;
	std::vector<class osgBridgeMeshSection*> _meshSectionsToRender;
	std::vector <std::pair<int64, int32 >> _meshSectionsToRemove;
	std::set<int32> _visibleMeshSections;

private:
	class URuntimeMeshComponentStatic* _pRuntimeMeshComponentStatic;
	
	std::mutex _pendingKillMaterialsMutex;
	std::vector <std::pair<int64, int32 >> _pendingKillMaterials;

	std::mutex _meshSectionIDPoolMutex;
	int32 _nextSectionID;
	TArray<int32> _meshSectionIDPool;
	std::mutex _materialSlotPoolMutex;
	int32 _nextMaterialSlot;
	TArray<int32> _materialSlotPool;

protected:
	class osgBridgeView* _pView;
};