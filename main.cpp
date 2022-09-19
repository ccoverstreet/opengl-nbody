#include <SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "./util.h"
#include "./mesh.h"

struct AppState {
	bool quit;
	bool move_left;
	bool move_right;
	bool move_up;
	bool move_down;
	bool zoom_in;
	bool zoom_out;
	bool clear_screen;
	bool is_paused;
	bool increase_speed;
	bool decrease_speed;

	AppState() {
		quit = false;		
		move_left = false;
		move_right = false;
		move_up = false;
		move_down = false;
		zoom_in = false;
		zoom_in = false;
		clear_screen = true;
		is_paused = false;
		increase_speed = false;
		decrease_speed = false;
	}
};

void pollEvents(AppState &state) {
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			state.quit = true;	
		} else if (e.type == SDL_KEYDOWN || e.type==SDL_KEYUP) {
			switch(e.key.keysym.sym) {
				case SDLK_q:
					state.quit = true;
					break;

				case SDLK_a:
					state.move_left = e.type == SDL_KEYDOWN;
					break;

				case SDLK_d:
					state.move_right = e.type == SDL_KEYDOWN;
					break;

				case SDLK_w:
					state.move_up = e.type == SDL_KEYDOWN;
					break;

				case SDLK_s:
					state.move_down = e.type == SDL_KEYDOWN;
					break;

				case SDLK_EQUALS:
					state.zoom_in = e.type == SDL_KEYDOWN;
					break;

				case SDLK_MINUS:
					state.zoom_out = e.type == SDL_KEYDOWN;
					break;

				case SDLK_SPACE:
					state.clear_screen ^= (e.type == SDL_KEYDOWN);
					break;

				case SDLK_p:
					state.is_paused ^= (e.type == SDL_KEYDOWN);
					break;

				case SDLK_2:
					state.increase_speed = (e.type == SDL_KEYDOWN);
					break;

				case SDLK_1:
					state.decrease_speed = (e.type == SDL_KEYDOWN);
					break;
			}

		}
	}
}

struct Performance {
	float last_time;
	size_t frame_count;
};

struct Particle {
	glm::vec2 p;
	glm::vec2 v;
	float charge;
	float radius;
};

void init_particles(std::vector<Particle> &parts) {
	float p_min = -20;
	float p_max = 20;
	float v_min = -1;		
	float v_max = 1;

	for (auto &p : parts) {
		p.p.x = (float(rand()) / RAND_MAX) * (p_max - p_min) + p_min;
		p.p.y = (float(rand()) / RAND_MAX) * (p_max - p_min) + p_min;
		p.v.x = (float(rand()) / RAND_MAX) * (v_max - v_min) + v_min;
		p.v.y = (float(rand()) / RAND_MAX) * (v_max - v_min) + v_min;
		if (float(rand()) / RAND_MAX < 0.3333)  {
			p.charge = 4;
			p.radius = 1;
		} else {
			p.charge = -2;
			p.radius = 0.2;
		}
	}
}

void physics(std::vector<Particle> &parts, float delta) {
	//const float friction = 0.9;
	const float friction = 0.95;
	float k = 100.0;
	float thermal = 100.0;
	float pressure = 0.1;

	//delta = delta * delta_scale;

#pragma omp parallel for
	for (size_t i = 0; i < parts.size(); i++) {
		auto &p1 = parts[i];
		for (size_t j = 0; j < parts.size(); j++) {
			if (j == i) continue;
			auto &p2 = parts[j];

			auto dp = p2.p - p1.p;
			auto r_sq = glm::dot(dp, dp);


			dp = p2.p - p1.p;
			r_sq = glm::dot(dp, dp);
			float r = sqrt(r_sq);

			auto normed = dp / r;

			// Particles intersecting
			float r_0 = p1.radius + p2.radius;
			if (r < r_0) {
				float shift = 0.5 * (r_0 - r);
			
				p1.p += -shift * normed;
				p2.p += shift * normed;
			}

			p1.v += (-1 / (r * r * r) + -k *p1.charge * p2.charge/ (r * r)
					+ pressure) * normed * delta;
		}

		float tx = thermal * (float(rand()) / RAND_MAX - 0.5) * delta;
		float ty = thermal * (float(rand()) / RAND_MAX - 0.5) * delta;

		p1.v = p1.v * friction + glm::vec2(tx, ty);
	}

	for (auto &p : parts) {
		p.p += p.v * delta;
	}
}

int main() {
	srand(SDL_GetPerformanceCounter());

	const int WIDTH = 1600;
	const int HEIGHT = 800;

	SDL_Window *window = SDL_CreateWindow("OpenGL Simple",
			0, 0, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GLContext context = SDL_GL_CreateContext(window);
	glewInit();
	SDL_GL_SetSwapInterval(1);
	glViewport(0, 0, WIDTH, HEIGHT);

	float sprite[] {
		-0.5, -0.5, 
		0.5, -0.5,
		0.0, 0.5,
		0.0, 0.5,
		1.0, 0.5, 
		1.0, -0.5,
	};

	float sprite_scale = 1.0;

	std::vector<Particle> particles(1000);
	init_particles(particles);

	/*
	for (auto &p : particles) {
		printf("%f %f %f %f\n", p.p.x, p.p.y, p.v.x, p.v.y);
	}
	*/

	Mesh circ_mesh = create_circle_mesh(0.5, 100);
	//Mesh circ_mesh = create_triangle_mesh();

	size_t index = 0;
	for (auto &v : circ_mesh.verts) {
		printf("(%u) %f %f\n", index++, v.x, v.y);
	}

	for (auto &e : circ_mesh.elems) {
		printf("%u %u %u\n", e.a, e.b, e.c);
	}

	uint32_t shader_prog = create_shader_program("basic.vert", "basic.frag");

	uint32_t vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Vertices
	uint32_t vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
			circ_mesh.verts.size() * sizeof(Vertex),
			&circ_mesh.verts[0],
			GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  

	// Element buffer
	uint32_t ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			circ_mesh.elems.size() * sizeof(Triangle),
			&circ_mesh.elems[0],
			GL_STATIC_DRAW);


	// Particle data
	uint32_t pbo;
	glGenBuffers(1, &pbo);
	glBindBuffer(GL_ARRAY_BUFFER, pbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * particles.size(), &particles[0], GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);  
	glEnableVertexAttribArray(2);  
	glEnableVertexAttribArray(3);  
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
	glVertexAttribDivisor(2, 1);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	glVertexAttribDivisor(3, 1);

	float camZoom = 0.05;
	glm::vec2 camPos = {0, 0};
	float move_speed = 0.04;

	float sim_speed = 0.1;

	auto STATE = AppState();
	auto perf = Performance{0, 0};

	double prev = SDL_GetPerformanceCounter() / 1e9;
	while (!STATE.quit) {
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		float aspect_ratio = float(w) / h;
		glViewport(0, 0, w, h);

		//printf("%d, %d\n", w, h);

		pollEvents(STATE);

		glClearColor(0, 0, 0, 0);
		if (STATE.clear_screen) glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_prog);
		glBindVertexArray(vao);

		if (STATE.move_left) 
			camPos.x -= move_speed / camZoom;
		if (STATE.move_right) 
			camPos.x += move_speed / camZoom;
		if (STATE.move_up) 
			camPos.y += move_speed / camZoom;
		if (STATE.move_down) 
			camPos.y -= move_speed / camZoom;

		if (STATE.zoom_in)
			camZoom *= 1.05;
		if (STATE.zoom_out)
			camZoom *= 0.95;
		
		if (STATE.increase_speed)
			sim_speed *= 1/0.99;
		if (STATE.decrease_speed) 
			sim_speed *= 0.99;

		uint32_t camZoomUni = glGetUniformLocation(shader_prog, "camZoom");
		glUniform1f(camZoomUni, camZoom);

		uint32_t camPosUni = glGetUniformLocation(shader_prog, "camPos");
		glUniform2fv(camPosUni, 1, glm::value_ptr(camPos));

		uint32_t aspectRatioUni = glGetUniformLocation(shader_prog, "aspectRatio");
		glUniform1f(aspectRatioUni, aspect_ratio);

		//glDrawArrays(GL_TRIANGLES,0, 3);
		//glDrawElements(GL_LINE_LOOP, 3*circ_mesh.elems.size(), GL_UNSIGNED_INT, 0);
		//glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 10);
		glDrawElementsInstanced(GL_TRIANGLES, 3*circ_mesh.elems.size(), GL_UNSIGNED_INT, (void*)0, particles.size());
		SDL_GL_SwapWindow(window);

		perf.frame_count++;
		if (perf.frame_count >= 100) {
			double this_time = double(SDL_GetPerformanceCounter()) / 1e9;

			printf("FPS: %f\n", perf.frame_count / (this_time - perf.last_time));

			perf.last_time = this_time;
			perf.frame_count = 0;
		}

		double now = SDL_GetPerformanceCounter() / 1e9;
		double delta = now - prev;
		prev =  now;

		if (!STATE.is_paused) 
			physics(particles, delta * sim_speed);

		glNamedBufferSubData(pbo, 0, sizeof(Particle)*particles.size(), &particles[0]);

		//SDL_Delay(1000);
	}

	glewInit();

	return 0;
}
