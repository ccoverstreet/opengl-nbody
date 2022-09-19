#include "./mesh.h"

#include <cmath>

Mesh create_circle_mesh(float radius, size_t n_points) {
	std::vector<Vertex> verts(n_points + 1); // curve points + center
	std::vector<Triangle> elems(n_points);

	verts[0] = {0, 0};

	float dtheta = 2 * M_PI / n_points;
	for (size_t i = 0; i < n_points; i++) {
		float x = radius * cos(dtheta*i-1);
		float y = radius * sin(dtheta*i-1);
		verts[i+1] = Vertex{x, y};
	}

	for (size_t i = 0; i < n_points; i++) {
		uint32_t start = (i) % (n_points) + 1;
		uint32_t end = (i+1) % (n_points) + 1;
		elems[i] = {0, start, end};
	}

	return Mesh{verts, elems};
}


Mesh create_triangle_mesh() {
	std::vector<Vertex> verts = {
		{-0.5, -0.5},
		{0.5, -0.5},
		{0, 0.5}
	};

	std::vector<Triangle> elems = {
		{0, 1, 2}
	};

	return Mesh{verts, elems};
}
