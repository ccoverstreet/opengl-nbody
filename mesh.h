#include <cstdint>
#include <vector>
#include <cstdio>

struct Vertex {
	float x;
	float y;
};

struct Triangle {
	uint32_t a;
	uint32_t b;
	uint32_t c;
};

struct Mesh {
	std::vector<Vertex> verts;
	std::vector<Triangle> elems;
};

// n_points is the number of points used to create the outside curve of the circle
Mesh create_circle_mesh(float radius, size_t n_points);
Mesh create_triangle_mesh();
