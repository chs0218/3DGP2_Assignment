SamplerState gSamplerState : register(s0);
Texture2D gtxtInputTextures[7] : register(t1); //Color, NormalW, Texture, Illumination, ObjectID+zDepth, NormalV, Depth 

cbuffer cbDrawOptions : register(b5)
{
	int4 gvDrawOptions : packoffset(c0);
};

struct VS_SCREEN_RECT_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 viewSpaceDir : TEXCOORD1;
};

float4 AlphaBlend(float4 top, float4 bottom)
{
	float3 color = (top.rgb * top.a) + (bottom.rgb * (1 - top.a));
	float alpha = top.a + bottom.a * (1 - top.a);

	return(float4(color, alpha));
}

float4 Outline(VS_SCREEN_RECT_TEXTURED_OUTPUT input)
{
	float fHalfScaleFloor = floor(1.0f * 0.5f);
	float fHalfScaleCeil = ceil(1.0f * 0.5f);

	float2 f2BottomLeftUV = input.uv - float2((1.0f / gtxtInputTextures[0].Length.x), (1.0f / gtxtInputTextures[0].Length.y)) * fHalfScaleFloor;
	float2 f2TopRightUV = input.uv + float2((1.0f / gtxtInputTextures[0].Length.x), (1.0f / gtxtInputTextures[0].Length.y)) * fHalfScaleCeil;
	float2 f2BottomRightUV = input.uv + float2((1.0f / gtxtInputTextures[0].Length.x) * fHalfScaleCeil, -(1.0f / gtxtInputTextures[0].Length.y * fHalfScaleFloor));
	float2 f2TopLeftUV = input.uv + float2(-(1.0f / gtxtInputTextures[0].Length.x) * fHalfScaleFloor, (1.0f / gtxtInputTextures[0].Length.y) * fHalfScaleCeil);

	float3 f3NormalV0 = gtxtInputTextures[5].Sample(gSamplerState, f2BottomLeftUV).rgb;
	float3 f3NormalV1 = gtxtInputTextures[5].Sample(gSamplerState, f2TopRightUV).rgb;
	float3 f3NormalV2 = gtxtInputTextures[5].Sample(gSamplerState, f2BottomRightUV).rgb;
	float3 f3NormalV3 = gtxtInputTextures[5].Sample(gSamplerState, f2TopLeftUV).rgb;

	float fDepth0 = gtxtInputTextures[6].Sample(gSamplerState, f2BottomLeftUV).r;
	float fDepth1 = gtxtInputTextures[6].Sample(gSamplerState, f2TopRightUV).r;
	float fDepth2 = gtxtInputTextures[6].Sample(gSamplerState, f2BottomRightUV).r;
	float fDepth3 = gtxtInputTextures[6].Sample(gSamplerState, f2TopLeftUV).r;

	float3 f3NormalV = f3NormalV0 * 2.0f - 1.0f;
	float fNdotV = 1.0f - dot(f3NormalV, -input.viewSpaceDir);

	float fNormalThreshold01 = saturate((fNdotV - 0.5f) / (1.0f - 0.5f));
	float fNormalThreshold = (fNormalThreshold01 * 7.0f) + 1.0f;

	float fDepthThreshold = 1.5f * fDepth0 * fNormalThreshold;

	float fDepthDifference0 = fDepth1 - fDepth0;
	float fDepthDifference1 = fDepth3 - fDepth2;
	float fDdgeDepth = sqrt(pow(fDepthDifference0, 2) + pow(fDepthDifference1, 2)) * 100.0f;
	fDdgeDepth = (fDdgeDepth > 1.5f) ? 1.0f : 0.0f;

	float3 fNormalDifference0 = f3NormalV1 - f3NormalV0;
	float3 fNormalDifference1 = f3NormalV3 - f3NormalV2;
	float fEdgeNormal = sqrt(dot(fNormalDifference0, fNormalDifference0) + dot(fNormalDifference1, fNormalDifference1));
	fEdgeNormal = (fEdgeNormal > 0.4f) ? 1.0f : 0.0f;

	float fEdge = max(fDdgeDepth, fEdgeNormal);
	float4 f4EdgeColor = float4(1.0f, 1.0f, 1.0f, 1.0f * fEdge);

	float4 f4Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	//float4 f4Color = gtxtInputTextures[0].Sample(gSamplerState, input.uv);

	return(AlphaBlend(f4EdgeColor, f4Color));
}

float4 GetColorFromDepth(float fDepth)
{
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (fDepth > 1.0f) cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	else if (fDepth < 0.00625f) cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	else if (fDepth < 0.0125f) cColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	else if (fDepth < 0.025f) cColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	else if (fDepth < 0.05f) cColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
	else if (fDepth < 0.075f) cColor = float4(0.0f, 1.0f, 1.0f, 1.0f);
	else if (fDepth < 0.1f) cColor = float4(1.0f, 0.5f, 0.5f, 1.0f);
	else if (fDepth < 0.4f) cColor = float4(0.5f, 1.0f, 1.0f, 1.0f);
	else if (fDepth < 0.6f) cColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
	else if (fDepth < 0.8f) cColor = float4(0.5f, 0.5f, 1.0f, 1.0f);
	else if (fDepth < 0.9f) cColor = float4(0.5f, 1.0f, 0.5f, 1.0f);
	else cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return(cColor);
}

float4 PSScreenRectSamplingTextured(VS_SCREEN_RECT_TEXTURED_OUTPUT input) : SV_Target
{
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	switch (gvDrawOptions.x)
	{
		case 84: //'T'
		{
			cColor = gtxtInputTextures[2].Sample(gSamplerState, input.uv);
			break;
		}
		case 76: //'L'
		{
			cColor = gtxtInputTextures[3].Sample(gSamplerState, input.uv);
			break;
		}
		case 78: //'N'
		{
			cColor = gtxtInputTextures[1].Sample(gSamplerState, input.uv);
			break;
		}
		case 68: //'D'
		{
			float fDepth = gtxtInputTextures[6].Load(uint3((uint)input.position.x, (uint)input.position.y, 0)).r;
			cColor = GetColorFromDepth(1.0f - fDepth);
			break;
		}
		case 90: //'Z' 
		{
			float fDepth = gtxtInputTextures[5].Load(uint3((uint)input.position.x, (uint)input.position.y, 0)).r;
			cColor = GetColorFromDepth(fDepth);
			break;
		}
		case 79: //'O'
		{
			uint fObjectID = (uint)gtxtInputTextures[4].Load(uint3((uint)input.position.x, (uint)input.position.y, 0)).r;
//			uint fObjectID = (uint)gtxtInputTextures[4][int2(input.position.xy)].r;
			if (fObjectID == 0) cColor.rgb = float3(1.0f, 1.0f, 1.0f);
			else if (fObjectID <= 1000) cColor.rgb = float3(1.0f, 0.0f, 0.0f);
			else if (fObjectID <= 2000) cColor.rgb = float3(0.0f, 1.0f, 0.0f);
			else if (fObjectID <= 3000) cColor.rgb = float3(0.0f, 0.0f, 1.0f);
			else if (fObjectID <= 4000) cColor.rgb = float3(0.0f, 1.0f, 1.0f);
			else if (fObjectID <= 5000) cColor.rgb = float3(1.0f, 1.0f, 0.0f);
			else if (fObjectID <= 6000) cColor.rgb = float3(1.0f, 1.0f, 1.0f);
			else if (fObjectID <= 7000) cColor.rgb = float3(1.0f, 0.5f, 0.5f);
			else cColor.rgb = float3(0.3f, 0.75f, 0.5f);

//			cColor.rgb = fObjectID;
			break;
		}
		case 69: //'E'
		{
			//cColor = LaplacianEdge(input.position);
			cColor = Outline(input);
			break;
		}
	}
	return(cColor);
}