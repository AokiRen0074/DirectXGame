cbuffer NeonSettings : register(b1)
{
    float4 neonColor;
    float intensity;
    float radius;
    float softness;
    float tubeLength; // ★追加：C++の length と繋がる変数
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 main(VSOutput input) : SV_TARGET
{
    // UVを -1.0 ～ 1.0 に変換
    float2 p = input.uv * 2.0f - 1.0f;

    // ==========================================
    // ★最大のポイント：Y座標に長さを掛けて「空間の歪み」を直す！
    // これにより、どんなに細長い板でも、端っこが真ん丸になります。
    // ==========================================
    p.y *= tubeLength;

    // 板の端っこから少し内側（0.15くらい）で線分を止めて、光の余白を作る
    float halfLength = max(0.0f, tubeLength - 0.6f);
    
    // カプセル（線分）からの距離を計算
    float2 d = float2(abs(p.x), max(0.0f, abs(p.y) - halfLength));
    float dist = length(d);

    // 質感の計算
    float core = smoothstep(radius, radius * 0.5f, dist);
    float glow = exp(-dist * softness);

    float3 finalColor = neonColor.rgb * intensity * glow;
    finalColor += core * intensity * 1.5f;

    return float4(finalColor, 1.0f);
}