#include "HexagonSevenSegmentDisplay.h"

#include <string>    
#include <algorithm> 
#include <array>

#include "TOMATOsEngine.h"

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

void HexagonSevenSegmentDisplay::Draw(int num, const Vector2& pos, float scale , float spacing , uint32_t color)
{
	std::string numStr = std::to_string(std::abs(num));

	const int numDigits = static_cast<int>(numStr.length());
	if (numDigits == 0) return;

	const float totalWidth = (numDigits - 1) * spacing;

	Vector2 drawingStartPosition = pos;
	drawingStartPosition.x -= totalWidth *0.5f;

	Vector2 currentCharacterPosition = drawingStartPosition;
	const int verticesPerHexagon = 6;

	for (char c : numStr) {
		int digit = c - '0';

		std::vector<Vector2> baseVertices = GetNumberVertex(digit);

		for (size_t i = 0; i < baseVertices.size(); i += verticesPerHexagon) {

			std::array<Vector2, verticesPerHexagon> transformedCorners;
			for (int j = 0; j < verticesPerHexagon; ++j) {
				const Vector2& baseVertex = baseVertices[i + j];
				transformedCorners[j] = (baseVertex * scale) + currentCharacterPosition;
			}

			for (int j = 0; j < verticesPerHexagon; ++j) {
				const Vector2& start = transformedCorners[j];
				const Vector2& end = transformedCorners[(j + 1) % verticesPerHexagon];

				TOMATOsEngine::DrawLine3D({ start.x, start.y }, { end.x, end.y }, color);
			}
		}

		// 次の桁の描画位置へ移動
		currentCharacterPosition.x += spacing;
	}
}

void HexagonSevenSegmentDisplay::DrawWirefradme(int num, const Vector2& pos, float scale, float spacing, uint32_t color)
{
	std::string numStr = std::to_string(std::abs(num));

	const int numDigits = static_cast<int>(numStr.length());
	if (numDigits == 0) return;
	const float totalWidth = (numDigits - 1) * spacing;
	Vector2 drawingStartPosition = pos;
	drawingStartPosition.x -= totalWidth / 2.0f;

	Vector2 currentCharacterPosition = drawingStartPosition;

	for (char c : numStr) {
		int digit = c - '0';

		const auto& required_segments = numberMap_.at(digit);

		for (char segment_id : required_segments) {
			const Vector2& segmentCenter = segmentCenters_.at(segment_id);
			Vector2 localStart, localEnd;

			if (segment_id == 'b' || segment_id == 'c' || segment_id == 'e' || segment_id == 'f') {
				localStart = { 0.0f, -3.0f };
				localEnd = { 0.0f, 3.0f };
			}
			else {
				localStart = { -3.0f, 0.0f };
				localEnd = { 3.0f, 0.0f };
			}

			Vector2 finalStart = ((localStart + segmentCenter) * scale) + currentCharacterPosition;
			Vector2 finalEnd = ((localEnd + segmentCenter) * scale) + currentCharacterPosition;

			TOMATOsEngine::DrawLine3D({ finalStart.x, finalStart.y, 0.0f }, { finalEnd.x, finalEnd.y, 0.0f }, color);
		}

		currentCharacterPosition.x += spacing;
	}
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