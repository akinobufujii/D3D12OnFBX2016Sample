#include "Header.hlsli"

float4 main(VS_OUT vsin) : SV_TARGET
{
	return abs(vsin.color);
}
