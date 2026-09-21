struct VSInput {
  float2 position : POSITION;
};

struct VSOutput {
  float4 position : SV_Position;
};

[shader("vertex")]
VSOutput main(VSInput input) {
  VSOutput output;
  output.position = float4(input.position, 0.0, 1.0);
  return output;
}
