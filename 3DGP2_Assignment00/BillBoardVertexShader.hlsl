struct VS_INPUT
{
	float3 position : POSITION;
	float2 size : SIZE;
};

struct VS_OUTPUT
{
	float3 position : POSITION;
	float2 size : SIZE;
};

VS_OUTPUT VS_BillBoard(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = input.position;
	output.size = input.size;
	return(output);
}