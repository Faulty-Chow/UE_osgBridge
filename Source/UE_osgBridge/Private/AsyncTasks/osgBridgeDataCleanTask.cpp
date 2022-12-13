#include "AsyncTasks/osgBridgeDataCleanTask.h"
#include "Database/osgBridgeDatabase.h"
#include "Database/osgBridgeNode.h"
#include "Database/osgBridgePagedLOD.h"
#include "Database/osgBridgeGeometry.h"
#include "Database/osgBridgeMeshSection.h"
#include "osgBridgeEngineSubsystem.h"
#include "osgBridgeMeshActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "osgBridgeView.h"

osgBridgeDataCleanTask::osgBridgeDataCleanTask(FString databaseKey):
	_databaseKey(databaseKey)
{
	_lastCleanFrameNumber = -1;
}

void osgBridgeDataCleanTask::UpdatePriority()
{
	_priority = pow(2, UKismetSystemLibrary::GetFrameCount() - _lastCleanFrameNumber);
}

void osgBridgeDataCleanTask::Execute()
{
	osgBridgeNode* result = osgBridgeDatabase::GetOsgBridgeDatabase()->GetRootNode(std::string(TCHAR_TO_UTF8(*_databaseKey)));
	_allMeshSections.clear();
	for (int32 i = 0; i < result->GetNumPagedLODs(); i++)
		Traverse(result->GetPagedLOD(i)->GetSuccessor());

	for (auto& meshSection : _allMeshSections)
	{
		if (!UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetMeshActor()->IsMeshSectionVisible(meshSection.second->_sectionID))
		{
			// TODO
		}
	}
	_allMeshSections.clear();
	_lastCleanFrameNumber = UKismetSystemLibrary::GetFrameCount();
}

void osgBridgeDataCleanTask::Traverse(osgBridgeNode* node)
{
	if (node == nullptr)
		return;
	for (int32 i = 0; i < node->GetNumPagedLODs(); i++)
	{
		auto plod = node->GetPagedLOD(i);
		for (int32 j = 0; j < plod->GetNumGeometries(); j++)
		{
			auto geometry = plod->GetGeometry(j);
			if (UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
				GetCurrentViewInfo()->IsSphereBoundsInView(geometry->GetBoxSphereBounds()))
				continue;
			for (int32 k = 0; k < geometry->GetNumMeshSections(); k++)
			{
				auto meshSection = geometry->GetMeshSection(k);
				if (meshSection->_sectionID == INDEX_NONE)
				{
					UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
						GetMeshActor()->MarkMaterialPendingKill(meshSection->_materialSlotIndex);
					delete meshSection;
					geometry->SetMeshSection(k, nullptr);
				}
				else
					_allMeshSections.emplace(meshSection->_sectionID, meshSection);
			}
			Traverse(plod->GetSuccessor());
		}
	}
}