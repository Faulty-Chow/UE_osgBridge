//#pragma once
//#include "osgBridgeBaseThread.h"
//
//class UE_OSGBRIDGE_API osgBridgeCoreThread :public osgBridgeBaseThread
//{
//public:
//	osgBridgeCoreThread(FString threadName);
//
//	void StartThread() = delete;
//	void TimeToDie(bool bShouldWait = true);
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