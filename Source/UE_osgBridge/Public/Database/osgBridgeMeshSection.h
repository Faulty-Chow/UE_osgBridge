#pragma once
#include "UObject/StrongObjectPtr.h"

#include <mutex>
#include <functional>

#include <RuntimeMeshCore.h>

#include <osg/ref_ptr>
#include <osg/Geometry>

class UE_OSGBRIDGE_API osgBridgeMeshSection
{
public:
	struct UE_OSGBRIDGE_API MeshSectionData
	{
		int32 _sectionID;
		int32 _materialSlotIndex;
		TArray<FVector> _vertices;
		TArray<int32> _triangles;
		TArray<FVector> _normals;
		TArray<FColor> _vertexColors;
		TArray<FVector2D> _UVs;
		TArray<FRuntimeMeshTangent> _tangents;

		void* _pTextureData;
		int32 _rows, _cols;

		bool _bSetParma;
		TWeakObjectPtr<UTexture2D> _pTexture;
		TWeakObjectPtr<UMaterialInstanceDynamic> _pMaterial;

	private:
		MeshSectionData();
		~MeshSectionData();

		friend class osgBridgeMeshSection;
	};

	struct Triangle
	{
		void operator()(const unsigned int& v1, const unsigned int& v2, const unsigned int& v3);
		osgBridgeMeshSection* _pMeshSection;
	};
	friend struct Triangle;

public:
	osgBridgeMeshSection(class osgBridgeGeometry* pOwner, int32 index);
	~osgBridgeMeshSection();

	void Initialize(osg::ref_ptr<osg::Geometry> drawable);
	bool IsValid();

	/*MeshSectionData* Lock();
	void Unlock();*/

	class osgBridgeGeometry* _pOwner;
	int32 _index;

	int32 _sectionID;
	int32 _materialSlotIndex;
	TArray<FVector> _vertices;
	TArray<int32> _triangles;
	TArray<FVector> _normals;
	TArray<FColor> _vertexColors;
	TArray<FVector2D> _UVs;
	TArray<FRuntimeMeshTangent> _tangents;

	void* _pTextureData;
	int32 _rows, _cols;

	TStrongObjectPtr<UTexture2D> _pTexture;
	TStrongObjectPtr<UMaterialInstanceDynamic> _pMaterial;

	/*MeshSectionData* _pData;
	std::mutex _rwMutex;*/
};