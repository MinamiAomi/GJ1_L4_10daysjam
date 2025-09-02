#pragma once

#include <stdexcept>
#include <map>
#include <vector>

#include "Math/MathUtils.h"

class HexagonSevenSegmentDisplay {
public:
    HexagonSevenSegmentDisplay() {
        // 各数字がどのセグメントを使用するかを定義
        number_map_ = {
            {0, {'a', 'b', 'c', 'd', 'e', 'f'}},
            {1, {'b', 'c'}},
            {2, {'a', 'b', 'g', 'e', 'd'}},
            {3, {'a', 'b', 'g', 'c', 'd'}},
            {4, {'f', 'g', 'b', 'c'}},
            {5, {'a', 'f', 'g', 'c', 'd'}},
            {6, {'a', 'f', 'g', 'e', 'c', 'd'}},
            {7, {'a', 'b', 'c'}},
            {8, {'a', 'b', 'c', 'd', 'e', 'f', 'g'}},
            {9, {'a', 'b', 'c', 'd', 'f', 'g'}}
        };
    }

    //数字の頂点を取得
    std::vector<Vector2> GetNumberVertex(int num) {
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
private:
    // 六角形の基準となる頂点
    const std::vector<Vector2> base_hexagon_vertices_ = {
        {-3, 0}, {-2, 1}, {2, 1}, {3, 0}, {2, -1}, {-2, -1}
    };

    //    a
    //   ---
    //f | g | b
    //   ---
    // e|   |c
    //   ---
    //    d
    // 各セグメントの中心座標
    const std::map<char, Vector2> segment_centers_ = {
        {'a', {0, 6}},   {'b', {4, 3}},   {'c', {4, -3}},
        {'d', {0, -6}},  {'e', {-4, -3}}, {'f', {-4, 3}},
        {'g', {0, 0}}
    };

    // 数字と使用セグメントのマッピング
    std::map<int, std::vector<char>> number_map_;

    // 指定されたセグメントIDの六角形の頂点リストを生成
    std::vector<Vector2> GenerateSegmentVertices(char id) {
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
};