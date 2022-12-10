#include "osgBridgeEngineSubsystem.h"
#include "osgBridgePawn.h"
#include "AsyncTasks/osgBridgeFileReadTask.h"
#include "AsyncTasks/osgBridgeDataCleanTask.h"
#include "AsyncTasks/osgBridgeMaterialGenerateTask.h"
#include "Threads/osgBridgeThreadPoolStatic.h"
#include "Threads/osgBridgeTickableThread.h"
#include "Threads/osgBridgeCoreThread.h"
#include "Util/FileUtil.h"
#include "Database/osgBridgeNode.h"
#include "Database/osgBridgeDatabase.h"
#include "osgBridgeMeshActor.h"
#include "AsyncTasks/osgBridgeViewUpdateTask.h"

#include "Engine/GameEngine.h"

void UosgBridgeEngineSubsystem::SetMeshActor(AosgBridgeMeshActor* meshActor)
{
	check(meshActor);
	_pMeshActor = meshActor;
}

void UosgBridgeEngineSubsystem::LoadNodeFiles(FString dirPath)
{
	std::string str_dirPath(TCHAR_TO_UTF8(*dirPath));
	auto folders = FileUtil::GetFolders(str_dirPath);
	for (auto& folder : folders)
	{
		DatabaseKey key = FileUtil::PathCombine({ str_dirPath ,folder });
		MountRootNode(key);
		std::string nodeFilePath = FileUtil::PathCombine({ key ,folder + ".osgb" });
		auto fileReadTask = GetFileReadTaskFromPool(nodeFilePath);
		_pThreadPoolStatic->RequestExecuteTask(fileReadTask);
	}
}

bool UosgBridgeEngineSubsystem::IsTickable() const
{
	if (_pMeshActor && _mountRootNodes.Num() > 0)
		return true;
	return false;
}

void UosgBridgeEngineSubsystem::Tick(float DeltaTime)
{
	osgBridgeDatabase::GetOsgBridgeDatabase()->ClearPendingKillNodes();

	if (_pView->IsTickable())
		_pView->Tick(DeltaTime);

	osgBridgeViewUpdateTask* task;
	std::unique_lock<std::mutex> lock(_viewUpdateTaskQueueMutex);
	while (_viewUpdateTaskQueue.Dequeue(task))
		_pThreadPoolStatic->RequestExecuteTask(task);


	/*if (_mountRootNodeList.Num() > 0)
	{
		check(GetMeshActor());

		if (_pView->IsTickable())
			_pView->Tick(DeltaTime);

		if (_mountRootNodeList.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("No need tick."));
			return;
		}

		_traverseIndex = 0;
		_tickDoneCount = 0;
		for (auto& pThread : _tickableThreadPool)
			pThread->Tick(DeltaTime);

		{
			std::unique_lock<std::mutex> lock(_tickableThreadMutex);
			while (_tickDoneCount < _numTickableThreads)
				_tickDoneCondition.wait(lock);
		}
	}	*/
}

void UosgBridgeEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	_frameBuffer = 10;
	_pThreadPoolStatic = new osgBridgeThreadPoolStatic(12);
	_pThreadPoolStatic->Create();
	_pMeshActor = nullptr;
	_pView = new osgBridgePawn;
}

void UosgBridgeEngineSubsystem::Deinitialize()
{
	_pThreadPoolStatic->Destroy();
	delete _pThreadPoolStatic;
	_pThreadPoolStatic = nullptr;
}

UWorld* UosgBridgeEngineSubsystem::GetWorld() const
{
	if (_pMeshActor)
		return _pMeshActor->GetWorld();
	else
		return GetWorld();
}

UosgBridgeEngineSubsystem* UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()
{
	if (GEngine)
		return GEngine->GetEngineSubsystem<UosgBridgeEngineSubsystem>();
	return nullptr;
}

osgBridgeView* UosgBridgeEngineSubsystem::GetCurrentViewInfo()
{
	return _pView;
	/*if (GetWorld()->HasBegunPlay())
	{
		if (dynamic_cast<osgBridgePawn*>(_pView) == nullptr)
		{
			delete _pView;
			_pView = new osgBridgePawn;
		}
	}
	else
	{
		return nullptr;
	}*/
	return nullptr;
}

osgBridgeThreadPoolStatic* UosgBridgeEngineSubsystem::GetThreadPoolStatic()
{
	return _pThreadPoolStatic;
}

osgBridgeFileReadTask* UosgBridgeEngineSubsystem::GetFileReadTaskFromPool(const NodeFilePath& filePath)
{
	osgBridgeFileReadTask* task = nullptr;
	std::unique_lock<std::mutex> lock(_fileReadTaskPoolMutex);
	auto itr = _fileReadTaskPool.find(filePath);
	if (itr == _fileReadTaskPool.end())
	{
		task = new osgBridgeFileReadTask(filePath);
		_fileReadTaskPool.emplace(filePath, nullptr);
	}
	else
	{
		task = itr->second;
		itr->second = nullptr;
	}
	return task;
}

void UosgBridgeEngineSubsystem::ReturnFileReadTaskToPool(osgBridgeFileReadTask* task)
{
	std::unique_lock<std::mutex> lock(_fileReadTaskPoolMutex);
	_fileReadTaskPool.at(task->GetNodeFilePath()) = task;
}

bool UosgBridgeEngineSubsystem::ViewUpdateTaskDequeue(osgBridgeViewUpdateTask*& task)
{
	std::unique_lock<std::mutex> lock(_viewUpdateTaskQueueMutex);
	if (_viewUpdateTaskQueue.Dequeue(task))
		return true;
	else
		return false;
}

void UosgBridgeEngineSubsystem::ViewUpdateTaskEnqueue(osgBridgeViewUpdateTask* task)
{
	std::unique_lock<std::mutex> lock(_viewUpdateTaskQueueMutex);
	_viewUpdateTaskQueue.Enqueue(task);
}

AosgBridgeMeshActor* UosgBridgeEngineSubsystem::GetMeshActor()
{
	return _pMeshActor;
}

void UosgBridgeEngineSubsystem::MountRootNode(DatabaseKey databaseKey)
{
	{
		std::unique_lock<std::mutex> lock(_mountRootNodeListMutex);
		_mountRootNodes.Emplace(databaseKey);
	}
	ViewUpdateTaskEnqueue(new osgBridgeViewUpdateTask(databaseKey));
}
