#include "AsyncTasks/osgBridgeAsyncTaskQueue.h"
#include "AsyncTasks/osgBridgeAsyncTask.h"

template<typename T, QueueMode Mode>
osgBridgePriorityQueue<T, Mode>::osgBridgePriorityQueue() :
{
	std::function<bool(const T&, const T&)const> func_compare(this);
	_priorityQueue = std::set<T, decltype(func_compare)>(func_compare);
}

template<typename T, QueueMode Mode>
bool osgBridgePriorityQueue<T, Mode>::operator()(const T& lhs, const T& rhs) const
{
	return lhs < rhs;
}

template<typename T, QueueMode Mode>
void osgBridgePriorityQueue<T, Mode>::Enqueue(T task)
{
	std::unique_lock<std::mutex>* lock = new std::unique_lock<std::mutex>(_queueMutex, std::defer_lock_t);
	if (Mode == QueueMode::MPMC || Mode == QueueMode::MPSC)
		lock->lock();
	_priorityQueue.insert(task);
	delete lock;
}

template<typename T, QueueMode Mode>
bool osgBridgePriorityQueue<T, Mode>::Dequeue(T& task)
{
	std::unique_lock<std::mutex>* lock = new std::unique_lock<std::mutex>(_queueMutex, std::defer_lock_t);
	if (Mode == QueueMode::MPMC || Mode == QueueMode::MPSC)
		lock->lock();
	if (_priorityQueue.empty())
	{
		delete lock;
		return false;
	}
	else
	{
		task = *_priorityQueue.begin();
		_priorityQueue.erase(task);
		delete lock;
		return true;
	}
}

template<typename T, QueueMode Mode>
void osgBridgePriorityQueue<T, Mode>::Reset()
{
	std::unique_lock<std::mutex>* lock = new std::unique_lock<std::mutex>(_queueMutex, std::defer_lock_t);
	if (Mode == QueueMode::MPMC || Mode == QueueMode::MPSC)
		lock->lock();
	_priorityQueue.clear();
	delete lock;
}

template<typename T, QueueMode Mode>
int32 osgBridgePriorityQueue<T, Mode>::Size()
{
	int32 ans = 0;
	std::unique_lock<std::mutex>* lock = new std::unique_lock<std::mutex>(_queueMutex, std::defer_lock_t);
	if (Mode == QueueMode::MPMC || Mode == QueueMode::MPSC)
		lock->lock();
	ans = _priorityQueue.size();
	delete lock;
	return ans;
}


osgBridgeAsyncTaskPriorityQueue::osgBridgeAsyncTaskPriorityQueue(int32 maxSize):
	osgBridgePriorityQueue<osgBridgeAsyncTask*, QueueMode::MPMC>()
{
	_maxSize = maxSize;
}

void osgBridgeAsyncTaskPriorityQueue::Enqueue(osgBridgeAsyncTask* task)
{
	std::unique_lock<std::mutex> lock(_queueMutex);
	_priorityQueue.erase(task);
	task->UpdatePriority();
	_priorityQueue.insert(task);
	/*if (!Update(task))
		osgBridgePriorityQueue<osgBridgeAsyncTask*, QueueMode::MPMC>::Enqueue(task);*/
}

bool osgBridgeAsyncTaskPriorityQueue::Remove(osgBridgeAsyncTask* task)
{
	std::unique_lock<std::mutex> lock(_queueMutex);
	return _priorityQueue.erase(task) == 1;
}

//bool osgBridgeAsyncTaskPriorityQueue::Update(osgBridgeAsyncTask* task)
//{
//	std::unique_lock<std::mutex> lock(_queueMutex);
//	bool result = _priorityQueue.erase(task) == 1;
//	task->UpdatePriority();
//	if (result)
//		_priorityQueue.insert(task);
//	return result;
//}

bool osgBridgeAsyncTaskPriorityQueue::Exist(osgBridgeAsyncTask* task)
{
	std::unique_lock<std::mutex> lock(_queueMutex);
	return _priorityQueue.find(task) != _priorityQueue.end();
}