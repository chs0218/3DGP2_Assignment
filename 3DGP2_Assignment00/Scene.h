#pragma once
#include "Camera.h"
#include "Object.h"
#include "Player.h"

class CCamera;
class CPlayer;
class CShader;
//-----------------------------------------------------------------------------
// 22/09/20 CScene 클래스 추가 - Leejh
//-----------------------------------------------------------------------------
class CSkyBox;
class CHeightMapTerrain;
class CRippleWater;
class CMultiSpriteObject;
class CScene
{
public:
	CScene();
	virtual ~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature.Get()); }

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	virtual bool ProcessInput(UCHAR* pKeysBuffer);
	virtual void AnimateObjects(CGameObject* pPlayer, float fTimeElapsed);
	void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain.get()); }
	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	void CheckCollision();
private:
	ComPtr<ID3D12RootSignature> m_pd3dGraphicsRootSignature = nullptr;

	float m_fElapsedTime = 0.0f;

	std::unique_ptr<CSkyBox> m_pSkyBox = NULL;
	std::unique_ptr<CHeightMapTerrain> m_pTerrain = NULL;
	std::unique_ptr<CRippleWater> m_pWater = NULL;
	std::unique_ptr<CShader> m_pShader = NULL;
	std::unique_ptr<CShader> m_pShader2 = NULL;
	std::unique_ptr<CShader> m_pBoomShader = NULL;
	XMFLOAT4 m_fClearColor = {0.0f, 0.0f, 0.0f, 0.0f};
	CPlayer* m_pPlayer = NULL;
};

