#pragma once
#include "osgBridgeAsyncTask.h"
#include <atomic>
#include <map>

/*
	内存显存清理任务 version-3.0起弃用 内存清理任务并入更新任务
*/
class UE_OSGBRIDGE_API osgBridgeDataCleanTask :public osgBridgeAsyncTask
{
public:
	osgBridgeDataCleanTask(FString databaseKey);

	virtual void Execute() override;
	virtual void Abandon() override { }
	virtual void UpdatePriority() override;

	FORCEINLINE const FString& GetDatabaseKey() { return _databaseKey; }

private:
	void Traverse(class osgBridgeNode* node);

private:
	FString _databaseKey;
	int64 _lastCleanFrameNumber;
	std::map<int32, class osgBridgeMeshSection*> _allMeshSections;
};