#pragma once
#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "Graphics/UploadBuffer.h"
#include "Graphics/CommandContext.h"

#include "Math/MathUtils.h"

class LineRenderer {
public:
    const uint32_t kMaxLineCount = 8192;

    struct Vertex {
        Vector3 position;
        uint32_t color;
    };

    static LineRenderer* GetInstance();

    void Initialize(DXGI_FORMAT rtvFormat);
    void Draw(const Vector3& start, const Vector3& end, uint32_t color);
    void DrawCall(CommandContext& commandContext, const Matrix4x4& viewProj);

private:
    RootSignature rootSignature_;
    PipelineState pipelineStates_;

    std::vector<Vertex> vertices;
    uint32_t numLines = 0;
};