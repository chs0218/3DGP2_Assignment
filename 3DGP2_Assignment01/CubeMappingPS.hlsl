SamplerState gSamplerState : register(s0);
TextureCube gtxtCubeMap : register(t28);

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix gmtxInverseProjection : packoffset(c8);
	float3 gf3CameraPosition : packoffset(c12);
	float3 gf3CameraDirection : packoffset(c13);
};

struct VS_LIGHTING_OUTPUT
{
	float4	position    : SV_POSITION;
	float3	positionW   : POSITION;
	float3	normalW		: NORMAL;
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

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSCubeMapping(VS_LIGHTING_OUTPUT input) : SV_Target
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	input.normalW = normalize(input.normalW);

	float3 vFromCamera = normalize(input.positionW - gf3CameraPosition.xyz);
	float3 vReflected = normalize(reflect(vFromCamera, input.normalW));
	float4 cCubeTextureColor = gtxtCubeMap.Sample(gSamplerState, vReflected);

	output.f4Scene = output.f4Color = cCubeTextureColor;

	return(output);
}