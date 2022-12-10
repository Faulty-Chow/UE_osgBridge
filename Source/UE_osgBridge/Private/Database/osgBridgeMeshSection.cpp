#include "Database/osgBridgeMeshSection.h"
#include "Database/osgBridgeGeometry.h"
#include "RuntimeMeshCore.h"

#include "osg/Texture2D"
#include <osg/TriangleIndexFunctor>

osgBridgeMeshSection::MeshSectionData::MeshSectionData()
{
	_sectionID = INDEX_NONE;
	_materialSlotIndex = INDEX_NONE;
	_pTextureData = nullptr;
	_rows = 0;
	_cols = 0;
	_pTexture = nullptr;
	_pMaterial = nullptr;
	_bSetParma = false;
}

osgBridgeMeshSection::MeshSectionData::~MeshSectionData()
{
	_sectionID = INDEX_NONE;
	_materialSlotIndex = INDEX_NONE;

	_vertices.Empty();
	_triangles.Empty();
	_normals.Empty();
	_vertexColors.Empty();
	_UVs.Empty();
	_tangents.Empty();

	if (_pTextureData) {
		delete _pTextureData;
		_pTextureData = nullptr;
	}
	_rows = 0;
	_cols = 0;

	if (_pTexture.IsValid())
	{
		_pTexture->RemoveFromRoot();
		_pTexture->MarkPendingKill();
		// _pTexture = nullptr;
	}
	if (_pMaterial.IsValid())
	{
		_pMaterial->RemoveFromRoot();
		_pMaterial->MarkPendingKill();
		// _pTexture = nullptr;
	}
}

void osgBridgeMeshSection::Triangle::operator()(const unsigned int& v1, const unsigned int& v2, const unsigned int& v3)
{
	if (v1 == v2 || v1 == v3 || v2 == v3)
		return;
	_pMeshSection->_triangles.Add(v1);
	_pMeshSection->_triangles.Add(v2);
	_pMeshSection->_triangles.Add(v3);
}

osgBridgeMeshSection::osgBridgeMeshSection(osgBridgeGeometry* pOwner, int32 index) :
	_pOwner(pOwner), _index(index)
{
	_sectionID = INDEX_NONE;
	_materialSlotIndex = INDEX_NONE;
	_pTextureData = nullptr;
	_rows = 0;
	_cols = 0;
	_pTexture = nullptr;
	_pMaterial = nullptr;
}

osgBridgeMeshSection::~osgBridgeMeshSection()
{
	_sectionID = INDEX_NONE;
	_materialSlotIndex = INDEX_NONE;

	_vertices.Empty();
	_triangles.Empty();
	_normals.Empty();
	_vertexColors.Empty();
	_UVs.Empty();
	_tangents.Empty();

	if (_pTextureData) {
		delete _pTextureData;
		_pTextureData = nullptr;
	}
	_rows = 0;
	_cols = 0;

	if (_pTexture.IsValid())
	{
		_pTexture->MarkPendingKill();
		_pTexture.Reset();
	}
	if (_pMaterial.IsValid())
	{
		_pMaterial->MarkPendingKill();
		_pMaterial.Reset();
	}
}

bool osgBridgeMeshSection::IsValid()
{
	if (_vertices.Num() == 0)
		return false;
	if (_vertices.Num() != _UVs.Num())
		return false;
	if (_triangles.Num() % 3 != 0)
		return false;
	if (_pMaterial.IsValid() == false)
		return false;
	return true;
}

void osgBridgeMeshSection::Initialize(osg::ref_ptr<osg::Geometry> drawable)
{
	if (!drawable.valid())
		return;
	
	osg::ref_ptr<osg::Vec3Array> vertexArray = dynamic_cast<osg::Vec3Array*>(drawable->getVertexArray());
	osg::ref_ptr<osg::Vec3Array> normalArray = dynamic_cast<osg::Vec3Array*>(drawable->getNormalArray());
	osg::ref_ptr<osg::Texture2D> texture = dynamic_cast<osg::Texture2D*>(drawable->getStateSet()->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
	osg::ref_ptr<osg::Image> image = texture ? texture->getImage() : nullptr;
	osg::ref_ptr<osg::Vec2Array> vertsUV = dynamic_cast<osg::Vec2Array*>(drawable->getTexCoordArray(0));

	if (image)
	{
		_rows = image->t();
		_cols = image->s();
		int channels = 0;
		switch (image->getPixelFormat())
		{
		case 6407:
			channels = 3;
			break;
		case 6408:
			channels = 4;
			break;
		case 6409:
			channels = 1;
			break;
		}

		_pTextureData = new unsigned char[_rows * _cols * 4];
		check(_pTextureData);
		int pSrc = 0, pDst = 0;
		unsigned char* data = static_cast<unsigned char*>(_pTextureData);
		for (int y = 0; y < _rows; y++)
		{
			for (int x = 0; x < _cols; x++)
			{
				unsigned char r = image->data()[pSrc++];
				unsigned char g = image->data()[pSrc++];
				unsigned char b = image->data()[pSrc++];
				unsigned char a;
				if (channels == 4)
					a = image->data()[pSrc++];
				else
					a = UCHAR_MAX;
				data[pDst++] = b;
				data[pDst++] = g;
				data[pDst++] = r;
				data[pDst++] = a;
			}
		}
	}

	for (int j = 0; j < vertexArray->size(); j++)
	{
		_vertices.Emplace(100 * FVector{ vertexArray->operator[](j)._v[0],-vertexArray ->operator[](j)._v[1] ,vertexArray ->operator[](j)._v[2] });
		_UVs.Emplace(FVector2D{ vertsUV->operator[](j)._v[0], vertsUV->operator[](j)._v[1] });
		if (image)
		{
			osg::Vec4 color = image->getColor({ vertsUV->operator[](j)._v[0], vertsUV->operator[](j)._v[1] });
			_vertexColors.Emplace(FColor{ (uint8)(color.r() * 255),(uint8)(color.g() * 255),(uint8)(color.b() * 255) });
		}
		if (normalArray && !normalArray->empty())
			_normals.Emplace(FVector{ normalArray->operator[](j)._v[0],normalArray ->operator[](j)._v[1] ,normalArray ->operator[](j)._v[2] });
	}

	osg::TriangleIndexFunctor<Triangle> triangleIndex;
	triangleIndex._pMeshSection = this;
	drawable->accept(triangleIndex);
}

//bool osgBridgeMeshSection::IsValid()
//{
//	bool result = true;
//	bool bNeedUnlock = true;
//	try
//	{
//		_rwMutex.lock();
//	}
//	catch (std::system_error e)
//	{
//		bNeedUnlock = false;
//	}
//
//	if (!_pMaterial.IsValid() || !_bSetParma)
//		result = false;
//	if (_vertices.Num() > 0)
//	{
//		if (_vertices.Num() != _vertexColors.Num() ||
//			_vertices.Num() != _UVs.Num())
//			result = false;
//		if (_triangles.Num() % 3 != 0)
//			result = false;
//		if (_normals.Num() > 0 && _vertices.Num() != _normals.Num())
//			result = false;
//	}
//	else
//		result = false;
//
//	if (bNeedUnlock)
//		_rwMutex.unlock();
//
//	return result;
//}
//
//void osgBridgeMeshSection::Reset()
//{
//	bool bNeedUnlock = true;
//	try
//	{
//		_rwMutex.lock();
//	}
//	catch (std::system_error e)
//	{
//		bNeedUnlock = false;
//	}
//
//	_sectionID = INDEX_NONE;
//	_materialSlotIndex = INDEX_NONE;
//
//	_vertices.Empty();
//	_triangles.Empty();
//	_normals.Empty();
//	_vertexColors.Empty();
//	_UVs.Empty();
//	_tangents.Empty();
//
//	if (_pTextureData) {
//		delete _pTextureData;
//		_pTextureData = nullptr;
//	}
//	_rows = 0; 
//	_cols = 0;
//
//	if (_pTexture.IsValid())
//	{
//		_pTexture->RemoveFromRoot();
//		_pTexture->MarkPendingKill();
//		// _pTexture = nullptr;
//	}
//	if (_pMaterial.IsValid())
//	{
//		_pMaterial->RemoveFromRoot();
//		_pMaterial->MarkPendingKill();
//		// _pTexture = nullptr;
//	}
//	_bSetParma = false;
//
//	if(bNeedUnlock)
//		_rwMutex.unlock();
//}

//osgBridgeMeshSection::MeshSectionData* osgBridgeMeshSection::Lock()
//{
//	_rwMutex.lock();
//	return _pData;
//}
//
//void osgBridgeMeshSection::Unlock()
//{
//	_rwMutex.unlock();
//}