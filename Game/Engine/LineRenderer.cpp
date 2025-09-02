#include "LineRenderer.h"

#include <cassert>

#include "Graphics/CommandContext.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Helper.h"

struct Vertex {
    Vector3 position;
    uint32_t color;
};

LineRenderer* LineRenderer::GetInstance() {
    static LineRenderer instance;
    return &instance;
}

void LineRenderer::Initialize(DXGI_FORMAT rtvFormat) {
    // ルートシグネチャを生成
    {
        CD3DX12_ROOT_PARAMETER rootParameters[1]{};
        rootParameters[0].InitAsConstantBufferView(0);

        D3D12_ROOT_SIGNATURE_DESC desc{};
        desc.pParameters = rootParameters;
        desc.NumParameters = _countof(rootParameters);
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        rootSignature_.Create(L"LineRenderer RootSignature", desc);
    }

    // パイプラインを生成
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};

        auto shaderManager = ShaderManager::GetInstance();

        desc.pRootSignature = rootSignature_;

        auto vs = shaderManager->Compile(L"Resources/Shader/LineVS.hlsl", ShaderManager::kVertex);
        auto ps = shaderManager->Compile(L"Resources/Shader/LinePS.hlsl", ShaderManager::kPixel);
        desc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        desc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());

        D3D12_INPUT_ELEMENT_DESC inputElements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };
        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
        inputLayoutDesc.pInputElementDescs = inputElements;
        inputLayoutDesc.NumElements = _countof(inputElements);
        desc.InputLayout = inputLayoutDesc;

        desc.RasterizerState = Helper::RasterizerNoCull;
        desc.NumRenderTargets = 1;
        desc.RTVFormats[0] = rtvFormat;
        desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        desc.SampleDesc.Count = 1;

        desc.BlendState = Helper::BlendDisable;
        pipelineStates_.Create(L"LineRenderer BlendDisablePSO", desc);
    }
    // バッファを生成
    {
        vertices.resize(kMaxLineCount * 2);
    }
}

void LineRenderer::Draw(const Vector3& start, const Vector3& end, uint32_t color) {
    uint32_t copyStart = numLines * 2;
    vertices[copyStart].position = start;
    vertices[copyStart].color = color;
    vertices[copyStart + 1].position = end;
    vertices[copyStart + 1].color = color;
    numLines++;
}

void LineRenderer::DrawCall(CommandContext& commandContext, const Matrix4x4& viewProj) {

    if (numLines <= 0) {
        return;
    }

    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineStates_);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
    uint32_t numVertices = numLines * 2;
    commandContext.SetDynamicVertexBuffer(0, numVertices, sizeof(Vertex), vertices.data());
    commandContext.SetDynamicConstantBufferView(0, sizeof(viewProj), &viewProj);
    commandContext.Draw(numVertices);
    numLines = 0;
}
