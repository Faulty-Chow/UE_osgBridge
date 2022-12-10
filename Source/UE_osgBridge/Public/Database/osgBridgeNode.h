#pragma once
#include <vector>
#include <osg/PagedLOD>
#include <osg/ref_ptr>

class UE_OSGBRIDGE_API osgBridgeNode
{
public:
	osgBridgeNode(const std::string& nodeFilePath, osg::ref_ptr<osg::Node> osgPlod);
	~osgBridgeNode();

	void Initialize(osg::ref_ptr<osg::Node> osgPlod);
	
	/*FORCEINLINE const FName& GetName() const;
	FORCEINLINE const FString& GetDatabaseKey() const;*/
	FORCEINLINE const std::string& GetNodeFilePath() const { return _nodeFilePath; }

	class osgBridgePagedLOD* GetPagedLOD(int32 index);
	int32 AddPagedLOD(class osgBridgePagedLOD* child);
	int32 GetNumPagedLODs();

	bool IsValid();

private:
	std::vector<class osgBridgePagedLOD*> _pagedLODs;
	const std::string& _nodeFilePath;
};