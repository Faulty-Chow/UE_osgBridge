//#include "AsyncTasks/osgBridgeDataCleanTask.h"
//#include "Database/osgBridgeDatabase.h"
//#include "Database/osgBridgeNode.h"
//#include "Database/osgBridgePagedLOD.h"
//#include "Database/osgBridgeGeometry.h"
//#include "Database/osgBridgeMeshSection.h"
//#include "osgBridgeEngineSubsystem.h"
//#include "osgBridgeMeshActor.h"
//#include "Kismet/KismetSystemLibrary.h"
//#include "osgBridgeView.h"
//
//osgBridgeDataCleanTask::osgBridgeDataCleanTask(FString databaseKey):
//	_databaseKey(databaseKey)
//{
//	_lastCleanFrameNumber = -1;
//}
//
//void osgBridgeDataCleanTask::UpdatePriority()
//{
//	_priority = pow(2, UKismetSystemLibrary::GetFrameCount() - _lastCleanFrameNumber);
//}
//
//void osgBridgeDataCleanTask::Execute()
//{
//	osgBridgeNode* result = osgBridgeDatabase::GetOsgBridgeDatabase()->GetRootNode(std::string(TCHAR_TO_UTF8(*_databaseKey)));
//	_allMeshSections.clear();
//	for (int32 i = 0; i < result->GetNumChildren(); i++)
//		Traverse(result->GetChild(i));
//
//	for (auto& meshSection : _allMeshSections)
//	{
//		auto pData = meshSection.second->Lock();
//		if (!UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetMeshActor()->IsMeshSectionVisible(pData->_sectionID))
//		{
//			UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
//				GetMeshActor()->DeleteSectionFromComponents(pData->_materialSlotIndex, pData->_sectionID);
//			meshSection.second->Reset();
//		}
//		meshSection.second->Unlock();
//	}
//	_allMeshSections.clear();
//	_lastCleanFrameNumber = UKismetSystemLibrary::GetFrameCount();
//}
//
//void osgBridgeDataCleanTask::Traverse(osgBridgeNode* node)
//{
//	if (node == nullptr)
//		return;
//	if (osgBridgeDatabase::GetOsgBridgeDatabase()->LockNode(node))
//	{
//		for (auto& plod : node->GetPagedLODs())
//		{
//			for (auto& geometry : plod->GetGeometries())
//			{
//				if (UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
//					GetCurrentViewInfo()->IsSphereBoundsInView(geometry->GetBoxSphereBounds()))
//					continue;
//				for (auto& meshSection : geometry->GetMeshSections())
//				{
//					auto pData = meshSection->Lock();
//					if (pData->_sectionID == INDEX_NONE)
//					{
//						UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
//							GetMeshActor()->MarkMaterialPendingKill(pData->_materialSlotIndex);
//						meshSection->Reset();
//					}
//					else
//						_allMeshSections.emplace(pData->_sectionID, meshSection);
//					meshSection->Unlock();
//				}
//			}
//		}
//		osgBridgeDatabase::GetOsgBridgeDatabase()->UnlockNode(node);
//	}
//	for (int32 i = 0; i < node->GetNumChildren(); i++)
//		Traverse(node->GetChild(i));
//}