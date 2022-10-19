Texture2D gtxtBillBoardTexture : register(t7);
SamplerState gSamplerState : register(s0);

struct GS_OUT
{
	float4 position : SV_POSITION;
	float2 uv : SIZE;
};

float4 PS_BillBoard(GS_OUT input) : SV_TARGET
{
	float4 cColor = gtxtBillBoardTexture.Sample(gSamplerState, input.uv);

	return(cColor);
}