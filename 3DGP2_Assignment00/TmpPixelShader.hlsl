SamplerState gSamplerState : register(s0);
Texture2D gtxTmpTexture[7] : register(t11);

struct GS_OUTPUT
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float2 uv : TEXCOORD;
};

//[earlydepthstencil]
float4 PS_Tmp(GS_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxTmpTexture[0].Sample(gSamplerState, input.uv);

	return(cColor);
}