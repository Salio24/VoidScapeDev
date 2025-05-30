#version 460 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec4 a_Color;

out vec2 v_Position;
out vec4 v_Color;

uniform mat4 u_ViewProjection;
uniform mat4 u_ModelMatrix;

void main() {
    v_Position = a_Position;
    v_Color = a_Color;
    gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 0.0, 1.0);
}