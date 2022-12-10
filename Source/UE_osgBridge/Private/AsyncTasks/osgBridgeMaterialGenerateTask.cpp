#include "AsyncTasks/osgBridgeMaterialGenerateTask.h"
#include "AsyncTasks/osgBridgeFileReadTask.h"
#include "Database/osgBridgeDatabase.h"
#include "Database/osgBridgeNode.h"
#include "Database/osgBridgePagedLOD.h"
#include "Database/osgBridgeGeometry.h"
#include "Database/osgBridgeMeshSection.h"
#include "Threads/osgBridgeTaskThread.h"
#include "osgBridgeEngineSubsystem.h"

osgBridgeMaterialGenerateTask::osgBridgeMaterialGenerateTask(osgBridgeFileReadTask* pFileReadTask) :
	_pPreviousTask(pFileReadTask), _pNode(nullptr)
{
}

void osgBridgeMaterialGenerateTask::SetNode(osgBridgeNode* node)
{
	_pNode = node;
}

const osgBridgeNode* osgBridgeMaterialGenerateTask::GetNode() const
{
	return _pNode;
}

PRAGMA_DISABLE_OPTIMIZATION
void osgBridgeMaterialGenerateTask::Execute()
{
	if (_pNode)
	{
		for (int32 i = 0; i < _pNode->GetNumPagedLODs(); i++)
		{
			auto plod = _pNode->GetPagedLOD(i);
			for (int32 j = 0; j < plod->GetNumGeometries(); j++)
			{
				auto geometry = plod->GetGeometry(j);
				for (int32 k = 0; k < geometry->GetNumMeshSections(); k++)
				{
					auto meshSection = geometry->GetMeshSection(k);
					check(meshSection->_pTextureData && meshSection->_pTexture.IsValid() && meshSection->_pMaterial.IsValid());
					uint8* MipData = static_cast<uint8*>(meshSection->_pTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
					FMemory::Memcpy(MipData, meshSection->_pTextureData, meshSection->_cols * meshSection->_rows * 4);
					meshSection->_pTexture->PlatformData->Mips[0].BulkData.Unlock();
					meshSection->_pTexture->UpdateResource();
					meshSection->_pMaterial->SetTextureParameterValue("Param", meshSection->_pTexture.Get());
					delete meshSection->_pTextureData;
					meshSection->_pTextureData = nullptr;
				}
			}
		}
		osgBridgeDatabase::GetOsgBridgeDatabase()->LoadNodeFileComplete(_pNode);
		_pPreviousTask->Reset();
		UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->ReturnFileReadTaskToPool(_pPreviousTask);
		
		//osgBridgeTaskThread::DoInGameThread(
		//	[&](void) -> void
		//	{
		//		if(_pNode == nullptr)
		//			return;
		//		for (auto& plod : _pNode->GetPagedLODs())
		//		{
		//			for (auto& geometry : plod->GetGeometries())
		//			{
		//				for (auto& meshSection : geometry->GetMeshSections())
		//				{
		//					auto pData = meshSection->Lock();
		//					pData->_pTexture->UpdateResource();
		//					pData->_pMaterial->SetTextureParameterValue("Param", pData->_pTexture);
		//					/*pData->_pTexture->RemoveFromRoot();
		//					pData->_pTexture->MarkPendingKill();
		//					pData->_pTexture = nullptr;*/
		//					meshSection->Unlock();
		//				}
		//			}
		//		}
		//		if (_pNode->GetParent())
		//			_pNode->GetParent()->AddChild(_pNode->GetIndex(), _pNode);
		//		else
		//		{
		//			osgBridgeDatabase::GetOsgBridgeDatabase()->AddRootNode(_pNode);
		//			UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->MountRootNode(_pNode->GetDatabaseKey());
		//		}
		//		UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->
		//			GetFileReadTaskFromPool(_pNode->GetNodeFilePath())->Reset();
		//	}
		//);
	}
}
PRAGMA_ENABLE_OPTIMIZATION