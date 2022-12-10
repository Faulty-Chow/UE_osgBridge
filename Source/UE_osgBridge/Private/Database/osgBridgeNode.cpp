#include "Database/osgBridgeNode.h"
#include "Database/osgBridgePagedLOD.h"
#include "Util/osgBridgeNodeVisitor.h"
#include "Database/osgBridgeDatabase.h"

osgBridgeNode::osgBridgeNode(const std::string& nodeFilePath, osg::ref_ptr<osg::Node> osgPlod) :
	_nodeFilePath(nodeFilePath)
{
	Initialize(osgPlod);
}

osgBridgeNode::~osgBridgeNode()
{
	for (auto& plod : _pagedLODs)
	{
		if (plod)
		{
			delete plod;
			plod = nullptr;
		}
	}
	_pagedLODs.clear();
}

void osgBridgeNode::Initialize(osg::ref_ptr<osg::Node> osgPlod)
{
	if (osgPlod.valid())
	{
		osgBridgePagedLODVisitor plodVisitor(this);
		osgPlod->accept(plodVisitor);
	}
}

osgBridgePagedLOD* osgBridgeNode::GetPagedLOD(int32 index)
{
	if (index < _pagedLODs.size())
		return _pagedLODs[index];
	return nullptr;
}

int32 osgBridgeNode::AddPagedLOD(osgBridgePagedLOD* child)
{
	_pagedLODs.emplace_back(child);
	return _pagedLODs.size() - 1;
}

int32 osgBridgeNode::GetNumPagedLODs()
{
	return _pagedLODs.size();
}

bool osgBridgeNode::IsValid()
{
	for (auto& plod : _pagedLODs)
	{
		if (plod->IsValid() == false)
			return false;
	}
	return true;
}