//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2021 Ryo Suzuki
//	Copyright (c) 2016-2021 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

//
// Textures
//
Texture2D        g_texture0 : register(t0);
SamplerState     g_textrue0 : register(t0);

namespace s3d
{
	//
	//	VS Output / PS Input
	//
	struct PSInput
	{
		float4 position	: SV_POSITION;
		float4 color	: COLOR0;
		float2 uv		: TEXCOORD0;
	};
}

//
// HSV and RGB Conversion Functions
//
float rgb2hsv(float3 c)
{
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = lerp(float4(c.bg, K.wz), float4(c.gb, K.xy), step(c.b, c.g));
    float4 q = lerp(float4(p.xyw, c.r), float4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

float3 hsv2rgb(float3 c)
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float4 PS(s3d::PSInput input) : SV_TARGET
{
    // テクスチャから色を取得
    float4 texColor = g_texture0.Sample(g_sampler0, input.uv);
    
    // RGBからHSVに変換
    float3 hsv = rgb2hsv(texColor.rgb);
    
    // Value（明度）を反転
    hsv.z = 1.0 - hsv.z;
    
    // HSVからRGBに戻す
    float3 rgb = hsv2rgb(hsv);
    
    // 元のアルファ値を保持
    return float4(rgb, texColor.a) * input.color;
}

