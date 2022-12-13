#pragma once
#include <vector>

/*
	模型的几何图形 场景更新时加载卸载的最小单位 对应于 osg::Geode
*/
class UE_OSGBRIDGE_API osgBridgeGeometry
{
public:
	osgBridgeGeometry(class osgBridgePagedLOD* pOwner, int32 index, int32 numMeshSections, FBoxSphereBounds boxSphereBounds);
	~osgBridgeGeometry();

public:
	FORCEINLINE const class osgBridgePagedLOD* GetOwner() const;
	FORCEINLINE const FBoxSphereBounds& GetBoxSphereBounds() const { return _boxSphereBounds; }

	FORCEINLINE int32 GetNumMeshSections();
	class osgBridgeMeshSection* GetMeshSection(int32 index);
	void SetMeshSection(int32 index, class osgBridgeMeshSection* meshSection);

	bool IsValid();

private:
	const class osgBridgePagedLOD* _pOwner;
	int32 _index;
	const FBoxSphereBounds _boxSphereBounds;

	std::vector<class osgBridgeMeshSection*> _meshSections;
};