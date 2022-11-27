SamplerState gSamplerState : register(s0);
Texture2D gtxtMultiSpriteTexture : register(t7);

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

//[earlydepthstencil]
float4 PS_MultiSprite(VS_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtMultiSpriteTexture.Sample(gSamplerState, input.uv);

	return(cColor);
}