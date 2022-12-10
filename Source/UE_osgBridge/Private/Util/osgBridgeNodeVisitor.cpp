#include "Util/osgBridgeNodeVisitor.h"
#include "Database/osgBridgeNode.h"
#include "Database/osgBridgePagedLOD.h"
#include "Database/osgBridgeGeometry.h"
#include "Database/osgBridgeMeshSection.h"

#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/BoundingSphere>
#include <osg/TriangleIndexFunctor>

osgBridgePagedLODVisitor::osgBridgePagedLODVisitor(osgBridgeNode* invoker) :
	osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), _pInvoker(invoker)
{
	_applyCount = 0;
}

void osgBridgePagedLODVisitor::apply(osg::PagedLOD& plod)
{
	if (_pInvoker->GetPagedLOD(_applyCount))
	{
		if (_pInvoker->GetPagedLOD(_applyCount)->IsValid())
			return;
	}
	else
	{
		float threshold = plod.getRangeList().begin()->second;
		std::string successorFileName;
		if (plod.getNumFileNames() == 2)
			successorFileName = plod.getFileName(1);
		osg::BoundingSphere boundingSphere = plod.getBound();
		FVector origin(boundingSphere.center().x(), -boundingSphere.center().y(), boundingSphere.center().z());
		FBoxSphereBounds boxSphereBounds(FSphere(origin * 100, boundingSphere.radius() * 100));
		osgBridgePagedLOD* newPlod = new osgBridgePagedLOD(_pInvoker, threshold, boxSphereBounds, successorFileName);
		check(_pInvoker->AddPagedLOD(newPlod) == _applyCount);
		// _pInvoker->AddChild(_applyCount, nullptr);
	}
	osgBridgeGeometryVisitor geometryVisitor(_pInvoker->GetPagedLOD(_applyCount));
	plod.accept(geometryVisitor);
	_applyCount += 1;
}

osgBridgeGeometryVisitor::osgBridgeGeometryVisitor(osgBridgePagedLOD* invoker) :
	osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), _pInvoker(invoker)
{
	_applyCount = 0;
}

void osgBridgeGeometryVisitor::apply(osg::Geode& geode)
{
	uint32 numDrawables = geode.getNumDrawables();
	if (_pInvoker->GetGeometry(_applyCount))
	{
		if (_pInvoker->GetGeometry(_applyCount)->GetNumMeshSections() == numDrawables)
			return;
	}
	else
	{
		osg::BoundingSphere boundingSphere = geode.getBound();
		FVector origin(boundingSphere.center().x(), -boundingSphere.center().y(), boundingSphere.center().z());
		FBoxSphereBounds boxSphereBounds(FSphere(origin * 100, boundingSphere.radius() * 100));
		osgBridgeGeometry* newGeometry = new osgBridgeGeometry(_pInvoker, _applyCount, numDrawables, boxSphereBounds);
		_pInvoker->SetGeometry(_applyCount, newGeometry);
	}
	osgBridgeGeometry* geometry = _pInvoker->GetGeometry(_applyCount);
	for (uint32 i = 0; i < numDrawables; i++)
	{
		if (geometry->GetMeshSection(i))
		{
			if (geometry->GetMeshSection(i)->IsValid())
				continue;
		}
		else
		{
			osgBridgeMeshSection* meshSection = new osgBridgeMeshSection(geometry, i);
			geometry->SetMeshSection(i, meshSection);
		}
		osg::ref_ptr<osg::Geometry> drawable = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
		geometry->GetMeshSection(i)->Initialize(drawable);
	}
	_applyCount += 1;
}