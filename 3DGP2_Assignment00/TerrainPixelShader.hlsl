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

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gSamplerState, input.uv0);
	float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gSamplerState, input.uv1);
	float fAlpha = gtxtTerrainAlphaTexture.Sample(gSamplerState, input.uv0);

	float4 cColor = saturate(lerp(cBaseTexColor, cDetailTexColor, fAlpha));

	return(cColor);
}