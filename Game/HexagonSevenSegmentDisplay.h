#pragma once

#include <stdexcept>
#include <map>
#include <vector>

#include "Math/MathUtils.h"

class HexagonSevenSegmentDisplay {
public:
	static HexagonSevenSegmentDisplay* GetInstance();

	//数字の頂点を取得
	std::vector<Vector2> GetNumberVertex(int num);
private:
	HexagonSevenSegmentDisplay();
	HexagonSevenSegmentDisplay(const HexagonSevenSegmentDisplay&) = delete;
	HexagonSevenSegmentDisplay& operator=(const HexagonSevenSegmentDisplay&) = delete;


	std::vector<Vector2> GenerateSegmentVertices(char id);

	// 六角形の基準となる頂点
	const std::vector<Vector2> hexagonVertices_ = {
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
	const std::map<char, Vector2> segmentCenters_ = {
		{'a', {0, 6}},   {'b', {4, 3}},   {'c', {4, -3}},
		{'d', {0, -6}},  {'e', {-4, -3}}, {'f', {-4, 3}},
		{'g', {0, 0}}
	};

	// 数字と使用セグメントのマッピング
	std::map<int, std::vector<char>> numberMap_;
};