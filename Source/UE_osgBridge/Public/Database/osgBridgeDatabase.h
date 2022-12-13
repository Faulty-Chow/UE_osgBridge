#pragma once
#include <mutex>
#include <map>
#include <vector>

/*
	ȫ�ֵ��� ���ڴ������ģ������
*/
class UE_OSGBRIDGE_API osgBridgeDatabase
{
private:
	/*
		�����ã�����ֻ��Ϊ�˼���(maybe)
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