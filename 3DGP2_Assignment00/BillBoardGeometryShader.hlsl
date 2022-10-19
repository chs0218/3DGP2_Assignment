struct VS_OUTPUT
{
	float4 position : POSITION;
	float2 uv : SIZE;
};

struct GS_OUT
{
	float4 position : SV_POSITION;
	float2 uv : SIZE;
};

[maxvertexcount(1)]
void GS_BillBoard(
	point VS_OUTPUT input[1], 
	inout TriangleStream< GS_OUT > outStream
)
{
	GS_OUT output;
	output.position = input[0].position;
	output.uv = input[0].uv;
	outStream.Append(output);
}