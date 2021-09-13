#include "Data/Shaders/Fullscreen/FullscreenShaderStructs.hlsli"

PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput outp;
	if (input.myUV.x > 0 && input.myUV.y > 0 && input.myUV.x < 1 && input.myUV.y < 1)
	{
		outp.myColor.rgb = resource1.Sample(defaultSampler, input.myUV).rgb;
	}
	else
	{
		if(input.myPosition.x % 16 > 8 ^ input.myPosition.y%16 > 8)
		{
			outp.myColor.rgb = float3(0.95, 0.95, 0.95);
		}
		else
		{
			outp.myColor.rgb = float3(0.8, 0.8, 0.8);
		}
	}
	outp.myColor.a = 1;
	return outp;
}