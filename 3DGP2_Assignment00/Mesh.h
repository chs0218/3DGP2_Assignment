#pragma once
#include "stdafx.h"
//----------------------------------------------------------------------------
// 22/09/20 CMesh 클래스 추가 - Leejh
//----------------------------------------------------------------------------

#define VERTEXT_POSITION				0x01
#define VERTEXT_COLOR					0x02
#define VERTEXT_NORMAL					0x04
#define VERTEXT_TANGENT					0x08
#define VERTEXT_TEXTURE_COORD0			0x10
#define VERTEXT_TEXTURE_COORD1			0x20

class CVertex
{
public:
	XMFLOAT3						m_xmf3Position;

public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};

class CVertexWithNormal : public CVertex
{
public:
	XMFLOAT3						m_xmf3Normal;

public:
	CVertexWithNormal() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertexWithNormal(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal) { m_xmf3Position = xmf3Position; m_xmf3Normal = xmf3Normal; }
	~CVertexWithNormal() { }
};

class CTexturedVertexWithNormal : public CVertexWithNormal
{
public:
	XMFLOAT2						m_xmf2TexCoord;

public:
	CTexturedVertexWithNormal() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); }
	CTexturedVertexWithNormal(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal, XMFLOAT2 xmf2TexCoord) { m_xmf3Position = xmf3Position; m_xmf3Normal = xmf3Normal; m_xmf2TexCoord = xmf2TexCoord; }
	~CTexturedVertexWithNormal() { }
};
//-----------------------------------------------------------------------------


class CMesh
{
protected:
	UINT m_nType = 0x00;
	UINT m_nVertices = 0;

	std::vector<XMFLOAT3> m_pxmf3Positions;
	ComPtr<ID3D12Resource> m_pd3dPositionBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dPositionUploadBuffer = NULL;

	std::vector<XMFLOAT3> m_pxmf3Normals;
	ComPtr<ID3D12Resource> m_pd3dNormalBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dNormalUploadBuffer = NULL;

	UINT m_nVertexBufferViews = 0;
	std::vector<D3D12_VERTEX_BUFFER_VIEW> m_pd3dVertexBufferViews;

	UINT m_nIndices = 0;
	std::vector<UINT> m_pnIndices;

	std::vector<UINT> m_pnSubSetIndices;
	std::vector<UINT> m_pnSubSetStartIndices;
	std::vector<std::vector<UINT>> m_ppnSubSetIndices;

	std::vector<ComPtr<ID3D12Resource>> m_ppd3dIndexBuffers;
	std::vector<ComPtr<ID3D12Resource>> m_ppd3dIndexUploadBuffers;
	std::vector<D3D12_INDEX_BUFFER_VIEW> m_pd3dIndexBufferViews;

	D3D12_PRIMITIVE_TOPOLOGY        m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT                            m_nSlot = 0;
	UINT                            m_nSubMeshes = 0;
	UINT                            m_nStride = 0;
	UINT                            m_nOffset = 0;
	UINT                            m_nStartIndex = 0;
	int                             m_nBaseVertex = 0;
	BoundingBox                     m_xmBoundingBox;
public:
	CMesh() {};
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName = NULL);
	virtual ~CMesh();
	virtual void ReleaseUploadBuffers();

	void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nSubset);

	void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName);
};

class CCubeMeshDiffused : public CMesh
{
public:
	CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshDiffused();
};

class CTexturedModelingMesh : public CMesh
{
protected:
	char m_pstrMeshName[256] = { 0 };

	XMFLOAT3 m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	std::vector<XMFLOAT2> m_pxmf2TextureCoords0;
	ComPtr<ID3D12Resource> m_pd3dTextureCoord0Buffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dTextureCoord0UploadBuffer = NULL;

	std::vector<XMFLOAT2> m_pxmf2TextureCoords1;
	ComPtr<ID3D12Resource> m_pd3dTextureCoord1Buffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dTextureCoord1UploadBuffer = NULL;

	std::vector<XMFLOAT3> m_pxmf3Tangents;
	ComPtr<ID3D12Resource> m_pd3dTangentBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dTangentUploadBuffer = NULL;

	std::vector<XMFLOAT3> m_pxmf3BiTangents;
	ComPtr<ID3D12Resource> m_pd3dBiTangentBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dBiTangentUploadBuffer = NULL;
public:
	CTexturedModelingMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
	virtual ~CTexturedModelingMesh() {};

	virtual void ReleaseUploadBuffers() {};
	void LoadTexturedMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
};