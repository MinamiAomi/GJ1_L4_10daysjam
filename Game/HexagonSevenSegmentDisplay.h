#pragma once

#include <stdexcept>
#include <map>
#include <vector>

#include "Math/MathUtils.h"

class HexagonSevenSegmentDisplay {
public:
	static HexagonSevenSegmentDisplay* GetInstance();

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
	std::vector<Vector2> GetNumberVertex(int num, const Vector2& position);

	// 指定されたセグメントIDの六角形の頂点リストを生成
	std::vector<Vector2> GenerateSegmentVertices(char id, const Vector2& position);
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
};