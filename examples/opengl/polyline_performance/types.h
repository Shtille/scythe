#pragma once

#include <vector>
#include <array>
#include <cstdint>

typedef std::array<float, 2> Point;
typedef std::vector<Point> PointArray;

typedef std::array<float, 3> Point3D;
typedef std::vector<Point3D> Point3DArray;

typedef std::array<float, 4> Color;

typedef std::vector<uint32_t> IndicesArray;

struct Viewport
{
	int width;
	int height;
	float aspect_ratio;
};