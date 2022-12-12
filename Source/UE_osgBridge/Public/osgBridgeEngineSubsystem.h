#pragma once
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>
#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "osgBridgeEngineSubsystem.generated.h"

UCLASS()
class UE_OSGBRIDGE_API UosgBridgeEngineSubsystem : public UEngineSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

	typedef std::string NodeFilePath;
	typedef std::string DatabaseKey;

public:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override { return false; }
	virtual __forceinline TStatId GetStatId() const override {
		// RETURN_QUICK_DECLARE_CYCLE_STAT(UosgBridgeEngineSubsystem, STATGROUP_Tickables);
		return TStatId();
	}

public:
	UFUNCTION(BlueprintCallable, Category = "osgBridge", Meta = (DisplayName = "Set Mesh Actor"))
		void SetMeshActor(class AosgBridgeMeshActor* meshActor);

	UFUNCTION(BlueprintCallable, Category = "osgBridge", Meta = (DisplayName = "Load Node Files"))
		void LoadNodeFiles(FString dirPath);

	UFUNCTION(BlueprintCallable, Category = "osgBridge", Meta = (DisplayName = "Get Frame Buffer"))
		int64 GetFrameBuffer() { return _frameBuffer; }

	UFUNCTION(BlueprintCallable, Category = "osgBridge", Meta = (DisplayName = "Set Frame Buffer"))
		void SetFrameBuffer(int64 value) { _frameBuffer = value; }

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual UWorld* GetWorld() const override;

public:
	static UosgBridgeEngineSubsystem* GetOsgBridgeEngineSubsystem();
	class osgBridgeView* GetCurrentViewInfo();
	class osgBridgeThreadPoolStatic* GetThreadPoolStatic();
	class AosgBridgeMeshActor* GetMeshActor();
	
	class osgBridgeFileReadTask* GetFileReadTaskFromPool(const NodeFilePath& filePath);
	void ReturnFileReadTaskToPool(class osgBridgeFileReadTask* task);
	bool ViewUpdateTaskDequeue(class osgBridgeViewUpdateTask*& task);
	void ViewUpdateTaskEnqueue(class osgBridgeViewUpdateTask* task);

	void MountRootNode(DatabaseKey databaseKey);

private:
	int64 _frameBuffer;

	std::mutex _fileReadTaskPoolMutex;
	std::map<NodeFilePath, class osgBridgeFileReadTask*> _fileReadTaskPool;
	std::mutex _viewUpdateTaskQueueMutex;
	TQueue<class osgBridgeViewUpdateTask*> _viewUpdateTaskQueue;

	class AosgBridgeMeshActor* _pMeshActor;
	class osgBridgeThreadPoolStatic* _pThreadPoolStatic;
	class osgBridgeView* _pView;

	std::mutex _mountRootNodeListMutex;
	TArray<DatabaseKey> _mountRootNodes;

	/*std::mutex _tickableThreadMutex;
	std::condition_variable _tickDoneCondition;
	int32 _tickDoneCount;*/
};