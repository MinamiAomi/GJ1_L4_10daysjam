#include "HexagonSevenSegmentDisplay.h"

HexagonSevenSegmentDisplay* HexagonSevenSegmentDisplay::GetInstance()
{
	static HexagonSevenSegmentDisplay instance;
	return &instance;
}

std::vector<Vector2> HexagonSevenSegmentDisplay::GetNumberVertex(int num)
{
    if (number_map_.find(num) == number_map_.end()) {
        throw std::out_of_range("Input number must be between 0 and 9.");
    }

    std::vector<Vector2> all_vertices;
    const auto& required_segments = number_map_.at(num);

    for (char segment_id : required_segments) {
        auto segment_vertices = GenerateSegmentVertices(segment_id);
        // 取得した頂点を全体のリストに追加
        all_vertices.insert(all_vertices.end(), segment_vertices.begin(), segment_vertices.end());
    }

    return all_vertices;
}

std::vector<Vector2> HexagonSevenSegmentDisplay::GenerateSegmentVertices(char id)
{
    std::vector<Vector2> transformedVertices;
    const Vector2& center = segment_centers_.at(id);

    for (const auto& baseVertex : base_hexagon_vertices_) {
        Vector2 transformedVertex = baseVertex;

        // b, c, e, f セグメントは90度回転させる
        if (id == 'b' || id == 'c' || id == 'e' || id == 'f') {
            // 90度回転の公式: x' = -y, y' = x
            float new_x = -baseVertex.y;
            float  new_y = baseVertex.x;
            transformedVertex = { new_x, new_y };
        }

        // セグメントの中心座標へ平行移動
        transformedVertices.push_back(transformedVertex + center);
    }
    return transformedVertices;
}
