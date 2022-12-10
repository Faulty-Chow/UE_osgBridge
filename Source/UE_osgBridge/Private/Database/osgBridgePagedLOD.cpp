#include "Database/osgBridgePagedLOD.h"
#include "Database/osgBridgeNode.h"
#include "Database/osgBridgeGeometry.h"
#include "AsyncTasks/osgBridgeFileReadTask.h"

osgBridgePagedLOD::osgBridgePagedLOD(osgBridgeNode* pOwner, float threshold,
	FBoxSphereBounds boxSphereBounds, std::string successorFileName) :
	_pOwner(pOwner), _threshold(threshold), _boxSphereBounds(boxSphereBounds),
	_successorFileName(successorFileName), _pSuccessor(nullptr)
{
}

osgBridgePagedLOD::~osgBridgePagedLOD()
{
	for (auto& geometry : _geometries)
	{
		if (geometry)
		{
			delete geometry;
			geometry = nullptr;
		}
	}
	_geometries.clear();

	if (_pSuccessor)
	{
		delete _pSuccessor;
		_pSuccessor = nullptr;
	}
}

osgBridgeNode* osgBridgePagedLOD::GetOwner()
{
	return _pOwner;
}

osgBridgeGeometry* osgBridgePagedLOD::GetGeometry(int32 index)
{
	if (index < _geometries.size())
		return _geometries[index];
	return nullptr;
}

int32 osgBridgePagedLOD::GetNumGeometries()
{
	return _geometries.size();
}

int32 osgBridgePagedLOD::AddGeometry(osgBridgeGeometry* geometry)
{
	_geometries.emplace_back(geometry);
	return _geometries.size() - 1;
}

void osgBridgePagedLOD::SetGeometry(int32 index, osgBridgeGeometry* geometry)
{
	if (index < _geometries.size())
	{
		// check(_geometries[index] == nullptr);
		_geometries[index] = geometry;
	}
	else
	{
		_geometries.resize(index + 1);
		_geometries[index] = geometry;
	}
}

osgBridgeNode* osgBridgePagedLOD::GetSuccessor()
{
	return _pSuccessor;
}

void osgBridgePagedLOD::SetSuccessor(class osgBridgeNode* successor)
{
	_pSuccessor = successor;
}

bool osgBridgePagedLOD::IsValid()
{
	for (auto& geometry : _geometries)
	{
		if (geometry == nullptr)
			return false;
	}
	return true;
}