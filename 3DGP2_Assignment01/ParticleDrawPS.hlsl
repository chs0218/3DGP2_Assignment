Texture2D<float4> gtxtParticleTexture : register(t25);
SamplerState gSamplerState : register(s0);

struct GS_PARTICLE_DRAW_OUTPUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXTURE;
	uint type : PARTICLETYPE;
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

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSParticleDraw(GS_PARTICLE_DRAW_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float4 cColor = gtxtParticleTexture.Sample(gSamplerState, input.uv);
	cColor *= input.color;
	output.f4Scene = output.f4Color = cColor;

	return(output);
}