#pragma once
#include "osgBridgeAsyncTask.h"

/*
	视图更新任务 用于加载卸载模型的几何图形 同时执行资源释放
*/
class UE_OSGBRIDGE_API osgBridgeViewUpdateTask :public osgBridgeAsyncTask
{
public:
	osgBridgeViewUpdateTask(std::string databaseKey);
	virtual void Execute() override;
	virtual void Abandon() override {}
	virtual void UpdatePriority() override;

private:
	void Traverse(class osgBridgeNode* node);

	bool LoadSuccessorFromDisk(class osgBridgePagedLOD* plod);
	void ActivatePagedLOD(class osgBridgePagedLOD* plod);
	void CullSuccessor(class osgBridgeNode* successor);

private:
	const std::string _databaseKey;
	int64 _lastUpdateFrameNumber;

	std::vector<class osgBridgeMeshSection*> _taskCache_MeshSectionsToRender;
	std::vector<int32> _taskCache_MeshSectionsToRemove;
};