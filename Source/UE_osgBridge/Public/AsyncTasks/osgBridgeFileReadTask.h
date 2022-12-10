#pragma once
#include "osgBridgeAsyncTask.h"

#include <string>
#include <functional>
#include <atomic>

#include <osg/Node>
#include <osg/ref_ptr>

class UE_OSGBRIDGE_API osgBridgeFileReadTask :public osgBridgeAsyncTask
{
public:
	osgBridgeFileReadTask(std::string nodeFilePath);
	~osgBridgeFileReadTask();

	FORCEINLINE void SetNode(class osgBridgeNode* node);
	FORCEINLINE const class osgBridgeNode* GetNode() const;

	void Reset();
	void Request();
	virtual void Execute() override;
	virtual void Abandon() override
	{
		UE_LOG(LogTemp, Warning, TEXT("A FileReadTask is Abandoned, %s"), *FString(_nodeFilePath.c_str()));
	}
	virtual void UpdatePriority() override;

	/*template<typename Function>
	void SetReadResultHandle(Function function);*/

	FORCEINLINE const std::string& GetNodeFilePath() const { return _nodeFilePath; }

private:
	std::string _nodeFilePath;
	// std::function<void(osg::ref_ptr<osg::Node>)> _readResultHandle;
	int32 _requestCount;
	int64 _requestLastFrameNumber;

	class osgBridgeNode* _pNode;
	class osgBridgeMaterialGenerateTask* _pSuccessionTask;
};