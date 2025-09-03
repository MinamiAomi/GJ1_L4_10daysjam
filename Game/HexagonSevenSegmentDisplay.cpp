#include "HexagonSevenSegmentDisplay.h"

HexagonSevenSegmentDisplay* HexagonSevenSegmentDisplay::GetInstance()
{
	static HexagonSevenSegmentDisplay instance;
	return &instance;
}

HexagonSevenSegmentDisplay::HexagonSevenSegmentDisplay()
{
	numberMap_ = {
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

std::vector<Vector2> HexagonSevenSegmentDisplay::GetNumberVertex(int num)
{
	if (numberMap_.find(num) == numberMap_.end()) {
		throw std::out_of_range("Input number must be between 0 and 9.");
	}

	std::vector<Vector2> all_vertices;
	const auto& required_segments = numberMap_.at(num);

	// 必要なセグメントの頂点を一つずつ生成し、リストにまとめていく
	for (char segment_id : required_segments) {
		auto segment_vertices = GenerateSegmentVertices(segment_id);
		all_vertices.insert(all_vertices.end(), segment_vertices.begin(), segment_vertices.end());
	}

	return all_vertices;
}

std::vector<Vector2> HexagonSevenSegmentDisplay::GenerateSegmentVertices(char id)
{
	std::vector<Vector2> transformedVertices;

	const Vector2& center = segmentCenters_.at(id);

	for (const auto& base_vertex : hexagonVertices_) {
		Vector2 transformed_vertex = base_vertex;

		// b, c, e, f セグメントは90度回転させる
		if (id == 'b' || id == 'c' || id == 'e' || id == 'f') {
			float new_x = -base_vertex.y;
			float new_y = base_vertex.x;
			transformed_vertex = { new_x, new_y };
		}

		transformedVertices.push_back(transformed_vertex + center);
	}
	return transformedVertices;
}