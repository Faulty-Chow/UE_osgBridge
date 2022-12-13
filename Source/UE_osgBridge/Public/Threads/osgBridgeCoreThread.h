#pragma once
#include "osgBridgeTickableThread.h"

/*
	Version-2.0中的核心线程，与游戏线程Tick同步，在Version-3.0中弃用，场景的更新也被作为异步任务
*/
class UE_OSGBRIDGE_API osgBridgeCoreThread :public osgBridgeTickableThread
{
public:
	osgBridgeCoreThread(FString threadName);

	FORCEINLINE void SetDatabaseKey(std::string key) { _databaseKey = key; }

protected:
	virtual void Tick() override;

private:
	void Traverse(class osgBridgeNode* node);

	bool LoadSuccessorFromDisk(class osgBridgePagedLOD* plod);
	void ActivatePagedLOD(class osgBridgePagedLOD* plod);
	void CullSuccessor(class osgBridgeNode* successor);

private:
	std::string _databaseKey;
	std::vector<class osgBridgeMeshSection*> _taskCache_MeshSectionsToRender;
	std::vector<int32> _taskCache_MeshSectionsToRemove;
};