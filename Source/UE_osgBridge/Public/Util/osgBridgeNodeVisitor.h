#pragma once
#include <osg/PagedLOD>
#include <osg/NodeVisitor>
#include <osg/Geode>

class UE_OSGBRIDGE_API osgBridgePagedLODVisitor:public osg::NodeVisitor
{
public:
	osgBridgePagedLODVisitor(class osgBridgeNode* invoker);
	virtual void apply(osg::PagedLOD& plod) override;

private:
	class osgBridgeNode* _pInvoker;
	int32 _applyCount;
};

class UE_OSGBRIDGE_API osgBridgeGeometryVisitor :public osg::NodeVisitor
{
public:
	osgBridgeGeometryVisitor(class osgBridgePagedLOD* invoker);
	virtual void apply(osg::Geode& geode) override;

private:
	class osgBridgePagedLOD* _pInvoker;
	int32 _applyCount;
};