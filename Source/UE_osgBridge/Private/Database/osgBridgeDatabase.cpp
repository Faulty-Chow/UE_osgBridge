#include "Database/osgBridgeDatabase.h"
#include "Database/osgBridgeNode.h"
#include "Database/osgBridgePagedLOD.h"
#include "Database/osgBridgeGeometry.h"
#include "Database/osgBridgeMeshSection.h"
#include "Kismet/KismetSystemLibrary.h"
#include "osgBridgeEngineSubsystem.h"

osgBridgeDatabase::Token osgBridgeDatabase::AdminToken;

osgBridgeDatabase* osgBridgeDatabase::GetOsgBridgeDatabase(Token token)
{
	static osgBridgeDatabase* pDatabase = new osgBridgeDatabase;
	return pDatabase;
}

osgBridgeNode* osgBridgeDatabase::GetRootNode(std::string key)
{
	std::unique_lock<std::mutex> lock(_rootNodesMutex);
	if (_rootNodes.find(key) == _rootNodes.end())
	{
		std::string filename = key.substr(key.find_last_of('\\') + 1);
		std::string filePath = key + "\\" + filename + ".osgb";
		osgBridgeNode* node = nullptr;
		if (CheckLoadResults(filePath, node))
		{
			_rootNodes.emplace(key, node);
			return node;
		}
		else
			return nullptr;
	}
	else
		return _rootNodes.at(key);
}

void osgBridgeDatabase::LoadNodeFileComplete(class osgBridgeNode* loadResult)
{
	std::unique_lock<std::mutex> lock(_loadResultsMutex);
	int64 frameNumber = UKismetSystemLibrary::GetFrameCount();
	_loadResults.emplace(loadResult->GetNodeFilePath(), std::pair<int64, class osgBridgeNode*>(frameNumber, loadResult));
}

bool osgBridgeDatabase::CheckLoadResults(const std::string& nodeFilePath, class osgBridgeNode*& node)
{
	std::unique_lock<std::mutex> lock(_loadResultsMutex);
	auto itr = _loadResults.find(nodeFilePath);
	if (itr == _loadResults.end())
		return false;
	else
	{
		node = itr->second.second;
		_loadResults.erase(nodeFilePath);
		return true;
	}
}

void osgBridgeDatabase::GarbageCollection(class osgBridgeNode* node)
{
	if (node)
	{
		std::unique_lock<std::mutex> lock(_pendingKillNodesMutex);
		_pendingKillNodes.emplace_back(std::pair<int64, class osgBridgeNode* >(UKismetSystemLibrary::GetFrameCount(), node));
	}
}

void osgBridgeDatabase::ClearPendingKillNodes()
{
	check(IsInGameThread());
	int64 frameNumber = UKismetSystemLibrary::GetFrameCount();
	std::unique_lock<std::mutex> lock(_pendingKillNodesMutex);
	for (auto itr = _pendingKillNodes.begin(); itr != _pendingKillNodes.end();)
	{
		if (frameNumber - itr->first > (2 * UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetFrameBuffer()))
		{
			delete itr->second;
			itr = _pendingKillNodes.erase(itr);
		}
		else
			break;
	}
}