#pragma once
#include "osgBridgeAsyncTask.h"

#include <functional>

class UE_OSGBRIDGE_API osgBridgeMaterialGenerateTask :public osgBridgeAsyncTask
{
public:
	osgBridgeMaterialGenerateTask(class osgBridgeFileReadTask* pFileReadTask);
	FORCEINLINE void SetNode(class osgBridgeNode* node);
	FORCEINLINE const class osgBridgeNode* GetNode() const;

	virtual void Execute() override;
	virtual void Abandon() override {}
	virtual void UpdatePriority() override {}

	FORCEINLINE class osgBridgeNode* GetNode() { return _pNode; }

private:
	class osgBridgeNode* _pNode;
	class osgBridgeFileReadTask* const _pPreviousTask;
	// std::function<void(class osgBridgeNode*)> _addToDatabaseHandle;
};