#version 460 core

out vec4 FragColor;

in vec4 vVertexColor;
in vec2 vTexturePosition;
flat in int vTextureIndex;

uniform sampler2DArray uTextures;

void main() {
	vec3 texCoords = vec3(vTexturePosition, vTextureIndex); // Encode TexIndex as the z-coordinate
    FragColor = texture(uTextures, texCoords) * vVertexColor;
}