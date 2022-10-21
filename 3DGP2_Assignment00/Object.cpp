#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "Camera.h"
#include "Enemy.h"

CGameObject::CGameObject()
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
}

CGameObject::~CGameObject()
{
	ReleaseShaderVariables();
}

void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh)
		m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::SetChild(std::shared_ptr<CGameObject> pChild, bool bReferenceUpdate)
{
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
	if (pChild)
	{
		pChild->m_pParent = this;
	}
}

void CGameObject::SetShader(std::shared_ptr<CShader> pShader, std::shared_ptr<CTexture> pTexture)
{
	if (!m_ppMaterials.data())
	{
		m_nMaterials = 1;
		m_ppMaterials.resize(m_nMaterials);
		std::shared_ptr<CMaterial> pMaterial = std::make_shared<CMaterial>();
		pMaterial->SetShader(pShader);
		if (pTexture)
			pMaterial->SetTexture(pTexture);
		m_ppMaterials[0] = pMaterial;
	}
}

void CGameObject::SetMaterial(int nMaterial, std::shared_ptr<CMaterial> pMaterial)
{
	m_ppMaterials[nMaterial] = pMaterial;
}

void CGameObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed, pxmf4x4Parent);
	if (m_pChild) m_pChild->Animate(fTimeElapsed, &m_xmf4x4World);
}

void CGameObject::SetLookAt(const XMFLOAT3& xmf3Target, const XMFLOAT3& xmf3Up)
{
	XMFLOAT3 xmf3Position(m_xmf4x4Transform._41, m_xmf4x4Transform._42, m_xmf4x4Transform._43);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(xmf3Position, xmf3Target, xmf3Up);
	m_xmf4x4Transform._11 = mtxLookAt._11; m_xmf4x4Transform._12 = mtxLookAt._21; m_xmf4x4Transform._13 = mtxLookAt._31;
	m_xmf4x4Transform._21 = mtxLookAt._12; m_xmf4x4Transform._22 = mtxLookAt._22; m_xmf4x4Transform._23 = mtxLookAt._32;
	m_xmf4x4Transform._31 = mtxLookAt._13; m_xmf4x4Transform._32 = mtxLookAt._23; m_xmf4x4Transform._33 = mtxLookAt._33;

	UpdateTransform(NULL);
}

void CGameObject::SetTexture(std::shared_ptr<CTexture> pTexture)
{
	if (!m_ppMaterials.data())
	{
		m_nMaterials = 1;
		m_ppMaterials.resize(m_nMaterials);
		std::shared_ptr<CMaterial> pMaterial = std::make_shared<CMaterial>();
		pMaterial->SetTexture(pTexture);
		m_ppMaterials[0] = pMaterial;
	}
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();
	
	if (m_pMesh)
	{
		// CGameObject의 정보를 넘길 버퍼가 있고, 해당 버퍼에 대한 CPU 포인터가 있으면 UpdateShaderVariables 함수를 호출한다.
		UpdateShaderVariables(pd3dCommandList);
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader) m_ppMaterials[0]->m_pShader->Render(pd3dCommandList);
				m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
			}
			// 여기서 메쉬의 렌더를 한다.
			m_pMesh->OnPreRender(pd3dCommandList);
			m_pMesh->Render(pd3dCommandList, i);
		}
	}

	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return XMFLOAT3(m_xmf4x4Transform._41, m_xmf4x4Transform._42, m_xmf4x4Transform._43);
}

XMFLOAT3 CGameObject::GetLook()
{
	return Vector3::Normalize(XMFLOAT3(m_xmf4x4Transform._31, m_xmf4x4Transform._32, m_xmf4x4Transform._33));
}

XMFLOAT3 CGameObject::GetUp()
{
	return Vector3::Normalize(XMFLOAT3(m_xmf4x4Transform._21, m_xmf4x4Transform._22, m_xmf4x4Transform._23));
}

XMFLOAT3 CGameObject::GetRight()
{
	return Vector3::Normalize(XMFLOAT3(m_xmf4x4Transform._11, m_xmf4x4Transform._12, m_xmf4x4Transform._13));
}

XMFLOAT4X4 CGameObject::GetTransform()
{
	return m_xmf4x4Transform;
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;
	m_xmf4x4Transform._43 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

CGameObject* CGameObject::FindFrame(const char* pstrFrameName)
{
	CGameObject* pFrameObject = NULL;
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void CGameObject::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pParent, FILE* pInFile, CShader* pShader)
{
	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;
	
	int nFrame = 0, nTextures = 0;

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			nReads = (UINT)::fread(&nFrame, sizeof(int), 1, pInFile);
			nReads = (UINT)::fread(&nTextures, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(m_pstrFrameName, sizeof(char), nStrLength, pInFile);
			m_pstrFrameName[nStrLength] = '\0';
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&m_xmf4x4Transform, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			std::shared_ptr<CTexturedModelingMesh> pMesh = std::make_shared<CTexturedModelingMesh>(pd3dDevice, pd3dCommandList);
			pMesh->LoadTexturedMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			SetMesh(pMesh);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = 0;
			nReads = (UINT)::fread(&nChilds, sizeof(int), 1, pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					std::shared_ptr<CGameObject> pChild = std::make_shared<CGameObject>();
					pChild->LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, pInFile, pShader);
					if (pChild) SetChild(pChild);
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
}

std::shared_ptr<CGameObject> CGameObject::LoadGeometryFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* pstrFileName, CShader* pShader)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	std::shared_ptr<CGameObject> pGameObject = std::make_shared<CGameObject>();
	pGameObject->LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader);
#define _WITH_DEBUG_FRAME_HIERARCHY
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pGameObject.get(), NULL);
#endif

	return(pGameObject);
}

void CGameObject::PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) CGameObject::PrintFrameInfo(pGameObject->m_pSibling.get(), pParent);
	if (pGameObject->m_pChild) CGameObject::PrintFrameInfo(pGameObject->m_pChild.get(), pGameObject);
}

void CGameObject::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;
	BYTE nStrLength = 0;

	UINT nReads = (UINT)::fread(&m_nMaterials, sizeof(int), 1, pInFile);

	m_ppMaterials.resize(m_nMaterials);
	for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;

	std::shared_ptr<CMaterial> pMaterial = NULL;
	std::shared_ptr<CTexture> pTexture = NULL;

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nReads = (UINT)::fread(&nMaterial, sizeof(int), 1, pInFile);

			pMaterial = std::make_shared<CMaterial>();
			pTexture = std::make_shared<CTexture>(7, RESOURCE_TEXTURE2D, 0, 1); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal
			pTexture->SetRootParameterIndex(0, 2);

			pMaterial->SetTexture(pTexture);
			SetMaterial(nMaterial, pMaterial);

			UINT nMeshType = GetMeshType();
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 0)) pMaterial->SetMaterialType(MATERIAL_ALBEDO_MAP);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 1)) pMaterial->SetMaterialType(MATERIAL_SPECULAR_MAP);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 2)) pMaterial->SetMaterialType(MATERIAL_NORMAL_MAP);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 3)) pMaterial->SetMaterialType(MATERIAL_METALLIC_MAP);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 4)) pMaterial->SetMaterialType(MATERIAL_EMISSION_MAP);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 5)) pMaterial->SetMaterialType(MATERIAL_DETAIL_ALBEDO_MAP);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 6)) pMaterial->SetMaterialType(MATERIAL_DETAIL_NORMAL_MAP);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
}

int CGameObject::FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle)
{
	int nParameterIndex = -1;

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i] && m_ppMaterials[i]->m_pTexture)
		{
			int nTextures = m_ppMaterials[i]->m_pTexture->GetTextures();
			for (int j = 0; j < nTextures; j++)
			{
				if (!_tcsncmp(m_ppMaterials[i]->m_pTexture->GetTextureName(j), pstrTextureName, _tcslen(pstrTextureName)))
				{
					*pd3dSrvGpuDescriptorHandle = m_ppMaterials[i]->m_pTexture->GetGpuDescriptorHandle(j);
					nParameterIndex = m_ppMaterials[i]->m_pTexture->GetRootParameter(j);
					return(nParameterIndex);
				}
			}
		}
	}
	if (m_pSibling) if ((nParameterIndex = m_pSibling->FindReplicatedTexture(pstrTextureName, pd3dSrvGpuDescriptorHandle)) > 0) return(nParameterIndex);
	if (m_pChild) if ((nParameterIndex = m_pChild->FindReplicatedTexture(pstrTextureName, pd3dSrvGpuDescriptorHandle)) > 0) return(nParameterIndex);

	return(nParameterIndex);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
CCubeObject::CCubeObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CShader* pShader)
{
	std::shared_ptr<CCubeMeshDiffused> pCubeMesh = std::make_shared<CCubeMeshDiffused>(pd3dDevice, pd3dCommandList, 4.0f, 4.0f, 4.0f);
	SetMesh(pCubeMesh);

	std::shared_ptr<CTexture> pCubeTexture = std::make_shared<CTexture>(7, RESOURCE_TEXTURE2D, 0, 1);
	pCubeTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Stone.dds", RESOURCE_TEXTURE2D, 0);
	pCubeTexture->SetRootParameterIndex(0, 2);

	
	pShader->CreateShaderResourceView(pd3dDevice, pCubeTexture.get(), 0);
	
	std::shared_ptr<CMaterial> pMaterial = std::make_shared<CMaterial>();
	pMaterial->SetMaterialType(MATERIAL_ALBEDO_MAP);
	pMaterial->SetTexture(pCubeTexture);

	m_nMaterials = 1;
	m_ppMaterials.push_back(pMaterial);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CSuperCobraObject::PrepareAnimate()
{
	m_pMainRotorFrame = FindFrame("MainRotor");
	m_pTailRotorFrame = FindFrame("TailRotor");
}

void CSuperCobraObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pMainRotorFrame->SetTransform(Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->GetTransform()));
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->SetTransform(Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->GetTransform()));
	}

	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CMi24Object::PrepareAnimate()
{
	m_pMainRotorFrame = FindFrame("Top_Rotor");
	m_pTailRotorFrame = FindFrame("Tail_Rotor");
}

void CMi24Object::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pMainRotorFrame->SetTransform(Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->GetTransform()));
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->SetTransform(Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->GetTransform()));
	}

	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CGunshipObject::PrepareAnimate()
{
	m_pMainRotorFrame = FindFrame("Rotor");
	m_pTailRotorFrame = FindFrame("Back_Rotor");
}

void CGunshipObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pMainRotorFrame->SetTransform(Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->GetTransform()));
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->SetTransform(Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->GetTransform()));
	}

	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSkyBox::CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nMeshes = 6;
	m_ppMeshes.resize(m_nMeshes);

	std::shared_ptr<CTexture> pSkyBoxTexture = std::make_shared<CTexture>(6, RESOURCE_TEXTURE2D, 0, 1);

	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Front_0.dds", RESOURCE_TEXTURE2D, 0);
	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Back_0.dds", RESOURCE_TEXTURE2D, 1);
	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Left_0.dds", RESOURCE_TEXTURE2D, 2);
	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Right_0.dds", RESOURCE_TEXTURE2D, 3);
	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Top_0.dds", RESOURCE_TEXTURE2D, 4);
	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Bottom_0.dds", RESOURCE_TEXTURE2D, 5);

	// Shader 생성
	std::shared_ptr<CShader> pSkyBoxShader = std::make_shared<CSkyBoxShader>();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pSkyBoxShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 6);
	pSkyBoxShader->CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture.get(), 0, 3);

	std::shared_ptr<CTexturedRectMesh> pSkyBoxFrontMesh = std::make_shared<CTexturedRectMesh>(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	std::shared_ptr<CTexturedRectMesh> pSkyBoxBackMesh = std::make_shared<CTexturedRectMesh>(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	std::shared_ptr<CTexturedRectMesh> pSkyBoxLeftMesh = std::make_shared<CTexturedRectMesh>(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, 0.0f);
	std::shared_ptr<CTexturedRectMesh> pSkyBoxRightMesh = std:: make_shared<CTexturedRectMesh>(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, +10.0f, 0.0f, 0.0f);
	std::shared_ptr<CTexturedRectMesh> pSkyBoxTopMesh = std::make_shared<CTexturedRectMesh>(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, +10.0f, 0.0f);
	std::shared_ptr<CTexturedRectMesh> pSkyBoxBottomMesh = std::make_shared<CTexturedRectMesh>(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, -10.0f, 0.0f);

	SetMesh(0, pSkyBoxFrontMesh);
	SetMesh(1, pSkyBoxBackMesh);
	SetMesh(2, pSkyBoxLeftMesh);
	SetMesh(3, pSkyBoxRightMesh);
	SetMesh(4, pSkyBoxTopMesh);
	SetMesh(5, pSkyBoxBottomMesh);

	SetShader(pSkyBoxShader, pSkyBoxTexture);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::SetMesh(int nIndex, std::shared_ptr<CMesh> pMesh)
{
	if (m_ppMeshes.data())
	{
		m_ppMeshes[nIndex] = pMesh;
	}
}

void CSkyBox::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);
	UpdateTransform(NULL);

	OnPrepareRender();

	if (m_ppMaterials[0])
	{
		if (m_ppMaterials[0]->m_pShader)
		{
			m_ppMaterials[0]->m_pShader->Render(pd3dCommandList);
		}
	}

	UpdateShaderVariables(pd3dCommandList);

	if (m_ppMeshes.data())
	{
		for (int i = 0; i < m_ppMeshes.size(); i++)
		{
			if (m_ppMaterials[0]->m_pTexture)
			{
				m_ppMaterials[0]->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0, i);
			}
			// 여기서 메쉬의 렌더를 한다.
			m_ppMeshes[i]->OnPreRender(pd3dCommandList);
			m_ppMeshes[i]->Render(pd3dCommandList, i);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes.resize(m_nMeshes);
	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	std::shared_ptr<CHeightMapGridMesh> pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = std::make_shared<CHeightMapGridMesh>(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}

	std::shared_ptr<CTexture> pTerrainTexture = std::make_shared<CTexture>(3, RESOURCE_TEXTURE2D, 0, 3);

	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Base_Texture.dds", RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Detail_Texture_7.dds", RESOURCE_TEXTURE2D, 1);
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/HeightMap(Alpha).dds", RESOURCE_TEXTURE2D, 2);

	std::shared_ptr<CTerrainShader> pTerrainShader = std::make_shared<CTerrainShader>();
	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTerrainShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 3);
	pTerrainShader->CreateShaderResourceViews(pd3dDevice, pTerrainTexture.get(), 0, 4);

	SetShader(pTerrainShader, pTerrainTexture);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

void CHeightMapTerrain::SetMesh(int nIndex, std::shared_ptr<CMesh> pMesh)
{
	if (m_ppMeshes.data())
	{
		m_ppMeshes[nIndex] = pMesh;
	}
}

void CHeightMapTerrain::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	UpdateTransform(NULL);
	OnPrepareRender();

	if (m_ppMaterials[0])
	{
		if (m_ppMaterials[0]->m_pShader)
		{
			m_ppMaterials[0]->m_pShader->Render(pd3dCommandList);
		}

		if (m_ppMaterials[0]->m_pTexture)
		{
			m_ppMaterials[0]->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}

	UpdateShaderVariables(pd3dCommandList);

	if (m_ppMeshes.data())
	{
		for (int i = 0; i < m_ppMeshes.size(); i++)
		{
			// 여기서 메쉬의 렌더를 한다.
			m_ppMeshes[i]->OnPreRender(pd3dCommandList);
			m_ppMeshes[i]->Render(pd3dCommandList, i);
		}
	}
}

CRippleWater::CRippleWater(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes.resize(m_nMeshes);

	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	std::shared_ptr<CHeightMapGridMesh> pGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pGridMesh = std::make_shared<CHeightMapGridMesh>(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color);
			SetMesh(x + (z * cxBlocks), pGridMesh);
		}
	}

	std::shared_ptr<CTexture> pWaterTexture = std::make_shared<CTexture>(2, RESOURCE_TEXTURE2D, 0, 2);

	pWaterTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Water_Base_Texture_0.dds", RESOURCE_TEXTURE2D, 0);
	pWaterTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Water_Detail_Texture_0.dds", RESOURCE_TEXTURE2D, 1);

	std::shared_ptr<CRippleWaterShader> pRippleWaterShader = std::make_shared<CRippleWaterShader>();
	pRippleWaterShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pRippleWaterShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pRippleWaterShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 2);
	pRippleWaterShader->CreateShaderResourceViews(pd3dDevice, pWaterTexture.get(), 0, 4);


	SetShader(pRippleWaterShader, pWaterTexture);
}

CRippleWater::~CRippleWater()
{
}

void CRippleWater::SetMesh(int nIndex, std::shared_ptr<CMesh> pMesh)
{
	if (m_ppMeshes.data())
	{
		m_ppMeshes[nIndex] = pMesh;
	}
}

void CRippleWater::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	UpdateTransform(NULL);
	OnPrepareRender();

	if (m_ppMaterials[0])
	{
		if (m_ppMaterials[0]->m_pShader)
		{
			m_ppMaterials[0]->m_pShader->Render(pd3dCommandList);
		}

		if (m_ppMaterials[0]->m_pTexture)
		{
			m_ppMaterials[0]->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}

	UpdateShaderVariables(pd3dCommandList);

	if (m_ppMeshes.data())
	{
		for (int i = 0; i < m_ppMeshes.size(); i++)
		{
			// 여기서 메쉬의 렌더를 한다.
			m_ppMeshes[i]->OnPreRender(pd3dCommandList);
			m_ppMeshes[i]->Render(pd3dCommandList, i);
		}
	}
}

CBullet::CBullet() {
	direction = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	isEnable = false;
	fVelocity = 200.0f;
	m_fmovingDistance = 0.0f;
	m_xmOOBB = BoundingOrientedBox{ XMFLOAT3{0.0f, 0.0f, 0.0f}, XMFLOAT3{3.5f, 3.5f, 3.5f}, XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f} };
}

void CBullet::Update(float fTimeElapsed)
{
	XMFLOAT3 cur_Position = GetPosition();
	XMFLOAT3 reult_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float distance = fVelocity * fTimeElapsed;
	m_fmovingDistance += distance;
	if (m_fmovingDistance > 1000.0f)
		Reset();
	else
	{
		XMFLOAT3 deltaPos = Vector3::ScalarProduct(direction, distance);
		reult_Position = Vector3::Add(cur_Position, deltaPos);
		SetPosition(reult_Position);
		Rotate(0.0f, 360.0f * fTimeElapsed, 0.0f);
		UpdateBoundingBox();
	}
}

void CBullet::ShootBullet(CGameObject* pPlayer)
{
	isEnable = true;
	SetPosition(pPlayer->GetPosition());
}

void CBullet::Reset()
{
	direction = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	isEnable = false;
	m_fmovingDistance = 0.0f;
}

void CBullet::UpdateBoundingBox()
{
	XMFLOAT4X4 xmf4x5Transform = GetTransform();
	m_xmOOBB.Transform(m_xmOOBB_result, XMLoadFloat4x4(&xmf4x5Transform));
	XMStoreFloat4(&m_xmOOBB_result.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
}

bool CBullet::CheckCollision(BoundingOrientedBox p_xmOOBB)
{
	if (isEnable)
	{
		if (m_xmOOBB_result.Intersects(p_xmOOBB))
			return true;
	}
	return false;
}
