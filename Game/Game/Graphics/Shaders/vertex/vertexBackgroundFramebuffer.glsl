#version 460 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texCoords;

out vec2 vTexCoords;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
	vTexCoords = texCoords;
}
