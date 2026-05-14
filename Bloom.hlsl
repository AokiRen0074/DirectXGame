// Bloom.hlsl

// SRV 
Texture2D<float4> g_InputTex : register(t0);

// UAV
RWTexture2D<float4> g_OutputTex : register(u0);

[numthreads(8, 8, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 pos = dispatchThreadID.xy;

    // 画面のサイズを取得し、範囲外へのアクセスを防ぐ
    uint width, height;
    g_InputTex.GetDimensions(width, height);
    if (pos.x >= width || pos.y >= height)
        return;

    // 元の画像のピクセルの色
    float4 baseColor = g_InputTex[pos];

    // ==========================================
    // 光の漏れ出し
    // ==========================================
    float4 bloomColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float totalWeight = 0.0f;

    // 周囲のピクセルを集める範囲（半径）
    int blurRadius = 6;

    // 周囲のピクセルをループして色をかき集める
    for (int y = -blurRadius; y <= blurRadius; ++y)
    {
        for (int x = -blurRadius; x <= blurRadius; ++x)
        {
            
            // 読み取るピクセルの座標
            int2 samplePos = int2(pos.x + x, pos.y + y);
            samplePos.x = clamp(samplePos.x, 0, width - 1);
            samplePos.y = clamp(samplePos.y, 0, height - 1);

            float4 sampleColor = g_InputTex[samplePos];

            // 輝度抽出：色が 1.0 を超えている「強い光」の部分だけを抜き出す
            // 1.0を引いて、マイナスになった部分は0として切り捨てる
            float4 extractColor = max(sampleColor - 1.0f, 0.0f);

            // 中心からの距離に応じて重みを計算（ガウス関数の簡易版）
            // 遠くのピクセルほど影響を小さくする
            float distanceSq = (float) (x * x + y * y);
            float weight = exp(-distanceSq / 15.0f); // 15.0は光の広がり具合

            bloomColor += extractColor * weight;
            totalWeight += weight;
        }
    }

    // 集めた光を重みの合計で割って平均化する
    bloomColor /= totalWeight;

    // ネオンっぽさを強調するために、ぼかした光の強さを少し倍増させる
    bloomColor *= 1.5f;

    // ==========================================
    // 最終合成
    // ==========================================
    // 元の画像に、ぼかして溢れ出した光を足し合わせる！
    g_OutputTex[pos] = baseColor + bloomColor;
}