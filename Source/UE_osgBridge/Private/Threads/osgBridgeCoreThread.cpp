//#include "Threads/osgBridgeCoreThread.h"
//#include "osgBridgeEngineSubsystem.h"
//#include "Database/osgBridgeDatabase.h"
//#include "Database/osgBridgeNode.h"
//#include "Database/osgBridgePagedLOD.h"
//#include "Database/osgBridgeGeometry.h"
//#include "Database/osgBridgeMeshSection.h"
//#include "osgBridgePawn.h"
//#include "osgBridgeMeshActor.h"
//#include "Util/FileUtil.h"
//#include "Threads/osgBridgeThreadPoolStatic.h"
//#include "AsyncTasks/osgBridgeFileReadTask.h"
//#include "AsyncTasks/osgBridgeDataCleanTask.h"
//
//osgBridgeCoreThread::osgBridgeCoreThread(FString threadName) :
//	osgBridgeBaseThread(threadName), _bTimeToDie(false)
//{
//}
//
//void osgBridgeCoreThread::TimeToDie(bool bShouldWait /*= true*/)
//{
//	_bTimeToDie.store(true);
//	_pThread->Suspend(false);
//	_pThread->Kill(bShouldWait);
//}
//
//uint32 osgBridgeCoreThread::Run()
//{
//	if (_pStartEvent)
//		_pStartEvent->Wait();
//	_pStartEvent = nullptr;
//	while (!_bTimeToDie.load())
//	{
//		if (UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->UpdateMeshSectionsInView(_databaseKey))
//		{
//			auto rootNode = osgBridgeDatabase::GetOsgBridgeDatabase()->SelectRootNode(_databaseKey);
//			Traverse(rootNode);
//			UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetThreadPoolStatic()->RequestExecuteTask(
//				UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetDataCleanTaskFromPool(rootNode->GetDatabaseKey()));
//			UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->UpdateCallback(_databaseKey);
//		}
//		else
//			FPlatformProcess::Sleep(1);
//	}
//	return 0;
//}
//
//void osgBridgeCoreThread::Traverse(osgBridgeNode* node)
//{
//	if (node == nullptr)
//		return;
//	for (auto& plod : node->GetPagedLODs())
//	{
//		if (UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
//			GetCurrentViewInfo()->IsSphereBoundsInView(plod->GetBoxSphereBounds()))
//		{
//			float pixelSizeInView = UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
//				GetCurrentViewInfo()->GetSphereBoundsPixelSizeInView(plod->GetBoxSphereBounds());
//			if (pixelSizeInView > plod->GetThreshold())
//			{
//				// Need higher LOD
//				if (node->GetChild(plod->GetIndex()))
//				{
//					RemoveAllGeometriesBelongTo(plod);
//					Traverse(node->GetChild(plod->GetIndex()));
//				}
//				else
//				{
//					// Load heigher LOD from disk.
//					std::string nodeFilePath = FileUtil::PathCombine(
//						{ std::string(TCHAR_TO_UTF8(*_databaseKey)), plod->GetSuccessorFileName() });
//					osgBridgeFileReadTask* fileReadTask = UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetFileReadTaskFromPool(nodeFilePath);
//					if (fileReadTask->GetNode() == nullptr)
//					{
//						fileReadTask->SetNode(new osgBridgeNode(node, plod->GetIndex(), nodeFilePath));
//						UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetThreadPoolStatic()->RequestExecuteTask(fileReadTask);
//					}
//					goto RenderCurrentPagedLOD;
//				}
//			}
//			else
//			{
//				ClearHigherLODs(plod);
//			RenderCurrentPagedLOD:
//				for (auto& geometry : plod->GetGeometries())
//				{
//					bool bInView = UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
//						GetCurrentViewInfo()->IsSphereBoundsInView(geometry->GetBoxSphereBounds());
//					if (bInView)
//						AddToView(geometry);
//					else
//						RemoveFromView(geometry);
//				}
//			}
//		}
//	}
//}
//
//void osgBridgeCoreThread::AddToView(osgBridgeGeometry* geometry)
//{
//	bool bNeedReload = false;
//	for (auto& meshSection : geometry->GetMeshSections())
//	{
//		if (meshSection->IsValid())
//		{
//			auto pData = meshSection->Lock();
//			check(UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetMeshActor()->CreateSectionFromComponents(pData) != INDEX_NONE)
//				meshSection->Unlock();
//		}
//		else
//			bNeedReload = true;
//	}
//	if (bNeedReload)
//		UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetThreadPoolStatic()->RequestExecuteTask(
//			UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
//			GetFileReadTaskFromPool(geometry->GetOwner()->GetOwner()->GetNodeFilePath()));
//}
//
//void osgBridgeCoreThread::RemoveFromView(class osgBridgeGeometry* geometry)
//{
//	for (auto& meshSection : geometry->GetMeshSections())
//	{
//		auto pData = meshSection->Lock();
//		UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
//			GetMeshActor()->RemoveSectionFromComponents(pData->_sectionID);
//		meshSection->Unlock();
//	}
//}
//
//void osgBridgeCoreThread::ClearHigherLODs(osgBridgePagedLOD* plod)
//{
//	if (plod == nullptr)
//		return;
//	osgBridgeNode* higherLODsGroup = plod->GetOwner()->GetChild(plod->GetIndex());
//	if (higherLODsGroup)
//	{
//		for (auto& higherLOD : higherLODsGroup->GetPagedLODs())
//		{
//			RemoveAllGeometriesBelongTo(higherLOD);
//			ClearHigherLODs(higherLOD);
//		}
//	}
//}
//
//void osgBridgeCoreThread::RemoveAllGeometriesBelongTo(osgBridgePagedLOD* plod)
//{
//	for (auto& geometry : plod->GetGeometries())
//		RemoveFromView(geometry);
//}