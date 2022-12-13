#include "Threads/osgBridgeCoreThread.h"
#include "osgBridgeEngineSubsystem.h"
#include "Database/osgBridgeDatabase.h"
#include "Database/osgBridgeNode.h"
#include "Database/osgBridgePagedLOD.h"
#include "Database/osgBridgeGeometry.h"
#include "Database/osgBridgeMeshSection.h"
#include "osgBridgePawn.h"
#include "osgBridgeMeshActor.h"
#include "Util/osgBridgeFileUtil.h"
#include "Threads/osgBridgeThreadPoolStatic.h"
#include "AsyncTasks/osgBridgeFileReadTask.h"
#include "AsyncTasks/osgBridgeDataCleanTask.h"

osgBridgeCoreThread::osgBridgeCoreThread(FString threadName) :
	osgBridgeTickableThread(threadName)
{
}

void osgBridgeCoreThread::Tick()
{
	while (UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->CoreThreadTickCallback(this))
	{
		check(_taskCache_MeshSectionsToRender.empty() && _taskCache_MeshSectionsToRemove.empty());
		auto rootNode = osgBridgeDatabase::GetOsgBridgeDatabase()->GetRootNode(_databaseKey);
		Traverse(rootNode);
		UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetMeshActor()->
			ViewUpdateRequest(_taskCache_MeshSectionsToRender, _taskCache_MeshSectionsToRemove);
		_taskCache_MeshSectionsToRender.clear();
		_taskCache_MeshSectionsToRemove.clear();
	}
}

void osgBridgeCoreThread::Traverse(osgBridgeNode* node)
{
	if (node == nullptr)
		return;
	for (int32 i = 0; i < node->GetNumPagedLODs(); i++)
	{
		osgBridgePagedLOD* plod = node->GetPagedLOD(i);
		check(plod->IsValid());
		float pixelSizeInView = UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
			GetCurrentViewInfo()->GetSphereBoundsPixelSizeInView(plod->GetBoxSphereBounds());
		if (pixelSizeInView > plod->GetThreshold())
		{
			if (plod->GetSuccessor() || LoadSuccessorFromDisk(plod) == false)
			{
				for (int32 j = 0; j < plod->GetNumGeometries(); j++)
				{
					auto geometry = plod->GetGeometry(j);
					for (int32 k = 0; k < geometry->GetNumMeshSections(); k++)
					{
						_taskCache_MeshSectionsToRemove.emplace_back(geometry->GetMeshSection(k)->_sectionID);
					}
				}
				Traverse(plod->GetSuccessor());
			}
			else
				ActivatePagedLOD(plod);
		}
		else
		{
			ActivatePagedLOD(plod);

			auto successor = plod->GetSuccessor();
			if (successor)
			{
				CullSuccessor(successor);
				osgBridgeDatabase::GetOsgBridgeDatabase()->GarbageCollection(successor);
			}
			plod->SetSuccessor(nullptr);
		}
	}
}

bool osgBridgeCoreThread::LoadSuccessorFromDisk(class osgBridgePagedLOD* plod)
{
	std::string nodeFilePath = osgBridgeFileUtil::PathCombine({ std::string(_databaseKey), plod->GetSuccessorFileName() });
	osgBridgeNode* getFromCache = nullptr;
	if (osgBridgeDatabase::GetOsgBridgeDatabase()->CheckLoadResults(nodeFilePath, getFromCache))
	{
		plod->SetSuccessor(getFromCache);
		return false;
	}
	else
	{
		osgBridgeFileReadTask* fileReadTask = UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetFileReadTaskFromPool(nodeFilePath);
		if (fileReadTask)
		{
			fileReadTask->Request();
			UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetThreadPoolStatic()->RequestExecuteTask(fileReadTask);
			return true;
		}
		return false;
	}
}

void osgBridgeCoreThread::ActivatePagedLOD(class osgBridgePagedLOD* plod)
{
	for (int32 i = 0; i < plod->GetNumGeometries(); i++)
	{
		auto geometry = plod->GetGeometry(i);
		bool bGeometryInView = UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
			GetCurrentViewInfo()->IsSphereBoundsInView(geometry->GetBoxSphereBounds());
		if (bGeometryInView)
		{
			for (int32 j = 0; j < geometry->GetNumMeshSections(); j++)
			{
				_taskCache_MeshSectionsToRender.emplace_back(geometry->GetMeshSection(j));
			}
		}
		else
		{
			for (int32 j = 0; j < geometry->GetNumMeshSections(); j++)
			{
				_taskCache_MeshSectionsToRemove.emplace_back(geometry->GetMeshSection(j)->_sectionID);
			}
		}
	}
}

void osgBridgeCoreThread::CullSuccessor(osgBridgeNode* successor)
{
	if (successor)
	{
		for (int32 i = 0; i < successor->GetNumPagedLODs(); i++)
		{
			auto plod = successor->GetPagedLOD(i);
			for (int32 j = 0; j < plod->GetNumGeometries(); j++)
			{
				auto geometry = plod->GetGeometry(j);
				for (int32 k = 0; k < geometry->GetNumMeshSections(); k++)
				{
					auto meshSection = geometry->GetMeshSection(k);
					_taskCache_MeshSectionsToRemove.emplace_back(meshSection->_sectionID);
					UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
						GetMeshActor()->ReturnSectionIDToPool(meshSection->_sectionID);
					UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
						GetMeshActor()->MarkMaterialPendingKill(meshSection->_materialSlotIndex);
				}
			}
			if (plod->GetSuccessor())
				CullSuccessor(plod->GetSuccessor());
		}
	}
}