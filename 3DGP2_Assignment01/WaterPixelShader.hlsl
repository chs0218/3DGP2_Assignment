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
	float 		gfCurrentTime : packoffset(c0.x);
	float		gfElapsedTime : packoffset(c0.y);
	float2		gf2CursorPos : packoffset(c0.z);
	int4		gvDrawOptions : packoffset(c4);
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
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PS_Water(VS_WATER_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	//float4 cBaseTexColor = gtxtWaterBaseTexture.Sample(gSamplerState, input.uv0);
	float4 cBaseTexColor = gtxtWaterBaseTexture.Sample(gSamplerState, float2(input.uv0.x, input.uv0.y - abs(sin(gfCurrentTime)) * 0.0151f));
	float4 cDetailTexColor = gtxtWaterDetailTexture.Sample(gSamplerState, float2((input.uv0.x + (gfCurrentTime * 0.01f)) * 10.0f, input.uv0.y * 10.0f));

	float4 cColor = (cBaseTexColor * 0.3f + cDetailTexColor * 0.7f) + float4(0.0f, 0.0f, 0.15f, 0.0f);
	//float4 cColor = cBaseTexColor;

	
	cColor.a = 0.55f;

	output.f4Scene = output.f4Color = cColor;
	/*cColor *= input.color;*/
	return(output);
}