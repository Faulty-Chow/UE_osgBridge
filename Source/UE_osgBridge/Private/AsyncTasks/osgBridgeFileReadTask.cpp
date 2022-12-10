#include "AsyncTasks/osgBridgeFileReadTask.h"
#include "AsyncTasks/osgBridgeMaterialGenerateTask.h"
#include "Kismet/KismetSystemLibrary.h"
#include <osgDB/ReadFile>
#include "Database/osgBridgeDatabase.h"
#include "Database/osgBridgeNode.h"
#include "Database/osgBridgePagedLOD.h"
#include "Database/osgBridgeGeometry.h"
#include "Database/osgBridgeMeshSection.h"
#include "osgBridgeEngineSubsystem.h"
#include "osgBridgeMeshActor.h"
#include "Threads/osgBridgeThreadPoolStatic.h"
#include <osg/PagedLOD>

osgBridgeFileReadTask::osgBridgeFileReadTask(std::string nodeFilePath) :
	_nodeFilePath(nodeFilePath), _pNode(nullptr), _requestCount(0), _requestLastFrameNumber(-1)
{
	_pSuccessionTask = new osgBridgeMaterialGenerateTask(this);
}

osgBridgeFileReadTask::~osgBridgeFileReadTask()
{
	delete _pSuccessionTask;
}

void osgBridgeFileReadTask::SetNode(osgBridgeNode* node)
{
	_pNode = node;
}

const osgBridgeNode* osgBridgeFileReadTask::GetNode() const
{
	return _pNode;
}

void osgBridgeFileReadTask::Reset()
{
	_requestCount = 0;
	_requestLastFrameNumber = -1;
	_pNode = nullptr;
}

void osgBridgeFileReadTask::Request()
{
	_requestCount++;
	_requestLastFrameNumber = UKismetSystemLibrary::GetFrameCount();
}

void osgBridgeFileReadTask::UpdatePriority()
{
	int64 temp = UKismetSystemLibrary::GetFrameCount() - _requestLastFrameNumber;
	if (temp > 1)
		_priority = pow(2, _requestCount / temp);
	else
		_priority = pow(2, _requestCount);
}

void osgBridgeFileReadTask::Execute()
{
	osg::ref_ptr<osg::Node> readResult = osgDB::readNodeFile(_nodeFilePath);
	UE_LOG(LogTemp, Log, TEXT("ReadNodeFile %s"), *FString(_nodeFilePath.c_str()));
	if (readResult.valid())
	{
		if (_pNode == nullptr)
			_pNode = new osgBridgeNode(_nodeFilePath, readResult);
		check(_pNode);
		osgBridgeTaskThread::DoInGameThread(
			[=](void) -> void
			{
				for (int32 i = 0; i < _pNode->GetNumPagedLODs(); i++)
				{
					osgBridgePagedLOD* plod = _pNode->GetPagedLOD(i);
					for (int32 j = 0; j < plod->GetNumGeometries(); j++)
					{
						osgBridgeGeometry* geometry = plod->GetGeometry(j);
						for (int32 k = 0; k < geometry->GetNumMeshSections(); k++)
						{
							osgBridgeMeshSection* meshSection = geometry->GetMeshSection(k);
							meshSection->_pTexture = TStrongObjectPtr<UTexture2D>(
								UTexture2D::CreateTransient(meshSection->_cols, meshSection->_rows, PF_B8G8R8A8));
							meshSection->_pMaterial = TStrongObjectPtr<UMaterialInstanceDynamic>(UMaterialInstanceDynamic::Create(
								UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetMeshActor()->GetDefaultMaterial(),
								UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetWorld(), NAME_None));
							meshSection->_materialSlotIndex = UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
								GetMeshActor()->SetupMaterialSlot(meshSection->_pMaterial.Get());
						}
					}
				}
				_pSuccessionTask->SetNode(_pNode);
				UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetThreadPoolStatic()->RequestExecuteTask(_pSuccessionTask);
			}
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Read Node File failed: %s"), *FString(_nodeFilePath.c_str()));
		Reset();
		UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->ReturnFileReadTaskToPool(this);
	}
}

//template<typename Function>
//void SetReadResultHandle(Function function)
//{
//	_readResultHandle = function;
//}