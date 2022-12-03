cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix gmtxInverseProjection : packoffset(c8);
	float3 gf3CameraPosition : packoffset(c12);
	float3 gf3CameraDirection : packoffset(c13);
};

struct VS_PARTICLE_DRAW_OUTPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float size : SCALE;
	uint type : PARTICLETYPE;
};

struct GS_PARTICLE_DRAW_OUTPUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXTURE;
	uint type : PARTICLETYPE;
};

static float3 gf3Positions[4] = { float3(-1.0f, +1.0f, 0.5f), float3(+1.0f, +1.0f, 0.5f), float3(-1.0f, -1.0f, 0.5f), float3(+1.0f, -1.0f, 0.5f) };
static float2 gf2QuadUVs[4] = { float2(0.0f, 0.0f), float2(1.0f, 0.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f) };

[maxvertexcount(4)]
void GSParticleDraw(point VS_PARTICLE_DRAW_OUTPUT input[1], inout TriangleStream<GS_PARTICLE_DRAW_OUTPUT> outputStream)
{
	GS_PARTICLE_DRAW_OUTPUT output = (GS_PARTICLE_DRAW_OUTPUT)0;

	output.type = input[0].type;
	output.color = input[0].color;
	for (int i = 0; i < 4; i++)
	{
		float3 positionW = mul(gf3Positions[i] * input[0].size, (float3x3)gmtxInverseProjection) + input[0].position;
		output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
		output.uv = gf2QuadUVs[i];

		outputStream.Append(output);
	}
	outputStream.RestartStrip();
}