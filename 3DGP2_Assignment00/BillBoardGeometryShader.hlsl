struct MATERIAL
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular; //a = power
	float4 m_cEmissive;
};

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	MATERIAL gMaterial : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	float3 gvCameraPosition : packoffset(c8);
}

struct VS_OUTPUT
{
	float3 position : POSITION;
	float2 size : TEXCOORD;
};

struct GS_OUT
{
	float4 position : SV_POSITION;
	float2 size : TEXCOORD;
};

[maxvertexcount(3)]
void GS_BillBoard(
	triangle VS_OUTPUT input[3], 
	inout TriangleStream< GS_OUT > outStream
)
{
	GS_OUT output;
	for (int i = 0; i < 3; ++i)
	{
		output.position = mul(mul(mul(float4(input[i].position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
		output.size = input[i].size;
		outStream.Append(output);
	}
}