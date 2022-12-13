#pragma once
#include <atomic>
#include <string>

/*
	定义模型LOD层级结构，对应与 osg::PagedLOD
*/
class UE_OSGBRIDGE_API osgBridgePagedLOD
{
public:
	osgBridgePagedLOD(class osgBridgeNode* pOwner, float threshold, FBoxSphereBounds boxSphereBounds, std::string successorFileName);
	~osgBridgePagedLOD();

	FORCEINLINE class osgBridgeNode* GetOwner();
	FORCEINLINE float GetThreshold() { return _threshold; }
	FORCEINLINE const FBoxSphereBounds& GetBoxSphereBounds() const { return _boxSphereBounds; }

	class osgBridgeGeometry* GetGeometry(int32 index);
	int32 GetNumGeometries();
	int32 AddGeometry(class osgBridgeGeometry* geometry);
	void SetGeometry(int32 index, class osgBridgeGeometry* geometry);

	FORCEINLINE const std::string& GetSuccessorFileName() const { return _successorFileName; }
	FORCEINLINE class osgBridgeNode* GetSuccessor();
	FORCEINLINE void SetSuccessor(class osgBridgeNode* successor);

	bool IsValid();

private:
	class osgBridgeNode* const _pOwner;
	const float _threshold;
	const FBoxSphereBounds _boxSphereBounds;

	std::vector<class osgBridgeGeometry*> _geometries;

	const std::string _successorFileName;
	class osgBridgeNode* _pSuccessor;
};