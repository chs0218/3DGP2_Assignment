cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	int m_nType : packoffset(c4);
}

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
}

Texture2D gtxtTexture[8] : register(t7);
SamplerState gSamplerState : register(s0);

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

float4 PS_Tmp(VS_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture[m_nType].Sample(gSamplerState, input.uv);
	return(cColor);
}