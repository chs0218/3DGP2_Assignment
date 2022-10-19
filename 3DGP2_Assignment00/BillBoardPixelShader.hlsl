Texture2D gtxtBillBoardTexture : register(t7);
SamplerState gSamplerState : register(s0);

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : SIZE;
};

float4 PS_BillBoard(VS_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtBillBoardTexture.Sample(gSamplerState, input.uv);

	return(cColor);
}