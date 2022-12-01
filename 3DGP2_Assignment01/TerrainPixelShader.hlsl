Texture2D<float4> gtxtTerrainBaseTexture : register(t8);
Texture2D<float4> gtxtTerrainDetailTexture : register(t9);
Texture2D<float> gtxtTerrainAlphaTexture : register(t10);
SamplerState gSamplerState : register(s0);

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
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
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gSamplerState, input.uv0);
	float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gSamplerState, input.uv1);
	float fAlpha = gtxtTerrainAlphaTexture.Sample(gSamplerState, input.uv0);

	float4 cColor = saturate(lerp(cBaseTexColor, cDetailTexColor, fAlpha));
	output.f4Scene = output.f4Color = cColor;
	return(output);
}