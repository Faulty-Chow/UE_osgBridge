//#pragma once
//#include <set>
//#include <mutex>
//#include <functional>
//
//enum QueueMode
//{
//	/** Single-producer, single-consumer queue. */
//	SPSC,
//	/** Multiple-producers, single-consumer queue. */
//	MPSC,
//	/** Multiple-producers, multiple-consumer queue. */
//	MPMC
//};
//
//template<typename T, QueueMode Mode = QueueMode::MPMC>
//class UE_OSGBRIDGE_API osgBridgePriorityQueue
//{
//public:
//	osgBridgePriorityQueue();
//
//	virtual void Enqueue(T task);
//	virtual bool Dequeue(T& task);
//	virtual void Reset();
//	virtual int32 Size();
//
//	virtual bool operator()(const T& lhs, const T& rhs) const;
//
//protected:
//	std::mutex _queueMutex;
//	std::set<T, std::function<bool(T, T)>> _priorityQueue;
//};
//
//class UE_OSGBRIDGE_API osgBridgeAsyncTaskPriorityQueue :
//	public osgBridgePriorityQueue<class osgBridgeAsyncTask*, QueueMode::MPMC>
//{
//public:
//	osgBridgeAsyncTaskPriorityQueue(int32 maxSize);
//
//	virtual void Enqueue(class osgBridgeAsyncTask* task) override;
//	bool Remove(class osgBridgeAsyncTask* task);
//	// bool Update(class osgBridgeAsyncTask* task);
//	bool Exist(class osgBridgeAsyncTask* task);
//
//private:
//	int32 _maxSize;
//};