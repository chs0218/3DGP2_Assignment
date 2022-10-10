#pragma once

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

class CTexture
{
public:
	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nRootParameters);
	virtual ~CTexture();
private:
	UINT m_nTextureType;		// Texture 타입
	int	m_nTextures = 0;		// Texture 개수
	ID3D12Resource** m_ppd3dTextures;	// Texture 리소스
	ID3D12Resource** m_ppd3dTextureUploadBuffers;	//Texture 업로드 버퍼

	UINT* m_pnResourceTypes;		// 각각의 Texture 들의 타입
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;	// Texture의 이름

	DXGI_FORMAT* m_pdxgiBufferFormats = NULL;	// Texture Dxgi 포맷
	int* m_pnBufferElements;

	int	m_nRootParameters = 0;		// 루트파라미터 번호
	int* m_pnRootParameterIndices;		// 루트파라미터 인덱스
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSrvGpuDescriptorHandles;	// Srv 디스크립터 핸들

	ID3D12DescriptorHeap* m_pd3dSrvDescriptorHeap = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorNextHandle;
public:
	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	void SetRootParameterIndex(int nIndex, UINT nRootParameterIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);

	void ReleaseUploadBuffers();

	void CreateSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nShaderResourceViews);
	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex);
};

