Texture2D<float4> gtxtParticleTexture : register(t25);
SamplerState gSamplerState : register(s0);

struct GS_PARTICLE_DRAW_OUTPUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXTURE;
	uint type : PARTICLETYPE;
};

float4 PSParticleDraw(GS_PARTICLE_DRAW_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtParticleTexture.Sample(gSamplerState, input.uv);
	cColor *= input.color;

	return(cColor);
}