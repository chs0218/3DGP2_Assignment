cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix gmtxInverseProjection : packoffset(c8);
	float3 gf3CameraPosition : packoffset(c12);
	float3 gf3CameraDirection : packoffset(c13);
};

cbuffer cbFrameworkInfo : register(b2)
{
	float 		gfCurrentTime : packoffset(c0.x);
	float		gfElapsedTime : packoffset(c0.y);
	float2		gf2CursorPos : packoffset(c0.z);
	int4		gvDrawOptions : packoffset(c1);
	float3		gf3Gravity : packoffset(c2.x);
	int			gnMaxFlareType2Particles : packoffset(c2.w);;
	float		gfSecondsPerFirework : packoffset(c3.x);
	int			gnFlareParticlesToEmit : packoffset(c3.y);;
};

struct VS_WATER_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_WATER_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_WATER_OUTPUT VS_Water(VS_WATER_INPUT input)
{
	VS_WATER_OUTPUT output;
	input.position.y += sin(gfCurrentTime * 0.5f + input.position.x * 0.01f + input.position.z * 0.01f) * 20.0f;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = (input.position.y / 200.0f) + 0.55f;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}