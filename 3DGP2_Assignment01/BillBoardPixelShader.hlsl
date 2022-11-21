SamplerState gSamplerState : register(s0);
Texture2D gtxTmpTexture[7] : register(t11);

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}

struct GS_OUTPUT
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float2 uv : TEXCOORD;
};

//[earlydepthstencil]
float4 PS_BillBoard(GS_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxTmpTexture[gnTexturesMask].Sample(gSamplerState, input.uv);

	return(cColor);
}