#include "TOMATOsEngine.h"

#include <cassert>
#include <memory>
#include <chrono>
#include <thread>

#include "GameWindow.h"
#include "RenderManager.h"
#include "TriangleRenderer.h"
#include "SpriteRenderer.h"
#include "LineRenderer.h"
#include "TextureManager.h"
#include "Model.h"
#include "RealWorld.h"
#include "Monitor.h"
#include "Math/Random.h"

#include "Audio/Audio.h"
#include "ImGuiManager.h"
#include "Transition.h"

#pragma comment(lib, "winmm.lib")

namespace {
    const wchar_t kWindowTitle[] = L"BB";

    GameWindow* gameWindow = nullptr;
    RenderManager* renderManager = nullptr;
    TriangleRenderer* triangleRenderer = nullptr;
    SpriteRenderer* spriteRenderer = nullptr;
    LineRenderer* lineRenderer = nullptr;
    TextureManager* textureManager = nullptr;
    Transition* transition = nullptr;
    Input* input = nullptr;
    Audio* audio = nullptr;

    Matrix4x4 screenMatrix;
    Matrix4x4 frameMatrix;
    Matrix4x4 cameraMatrix;

    Monitor* monitor = nullptr;
    std::unique_ptr<RealWorld> realWorld;
    RealWorld::ViewMode viewMode;

    Random::RandomNumberGenerator rng;
    bool applyLineShakeX = false;
    bool applyLineShakeY = false;
    bool applyLineShakeZ = false;
    Vector3 lineShakeValue = {};

    std::chrono::steady_clock::time_point referenceTime;

    bool isEndRequest = false;

    inline uint32_t RGBAtoABGR(uint32_t argb) {
        uint8_t R = (argb >> 24) & 0xFFu;
        uint8_t G = (argb >> 16) & 0xFFu;
        uint8_t B = (argb >> 8) & 0xFFu;
        uint8_t A = (argb >> 0) & 0xFFu;
        return (A << 24) | (B << 16) | (G << 8) | (R << 0);
    }
}

namespace TOMATOsEngine {

    void Initialize() {
        assert(!gameWindow);
        assert(!renderManager);
        assert(!triangleRenderer);
        assert(!spriteRenderer);
        assert(!textureManager);
        assert(!input);
        assert(!monitor);
        assert(!audio);

        gameWindow = GameWindow::GetInstance();
        gameWindow->Initialize(kWindowTitle, 1280, 720);

        input = Input::GetInstance();
        input->Initialize(gameWindow->GetHWND());

        renderManager = RenderManager::GetInstance();
        renderManager->Initialize();

        Model::CreatePipeline(renderManager->GetMainBufferRTVFormat(), renderManager->GetMainDepthDSVFormat());

        monitor = Monitor::GetInstance();
        monitor->Initilaize(kMonitorWidth, kMonitorHeight, renderManager->GetMainBufferRTVFormat(), renderManager->GetMainDepthDSVFormat());

        realWorld = std::make_unique<RealWorld>();
        realWorld->Initialize();

        triangleRenderer = TriangleRenderer::GetInstance();
        triangleRenderer->Initialize(monitor->GetColorBuffer().GetFormat());

        spriteRenderer = SpriteRenderer::GetInstance();
        spriteRenderer->Initialize(monitor->GetColorBuffer().GetFormat());

        lineRenderer = LineRenderer::GetInstance();
        lineRenderer->Initialize(monitor->GetColorBuffer().GetFormat());

        textureManager = TextureManager::GetInstance();

        audio = Audio::GetInstance();
        audio->Initialize();

        gameWindow->SetFullScreen(true);

        frameMatrix = Matrix4x4::MakeTranslation({ 0.0f, 0.0f, 0.0f });
        frameMatrix *= Matrix4x4::MakeOrthographicProjection(float(kMonitorWidth), float(kMonitorHeight), 0.0f, 1.0f);
        frameMatrix *= Matrix4x4::MakeScaling({ 1.0f, 01.0f, 1.0f });
        frameMatrix *= Matrix4x4::MakeTranslation({ -1.0f, -1.0f, 0.0f });

        screenMatrix = frameMatrix * Matrix4x4::MakeScaling({ 0.9999f, 0.9999f, 0.0f });

        renderManager->Reset();

        renderManager->BeginRender();

        isEndRequest = false;

        referenceTime = std::chrono::steady_clock::now();

        transition = Transition::GetInstance();
    }

    void Shutdown() {
        renderManager->Shutdown();
        realWorld.reset();
        Model::DestroyPipeline();
        gameWindow->Shutdown();
    }

    bool BeginFrame() {
        lineRenderer->DrawCall(renderManager->GetCommandContext(), cameraMatrix);

        realWorld->Update();

        renderManager->BeginRender();
        realWorld->Draw(renderManager->GetCommandContext());
        renderManager->EndRender();

        // FPS固定
        {
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            std::chrono::microseconds elapsed =
                std::chrono::duration_cast<std::chrono::microseconds>(now - referenceTime);

            static const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 62.0f));
            static const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
            std::chrono::microseconds check = kMinCheckTime - elapsed;
            if (check > std::chrono::microseconds(0)) {
                std::chrono::microseconds waitTime = kMinTime - elapsed;

                std::chrono::steady_clock::time_point waitStart = std::chrono::steady_clock::now();
                timeBeginPeriod(1);
                do {
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                } while (std::chrono::steady_clock::now() - waitStart < waitTime);
                timeEndPeriod(1);
            }

            elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - referenceTime);
            referenceTime = std::chrono::steady_clock::now();
        }

        if (!gameWindow->ProcessMessage() ||
            isEndRequest) {
            return false;
        }

        input->Update();
        audio->Update();

        renderManager->Reset();

        monitor->BeginRender(renderManager->GetCommandContext());

#ifdef _DEBUG
        ImGui::Begin("Menu");
        if (ImGui::TreeNode("Engine")) {
            ImGui::Text("Apply Line Shake");
            ImGui::SameLine();
            ImGui::Checkbox("X", &applyLineShakeX);
            ImGui::SameLine();
            ImGui::Checkbox("Y", &applyLineShakeY);
            ImGui::SameLine();
            ImGui::Checkbox("Z", &applyLineShakeZ);
            ImGui::DragFloat3("Line Shake Value", &lineShakeValue.x, 0.01f, 0.0f, 10.0f);
            ImGui::TreePop();
        }
        ImGui::End();
#endif // _DEBUG

        return true;
    }

    TextureHandle LoadTexture(const std::string& name) {
        return textureManager->Load(name);
    }

    void SetBlendMode(BlendMode blendMode) {
        switch (blendMode)
        {
        case kBlendModeDisable:
            triangleRenderer->SetBlendMode(TriangleRenderer::kBlendDisable);
            spriteRenderer->SetBlendMode(SpriteRenderer::kBlendDisable);
            break;
        case kBlendModeNormal:
            triangleRenderer->SetBlendMode(TriangleRenderer::kBlendNormal);
            spriteRenderer->SetBlendMode(SpriteRenderer::kBlendNormal);
            break;
        case kBlendModeAdditive:
            triangleRenderer->SetBlendMode(TriangleRenderer::kBlendAdditive);
            spriteRenderer->SetBlendMode(SpriteRenderer::kBlendAdditive);
            break;
        case kBlendModeSubtract:
            triangleRenderer->SetBlendMode(TriangleRenderer::kBlendSubtract);
            spriteRenderer->SetBlendMode(SpriteRenderer::kBlendSubtract);
            break;
        default:
            assert(false);
            break;
        }

    }

    void SetCameraMatrix(const Matrix4x4& matrix) {
        cameraMatrix = matrix;
    }

    void SetLineShakeX(bool apply, float value) {
        applyLineShakeX = apply;
        lineShakeValue.x = value;
    }

    void SetLineShakeY(bool apply, float value) {
        applyLineShakeY = apply;
        lineShakeValue.y = value;
    }
    
    void SetLineShakeZ(bool apply, float value) {
        applyLineShakeZ = apply;
        lineShakeValue.z = value;
    }

    void DrawLine3D(const Vector2& start, const Vector2& end, uint32_t color) {
        DrawLine3D({ start.x, start.y, 0.0f }, { end.x, end.y, 0.0f }, color);
    }

    void DrawLine3D(const Vector2& start, const Vector2& end, float z, uint32_t color) {
        DrawLine3D({ start.x, start.y, z }, { end.x, end.y, z }, color);
    }

    void DrawTriangle(const Vector2& pos0, const Vector2& pos1, const Vector2& pos2, uint32_t color) {
        color = RGBAtoABGR(color);

        TriangleRenderer::Vertex vertices[] = {
           { Vector3(pos0) * screenMatrix, color },
           { Vector3(pos1) * screenMatrix, color },
           { Vector3(pos2) * screenMatrix, color },
        };

        triangleRenderer->Draw(
            renderManager->GetCommandContext(),
            vertices, 3);
    }

    void DrawTriangle(const Vector2& p0, uint32_t c0, const Vector2& p1, uint32_t c1, const Vector2& p2, uint32_t c2) {
        c0 = RGBAtoABGR(c0);
        c1 = RGBAtoABGR(c1);
        c2 = RGBAtoABGR(c2);

        TriangleRenderer::Vertex vertices[] = {
           { Vector3(p0) * screenMatrix, c0 },
           { Vector3(p1) * screenMatrix, c1 },
           { Vector3(p2) * screenMatrix, c2 },
        };

        triangleRenderer->Draw(
            renderManager->GetCommandContext(),
            vertices, 3);
    }

    void DrawRect(const Vector2& min, const Vector2& max, uint32_t color) {
        color = RGBAtoABGR(color);
        TriangleRenderer::Vertex vertices[] = {
            { { min.x, max.y, 0.0f }, color },
            { { min.x, min.y, 0.0f }, color },
            { { max.x, min.y, 0.0f }, color },
            { { min.x, max.y, 0.0f }, color },
            { { max.x, min.y, 0.0f }, color },
            { { max.x, max.y, 0.0f }, color },
        };

        for (auto& vertex : vertices) {
            vertex.position = vertex.position * screenMatrix;
        }

        triangleRenderer->Draw(
            renderManager->GetCommandContext(),
            vertices, 2);
    }

    void DrawRectAngle(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, float angle, uint32_t color) {
        color = RGBAtoABGR(color);
        Vector2 tmp[] = {
            { 0.0f, 1.0f },
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
        };

        Matrix3x3 matrix = Matrix3x3::MakeTranslation(-anchorPoint) * Matrix3x3::MakeAffineTransform(size, angle, pos);

        for (auto& vertex : tmp) {
            vertex = vertex * matrix;
            vertex = (Vector3(vertex) * screenMatrix).GetXY();
        }

        TriangleRenderer::Vertex vertices[] = {
            { {tmp[0], 0.0f}, color },
            { {tmp[1], 0.0f}, color },
            { {tmp[2], 0.0f}, color },
            { {tmp[0], 0.0f}, color },
            { {tmp[2], 0.0f}, color },
            { {tmp[3], 0.0f}, color },
        };

        triangleRenderer->Draw(
            renderManager->GetCommandContext(),
            vertices, 2);
    }



    void DrawCircle(const Vector2& pos, float radius, uint32_t color) {
        color = RGBAtoABGR(color);
        constexpr uint32_t kNumSubdivisions = 16;
        constexpr uint32_t kNumTriangles = kNumSubdivisions - 2;

        struct UnitCircle {
            UnitCircle() {
                float every = Math::TwoPi / float(kNumSubdivisions);
                for (uint32_t i = 0; i < kNumSubdivisions; ++i) {
                    float angle = every * i;
                    vertices[i].x = std::cos(angle);
                    vertices[i].y = std::sin(angle);
                }
            }
            Vector2 vertices[kNumSubdivisions];
        };
        static const UnitCircle unitCircle;

        Matrix3x3 matrix{
            radius, 0.0f, 0.0f,
            0.0f, radius, 0.0f,
            pos.x, pos.y, 1.0f };

        Vector3 global[kNumSubdivisions]{};
        for (uint32_t i = 0; i < kNumSubdivisions; ++i) {
            global[i] = Vector3(unitCircle.vertices[i] * matrix) * screenMatrix;
        }

        TriangleRenderer::Vertex vertices[kNumTriangles][3]{};
        for (uint32_t i = 0; i < kNumTriangles; ++i) {
            vertices[i][0].position = global[0];
            vertices[i][0].color = color;
            vertices[i][1].position = global[i + 1];
            vertices[i][1].color = color;
            vertices[i][2].position = global[i + 2];
            vertices[i][2].color = color;
        }

        triangleRenderer->Draw(
            renderManager->GetCommandContext(),
            vertices[0], kNumTriangles);
    }

    void DrawSpriteRect(const Vector2& min, const Vector2& max, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        color = RGBAtoABGR(color);
        assert(texHandle.IsValid());

        auto& texture = textureManager->GetTexture(texHandle);
        auto& desc = texture.GetDesc();

        float invWidth = 1.0f / float(desc.Width);
        float invHeight = 1.0f / float(desc.Height);

        float ul = texBase.x * invWidth;
        float ur = (texBase.x + texSize.x) * invWidth;
        float ut = texBase.y * invHeight;
        float ub = (texBase.y + texSize.y) * invHeight;

        SpriteRenderer::Vertex vertices[] = {
        { { min.x, max.y, 0.0f }, color, {ul, ut} },
        { { min.x, min.y, 0.0f }, color, {ul, ub} },
        { { max.x, min.y, 0.0f }, color, {ur, ub} },
        { { min.x, max.y, 0.0f }, color, {ul, ut} },
        { { max.x, min.y, 0.0f }, color, {ur, ub} },
        { { max.x, max.y, 0.0f }, color, {ur, ut} },
        };

        for (auto& vertex : vertices) {
            vertex.position = vertex.position * screenMatrix;
        }

        spriteRenderer->Draw(renderManager->GetCommandContext(), vertices, 2, texture.GetSRV());
    }

    void DrawSpriteRectCenter(const Vector2& center, const Vector2& size, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        Vector2 half = size * 0.5f;
        Vector2 min = center - half;
        Vector2 max = center + half;
        DrawSpriteRect(min, max, texBase, texSize, texHandle, color);
    }

    void DrawSpriteRectAngle(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, float angle, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        color = RGBAtoABGR(color);
        assert(texHandle.IsValid());

        auto& texture = textureManager->GetTexture(texHandle);
        auto& desc = texture.GetDesc();

        float invWidth = 1.0f / float(desc.Width);
        float invHeight = 1.0f / float(desc.Height);

        float ul = texBase.x * invWidth;
        float ur = (texBase.x + texSize.x) * invWidth;
        float ut = texBase.y * invHeight;
        float ub = (texBase.y + texSize.y) * invHeight;

        Vector2 tmp[] = {
           { 0.0f, 0.0f },
           { 0.0f, 1.0f },
           { 1.0f, 1.0f },
           { 1.0f, 0.0f },
        };

        Matrix3x3 matrix = Matrix3x3::MakeTranslation(-anchorPoint) * Matrix3x3::MakeAffineTransform(size, angle, pos);

        for (auto& vertex : tmp) {
            vertex = vertex * matrix;
            vertex = (Vector3(vertex) * screenMatrix).GetXY();
        }

        SpriteRenderer::Vertex vertices[] = {
            { {tmp[0], 0.0f}, color, {ul, ub} },
            { {tmp[1], 0.0f}, color, {ul, ut} },
            { {tmp[2], 0.0f}, color, {ur, ut} },
            { {tmp[0], 0.0f}, color, {ul, ub} },
            { {tmp[2], 0.0f}, color, {ur, ut} },
            { {tmp[3], 0.0f}, color, {ur, ub} },
        };
        spriteRenderer->Draw(renderManager->GetCommandContext(), vertices, 2, texture.GetSRV());
    }

    void DrawSpriteQuad(const Vector2& lt, const Vector2& rt, const Vector2& lb, const Vector2& rb, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        color = RGBAtoABGR(color);
        auto& texture = textureManager->GetTexture(texHandle);
        auto& desc = texture.GetDesc();

        float invWidth = 1.0f / float(desc.Width);
        float invHeight = 1.0f / float(desc.Height);

        float ul = texBase.x * invWidth;
        float ur = (texBase.x + texSize.x) * invWidth;
        float ut = texBase.y * invHeight;
        float ub = (texBase.y + texSize.y) * invHeight;

        Vector2 tmp[] = {
           lb,
           lt,
           rt,
           rb,
        };

        for (auto& vertex : tmp) {
            vertex = (Vector3(vertex) * screenMatrix).GetXY();
        }

        SpriteRenderer::Vertex vertices[] = {
            { {tmp[0], 0.0f}, color, {ul, ub} },
            { {tmp[1], 0.0f}, color, {ul, ut} },
            { {tmp[2], 0.0f}, color, {ur, ut} },
            { {tmp[0], 0.0f}, color, {ul, ub} },
            { {tmp[2], 0.0f}, color, {ur, ut} },
            { {tmp[3], 0.0f}, color, {ur, ub} },
        };
        spriteRenderer->Draw(renderManager->GetCommandContext(), vertices, 2, texture.GetSRV());
    }

    void DrawFrame(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, float angle, const Vector2& texBase, const Vector2& texSize, TextureHandle texHandle, uint32_t color) {
        color = RGBAtoABGR(color);
        assert(texHandle.IsValid());

        auto& texture = textureManager->GetTexture(texHandle);
        auto& desc = texture.GetDesc();

        float invWidth = 1.0f / float(desc.Width);
        float invHeight = 1.0f / float(desc.Height);

        float ul = texBase.x * invWidth;
        float ur = (texBase.x + texSize.x) * invWidth;
        float ut = texBase.y * invHeight;
        float ub = (texBase.y + texSize.y) * invHeight;

        Vector2 tmp[] = {
           { 0.0f, 0.0f },
           { 0.0f, 1.0f },
           { 1.0f, 1.0f },
           { 1.0f, 0.0f },
        };

        Matrix3x3 matrix = Matrix3x3::MakeTranslation(-anchorPoint) * Matrix3x3::MakeAffineTransform(size, angle, pos);

        for (auto& vertex : tmp) {
            vertex = vertex * matrix;
            vertex = (Vector3(vertex) * frameMatrix).GetXY();
        }

        SpriteRenderer::Vertex vertices[] = {
            { {tmp[0], 0.0f}, color, {ul, ub} },
            { {tmp[1], 0.0f}, color, {ul, ut} },
            { {tmp[2], 0.0f}, color, {ur, ut} },
            { {tmp[0], 0.0f}, color, {ul, ub} },
            { {tmp[2], 0.0f}, color, {ur, ut} },
            { {tmp[3], 0.0f}, color, {ur, ub} },
        };
        spriteRenderer->Draw(renderManager->GetCommandContext(), vertices, 2, texture.GetSRV());
    }

    void DrawLine3D(const Vector3& start, const Vector3& end, uint32_t color) {

        if (transition->isPreScene) {
            color = Math::LerpRGBA(color, 0xFFFFFFFF, transition->t);
        }
        if (transition->isNextScene) {
            color = Math::LerpRGBA(0xFFFFFFFF, color, transition->t2);
        }
        color = RGBAtoABGR(color);

        Vector3 s = start;
        Vector3 e = end;

        if (applyLineShakeX) {
            s.x += rng.NextFloatRange(-lineShakeValue.x, lineShakeValue.x);
            e.x += rng.NextFloatRange(-lineShakeValue.x, lineShakeValue.x);
        }

        if (applyLineShakeY) {
            s.y += rng.NextFloatRange(-lineShakeValue.y, lineShakeValue.y);
            e.y += rng.NextFloatRange(-lineShakeValue.y, lineShakeValue.y);
        }

        if (applyLineShakeZ) {
            s.z += rng.NextFloatRange(-lineShakeValue.z, lineShakeValue.z);
            e.z += rng.NextFloatRange(-lineShakeValue.z, lineShakeValue.z);
        }

        lineRenderer->Draw(
            s,
            e,
            color);

    }

    void DrawBoxLine3D(const Vector2& center, const Vector2& size, uint32_t color)

    {

        Vector2 leftTop = { center.x - size.x / 2.0f,center.y + size.y / 2.0f };

        Vector2 rightTop = center + size / 2.0f;

        Vector2 leftBottom = center - size / 2.0f;

        Vector2 rightBottom = { center.x + size.x / 2.0f,center.y - size.y / 2.0f };

        DrawLine3D(leftTop, rightTop, color);

        DrawLine3D(rightTop, rightBottom, color);

        DrawLine3D(leftBottom, rightBottom, color);

        DrawLine3D(leftTop, leftBottom, color);

    }



    void DrawBoxLine3D(const Vector2& center, const Vector2& size, float radian, uint32_t color)
    {
        Vector2 halfSize = { size.x / 2.0f, size.y / 2.0f };
        Vector2 corners[4] = {
            {-halfSize.x,  halfSize.y}, // 左上
            { halfSize.x,  halfSize.y}, // 右上
            { halfSize.x, -halfSize.y}, // 右下
            {-halfSize.x, -halfSize.y}  // 左下
        };

        Matrix3x3 rotateMatrix = Matrix3x3::MakeRotation(radian);

        Vector2 rotatedCorners[4];

        for (int i = 0; i < 4; ++i) {

            rotatedCorners[i] = (corners[i] * rotateMatrix) + center;
        }

        DrawLine3D(rotatedCorners[0], rotatedCorners[1], color); // 左上 -> 右上
        DrawLine3D(rotatedCorners[1], rotatedCorners[2], color); // 右上 -> 右下
        DrawLine3D(rotatedCorners[2], rotatedCorners[3], color); // 右下 -> 左下
        DrawLine3D(rotatedCorners[3], rotatedCorners[0], color); // 左下 -> 左上
    }



    void DrawBoxLine3D(const Square& square, uint32_t color)
    {
        DrawBoxLine3D(square.center, square.size, square.radian, color);
    }

    void DrawStar2D(const Vector2& center, float outerRadius, float innerRadius, float z, uint32_t color) {
        const int numVertices = 10;
        std::vector<Vector2> vertices(numVertices);

        for (int i = 0; i < numVertices; ++i) {
            float radius = (i % 2 == 0) ? outerRadius : innerRadius;

            float angle = (2.0f * Math::Pi * i / numVertices) - (Math::Pi / 2.0f);

            vertices[i].x = center.x + radius * std::cos(angle);
            vertices[i].y = center.y + radius * std::sin(angle);
        }

        for (int i = 0; i < numVertices; ++i) {
            const Vector2& startPoint = vertices[i];
            const Vector2& endPoint = vertices[(i + 1) % numVertices];

            DrawLine3D(startPoint, endPoint, z, color);
        }
    }

    void DrawWavingFlower(const Vector2& basePos, float z, float stemHeight, float time, uint32_t color) {
        // === 揺れの計算 ===
        float flowerTilt = std::sin(time * 1.0f) * 0.1f;

        // === 花の描画 ===

        float petalLength = 3.0f;
        float petalRadius = 2.0f;
        float centerRadius = 1.25f;

        Vector2 flowerCenter = { basePos.x, basePos.y + stemHeight + centerRadius };
        flowerCenter.x += std::sin(time * 1.0f) * 1.0f;
        flowerCenter.y;


        for (int i = 0; i < 5; ++i) {
            float angle = (2.0f * Math::Pi * i / 5.0f) + flowerTilt;
            Vector2 p2 = { flowerCenter.x + std::cos(angle) * petalRadius, flowerCenter.y + std::sin(angle) * petalRadius };
            Vector2 p3 = { flowerCenter.x + std::cos(angle) * petalLength, flowerCenter.y + std::sin(angle) * petalLength };
            DrawLine3D(p2, p3, z, color);
        }

        Vector2 underFlowerPoint;

        for (int i = 0; i < 8; ++i) {
            float angle1 = (2.0f * Math::Pi * i / 8.0f);
            float angle2 = (2.0f * Math::Pi * (i + 1) / 8.0f);
            Vector2 c1 = { flowerCenter.x + std::cos(angle1) * centerRadius, flowerCenter.y + std::sin(angle1) * centerRadius };
            Vector2 c2 = { flowerCenter.x + std::cos(angle2) * centerRadius, flowerCenter.y + std::sin(angle2) * centerRadius };
            DrawLine3D(c1, c2, z, color);
            if (i == 5) {
                underFlowerPoint = c2;
            }
        }

        // === 茎の描画 ===

        DrawLine3D(basePos, underFlowerPoint, z, color);

        // === 草の描画 ===
        float grassWidth = 4.0f;
        float grassHeight = 2.0f;
        float grassSwayXOffset = std::sin(time * 1.0f) * 0.5f;
        float grassSwayYOffset = std::sin(time * 1.0f) * 0.5f;

        Vector2 grassL_tip = { basePos.x - grassWidth / 2.0f + grassSwayXOffset, basePos.y + grassHeight + grassSwayYOffset };
        Vector2 grassR_tip = { basePos.x + grassWidth / 2.0f + grassSwayXOffset, basePos.y + grassHeight + grassSwayYOffset };

        DrawLine3D(basePos, grassL_tip, z, color);
        DrawLine3D(basePos, grassR_tip, z, color);
    }

    void DrawCrescentMoon(const Vector2& center,float z, float outerRadius, float thickness, float rotation, uint32_t color) {
        const int segments = 30; // 形状の滑らかさ (値を大きくすると滑らかになる)

        // 内側の円の中心のX座標オフセット。
        // thicknessが外側の円と内側の円の中心の距離になります。
        // このoffsetが小さいほど太く、大きいほど細い三日月になります。
        float innerCircleOffsetX = thickness;

        // 内側の円の半径は、外側の円と同じか、わずかに小さくする
        float innerRadius = outerRadius * 0.95f; // 例: 外側の95%の半径

        // 内側の円の中心座標を計算
        // outerRadiusの「方向」にinnerCircleOffsetXだけずらす
        Vector2 innerCircleCenter;
        innerCircleCenter.x = center.x + std::cos(rotation) * innerCircleOffsetX;
        innerCircleCenter.y = center.y + std::sin(rotation) * innerCircleOffsetX;

        // 三日月を構成する2つの円弧の頂点を格納するリスト
        std::vector<Vector2> pointsOuterArc;
        std::vector<Vector2> pointsInnerArc;

        // 描画する円弧の角度範囲を決定
        // 三日月は通常、半円よりも少し狭い範囲で構成されます
        // 例えば、-PI/2 から PI/2 (Y軸に沿った半円) の代わりに、
        // -PI/3 から PI/3 (より狭い範囲) など
        float arcStartAngle = rotation - Math::Pi / 2.0f; // 外側の円弧の開始角度 (例: -90度から)
        float arcEndAngle = rotation + Math::Pi / 2.0f; // 外側の円弧の終了角度 (例: +90度まで)
        // この角度範囲を調整することで三日月の「太さ」や「切り込み」の深さが変わります。
        // 例: rotation - PI * 0.4f から rotation + PI * 0.4f にすると少し細くなる

        // 外側の円弧の頂点を計算
        for (int i = 0; i <= segments; ++i) {
            float angle = arcStartAngle + (arcEndAngle - arcStartAngle) * ((float)i / segments);
            pointsOuterArc.push_back({
                center.x + std::cos(angle) * outerRadius,
                center.y + std::sin(angle) * outerRadius
                });
        }

        // 内側の円弧の頂点を計算
        // こちらも同じ角度範囲で、ずらした中心から描画する
        for (int i = 0; i <= segments; ++i) {
            float angle = arcStartAngle + (arcEndAngle - arcStartAngle) * ((float)i / segments);
            pointsInnerArc.push_back({
                innerCircleCenter.x + std::cos(angle) * innerRadius,
                innerCircleCenter.y + std::sin(angle) * innerRadius
                });
        }

        // 三日月の輪郭を描画
        for (size_t i = 0; i < segments; ++i) {
            DrawLine3D(pointsOuterArc[i], pointsOuterArc[i + 1],z, color); // 外側の弧
            DrawLine3D(pointsInnerArc[i], pointsInnerArc[i + 1],z, color); // 内側の弧
        }

        // 始点と終点を結んで三日月の形を閉じる
        DrawLine3D(pointsOuterArc[0], pointsInnerArc[0],z, color); // 三日月の尖った部分の片方
        DrawLine3D(pointsOuterArc[segments], pointsInnerArc[segments],z, color); // 三日月のもう一方の尖った部分

        // 中を塗りつぶすための線
        // 外側の弧と内側の弧の対応する点を結ぶ
        for (size_t i = 0; i <= segments; ++i) {
            DrawLine3D(pointsOuterArc[i], pointsInnerArc[i],z, color);
        }
    }


    bool IsKeyPressed(unsigned char keycode) {
        return input->IsKeyPressed(keycode);
    }

    bool IsKeyTrigger(unsigned char keycode) {
        return input->IsKeyTrigger(keycode);
    }

    bool IsKeyRelease(unsigned char keycode) {
        return input->IsKeyRelease(keycode);
    }

    bool IsMousePressed(int button) {
        return input->IsMousePressed(button);
    }

    bool IsMouseTrigger(int button) {
        return input->IsMouseTrigger(button);
    }

    bool IsMouseRelease(int button) {
        return input->IsMouseRelease(button);
    }

    Vector2 GetMousePosition() {
        auto point = input->GetMousePosition();
        return { float(point.x), float(kWindowHeight - point.y) };
    }

    Vector2 GetMouseMove() {
        return { float(input->GetMouseMoveX()), float(input->GetMouseMoveY()) };
    }

    float GetMouseWheel() {
        return float(input->GetMouseWheel());
    }

    const XINPUT_STATE& GetGamePadState() {
        return input->GetXInputState();
    }

    const XINPUT_STATE& GetGamePadPreState() {
        return input->GetPreXInputState();
    }

    size_t LoadAudio(const std::string& name) {
        return audio->SoundLoadWave(name.c_str());
    }

    size_t PlayAudio(size_t soundHandle, bool loop) {
        if (loop)
            return audio->SoundPlayLoopStart(soundHandle);
        return audio->SoundPlayWave(soundHandle);
    }

    void StopAudio(size_t playHandle) {
        audio->StopSound(playHandle);
    }

    size_t IsPlayAudio(size_t playHandle) {
        return audio->IsValidPlayHandle(playHandle);
    }
    void SetPitch(size_t soundHandle, float pitch) {
        audio->SetPitch(soundHandle, pitch);
    }
    void SetVolume(size_t soundHandle, float volume) {
        audio->SetValume(soundHandle, volume);
    }
    void SetFullScreen(bool fullScreen) {
        gameWindow->SetFullScreen(fullScreen);
    }
    void SwitchViewMode() {
        if (realWorld) {
            switch (viewMode)
            {
            default:
            case RealWorld::ViewMode::kMonitor:
                viewMode = RealWorld::ViewMode::kBoard;
                realWorld->SetViewMode(RealWorld::ViewMode::kBoard);
                break;
            case RealWorld::ViewMode::kBoard:
                viewMode = RealWorld::ViewMode::kMonitor;
                realWorld->SetViewMode(RealWorld::ViewMode::kMonitor);
                break;
            }
        }
    }
    void RequestQuit() {
        isEndRequest = true;
    }
}
