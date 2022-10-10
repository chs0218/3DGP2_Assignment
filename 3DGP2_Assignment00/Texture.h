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
	UINT m_nTextureType;		// Texture Ÿ��
	int	m_nTextures = 0;		// Texture ����
	ID3D12Resource** m_ppd3dTextures;	// Texture ���ҽ�
	ID3D12Resource** m_ppd3dTextureUploadBuffers;	//Texture ���ε� ����

	UINT* m_pnResourceTypes;		// ������ Texture ���� Ÿ��
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;	// Texture�� �̸�

	DXGI_FORMAT* m_pdxgiBufferFormats = NULL;	// Texture Dxgi ����
	int* m_pnBufferElements;

	int	m_nRootParameters = 0;		// ��Ʈ�Ķ���� ��ȣ
	int* m_pnRootParameterIndices;		// ��Ʈ�Ķ���� �ε���
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSrvGpuDescriptorHandles;	// Srv ��ũ���� �ڵ�
public:
	int GetTextures() { return(m_nTextures); }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	_TCHAR* GetTextureName(int nIndex) { return(m_ppstrTextureNames[nIndex]); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(int nIndex) { return(m_pd3dSrvGpuDescriptorHandles[nIndex]); }
	int GetRootParameter(int nIndex) { return(m_pnRootParameterIndices[nIndex]); }
	int GetRootParameters() { return(m_nRootParameters); }

	void SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);
	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	void SetRootParameterIndex(int nIndex, UINT nRootParameterIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);

	void ReleaseUploadBuffers();
};

