#pragma once
#include <vector>

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