#include "stdafx.h"
#include "Mesh.h"

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
	pd3dCommandList->IASetVertexBuffers(m_nSlot, m_nVertexBufferViews, m_pd3dVertexBufferViews.data());
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nSubset)
{
	// 메쉬를 렌더링하는 함수이다.
	
	if (m_nSubMeshes > 0)
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

	//정점 버퍼는 직육면체의 꼭지점 24개에 대한 정점 데이터를 가진다. 
	//front
	pVertices[0] = CTexturedVertexWithNormal(XMFLOAT3(+fx, -fy, +fz), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f));
	pVertices[1] = CTexturedVertexWithNormal(XMFLOAT3(+fx, +fy, +fz), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f));
	pVertices[2] = CTexturedVertexWithNormal(XMFLOAT3(-fx, +fy, +fz), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f));
	pVertices[3] = CTexturedVertexWithNormal(XMFLOAT3(-fx, -fy, +fz), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f));

	// back
	pVertices[4] = CTexturedVertexWithNormal(XMFLOAT3(-fx, +fy, -fz), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f));
	pVertices[5] = CTexturedVertexWithNormal(XMFLOAT3(+fx, +fy, -fz), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
	pVertices[6] = CTexturedVertexWithNormal(XMFLOAT3(+fx, -fy, -fz), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f));
	pVertices[7] = CTexturedVertexWithNormal(XMFLOAT3(-fx, -fy, -fz), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f));

	// top
	pVertices[8] = CTexturedVertexWithNormal(XMFLOAT3(-fx, +fy, +fz), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	pVertices[9] = CTexturedVertexWithNormal(XMFLOAT3(+fx, +fy, +fz), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
	pVertices[10] = CTexturedVertexWithNormal(XMFLOAT3(+fx, +fy, -fz), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
	pVertices[11] = CTexturedVertexWithNormal(XMFLOAT3(-fx, +fy, -fz), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));

	// bottom
	pVertices[12] = CTexturedVertexWithNormal(XMFLOAT3(+fx, -fy, -fz), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	pVertices[13] = CTexturedVertexWithNormal(XMFLOAT3(+fx, -fy, +fz), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
	pVertices[14] = CTexturedVertexWithNormal(XMFLOAT3(-fx, -fy, +fz), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
	pVertices[15] = CTexturedVertexWithNormal(XMFLOAT3(-fx, -fy, -fz), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));

	// left
	pVertices[17] = CTexturedVertexWithNormal(XMFLOAT3(-fx, +fy, -fz), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	pVertices[16] = CTexturedVertexWithNormal(XMFLOAT3(-fx, +fy, +fz), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
	pVertices[18] = CTexturedVertexWithNormal(XMFLOAT3(-fx, -fy, -fz), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
	pVertices[19] = CTexturedVertexWithNormal(XMFLOAT3(-fx, -fy, +fz), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));

	// right
	pVertices[20] = CTexturedVertexWithNormal(XMFLOAT3(+fx, +fy, -fz), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	pVertices[21] = CTexturedVertexWithNormal(XMFLOAT3(+fx, +fy, +fz), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
	pVertices[22] = CTexturedVertexWithNormal(XMFLOAT3(+fx, -fy, +fz), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
	pVertices[23] = CTexturedVertexWithNormal(XMFLOAT3(+fx, -fy, -fz), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	
	m_nSubMeshes = 1;
	m_nVertexBufferViews = 1;
	m_pd3dVertexBufferViews.resize(m_nVertexBufferViews);

	m_pd3dVertexBufferViews[0].BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[0].StrideInBytes = m_nStride;
	m_pd3dVertexBufferViews[0].SizeInBytes = m_nStride * m_nVertices;

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

	m_pnSubSetIndices[0] = m_nIndices;
	//인덱스 버퍼를 생성한다. 
	m_ppd3dIndexBuffers[0] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices.data(), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[0]);

	//인덱스 버퍼 뷰를 생성한다. 
	m_pd3dIndexBufferViews[0].BufferLocation = m_ppd3dIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_nIndices;
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
				m_pxmf3Positions.resize(nPositions);
				nReads = (UINT)::fread(m_pxmf3Positions.data(), sizeof(XMFLOAT3), nPositions, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, pInFile);
			if (nColors > 0)
			{
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
				m_pxmf2TextureCoords0.resize(nTextureCoords);
				nReads = (UINT)::fread(m_pxmf2TextureCoords0.data(), sizeof(XMFLOAT2), nTextureCoords, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_pxmf2TextureCoords1.resize(nTextureCoords);
				nReads = (UINT)::fread(m_pxmf2TextureCoords1.data(), sizeof(XMFLOAT2), nTextureCoords, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&nNormals, sizeof(int), 1, pInFile);
			if (nNormals > 0)
			{
				m_pxmf3Normals.resize(nNormals);
				nReads = (UINT)::fread(m_pxmf3Normals.data(), sizeof(XMFLOAT3), nNormals, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, pInFile);
			if (nTangents > 0)
			{
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

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);
	m_pd3dTextureCoord1Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords1.data(), sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord1UploadBuffer);
	m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);
	m_pd3dTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Tangents.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTangentUploadBuffer);
	m_pd3dBiTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3BiTangents.data(), sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBiTangentUploadBuffer);
	
	m_nVertexBufferViews = 6;
	m_pd3dVertexBufferViews.resize(m_nVertexBufferViews);

	m_pd3dVertexBufferViews[0].BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dVertexBufferViews[1].BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT2);
	m_pd3dVertexBufferViews[1].SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;

	m_pd3dVertexBufferViews[2].BufferLocation = m_pd3dTextureCoord1Buffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[2].StrideInBytes = sizeof(XMFLOAT2);
	m_pd3dVertexBufferViews[2].SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;

	m_pd3dVertexBufferViews[3].BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[3].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[3].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dVertexBufferViews[4].BufferLocation = m_pd3dTangentBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[4].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[4].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dVertexBufferViews[5].BufferLocation = m_pd3dBiTangentBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[5].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[5].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;


	// 서브 메쉬 인덱스 버퍼
	m_ppd3dIndexBuffers.resize(m_nSubMeshes);

	// 서브 메쉬 인덱스 업로드 버퍼
	m_ppd3dIndexUploadBuffers.resize(m_nSubMeshes);

	// 서브 메쉬 인덱스 버퍼 뷰
	m_pd3dIndexBufferViews.resize(m_nSubMeshes);

	for (int i = 0; i < m_nSubMeshes; ++i)
	{
		m_ppd3dIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i].data(), sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dIndexUploadBuffers[i]);

		m_pd3dIndexBufferViews[i].BufferLocation = m_ppd3dIndexBuffers[i]->GetGPUVirtualAddress();
		m_pd3dIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
		m_pd3dIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
	}
}
