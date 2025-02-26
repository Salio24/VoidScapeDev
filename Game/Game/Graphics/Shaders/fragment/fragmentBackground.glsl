#version 460 core

out vec4 FragColor;

in vec4 vVertexColor;
in vec2 vTexturePosition;
flat in int vLevelBackground;
flat in int vTextureIndex;

uniform sampler2D uTexture;

uniform sampler2DArray uTextureArray;

void main() {
	if (vLevelBackground == 0) {
		FragColor = texture(uTexture, vTexturePosition) * vVertexColor; 
	} else if (vLevelBackground == 1) {		
		vec3 texCoords = vec3(vTexturePosition, vTextureIndex); // Encode TexIndex as the z-coordinate
		FragColor = texture(uTextureArray, texCoords) * vVertexColor;
	}
} 