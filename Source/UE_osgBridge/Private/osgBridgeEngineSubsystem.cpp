#include "osgBridgeEngineSubsystem.h"
#include "osgBridgePawn.h"
#include "AsyncTasks/osgBridgeFileReadTask.h"
#include "AsyncTasks/osgBridgeDataCleanTask.h"
#include "AsyncTasks/osgBridgeMaterialGenerateTask.h"
#include "Threads/osgBridgeThreadPoolStatic.h"
#include "Threads/osgBridgeTickableThread.h"
#include "Threads/osgBridgeCoreThread.h"
#include "Util/osgBridgeFileUtil.h"
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
	auto folders = osgBridgeFileUtil::GetFolders(str_dirPath);
	for (auto& folder : folders)
	{
		DatabaseKey key = osgBridgeFileUtil::PathCombine({ str_dirPath ,folder });
		MountRootNode(key);
		std::string nodeFilePath = osgBridgeFileUtil::PathCombine({ key ,folder + ".osgb" });
		auto fileReadTask = GetFileReadTaskFromPool(nodeFilePath);
		_pThreadPoolStatic->RequestExecuteTask(fileReadTask);
	}
}

bool UosgBridgeEngineSubsystem::IsTickable() const
{
	if ( _pMeshActor && _mountRootNodeList.Num() > 0)
		return true;
	return false;
}

void UosgBridgeEngineSubsystem::Tick(float DeltaTime)
{
	osgBridgeViewUpdateTask* task;
	std::unique_lock<std::mutex> lock(_viewUpdateTaskQueueMutex);
	while (_viewUpdateTaskQueue.Dequeue(task))
		_pThreadPoolStatic->RequestExecuteTask(task);

	osgBridgeDatabase::GetOsgBridgeDatabase()->ClearPendingKillNodes();
}

void UosgBridgeEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	_frameBuffer = 10;
	_pThreadPoolStatic = new osgBridgeThreadPoolStatic(20);
	_pThreadPoolStatic->Create();
	_pMeshActor = nullptr;
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
	if (_pMeshActor)
		return _pMeshActor->GetViewInfo();
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
		_mountRootNodeList.Emplace(databaseKey);
	}
	ViewUpdateTaskEnqueue(new osgBridgeViewUpdateTask(databaseKey));
}

bool UosgBridgeEngineSubsystem::CoreThreadTickCallback(osgBridgeCoreThread* pThread)
{
	static int32 index = 0;
	std::unique_lock<std::mutex> lock(_mountRootNodeListMutex);
	if (index < _mountRootNodeList.Num())
	{
		pThread->SetDatabaseKey(_mountRootNodeList[index]);
		++index;
		return true;
	}
	else
		return false;
}
