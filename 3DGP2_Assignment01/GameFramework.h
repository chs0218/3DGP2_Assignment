#pragma once
#include "Timer.h"
#include "Object.h"
#include "Shader.h"
#include "Mesh.h"
#include "Scene.h"
#include "Camera.h"
#include "Texture.h"

#define DRAW_SCENE_COLOR 'S'
#define DRAW_SCENE_EDGE 'F'

struct CB_FRAME_INFO
{
	float 		fCurrentTime;
	float		fElapsedTime;
	XMFLOAT2	f2CursorPos;
	XMINT4		m_xmn4DrawOptions;
	XMFLOAT3	m_xmf3Gravity = XMFLOAT3(0.0f, -9.8f, 0.0f);
	int			m_nMaxFlareType2Particles = 15;
	float		m_fSecondsPerFirework = 1.0f;
	int			m_nFlareParticlesToEmit = 30;
};


class CTerrianFlyingPlayer;
class CGameFramework
{
private:
	LiveObjectReporter reporter;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nWndClientWidth;
	int m_nWndClientHeight;

	//DXGI 팩토리 인터페이스에 대한 포인터이다.
	ComPtr<IDXGIFactory4> m_pdxgiFactory;

	//스왑 체인 인터페이스에 대한 포인터이다.
	ComPtr<IDXGISwapChain3> m_pdxgiSwapChain;

	//Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	ComPtr<ID3D12Device> m_pd3dDevice;

	//MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.
	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;

	//스왑 체인의 후면 버퍼의 개수와 현재 스왑 체인의 푸면 버퍼 인덱스이다.
	static const UINT m_nSwapChainBuffers = 2;
	UINT m_nSwapChainBufferIndex;

	//렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기이다.
	ComPtr<ID3D12Resource> m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ComPtr<ID3D12DescriptorHeap> m_pd3dRtvDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_pd3dSwapRTVCPUHandles[m_nSwapChainBuffers];

	//깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기이다.
	ComPtr<ID3D12Resource> m_pd3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dDsvDescriptorCPUHandle;

	//명령 큐, 명령 할당자, 명령 리스트 인터페이스 포인터이다.
	ComPtr<ID3D12CommandQueue> m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList;

	//그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터이다
	ComPtr<ID3D12PipelineState> m_pd3dPipelineState;

	//펜스 인터페이스 포인터, 펜스의 값, 이벤트 핸들이다.
	ComPtr<ID3D12Fence> m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;

	std::unique_ptr<CScene> m_pScene = NULL;
	std::unique_ptr<CTerrianFlyingPlayer> m_pPlayer = NULL;
	std::unique_ptr<CGameObject> m_pObject = NULL;
	CCamera* m_pCamera = NULL;

	ComPtr<ID3D12Resource> m_pd3dcbFrame = NULL;
	CB_FRAME_INFO* m_pcbMappedFrame = NULL;

	CGameTimer					m_GameTimer;
	POINT						m_ptOldCursorPos;
	_TCHAR						m_pszFrameRate[50];

	int	m_nDrawOptions = DRAW_SCENE_COLOR;
public:
	CGameFramework();
	~CGameFramework();

	//프레임워크를 초기화하는 함수이다(주 윈도우가 생성되면 호출된다). 
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	//스왑 체인, 디바이스, 서술자 힙, 명령 큐/할당자/리스트를 생성하는 함수이다.
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	//렌더 타겟 뷰와 깊이-스텐실 뷰를 생성하는 함수이다. 
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	//렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다. 
	void BuildObjects();
	void ReleaseObjects();

	//프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다. 
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	void MoveToNextFrame();

	//프레임워크의 현재 시간, 경과 시간, 마우스 x 좌표, 마우스 y 좌표를 업데이트해주는 함수이다.
	void CreateShaderVariables();
	void UpdateShaderVariables();

	//CPU와 GPU를 동기화하는 함수이다.
	//void WaitForGpuComplete();

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//전체화면 모드 <-> 윈도우 모드의 전환을 구현하는 함수이다.
	void ChangeSwapChainState();
	void ChangeShowEdge() { 
		if (m_nDrawOptions == DRAW_SCENE_COLOR)
			m_nDrawOptions = DRAW_SCENE_EDGE;
		else
			m_nDrawOptions = DRAW_SCENE_COLOR;
	}
};
