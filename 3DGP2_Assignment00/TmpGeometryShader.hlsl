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
	float3 gf3CameraPosition : packoffset(c8);
}

struct VS_OUTPUT
{
	float3 position : POSITION;
	float2 size : SIZE;
};

struct GS_OUTPUT 
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float2 uv : TEXCOORD;
};

[maxvertexcount(4)]
void GS_Tmp(point VS_OUTPUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_OUTPUT> outStream)
{
	float3 vUp = float3(0.0f, 1.0f, 0.0f);
	float3 vLook = gf3CameraPosition.xyz - input[0].position;
	vLook = normalize(vLook);
	float3 vRight = cross(vUp, vLook);
	float fHalfW = input[0].size.x * 0.5f;
	float fHalfH = input[0].size.y * 0.5f;
	float4 pVertices[4];
	pVertices[0] = float4(input[0].position + fHalfW * vRight - fHalfH * vUp, 1.0f);
	pVertices[1] = float4(input[0].position + fHalfW * vRight + fHalfH * vUp, 1.0f);
	pVertices[2] = float4(input[0].position - fHalfW * vRight - fHalfH * vUp, 1.0f);
	pVertices[3] = float4(input[0].position - fHalfW * vRight + fHalfH * vUp, 1.0f);

	float2 pUVs[4] = { float2(0.0f, 1.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(1.0f, 0.0f) };
	GS_OUTPUT output;
	for (int i = 0; i < 4; ++i)
	{
		output.posW = pVertices[i].xyz;
		output.posH = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.uv = pUVs[i];
		outStream.Append(output);
	}

}