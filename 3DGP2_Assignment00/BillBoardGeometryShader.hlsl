struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

[maxvertexcount(3)]
void GS_BillBoard(
	triangle VS_OUTPUT input[3], 
	inout TriangleStream< VS_OUTPUT > outStream
)
{
	VS_OUTPUT output;
	for (int i = 0; i < 3; ++i)
	{
		output.position = input[i].position;
		output.uv = input[i].uv;
		outStream.Append(output);
	}
}