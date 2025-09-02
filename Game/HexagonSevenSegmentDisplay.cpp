#include "HexagonSevenSegmentDisplay.h"

HexagonSevenSegmentDisplay* HexagonSevenSegmentDisplay::GetInstance()
{
	static HexagonSevenSegmentDisplay instance;
	return &instance;
}

std::vector<Vector2> HexagonSevenSegmentDisplay::GetNumberVertex(int num, const Vector2& position)
{
	if (number_map_.find(num) == number_map_.end()) {
		throw std::out_of_range("Input number must be between 0 and 9.");
	}

	std::vector<Vector2> all_vertices;
	const auto& required_segments = number_map_.at(num);

	for (char segment_id : required_segments) {
		auto segment_vertices = GenerateSegmentVertices(segment_id,position);
		// 取得した頂点を全体のリストに追加
		all_vertices.insert(all_vertices.end(), segment_vertices.begin(), segment_vertices.end());
	}

	return all_vertices;
}

std::vector<Vector2> HexagonSevenSegmentDisplay::GenerateSegmentVertices(char id, const Vector2& position)
{// この関数はクラス外にあるため、六角形の基本形状を独自に定義します
	const std::vector<Vector2> base_hexagon_vertices = {
		{-3, 0}, {-2, 1}, {2, 1}, {3, 0}, {2, -1}, {-2, -1}
	};

	std::vector<Vector2> transformedVertices;

	for (const auto& base_vertex : base_hexagon_vertices) {
		Vector2 transformed_vertex = base_vertex;

		// b, c, e, f セグメントは90度回転させる
		if (id == 'b' || id == 'c' || id == 'e' || id == 'f') {
			// 90度回転の公式: x' = -y, y' = x
			float new_x = -base_vertex.y;
			float new_y = base_vertex.x;
			transformed_vertex = { new_x, new_y };
		}

		// 引数で与えられた中心座標へ平行移動
		transformedVertices.push_back(transformed_vertex + position);
	}
	return transformedVertices;
}
