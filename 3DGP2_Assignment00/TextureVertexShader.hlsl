cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	uint gnTexturesMask : packoffset(c4);
}

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
}

struct VS_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
};

VS_OUTPUT VS_Tex(VS_INPUT input)
{
	VS_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;
	return(output);
}