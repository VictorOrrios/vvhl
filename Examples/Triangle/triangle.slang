
RWTexture2D<float4> outImage;

[shader("compute")]
[numthreads(16, 16, 1)]
void computeMain(uint3 threadIdx: SV_DispatchThreadID)
{
    int2 fragCoord = threadIdx.xy;
    int2 iResolution;
    outImage.GetDimensions(iResolution.x, iResolution.y);
    float2 uv = fragCoord / iResolution;

    outImage[fragCoord] = float4(uv, 1.0, 0.0);
}
