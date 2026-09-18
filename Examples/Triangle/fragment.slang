struct VSOutput
{
    float4 position : SV_Position;
};

[shader("fragment")]
float4 main(VSOutput input) : SV_Target
{
    return float4(1.0, 0.0, 0.0, 1.0);
}