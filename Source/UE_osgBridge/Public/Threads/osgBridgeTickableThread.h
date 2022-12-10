//#pragma once
//#include "osgBridgeBaseThread.h"
//
//class UE_OSGBRIDGE_API osgBridgeTickableThread :public osgBridgeBaseThread
//{
//public:
//	osgBridgeTickableThread(FString threadName);
//
//	void StartThread() = delete;
//	void TimeToDie(bool bShouldWait = true);
//
//	virtual void Tick(float DeltaTime);
//
//protected:
//	virtual uint32 Run() override;
//
//private:
//	void Traverse(class osgBridgeNode* node);
//
//	void AddToView(class osgBridgeGeometry* geometry);
//	void RemoveFromView(class osgBridgeGeometry* geometry);
//	void RemoveAllGeometriesBelongTo(class osgBridgePagedLOD* plod);
//	void ClearHigherLODs(class osgBridgePagedLOD* plod);
//
//private:
//	std::atomic_bool _bTimeToDie;
//
//	FString _databaseKey;
//};