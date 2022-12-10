#include "Database/osgBridgeGeometry.h"
#include "Database/osgBridgePagedLOD.h"
#include "Database/osgBridgeMeshSection.h"

osgBridgeGeometry::osgBridgeGeometry(osgBridgePagedLOD* pOwner, int32 index, int32 numMeshSections, FBoxSphereBounds boxSphereBounds) :
	_pOwner(pOwner), _index(index), _boxSphereBounds(boxSphereBounds)
{
	_meshSections.resize(numMeshSections);
}

osgBridgeGeometry::~osgBridgeGeometry()
{
	for (auto& meshSection : _meshSections)
	{
		if (meshSection)
		{
			delete meshSection;
			meshSection = nullptr;
		}
	}
	_meshSections.clear();
}

const osgBridgePagedLOD* osgBridgeGeometry::GetOwner() const
{
	return _pOwner; 
}

int32 osgBridgeGeometry::GetNumMeshSections()
{
	return _meshSections.size();
}

osgBridgeMeshSection* osgBridgeGeometry::GetMeshSection(int32 index)
{
	if (index < _meshSections.size())
		return _meshSections[index];
	else
		return nullptr;
}

void osgBridgeGeometry::SetMeshSection(int32 index, osgBridgeMeshSection* meshSection)
{
	check(index < _meshSections.size());
	// check(_meshSections[index] == nullptr);
	_meshSections[index] = meshSection;
}

bool osgBridgeGeometry::IsValid()
{
	for (int32 i = 0; i < _meshSections.size(); i++)
	{
		if (_meshSections[i] == nullptr)
			return false;
		if (_meshSections[i]->IsValid() == false)
			return false;
	}
	return true;
}