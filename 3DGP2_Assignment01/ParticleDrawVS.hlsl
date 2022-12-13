#define PARTICLE_TYPE_EMITTER		0
#define PARTICLE_TYPE_SHELL			1
#define PARTICLE_TYPE_FLARE01		2
#define PARTICLE_TYPE_FLARE02		3
#define PARTICLE_TYPE_FLARE03		4

#define SHELL_PARTICLE_LIFETIME		3.0f
#define FLARE01_PARTICLE_LIFETIME	2.5f
#define FLARE02_PARTICLE_LIFETIME	1.5f
#define FLARE03_PARTICLE_LIFETIME	2.0f

struct VS_PARTICLE_DRAW_OUTPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float size : SCALE;
	uint type : PARTICLETYPE;
};

struct VS_PARTICLE_INPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float lifetime : LIFETIME;
	uint type : PARTICLETYPE;
};

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}

VS_PARTICLE_DRAW_OUTPUT VSParticleDraw(VS_PARTICLE_INPUT input)
{
	VS_PARTICLE_DRAW_OUTPUT output = (VS_PARTICLE_DRAW_OUTPUT)0;

	output.position = mul(float4(input.position, 1.0f), gmtxGameObject);
	output.size = 2.5f;
	output.type = input.type;

	if (input.type == PARTICLE_TYPE_EMITTER) { output.color = float4(1.0f, 0.1f, 0.1f, 1.0f); output.size = 3.0f; }
	else if (input.type == PARTICLE_TYPE_SHELL) { output.color = float4(1.0f, 1.0f, 0.1f, 1.0f); output.size = 3.0f; }
	else if (input.type == PARTICLE_TYPE_FLARE01) { output.color = float4(0.1f, 1.0f, 1.0f, 1.0f); output.color *= (input.lifetime / FLARE01_PARTICLE_LIFETIME); }
	else if (input.type == PARTICLE_TYPE_FLARE02) output.color = float4(1.0f, 0.1f, 1.0f, 1.0f);
	else if (input.type == PARTICLE_TYPE_FLARE03) { output.color = float4(1.0f, 0.1f, 1.0f, 1.0f); output.color *= (input.lifetime / FLARE03_PARTICLE_LIFETIME); }

	return(output);
}