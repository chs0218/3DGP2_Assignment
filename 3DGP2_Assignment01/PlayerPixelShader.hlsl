#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix gmtxInverseProjection : packoffset(c8);
	float3 gf3CameraPosition : packoffset(c12);
	float3 gf3CameraDirection : packoffset(c13);
};

Texture2D gtxMappedTexture[7] : register(t0);
SamplerState gSamplerState : register(s0);

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
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

[earlydepthstencil]
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PS_Player(VS_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxMappedTexture[0].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxMappedTexture[1].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxMappedTexture[2].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_METALLIC_MAP) cMetallicColor = gtxMappedTexture[3].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_EMISSION_MAP) cEmissionColor = gtxMappedTexture[4].Sample(gSamplerState, input.uv);

	float4 cColor = cAlbedoColor + cSpecularColor + cEmissionColor;
	cColor.a = cAlbedoColor.a + 0.3f;
	output.f4Scene = output.f4Color = cColor;
	return output;
}