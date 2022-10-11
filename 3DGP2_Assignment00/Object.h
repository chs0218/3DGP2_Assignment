#pragma once
#include "stdafx.h"
#include "Mesh.h"
#include "Texture.h"
//----------------------------------------------------------------------------
// 22/09/20 CGameObject 클래스 추가 - Leejh
//----------------------------------------------------------------------------
#define MAX_FRAMENAME 64

class CMesh;
class CCamera;
class CTexture;

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	int n_Material;
};

class CGameObject
{
protected:
	char m_pstrFrameName[MAX_FRAMENAME];

	XMFLOAT4X4 m_xmf4x4Transform;
	XMFLOAT4X4 m_xmf4x4World;

	int	m_nMaterials = 0;
	CMaterial** m_ppMaterials = NULL;

	CGameObject* m_pParent = nullptr;
	std::shared_ptr<CGameObject> m_pChild = nullptr;
	std::shared_ptr<CGameObject> m_pSibling = nullptr;

	std::shared_ptr<CMesh> m_pMesh;
	BoundingOrientedBox m_xmOOBB;

	ComPtr<ID3D12Resource> m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObject = NULL;
	int m_Type = 0;
public:
	CGameObject();
	virtual ~CGameObject();

	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseUploadBuffers();

	char* GetFrameName() { return m_pstrFrameName; }
	void SetChild(std::shared_ptr<CGameObject> pChild, bool bReferenceUpdate = false) { m_pChild = pChild; }
	void SetTypes(int n_type) { m_Type = n_type; }
	void SetTexture(CTexture* pTexture);

	virtual void OnInitialize() { }
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual void OnPrepareRender() { }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	void SetMesh(std::shared_ptr<CMesh> target) { m_pMesh = target; }

	CGameObject* GetParent() { return (m_pParent); }

	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	CGameObject* FindFrame(char* pstrFrameName);

	void LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pParent, FILE* pInFile);
	std::shared_ptr<CGameObject> LoadGeometryFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile);
};