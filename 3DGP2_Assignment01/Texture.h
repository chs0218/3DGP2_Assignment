#pragma once

class CShader;
class CGameObject;
class CTexture
{
public:
	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nRootParameters);
	virtual ~CTexture();
private:
	UINT m_nTextureType;		// Texture Ÿ��
	int	m_nTextures = 0;		// Texture ����
	std::vector<ComPtr<ID3D12Resource>> m_ppd3dTextures;	// Texture ���ҽ�
	std::vector<ComPtr<ID3D12Resource>> m_ppd3dTextureUploadBuffers;	//Texture ���ε� ����

	UINT* m_pnResourceTypes;		// ������ Texture ���� Ÿ��
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;	// Texture�� �̸�

	DXGI_FORMAT* m_pdxgiBufferFormats = NULL;	// Texture Dxgi ����
	int* m_pnBufferElements;
	int* m_pnBufferStrides;

	int	m_nRootParameters = 0;		// ��Ʈ�Ķ���� ��ȣ
	int* m_pnRootParameterIndices;		// ��Ʈ�Ķ���� �ε���
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSrvGpuDescriptorHandles;	// Srv ��ũ���� �ڵ�
public:
	int GetTextures() { return(m_nTextures); }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppd3dTextures[nIndex].Get()); }
	_TCHAR* GetTextureName(int nIndex) { return(m_ppstrTextureNames[nIndex]); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(int nIndex) { return(m_pd3dSrvGpuDescriptorHandles[nIndex]); }
	int GetRootParameter(int nIndex) { return(m_pnRootParameterIndices[nIndex]); }
	int GetRootParameters() { return(m_nRootParameters); }
	UINT GetTextureType() { return(m_nTextureType); }

	void SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex);
	void ReleaseShaderVariables();

	void CreateBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT dxgiFormat, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, UINT nIndex);
	ID3D12Resource* CreateTexture(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex);
	void LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	int LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader, UINT nIndex);
	void SetRootParameterIndex(int nIndex, UINT nRootParameterIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);

	void ReleaseUploadBuffers();
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();
public:
	std::shared_ptr<CTexture> m_pTexture = NULL;

	XMFLOAT4 m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	void SetShader(std::shared_ptr<CShader> pShader);
	void SetMaterialType(UINT nType) { m_nType |= nType; }
	void SetTexture(std::shared_ptr<CTexture> pTexture);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

public:
	UINT							m_nType = 0x00;

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;

	std::shared_ptr<CShader> m_pShader;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////