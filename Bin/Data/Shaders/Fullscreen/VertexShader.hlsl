#include "Data/Shaders/Fullscreen/FullscreenShaderStructs.hlsli"

cbuffer ViewBuffer : register(b0)
{
	float4 myCorners;
	float myWidth;
	float myHeight;
}

VertexToPixel vertexShader(VertexInput input)
{
	float4 positions[6] =
	{
		float4(-1,-1,0,1),
		float4(-1,1,0,1),
		float4(1,-1,0,1),
		float4(1,-1,0,1),
		float4(-1,1,0,1),
		float4(1,1,0,1)
	};

	float2 uvs[6] =
	{
		float2(myCorners.x / myWidth, myCorners.w / myHeight),
		float2(myCorners.x / myWidth, myCorners.y / myHeight),
		float2(myCorners.z / myWidth, myCorners.w / myHeight),
		float2(myCorners.z / myWidth, myCorners.w / myHeight),
		float2(myCorners.x / myWidth, myCorners.y / myHeight),
		float2(myCorners.z / myWidth, myCorners.y / myHeight)
	};
	/*
	float2 uvs[6] =
	{
		float2(myCorners.x / myWidth, myCorners.y / myHeight),
		float2(myCorners.x / myWidth, myCorners.w / myHeight),
		float2(myCorners.z / myWidth, myCorners.y / myHeight),
		float2(myCorners.z / myWidth, myCorners.y / myHeight),
		float2(myCorners.x / myWidth, myCorners.w / myHeight),
		float2(myCorners.z / myWidth, myCorners.w / myHeight)
	};
	*/

	VertexToPixel returnValue;
	returnValue.myPosition = positions[input.myIndex];
	returnValue.myUV = uvs[input.myIndex];
	return returnValue;
}