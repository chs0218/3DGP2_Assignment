Texture2D<float4> gtxtWaterBaseTexture : register(t8);
Texture2D<float4> gtxtWaterDetailTexture : register(t9);
SamplerState gSamplerState : register(s0);

struct VS_WATER_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

cbuffer cbFrameworkInfo : register(b2)
{
	float 		gfCurrentTime;
	float		gfElapsedTime;
	float2		gf2CursorPos;
};

[earlydepthstencil]
float4 PS_Water(VS_WATER_OUTPUT input) : SV_TARGET
{
	//float4 cBaseTexColor = gtxtWaterBaseTexture.Sample(gSamplerState, input.uv0);
	float4 cBaseTexColor = gtxtWaterBaseTexture.Sample(gSamplerState, float2(input.uv0.x, input.uv0.y - abs(sin(gfCurrentTime)) * 0.0151f));
	float4 cDetailTexColor = gtxtWaterDetailTexture.Sample(gSamplerState, float2((input.uv0.x + (gfCurrentTime * 0.01f)) * 10.0f, input.uv0.y * 10.0f));

	float4 cColor = (cBaseTexColor * 0.3f + cDetailTexColor * 0.7f) + float4(0.0f, 0.0f, 0.15f, 0.0f);
	
	cColor.a = 0.55f;
	/*cColor *= input.color;*/
	return(cColor);
}