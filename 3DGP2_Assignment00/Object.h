#pragma once
#include "stdafx.h"
#include "Mesh.h"
#include "Texture.h"
//----------------------------------------------------------------------------
// 22/09/20 CGameObject 클래스 추가 - Leejh
//----------------------------------------------------------------------------
#define MAX_FRAMENAME 64

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

class CMesh;
class CCamera;
class CTexture;

struct MATERIAL
{
	XMFLOAT4 m_cAmbient;
	XMFLOAT4 m_cDiffuse;
	XMFLOAT4 m_cSpecular; //a = power
	XMFLOAT4 m_cEmissive;
};

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	MATERIAL m_xmf4x4Material;
	UINT m_TexMask;
};

class CGameObject
{
protected:
	char m_pstrFrameName[MAX_FRAMENAME];

	XMFLOAT4X4 m_xmf4x4Transform;
	XMFLOAT4X4 m_xmf4x4World;

	int	m_nMaterials = 0;
	CMaterial** m_ppMaterials = NULL;

	std::shared_ptr<CMesh> m_pMesh;
	BoundingOrientedBox m_xmOOBB;

	ComPtr<ID3D12Resource> m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObject = NULL;
	int m_Type = 0;
public:
	CGameObject* m_pParent = nullptr;
	std::shared_ptr<CGameObject> m_pChild = nullptr;
	std::shared_ptr<CGameObject> m_pSibling = nullptr;

	CGameObject();
	virtual ~CGameObject();

	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseUploadBuffers();

	char* GetFrameName() { return m_pstrFrameName; }
	void SetChild(std::shared_ptr<CGameObject> pChild, bool bReferenceUpdate = false);
	void SetTypes(int n_type) { m_Type = n_type; }
	void SetTexture(CTexture* pTexture);
	void SetMaterial(int nMaterial, CMaterial* pMaterial);
	void SetTransform(XMFLOAT4X4 xmf4x4Transform) { m_xmf4x4Transform = xmf4x4Transform; }

	virtual void OnInitialize() { }
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual void OnPrepareRender() { }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	XMFLOAT4X4 GetTransform();
	UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0x00); }

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	void SetMesh(std::shared_ptr<CMesh> target) { m_pMesh = target; }

	CGameObject* GetParent() { return (m_pParent); }

	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	CGameObject* FindFrame(const char* pstrFrameName);

	void LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pParent, FILE* pInFile, CShader* pShader);
	std::shared_ptr<CGameObject> LoadGeometryFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* pstrFileName, CShader* pShader);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader);
	int FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CSuperCobraObject : public CGameObject
{
public:
	CSuperCobraObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CSuperCobraObject();

private:
	CGameObject* m_pMainRotorFrame = NULL;
	CGameObject* m_pTailRotorFrame = NULL;

public:
	virtual void PrepareAnimate();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

class CGunshipObject : public CGameObject
{
public:
	CGunshipObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CGunshipObject();

private:
	CGameObject* m_pMainRotorFrame = NULL;
	CGameObject* m_pTailRotorFrame = NULL;

public:
	virtual void PrepareAnimate();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

class CMi24Object : public CGameObject
{
public:
	CMi24Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CMi24Object();

private:
	CGameObject* m_pMainRotorFrame = NULL;
	CGameObject* m_pTailRotorFrame = NULL;

public:
	virtual void PrepareAnimate();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////