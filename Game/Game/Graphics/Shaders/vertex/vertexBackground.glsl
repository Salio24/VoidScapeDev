#version 460 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 texturePosition;
layout(location = 3) in int textureIndex;

out vec4 vVertexColor;
out highp vec2 vTexturePosition;
out int vLevelBackground;
flat out int vTextureIndex;

uniform mat4 uModelMatrix;
uniform mat4 uProjectionMatrix;
uniform int uLevelBackground;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {

	vLevelBackground = uLevelBackground;
	vVertexColor = vertexColor;
	vTexturePosition = texturePosition;
	vTextureIndex = textureIndex;

	gl_Position = uProjectionMatrix * uModelMatrix * vec4(position, 0.0, 1.0);
}