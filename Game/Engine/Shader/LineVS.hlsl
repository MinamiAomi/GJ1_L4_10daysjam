struct Transformation {
    float4x4 viewProjMatrix;
};

ConstantBuffer<Transformation> transformation_ : register(b0);


struct VSInput {
    float3 position : POSITION0;
    float4 color : COLOR0;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

VSOutput main(VSInput input) {
    VSOutput output;
    output.position = mul(float4(input.position.xyz, 1.0f), transformation_.viewProjMatrix);
    output.color = input.color;
    return output;
}