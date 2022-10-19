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
class CShader;

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
	std::vector<std::shared_ptr<CMaterial>> m_ppMaterials;

	std::shared_ptr<CMesh> m_pMesh;
	BoundingOrientedBox m_xmOOBB;

	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;

	ComPtr<ID3D12Resource> m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObject = NULL;
public:
	CGameObject* m_pParent = nullptr;
	std::shared_ptr<CGameObject> m_pChild = nullptr;
	std::shared_ptr<CGameObject> m_pSibling = nullptr;

	CGameObject();
	virtual ~CGameObject();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseUploadBuffers();
	void PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent);

	char* GetFrameName() { return m_pstrFrameName; }
	void SetChild(std::shared_ptr<CGameObject> pChild, bool bReferenceUpdate = false);
	void SetShader(std::shared_ptr<CShader> pShader, std::shared_ptr<CTexture> pTexture = NULL);
	void SetMaterial(int nMaterial, std::shared_ptr<CMaterial> pMaterial);
	void SetTransform(XMFLOAT4X4 xmf4x4Transform) { m_xmf4x4Transform = xmf4x4Transform; }
	void SetTexture(std::shared_ptr<CTexture> pTexture);
	void SetLookAt(const XMFLOAT3& xmf3Target, const XMFLOAT3& xmf3Up);
	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }

	virtual void OnInitialize() { }
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual void OnPrepareRender() {}
	virtual void PrepareAnimate() {}
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

	virtual void SetMesh(std::shared_ptr<CMesh> pMesh) { m_pMesh = pMesh; }

	CGameObject* GetParent() { return (m_pParent); }

	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	CGameObject* FindFrame(const char* pstrFrameName);

	void LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pParent, FILE* pInFile, CShader* pShader);
	std::shared_ptr<CGameObject> LoadGeometryFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* pstrFileName, CShader* pShader);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader);
	int FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle);
};

class CBullet : public CGameObject
{
public:
	bool isEnable = false;
public:
	CBullet() {}
	~CBullet() {}

	bool CheckEnable() { return isEnable; }
	void DisableBullet() { isEnable = false; }
	void ShootBullet(CGameObject* pPlayer);
	void Update(float fTimeElapsed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CCubeObject : public CGameObject
{
public:
	CCubeObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CShader* pShader = NULL);
	~CCubeObject() {}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CSuperCobraObject : public CGameObject
{
public:
	CSuperCobraObject() {}
	~CSuperCobraObject() {}

private:
	CGameObject* m_pMainRotorFrame = NULL;
	CGameObject* m_pTailRotorFrame = NULL;

public:
	virtual void PrepareAnimate();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CMi24Object : public CGameObject
{
public:
	CMi24Object() {}
	~CMi24Object() {}

private:
	CGameObject* m_pMainRotorFrame = NULL;
	CGameObject* m_pTailRotorFrame = NULL;

public:
	virtual void PrepareAnimate();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CGunshipObject : public CGameObject
{
public:
	CGunshipObject() {}
	~CGunshipObject() {}

private:
	CGameObject* m_pMainRotorFrame = NULL;
	CGameObject* m_pTailRotorFrame = NULL;

public:
	virtual void PrepareAnimate();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CSkyBox : public CGameObject
{
private:
	int	m_nMeshes;
	std::vector<std::shared_ptr<CMesh>> m_ppMeshes;
public:
	CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CSkyBox();
	virtual void SetMesh(int nIndex, std::shared_ptr<CMesh> pMesh);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapImage* m_pHeightMapImage;

	int							m_nWidth;
	int							m_nLength;

	XMFLOAT3					m_xmf3Scale;

	int	m_nMeshes;
	std::vector<std::shared_ptr<CMesh>> m_ppMeshes;
public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetRawImageWidth() { return(m_pHeightMapImage->GetRawImageWidth()); }
	int GetRawImageLength() { return(m_pHeightMapImage->GetRawImageLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }

	virtual void SetMesh(int nIndex, std::shared_ptr<CMesh> pMesh);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CRippleWater : public CGameObject
{
public:
	CRippleWater(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CRippleWater();

private:
	int								m_nWidth;
	int								m_nLength;

	XMFLOAT3						m_xmf3Scale;

	int	m_nMeshes;
	std::vector<std::shared_ptr<CMesh>> m_ppMeshes;
public:
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }

	virtual void SetMesh(int nIndex, std::shared_ptr<CMesh> pMesh);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};
