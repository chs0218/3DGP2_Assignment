#pragma once
#include "Camera.h"

class CCamera;

//-----------------------------------------------------------------------------
// 22/09/20 CScene 클래스 추가 - Leejh
//-----------------------------------------------------------------------------
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
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

private:
	ComPtr<ID3D12RootSignature> m_pd3dGraphicsRootSignature = nullptr;

	float m_fElapsedTime = 0.0f;

	XMFLOAT4 m_fClearColor = {0.0f, 0.0f, 0.0f, 0.0f};
};

