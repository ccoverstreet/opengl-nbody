#version 430 core
layout (location = 0) in vec2 verts;
layout (location = 1) in vec4 pdata;
layout (location = 2) in float charge;
layout (location = 3) in float radius;

out vec3 color;
uniform vec2 camPos;
uniform float camZoom;
uniform float aspectRatio;

void main() {
	color = vec3((charge + 4) / 8, (-charge  +4) / 8, 1.0);
	vec2 temp = camZoom * (0.9 * radius * 2.0 * verts + vec2(pdata.x, pdata.y) - camPos);
	gl_Position = vec4(temp.x/aspectRatio, temp.y, 0.0, 1.0);
	//gl_Position = vec4(verts, 0, 0);
}
