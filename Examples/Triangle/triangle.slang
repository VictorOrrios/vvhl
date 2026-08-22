
RWTexture2D<float4> outImage;

[shader("compute")]
[numthreads(1, 1, 1)]
void main(uint3 threadIdx: SV_DispatchThreadID)
{
    int2 fragCoord = threadIdx.xy;
    int2 iResolution;
    outImage.GetDimensions(iResolution.x, iResolution.y);

    if (any(fragCoord > iResolution)) return;

    float2 uv = float2(fragCoord) / float2(iResolution);

    outImage[fragCoord] = float4(uv, 1.0, 1.0);
}
