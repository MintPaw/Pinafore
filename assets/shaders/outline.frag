precision highp float;

#if __VERSION__ >= 300
in vec2 v_texCoord;
out vec4 fragColor;
#define TEXTURE2D texture
#elif __VERSION__ >= 100
varying vec2 v_texCoord;
vec4 fragColor;
#define TEXTURE2D texture2D
#endif

uniform sampler2D u_texture;
uniform vec2 u_viewportInverse;
uniform float u_outlineSize;
uniform vec4 u_outlineColour;

void main(void) { 
	float alpha = 0.0;
	bool allIn = true;

	float ALPHA_VALUE_BORDER = 0.5;
	float CONVOLUTION_STEP_SIZE = 1.0;

#if 0
	vec2 offsets[9];
	int offsetsNum = 0;
	offsets[offsetsNum++] = vec2(-CONVOLUTION_STEP_SIZE, CONVOLUTION_STEP_SIZE);
	offsets[offsetsNum++] = vec2(-CONVOLUTION_STEP_SIZE, 0.0);
	offsets[offsetsNum++] = vec2(-CONVOLUTION_STEP_SIZE, -CONVOLUTION_STEP_SIZE);
	offsets[offsetsNum++] = vec2(0.0, CONVOLUTION_STEP_SIZE);
	offsets[offsetsNum++] = vec2(0.0, 0.0);
	offsets[offsetsNum++] = vec2(0.0, -CONVOLUTION_STEP_SIZE);
	offsets[offsetsNum++] = vec2(CONVOLUTION_STEP_SIZE, CONVOLUTION_STEP_SIZE);
	offsets[offsetsNum++] = vec2(CONVOLUTION_STEP_SIZE, 0.0);
	offsets[offsetsNum++] = vec2(CONVOLUTION_STEP_SIZE, -CONVOLUTION_STEP_SIZE);

	for (int i = 0; i < offsetsNum; i++) {
		float newAlpha = TEXTURE2D(u_texture, v_texCoord + offsets[i]*u_viewportInverse).a;
		allIn = allIn && newAlpha > ALPHA_VALUE_BORDER;
		if (newAlpha > ALPHA_VALUE_BORDER && newAlpha >= alpha) alpha = newAlpha;
	}
#else
	for (float ix = -u_outlineSize; ix <= u_outlineSize; ix += CONVOLUTION_STEP_SIZE) {
		for (float iy = -u_outlineSize; iy <= u_outlineSize; iy += CONVOLUTION_STEP_SIZE) {
			float newAlpha = TEXTURE2D(u_texture, v_texCoord + vec2(ix, iy) * u_viewportInverse).a;
			allIn = allIn && newAlpha > ALPHA_VALUE_BORDER;
			if (newAlpha > ALPHA_VALUE_BORDER && newAlpha >= alpha) alpha = newAlpha;
		}
	}
#endif

	if (allIn) alpha = 0.0;

	fragColor = u_outlineColour;
	fragColor.a *= alpha;

#if __VERSION__ == 100
	gl_FragColor = fragColor;
#endif
}
