#include "stdafx.h"
#include "Shader.h"
#include "Texture.h"
#include "Object.h"
#include "Camera.h"
#include "Enemy.h"

CShader::CShader()
{
}

CShader::~CShader()
{
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
	ID3DBlob* pd3dErrorBlob = NULL;

	::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, NULL);

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

// 컴파일된 셰이더 파일을 읽어오는 함수이다.
D3D12_SHADER_BYTECODE CShader::ReadCompiledShaderFile(const WCHAR* pszFile, ID3DBlob** ppd3dBlob)
{
	std::ifstream ifsFile{ pszFile, std::ios::in | std::ios::binary };
	std::vector<BYTE> pByteCode{ std::istreambuf_iterator<char> {ifsFile}, {} };
	UINT nReadBytes = (UINT)ifsFile.tellg();

	D3D12_SHADER_BYTECODE d3dByteCode;
	if (ppd3dBlob) {
		HRESULT hResult = D3DCreateBlob(nReadBytes, ppd3dBlob);
		memcpy((*ppd3dBlob)->GetBufferPointer(), pByteCode.data(), nReadBytes);
		d3dByteCode.BytecodeLength = (*ppd3dBlob)->GetBufferSize();
		d3dByteCode.pShaderBytecode = (*ppd3dBlob)->GetBufferPointer();
	}
	else {
		d3dByteCode.BytecodeLength = nReadBytes;
		d3dByteCode.pShaderBytecode = pByteCode.data();
	}

	return d3dByteCode;

}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;		// 다중 샘플링을 위해 렌더 타겟 0의 알파 값을 커버리지 마스크로 변환
	d3dBlendDesc.IndependentBlendEnable = FALSE;		// 각 렌더 타겟에서 독립적인 블렌딩을 수행 여부
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;		// 블렌딩을 활성화
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;			// 논리 연산을 활성화
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;		// 픽셀 색상에 곱하는 값(요소별 연산)
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;			// 렌더 타겟 색상에 곱하는 값
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;			// RGB색상 블렌드 연산자
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;		// 알파값에 대한 블렌드
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;		// 알파값에 대한 블렌드
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;		// 알파값에 대한 블렌드
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;		// 논리 연산자
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// 블렌드 타겟에 적용할 마스크

	return(d3dBlendDesc);
}

void CShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	ID3DBlob* pd3dVertexShaderBlob = NULL, * pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
}

void CShader::AnimateObjects(float fTimeElapsed)
{
}

void CShader::ReleaseObjects()
{
}

void CShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
}

void CShader::ReleaseShaderVariables()
{
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
}

void CShader::ReleaseUploadBuffers()
{
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dPipelineState) pd3dCommandList->SetPipelineState(m_pd3dPipelineState.Get());
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	UpdateShaderVariables(pd3dCommandList);
}

void CShader::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPrepareRender(pd3dCommandList);
}

void CShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender(pd3dCommandList);
}

void CShader::CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);

	m_d3dSrvCPUDescriptorNextHandle = m_d3dSrvCPUDescriptorStartHandle;
	m_d3dSrvGPUDescriptorNextHandle = m_d3dSrvGPUDescriptorStartHandle;
}

void CShader::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex)
{
	m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	int nTextures = pTexture->GetTextures();
	UINT nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource* pShaderResource = pTexture->GetResource(i);
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
		m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		pTexture->SetGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
		m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
	int nRootParameters = pTexture->GetRootParameters();
	for (int i = 0; i < nRootParameters; i++) pTexture->SetRootParameterIndex(i, nRootParameterStartIndex + i);
}

void CShader::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

void CShader::CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex)
{
	ID3D12Resource* pShaderResource = pTexture->GetResource(nIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dGpuDescriptorHandle = pTexture->GetGpuDescriptorHandle(nIndex);
	if (pShaderResource && !d3dGpuDescriptorHandle.ptr)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(nIndex);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
		m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetGpuDescriptorHandle(nIndex, m_d3dSrvGPUDescriptorNextHandle);
		m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}
//-------------------------------------------------------------------------
// CModeledTexturedShader 추가

D3D12_SHADER_BYTECODE CModeledTexturedShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"TextureVertexShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CModeledTexturedShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"TexturePixelShader.cso", ppd3dShaderBlob));
}

D3D12_INPUT_LAYOUT_DESC CModeledTexturedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSkyBoxShader::CSkyBoxShader()
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}

D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"SkyBoxVertexShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"SkyBoxPixelShader.cso", ppd3dShaderBlob));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"TerrainVertexShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"TerrainPixelShader.cso", ppd3dShaderBlob));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CBillboardObjectsShader* CBillboardObjectsShader::Instance()
{
	static CBillboardObjectsShader instance;
	return &instance;
}

CBillboardObjectsShader::CBillboardObjectsShader()
{
}

CBillboardObjectsShader::~CBillboardObjectsShader()
{
}

void CBillboardObjectsShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	ID3DBlob* pd3dVertexShaderBlob = NULL, * pd3dGeometryShaderBlob = NULL , * pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.GS = CreateGeometryShader(&pd3dGeometryShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dGeometryShaderBlob) pd3dGeometryShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_BLEND_DESC CBillboardObjectsShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

void CBillboardObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	pTexture = std::make_shared<CTexture>(7, RESOURCE_TEXTURE2D, 0, 1);

	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Grass01.dds", RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Grass02.dds", RESOURCE_TEXTURE2D, 1);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Tree01.dds", RESOURCE_TEXTURE2D, 2);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Tree02.dds", RESOURCE_TEXTURE2D, 3);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Tree03.dds", RESOURCE_TEXTURE2D, 4);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Flower01.dds", RESOURCE_TEXTURE2D, 5);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Flower02.dds", RESOURCE_TEXTURE2D, 6);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 7);

	CreateShaderResourceViews(pd3dDevice, pTexture.get(), 0, 8);

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	int nTerrainWidth = int(pTerrain->GetWidth());
	int nTerrainLength = int(pTerrain->GetLength());

	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3Position{ 0.0f, 0.0f, 0.0f };

	CRawFormatImage* pRawFormatImage = new CRawFormatImage(L"Image/ObjectsMap.raw", 257, 257, true);
	std::vector<XMFLOAT3> pGrassVertices[2], pTreeVertices[3], pFlowerVertices[2];

	for (int z = 2; z <= 254; z++)
	{
		for (int x = 2; x <= 254; x++)
		{
			BYTE nPixel = pRawFormatImage->GetRawImagePixel(x, z);
			switch (nPixel)
			{
			case 102:
				xmf3Position.x = x * xmf3Scale.x;
				xmf3Position.z = z * xmf3Scale.z;
				xmf3Position.y = pTerrain->GetHeight(xmf3Position.x, xmf3Position.z) + 8.0f * 0.5f;
				pGrassVertices[0].push_back(xmf3Position);
				break;
			case 128:
				xmf3Position.x = x * xmf3Scale.x;
				xmf3Position.z = z * xmf3Scale.z;
				xmf3Position.y = pTerrain->GetHeight(xmf3Position.x, xmf3Position.z) + 6.0f * 0.5f;
				pGrassVertices[1].push_back(xmf3Position);
				break;
			case 153:
				xmf3Position.x = x * xmf3Scale.x;
				xmf3Position.z = z * xmf3Scale.z;
				xmf3Position.y = pTerrain->GetHeight(xmf3Position.x, xmf3Position.z) + 16.0f * 0.5f;
				pFlowerVertices[0].push_back(xmf3Position);
				break;
			case 179:
				xmf3Position.x = x * xmf3Scale.x;
				xmf3Position.z = z * xmf3Scale.z;
				xmf3Position.y = pTerrain->GetHeight(xmf3Position.x, xmf3Position.z) + 16.0f * 0.5f;
				pFlowerVertices[1].push_back(xmf3Position);
				break;
			case 204:
				xmf3Position.x = x * xmf3Scale.x;
				xmf3Position.z = z * xmf3Scale.z;
				xmf3Position.y = pTerrain->GetHeight(xmf3Position.x, xmf3Position.z) + 33.0f * 0.5f;
				pTreeVertices[0].push_back(xmf3Position);
				break;
			case 225:
				xmf3Position.x = x * xmf3Scale.x;
				xmf3Position.z = z * xmf3Scale.z;
				xmf3Position.y = pTerrain->GetHeight(xmf3Position.x, xmf3Position.z) + 33.0f * 0.5f;
				pTreeVertices[1].push_back(xmf3Position);
				break;
			case 255:
				xmf3Position.x = x * xmf3Scale.x;
				xmf3Position.z = z * xmf3Scale.z;
				xmf3Position.y = pTerrain->GetHeight(xmf3Position.x, xmf3Position.z) + 40.0f * 0.5f;
				pTreeVertices[2].push_back(xmf3Position);
				break;
			default: break;
			}
		}
	}

	v_pMesh.push_back(std::make_shared<CBillBoardPointMesh>(pd3dDevice, pd3dCommandList, &pGrassVertices[0], 0));
	v_pMesh.push_back(std::make_shared<CBillBoardPointMesh>(pd3dDevice, pd3dCommandList, &pGrassVertices[1], 1));
	v_pMesh.push_back(std::make_shared<CBillBoardPointMesh>(pd3dDevice, pd3dCommandList, &pTreeVertices[0], 2));
	v_pMesh.push_back(std::make_shared<CBillBoardPointMesh>(pd3dDevice, pd3dCommandList, &pTreeVertices[1], 3));
	v_pMesh.push_back(std::make_shared<CBillBoardPointMesh>(pd3dDevice, pd3dCommandList, &pTreeVertices[2], 4));
	v_pMesh.push_back(std::make_shared<CBillBoardPointMesh>(pd3dDevice, pd3dCommandList, &pFlowerVertices[0], 5));
	v_pMesh.push_back(std::make_shared<CBillBoardPointMesh>(pd3dDevice, pd3dCommandList, &pFlowerVertices[1], 6));
}

void CBillboardObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	pTexture->UpdateShaderVariables(pd3dCommandList);

	for (int i = 0; i < v_pMesh.size(); ++i)
	{
		UINT nType = i;
		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &nType, 32);
		v_pMesh[i]->OnPreRender(pd3dCommandList);
		v_pMesh[i]->Render(pd3dCommandList, 0);
	}
}

D3D12_INPUT_LAYOUT_DESC CBillboardObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

void CBillboardObjectsShader::ReleaseUploadBuffers()
{
}

D3D12_SHADER_BYTECODE CBillboardObjectsShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"BillBoardVertexShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CBillboardObjectsShader::CreateGeometryShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"BillBoardGeometryShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CBillboardObjectsShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"BillBoardPixelShader.cso", ppd3dShaderBlob));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRippleWaterShader* CRippleWaterShader::Instance()
{
	static CRippleWaterShader instance;
	return &instance;
}

CRippleWaterShader::CRippleWaterShader()
{
}

CRippleWaterShader::~CRippleWaterShader()
{
}

D3D12_BLEND_DESC CRippleWaterShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_INPUT_LAYOUT_DESC CRippleWaterShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CRippleWaterShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"WaterVertexShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CRippleWaterShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"WaterPixelShader.cso", ppd3dShaderBlob));
}

CObjectShader* CObjectShader::Instance()
{
	static CObjectShader instance;
	return &instance;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
D3D12_SHADER_BYTECODE CObjectShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"TextureVertexShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CObjectShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"TexturePixelShader.cso", ppd3dShaderBlob));
}

D3D12_INPUT_LAYOUT_DESC CObjectShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

void CObjectShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_nObjects = 120;
	m_ppObjects.resize(m_nObjects);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 19); //SuperCobra(17), Gunship(2)

	std::shared_ptr<CGameObject> pSuperCobraObject = std::make_shared<CGameObject>();
	pSuperCobraObject = pSuperCobraObject->LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SuperCobra.bin", this);

	std::shared_ptr<CGameObject> pGunshipObject = std::make_shared<CGameObject>();
	pGunshipObject = pGunshipObject->LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Gunship.bin", this);
	
	int nColumnSpace = 5, nColumnSize = 30;
	int nFirstPassColumnSize = (m_nObjects % nColumnSize) > 0 ? (nColumnSize - 1) : nColumnSize;

	int nObjects = 0;

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	float f_Width = pTerrain->GetWidth();
	float f_Length = pTerrain->GetLength();

	std::shared_ptr<CGameObject> pGameObject;

	for (int h = 0; h < nFirstPassColumnSize; h++)
	{
		for (int i = 0; i < floor(float(m_nObjects) / float(nColumnSize)); i++)
		{
			if (nObjects % 2)
			{
				pGameObject = std::make_shared<CSuperCobraObject>();
				pGameObject->SetChild(pSuperCobraObject);
				m_ppObjects[nObjects] = new CEnemy();
				m_ppObjects[nObjects]->SetObject(pGameObject, f_Width, f_Length, nColumnSize, nColumnSpace, h);
				m_ppObjects[nObjects]->SetContext(pTerrain);
			}
			else
			{
				pGameObject = std::make_shared<CGunshipObject>();
				pGameObject->SetChild(pGunshipObject);
				m_ppObjects[nObjects] = new CEnemy();
				m_ppObjects[nObjects]->SetObject(pGameObject, f_Width, f_Length, nColumnSize, nColumnSpace, h);
				m_ppObjects[nObjects]->SetContext(pTerrain);
			}
			++nObjects;
		}
	}

	if (nFirstPassColumnSize != nColumnSize)
	{
		for (int i = 0; i < m_nObjects - int(floor(float(m_nObjects) / float(nColumnSize)) * nFirstPassColumnSize); i++)
		{
			if (nObjects % 2)
			{
				pGameObject = std::make_shared<CSuperCobraObject>();
				pGameObject->SetChild(pSuperCobraObject);
				m_ppObjects[nObjects] = new CEnemy();
				m_ppObjects[nObjects]->SetObject(pGameObject, f_Width, f_Length, nColumnSize, nColumnSpace);
				m_ppObjects[nObjects]->SetContext(pTerrain);
			}
			else
			{
				pGameObject = std::make_shared<CGunshipObject>();
				pGameObject->SetChild(pGunshipObject);
				m_ppObjects[nObjects] = new CEnemy();
				m_ppObjects[nObjects]->SetObject(pGameObject, f_Width, f_Length, nColumnSize, nColumnSpace);
				m_ppObjects[nObjects]->SetContext(pTerrain);
			}
			++nObjects;
		}
	}
}

void CObjectShader::AnimateObjects(CGameObject* pPlayer, float fTimeElapsed)
{
	if (m_ppObjects.data())
	{
		for (int i = 0; i < m_nObjects; i++) if (m_ppObjects[i]) {
			m_ppObjects[i]->Update(pPlayer, fTimeElapsed);
		}
	}
}

void CObjectShader::ReleaseObjects()
{
	if (m_ppObjects.data())
	{
		for (int i = 0; i < m_nObjects; i++) if (m_ppObjects[i]) delete m_ppObjects[i];
	}
}

void CObjectShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Animate(0.16f);
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMultiSpriteObjectsShader::CMultiSpriteObjectsShader()
{
}

CMultiSpriteObjectsShader::~CMultiSpriteObjectsShader()
{
}

D3D12_RASTERIZER_DESC CMultiSpriteObjectsShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_BLEND_DESC CMultiSpriteObjectsShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_INPUT_LAYOUT_DESC CMultiSpriteObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CMultiSpriteObjectsShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"MultiSpriteVertexShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CMultiSpriteObjectsShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFile(L"BillBoardPixelShader.cso", ppd3dShaderBlob));
}

void CMultiSpriteObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	pTexturedRectMesh = std::make_shared<CTexturedRectMesh>(pd3dDevice, pd3dCommandList, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	pTexture = std::make_shared<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Explode_8x8.dds", RESOURCE_TEXTURE2D, 0);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
	CreateShaderResourceViews(pd3dDevice, pTexture.get(), 0, 3);
}

void CMultiSpriteObjectsShader::AddObject(XMFLOAT3 xmf3Position)
{
	CMultiSpriteObject* pObject = new CMultiSpriteObject;
	pObject->SetMesh(pTexturedRectMesh);
	pObject->SetTexture(pTexture);
	pObject->SetPosition(xmf3Position);
	pObject->SetRowColumn(8, 8);
	pObject->Animate(0.0f);
	pObject->m_fSpeed = 3.0f / (8 * 8);

	m_ppObject.push_back(pObject);
}

void CMultiSpriteObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	
	CShader::Render(pd3dCommandList, pCamera);
	if (!m_ppObject.empty())
	{
		for (CMultiSpriteObject* pObject : m_ppObject)
		{
			pObject->SetLookAt(xmf3CameraPosition, XMFLOAT3(0.0f, 1.0f, 0.0f));
			pObject->UpdateTransform(NULL);
			pObject->Render(pd3dCommandList, pCamera);
		}
	}			
}

void CMultiSpriteObjectsShader::AnimateObjects(float fTimeElapsed)
{
	if (!m_ppObject.empty())
	{
		for (std::list<CMultiSpriteObject*>::iterator i = m_ppObject.begin(); i!=m_ppObject.end(); ++i)
		{
			(*i)->Animate(fTimeElapsed);
			if ((*i)->IsFullAnimated())
			{
				delete (*i);
				m_ppObject.erase(i);
			}
		}
	}
}

CMultiSpriteObjectsShader* CMultiSpriteObjectsShader::Instance()
{
	static CMultiSpriteObjectsShader instance;

	return &instance;
}
