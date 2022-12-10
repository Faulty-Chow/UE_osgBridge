#include "AsyncTasks/osgBridgeAsyncTask.h"

int32 osgBridgeAsyncTask::GetPriority(bool bUpdate /*= true*/)
{
	if (bUpdate)
		UpdatePriority();
	return _priority;
}

bool osgBridgeAsyncTask::operator<(const osgBridgeAsyncTask& rhs) const
{
	if (this->_priority != rhs._priority)
		return this->_priority < rhs._priority;
	else
		return this < &rhs;
}