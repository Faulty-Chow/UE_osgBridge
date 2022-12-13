#pragma once
#include <mutex>
#include <map>
#include <vector>

/*
	全局单例 用于存放所有模型数据
*/
class UE_OSGBRIDGE_API osgBridgeDatabase
{
private:
	/*
		已弃用，保留只是为了兼容(maybe)
	*/
	struct Token {
		explicit Token() = default;
	};
	static Token AdminToken;

public:
	static osgBridgeDatabase* GetOsgBridgeDatabase(Token token= AdminToken);
	class osgBridgeNode* GetRootNode(std::string key);

	void LoadNodeFileComplete(class osgBridgeNode* loadResult);
	bool CheckLoadResults(const std::string& nodeFilePath, class osgBridgeNode*& node);

	void GarbageCollection(class osgBridgeNode* node);
	void ClearPendingKillNodes();

private:
	std::mutex _rootNodesMutex;
	std::map<std::string, class osgBridgeNode*> _rootNodes;

	std::mutex _loadResultsMutex;
	std::map<std::string, std::pair<int64, class osgBridgeNode*>> _loadResults;

	std::mutex _pendingKillNodesMutex;
	std::vector <std::pair<int64, class osgBridgeNode* >> _pendingKillNodes;
};