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

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
	float4 f4Scene : SV_TARGET0; //Swap Chain Back Buffer
	float4 f4Color : SV_TARGET1;
	float4 f4Normal : SV_TARGET2;
	float4 f4Texture : SV_TARGET3;
	float4 f4Illumination : SV_TARGET4;
	float2 f2ObjectIDzDepth : SV_TARGET5;
	float4 f4CameraNormal : SV_TARGET6;
};

//[earlydepthstencil]
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PS_BillBoard(GS_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float4 cColor = gtxTmpTexture[gnTexturesMask].Sample(gSamplerState, input.uv);
	output.f4Scene = output.f4Color = cColor;

	return(output);
}