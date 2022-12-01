Texture2D gtxMappedTexture[7] : register(t0);
Texture2D gtxtSkyBoxTexture : register(t7);
SamplerState gSamplerState : register(s0);

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
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
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PS_SkyBox(VS_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float4 cColor = gtxtSkyBoxTexture.Sample(gSamplerState, input.uv);

	output.f4Scene = output.f4Color = cColor;
	return(output);
}