#version 460 core
layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 uProjection;
uniform mat4 uModelMatrix;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    gl_Position = uProjection * uModelMatrix * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}