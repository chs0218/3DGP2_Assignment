#include "stdafx.h"
#include "Mesh.h"
#include "Object.h"

//----------------------------------------------------------------------------
// 22/09/21 Mesh 함수들 정의 - ChoiHS
//----------------------------------------------------------------------------

// Mesh 생성자
CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName)
{
	// pstrFileName에 파일의 이름이 인자로 넘어오면 pstrFileName으로 LoadMeshFromFile을 호출
	if (pstrFileName) LoadMeshFromFile(pd3dDevice, pd3dCommandList, pstrFileName);
}

CMesh::~CMesh()
{
}

void CMesh::ReleaseUploadBuffers()
{
	// UploadBuffer를 Default힙에 내용을 전달한 후 불려지는 함수
	// 오브젝트를 생성한 뒤 필요없어진 UploadBuffer들을 Release() 하는 함수
	if (m_pd3dPositionUploadBuffer) m_pd3dPositionUploadBuffer.Reset();
	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer.Reset();
	if (m_ppd3dIndexUploadBuffers.data())
	{
		m_ppd3dIndexUploadBuffers.clear();
		std::vector<ComPtr<ID3D12Resource>>().swap(m_ppd3dIndexUploadBuffers);
	}
};

void CMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 기본도형 위상구조를 삼각형리스트로 설정
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	// 정점 버퍼 뷰를 파이프라인의 입력 슬롯에 binding 한다.
	// 첫번째 인자는 입력 슬롯, 두번째는 정점 뷰의 개수, 세번째는 정점 버퍼 뷰의 첫 원소를 가르키는 포인터이다.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, m_pd3dVertexBufferViews.size(), m_pd3dVertexBufferViews.data());
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nSubset)
{
	// 메쉬를 렌더링하는 함수이다.
	if ((m_nSubMeshes > 0) && (nSubset < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&m_pd3dIndexBufferViews[nSubset]);
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubset], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CMesh::LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName)
{
#ifdef _WITH_TEXT_MESH
	ifstream InFile(pstrFileName);

	char pstrToken[64] = { '\0' };

	for (; ; )
	{
		InFile >> pstrToken;
		if (!InFile) break;

		if (!strcmp(pstrToken, "<Vertices>:"))
		{
			InFile >> m_nVertices;
			m_pxmf3Positions = new XMFLOAT3[m_nVertices];
			for (UINT i = 0; i < m_nVertices; i++) InFile >> m_pxmf3Positions[i].x >> m_pxmf3Positions[i].y >> m_pxmf3Positions[i].z;
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			InFile >> pstrToken;
			m_pxmf3Normals = new XMFLOAT3[m_nVertices];
			for (UINT i = 0; i < m_nVertices; i++) InFile >> m_pxmf3Normals[i].x >> m_pxmf3Normals[i].y >> m_pxmf3Normals[i].z;
		}
		else if (!strcmp(pstrToken, "<Indices>:"))
		{
			InFile >> m_nIndices;
			m_pnIndices = new UINT[m_nIndices];
			for (UINT i = 0; i < m_nIndices; i++) InFile >> m_pnIndices[i];
		}
	}
#else
	FILE* pFile = NULL;
	::fopen_s(&pFile, pstrFileName, "rb");
	::rewind(pFile);

	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	while (!::feof(pFile))
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pFile);
		if (nReads == 0) break;
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<BoundingBox>:"))
		{
			nReads = (UINT)::fread(&m_xmBoundingBox.Center, sizeof(float), 3, pFile);
			nReads = (UINT)::fread(&m_xmBoundingBox.Extents, sizeof(float), 3, pFile);
		}
		else if (!strcmp(pstrToken, "<Vertices>:"))
		{
			nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pFile);
			m_pxmf3Positions.resize(m_nVertices);
			nReads = (UINT)::fread(m_pxmf3Positions.data(), sizeof(float), 3 * m_nVertices, pFile);
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pFile);
			m_pxmf3Normals.resize(m_nVertices);
			nReads = (UINT)::fread(m_pxmf3Normals.data(), sizeof(float), 3 * m_nVertices, pFile);
		}
		else if (!strcmp(pstrToken, "<Indices>:"))
		{
			nReads = (UINT)::fread(&m_nIndices, sizeof(int), 1, pFile);
			m_pnIndices.resize(m_nIndices);
			nReads = (UINT)::fread(m_pnIndices.data(), sizeof(UINT), m_nIndices, pFile);
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			nReads = (UINT)::fread(&m_nSubMeshes, sizeof(int), 1, pFile);

			m_pnSubSetIndices.resize(m_nSubMeshes);
			m_pnSubSetStartIndices.resize(m_nSubMeshes);
			m_ppnSubSetIndices.resize(m_nSubMeshes);

			for (UINT i = 0; i < m_nSubMeshes; i++)
			{
				nReads = (UINT)::fread(&m_pnSubSetStartIndices[i], sizeof(UINT), 1, pFile);
				nReads = (UINT)::fread(&m_pnSubSetIndices[i], sizeof(UINT), 1, pFile);
				nReads = (UINT)::fread(&m_nIndices, sizeof(int), 1, pFile);
				m_ppnSubSetIndices[i].resize(m_pnSubSetIndices[i]);
				nReads = (UINT)::fread(m_ppnSubSetIndices[i].data(), sizeof(UINT), m_pnSubSetIndices[i], pFile);
			}

			break;
		}
	}

	::fclose(pFile);
#endif

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

	m_nVertexBufferViews = 2;
	m_pd3dVertexBufferViews.resize(m_nVertexBufferViews);

	m_pd3dVertexBufferViews[0].BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dVertexBufferViews[1].BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[1].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_ppd3dIndexBuffers.resize(m_nSubMeshes);
	m_ppd3dIndexUploadBuffers.resize(m_nSubMeshes);
	m_pd3dIndexBufferViews.resize(m_nSubMeshes);

	for (UINT i = 0; i < m_nSubMeshes; i++)
	{
		m_ppd3dIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i].data(), sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[i]);

		m_pd3dIndexBufferViews[i].BufferLocation = m_ppd3dIndexBuffers[i]->GetGPUVirtualAddress();
		m_pd3dIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
		m_pd3dIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
	}
}

//========================== 임시 메쉬 ==========================
CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nStride = sizeof(CTexturedVertexWithNormal);
	//직육면체는 꼭지점(정점)이 8개이다. 
	m_nVertices = 24;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	CTexturedVertexWithNormal pVertices[24];
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fy, +fz));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(0.0f, 1.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(0.0f, 0.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(1.0f, 0.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(1.0f, 1.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));

	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fy, -fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fy, -fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fy, -fz));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fy, -fz));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(0.0f, 0.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(1.0f, 0.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(1.0f, 1.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(0.0f, 1.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));

	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fy, -fz));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fy, -fz));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(0.0f, 0.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(1.0f, 0.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(0.0f, 1.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(1.0f, 1.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));

	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fy, -fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fy, -fz));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(0.0f, 0.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(0.0f, 1.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(1.0f, 1.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(1.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));

	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, +fy, -fz));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fy, -fz));
	m_pxmf3Positions.push_back(XMFLOAT3(-fx, -fy, +fz));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(0.0f, 0.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(1.0f, 0.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(1.0f, 1.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(0.0f, 1.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));

	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fy, -fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, +fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fy, +fz));
	m_pxmf3Positions.push_back(XMFLOAT3(+fx, -fy, -fz));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(0.0f, 0.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(1.0f, 0.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(1.0f, 1.0f));
	m_pxmf2TextureCoords0.push_back(XMFLOAT2(0.0f, 1.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_pxmf3Normals.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	D3D12_VERTEX_BUFFER_VIEW m_pd3dPositionBufferView;
	m_pd3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
	m_pd3dVertexBufferViews.emplace_back(m_pd3dPositionBufferView);

	m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

	D3D12_VERTEX_BUFFER_VIEW m_pd3dTextureCoord0BufferView;
	m_pd3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
	m_pd3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
	m_pd3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
	m_pd3dVertexBufferViews.push_back(m_pd3dTextureCoord0BufferView);

	m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

	D3D12_VERTEX_BUFFER_VIEW m_pd3dNormalBufferView;
	m_pd3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_pd3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
	m_pd3dVertexBufferViews.push_back(m_pd3dNormalBufferView);

	m_nSubMeshes = 1;
	m_nIndices = 36;
	m_pnIndices.resize(m_nIndices);
	//Back
	m_pnIndices[0] = 0; m_pnIndices[1] = 1; m_pnIndices[2] = 2;
	m_pnIndices[3] = 0; m_pnIndices[4] = 2; m_pnIndices[5] = 3;
	//Front
	m_pnIndices[6] = 4; m_pnIndices[7] = 5; m_pnIndices[8] = 6;
	m_pnIndices[9] = 4; m_pnIndices[10] = 6; m_pnIndices[11] = 7;
	//Top
	m_pnIndices[12] = 8; m_pnIndices[13] = 9; m_pnIndices[14] = 10;
	m_pnIndices[15] = 8; m_pnIndices[16] = 10; m_pnIndices[17] = 11;
	//Bottom
	m_pnIndices[18] = 12; m_pnIndices[19] = 13; m_pnIndices[20] = 14;
	m_pnIndices[21] = 12; m_pnIndices[22] = 14; m_pnIndices[23] = 15;
	//Left
	m_pnIndices[24] = 16; m_pnIndices[25] = 17; m_pnIndices[26] = 18;
	m_pnIndices[27] = 16; m_pnIndices[28] = 18; m_pnIndices[29] = 19;
	//Right
	m_pnIndices[30] = 20; m_pnIndices[31] = 21; m_pnIndices[32] = 22;
	m_pnIndices[33] = 20; m_pnIndices[34] = 22; m_pnIndices[35] = 23;

	m_nSubMeshes = 1;

	m_ppd3dIndexBuffers.resize(m_nSubMeshes);
	m_ppd3dIndexUploadBuffers.resize(m_nSubMeshes);
	m_pd3dIndexBufferViews.resize(m_nSubMeshes);
	m_pnSubSetIndices.resize(m_nSubMeshes);

	m_pnSubSetIndices[0] = 36;
	//인덱스 버퍼를 생성한다. 
	m_ppd3dIndexBuffers[0] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices.data(), sizeof(UINT) * 36, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[0]);

	//인덱스 버퍼 뷰를 생성한다. 
	m_pd3dIndexBufferViews[0].BufferLocation = m_ppd3dIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dIndexBufferViews[0].SizeInBytes = sizeof(UINT) * 36;
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

void CTexturedModelingMesh::LoadTexturedMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	BYTE nStrLength = 0;
	int nPositions = 0, nColors = 0, nNormals = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	UINT nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(m_pstrMeshName, sizeof(char), nStrLength, pInFile);
	m_pstrMeshName[nStrLength] = '\0';

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nReads = (UINT)::fread(&nPositions, sizeof(int), 1, pInFile);
			if (nPositions > 0)
			{
				m_nType |= VERTEXT_POSITION;
				m_pxmf3Positions.resize(nPositions);
				nReads = (UINT)::fread(m_pxmf3Positions.data(), sizeof(XMFLOAT3), nPositions, pInFile);
				m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_pd3dPositionBufferView;
				m_pd3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
				m_pd3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_pd3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
				m_pd3dVertexBufferViews.emplace_back(m_pd3dPositionBufferView);
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, pInFile);
			if (nColors > 0)
			{
				m_nType |= VERTEXT_COLOR;
				std::vector<XMFLOAT4> m_pxmf4Colors;
				m_pxmf4Colors.resize(nColors);
				nReads = (UINT)::fread(m_pxmf4Colors.data(), sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD0;
				m_pxmf2TextureCoords0.resize(nTextureCoords);
				nReads = (UINT)::fread(m_pxmf2TextureCoords0.data(), sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_pd3dTextureCoord0BufferView;
				m_pd3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
				m_pd3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_pd3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
				m_pd3dVertexBufferViews.push_back(m_pd3dTextureCoord0BufferView);
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD1;

				m_pxmf2TextureCoords1.resize(nTextureCoords);
				nReads = (UINT)::fread(m_pxmf2TextureCoords1.data(), sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord1Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords1.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord1UploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_pd3dTextureCoord1BufferView;
				m_pd3dTextureCoord1BufferView.BufferLocation = m_pd3dTextureCoord1Buffer->GetGPUVirtualAddress();
				m_pd3dTextureCoord1BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_pd3dTextureCoord1BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
				m_pd3dVertexBufferViews.push_back(m_pd3dTextureCoord1BufferView);
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&nNormals, sizeof(int), 1, pInFile);
			if (nNormals > 0)
			{
				m_nType |= VERTEXT_NORMAL;
				m_pxmf3Normals.resize(nNormals);
				nReads = (UINT)::fread(m_pxmf3Normals.data(), sizeof(XMFLOAT3), nNormals, pInFile);

				m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

				D3D12_VERTEX_BUFFER_VIEW m_pd3dNormalBufferView;
				m_pd3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
				m_pd3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_pd3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
				m_pd3dVertexBufferViews.push_back(m_pd3dNormalBufferView);
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, pInFile);
			if (nTangents > 0)
			{
				m_nType |= VERTEXT_TANGENT;
				m_pxmf3Tangents.resize(nTangents);
				nReads = (UINT)::fread(m_pxmf3Tangents.data(), sizeof(XMFLOAT3), nTangents, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			nReads = (UINT)::fread(&nBiTangents, sizeof(int), 1, pInFile);
			if (nBiTangents > 0)
			{
				m_pxmf3BiTangents.resize(nBiTangents);
				nReads = (UINT)::fread(m_pxmf3BiTangents.data(), sizeof(XMFLOAT3), nBiTangents, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			nReads = (UINT)::fread(&(m_nSubMeshes), sizeof(int), 1, pInFile);
			if (m_nSubMeshes > 0)
			{
				// 서브 메쉬 인덱스 개수
				m_pnSubSetIndices.resize(m_nSubMeshes);
				m_ppnSubSetIndices.resize(m_nSubMeshes);


				// 서브 메쉬 인덱스 버퍼
				m_ppd3dIndexBuffers.resize(m_nSubMeshes);

				// 서브 메쉬 인덱스 업로드 버퍼
				m_ppd3dIndexUploadBuffers.resize(m_nSubMeshes);

				// 서브 메쉬 인덱스 버퍼 뷰
				m_pd3dIndexBufferViews.resize(m_nSubMeshes);

				for (int i = 0; i < m_nSubMeshes; i++)
				{
					nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
					nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
					pstrToken[nStrLength] = '\0';
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						nReads = (UINT)::fread(&nIndex, sizeof(int), 1, pInFile);
						nReads = (UINT)::fread(&(m_pnSubSetIndices[i]), sizeof(int), 1, pInFile);
						if (m_pnSubSetIndices[i] > 0)
						{
							// 서브 메쉬들의 인덱스 들
							m_ppnSubSetIndices[i].resize(m_pnSubSetIndices[i]);
							nReads = (UINT)::fread(m_ppnSubSetIndices[i].data(), sizeof(UINT) * m_pnSubSetIndices[i], 1, pInFile);

							m_ppd3dIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i].data(), sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[i]);

							m_pd3dIndexBufferViews[i].BufferLocation = m_ppd3dIndexBuffers[i]->GetGPUVirtualAddress();
							m_pd3dIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
							m_pd3dIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
						}
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexturedRectMesh::CTexturedRectMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth, float fxPosition, float fyPosition, float fzPosition) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CTexturedVertex pVertices[6];

	float fx = (fWidth * 0.5f) + fxPosition, fy = (fHeight * 0.5f) + fyPosition, fz = (fDepth * 0.5f) + fzPosition;

	if (fWidth == 0.0f)
	{
		if (fxPosition > 0.0f)
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
		}
	}
	else if (fHeight == 0.0f)
	{
		if (fyPosition > 0.0f)
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 0.0f));
		}
	}
	else if (fDepth == 0.0f)
	{
		if (fzPosition > 0.0f)
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
		}
	}

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	m_d3dVertexBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	m_pd3dVertexBufferViews.push_back(m_d3dVertexBufferView);
}

CTexturedRectMesh::~CTexturedRectMesh()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CRawFormatImage::CRawFormatImage(LPCTSTR pFileName, int nWidth, int nLength, bool bFlipY)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	BYTE* pRawImagePixels = new BYTE[m_nWidth * m_nLength];

	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pRawImagePixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	if (bFlipY)
	{
		m_pRawImagePixels = new BYTE[m_nWidth * m_nLength];
		for (int z = 0; z < m_nLength; z++)
		{
			for (int x = 0; x < m_nWidth; x++)
			{
				m_pRawImagePixels[x + ((m_nLength - 1 - z) * m_nWidth)] = pRawImagePixels[x + (z * m_nWidth)];
			}
		}

		if (pRawImagePixels) delete[] pRawImagePixels;
	}
	else
	{
		m_pRawImagePixels = pRawImagePixels;
	}
}

CRawFormatImage::~CRawFormatImage()
{
	if (m_pRawImagePixels) delete[] m_pRawImagePixels;
	m_pRawImagePixels = NULL;
}

CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale) : CRawFormatImage(pFileName, nWidth, nLength, true)
{
	m_xmf3Scale = xmf3Scale;
}

CHeightMapImage::~CHeightMapImage()
{
}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength)) return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	float y1 = (float)m_pRawImagePixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pRawImagePixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pRawImagePixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);

	return(xmf3Normal);
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER

float CHeightMapImage::GetHeight(float fx, float fz, bool bReverseQuad)
{
	fx = fx / m_xmf3Scale.x;
	fz = fz / m_xmf3Scale.z;
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);

	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pRawImagePixels[x + (z * m_nWidth)];
	float fBottomRight = (float)m_pRawImagePixels[(x + 1) + (z * m_nWidth)];
	float fTopLeft = (float)m_pRawImagePixels[x + ((z + 1) * m_nWidth)];
	float fTopRight = (float)m_pRawImagePixels[(x + 1) + ((z + 1) * m_nWidth)];
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	if (bReverseQuad)
	{
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return(fHeight);
}

CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* pContext) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = nWidth * nLength;
	//	m_nStride = sizeof(CTexturedVertex);
	m_nStride = sizeof(CDiffused2TexturedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	std::vector<CDiffused2TexturedVertex> pVertices(m_nVertices); 

	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	int cxHeightMap = m_nWidth;
	int czHeightMap = m_nLength;
	if (pHeightMapImage)
	{
		cxHeightMap = pHeightMapImage->GetRawImageWidth();
		czHeightMap = pHeightMapImage->GetRawImageLength();
	}

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			fHeight = OnGetHeight(x, z, pContext);
			pVertices[i].m_xmf3Position = XMFLOAT3((x * m_xmf3Scale.x), fHeight, (z * m_xmf3Scale.z));
			pVertices[i].m_xmf4Diffuse = Vector4::Add(OnGetColor(x, z, pContext), xmf4Color);
			pVertices[i].m_xmf2TexCoord0 = XMFLOAT2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) / float(czHeightMap - 1));
			pVertices[i].m_xmf2TexCoord1 = XMFLOAT2(float(x) / float(m_xmf3Scale.x * 0.5f), float(z) / float(m_xmf3Scale.z * 0.5f));
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices.data(), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	m_d3dVertexBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	m_pd3dVertexBufferViews.push_back(m_d3dVertexBufferView);

	m_nIndices = ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1);
	std::vector<UINT> pnIndices(m_nIndices);
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			for (int x = 0; x < nWidth; x++)
			{
				if ((x == 0) && (z > 0)) pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			for (int x = nWidth - 1; x >= 0; x--)
			{
				if (x == (nWidth - 1)) pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	m_nSubMeshes = 1;
	m_pnSubSetIndices.resize(m_nSubMeshes);
	m_ppd3dIndexBuffers.resize(m_nSubMeshes);
	m_ppd3dIndexUploadBuffers.resize(m_nSubMeshes);
	m_pd3dIndexBufferViews.resize(m_nSubMeshes);

	m_pnSubSetIndices[0] = m_nIndices;
	m_ppd3dIndexBuffers[0] = CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices.data(), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[0]);

	m_pd3dIndexBufferViews[0].BufferLocation = m_ppd3dIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_nIndices;
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void* pContext)
{
	float fHeight = 0.0f;
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	if (pHeightMapImage)
	{
		BYTE* pHeightMapPixels = pHeightMapImage->GetRawImagePixels();
		XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
		int nWidth = pHeightMapImage->GetRawImageWidth();
		fHeight = pHeightMapPixels[x + (z * nWidth)] * xmf3Scale.y;
	}
	return(fHeight);
}

XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void* pContext)
{
	XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	if (pHeightMapImage)
	{
		XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
		xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);
		CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
		XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
		XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);
		float fScale = Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z), xmf3LightDirection);
		fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z), xmf3LightDirection);
		fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1), xmf3LightDirection);
		fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1), xmf3LightDirection);
		fScale = (fScale / 4.0f) + 0.05f;
		if (fScale > 1.0f) fScale = 1.0f;
		if (fScale < 0.25f) fScale = 0.25f;
		xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);
	}
	return(xmf4Color);
}

CBillBoardPointMesh::CBillBoardPointMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::vector<XMFLOAT3>* pv_Vertices, UINT nType)
{
	std::vector<CTreeVertex> pVertices;
	XMFLOAT2 xmf2Size = { 0.0f, 0.0f };
	switch (nType)
	{
	case 0:
		xmf2Size.x = 8.0f;
		xmf2Size.y = 8.0f;
		break;
	case 1:
		xmf2Size.x = 8.0f;
		xmf2Size.y = 8.0f;
		break;
	case 2:
		xmf2Size.x = 24.0f;
		xmf2Size.y = 36.0f;
		break;
	case 3:
		xmf2Size.x = 16.0f;
		xmf2Size.y = 46.0f;
		break;
	case 4:
		xmf2Size.x = 16.0f;
		xmf2Size.y = 46.0f;
		break;
	case 5:
		xmf2Size.x = 8.0f;
		xmf2Size.y = 16.0f;
		break;
	case 6:
		xmf2Size.x = 8.0f;
		xmf2Size.y = 16.0f;
		break;
	default:
		break;
	}
	
	for (int i = 0; i < pv_Vertices->size(); ++i)
	{
		pVertices.push_back(CTreeVertex((*pv_Vertices)[i], xmf2Size));
	}

	m_nStride = sizeof(CTreeVertex);
	m_nVertices = pVertices.size();
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices.data(), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	m_d3dVertexBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_pd3dVertexBufferViews.push_back(m_d3dVertexBufferView);
}

CBillBoardPointMesh::~CBillBoardPointMesh()
{
}
