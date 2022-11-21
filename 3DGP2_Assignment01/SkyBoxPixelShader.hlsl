Texture2D gtxMappedTexture[7] : register(t0);
Texture2D gtxtSkyBoxTexture : register(t7);
SamplerState gSamplerState : register(s0);

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

[earlydepthstencil]
float4 PS_SkyBox(VS_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyBoxTexture.Sample(gSamplerState, input.uv);

	return(cColor);
}