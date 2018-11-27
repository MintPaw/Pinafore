precision highp float;

#if __VERSION__ >= 300
in vec2 v_blurTexCoordsOffset[11];
out vec4 fragColor;
#define TEXTURE2D texture
#elif __VERSION__ >= 100
varying vec2 v_blurTexCoordsOffset[11];
vec4 fragColor;
#define TEXTURE2D texture2D
#endif

uniform sampler2D u_texture;

void main(void) { 
	fragColor = vec4(0.0);
	fragColor += TEXTURE2D(u_texture, v_blurTexCoordsOffset[0]) * 0.0093;
	fragColor += TEXTURE2D(u_texture, v_blurTexCoordsOffset[1]) * 0.028002;
	fragColor += TEXTURE2D(u_texture, v_blurTexCoordsOffset[2]) * 0.065984;
	fragColor += TEXTURE2D(u_texture, v_blurTexCoordsOffset[3]) * 0.121703;
	fragColor += TEXTURE2D(u_texture, v_blurTexCoordsOffset[4]) * 0.175713;
	fragColor += TEXTURE2D(u_texture, v_blurTexCoordsOffset[5]) * 0.198596;
	fragColor += TEXTURE2D(u_texture, v_blurTexCoordsOffset[6]) * 0.175713;
	fragColor += TEXTURE2D(u_texture, v_blurTexCoordsOffset[7]) * 0.121703;
	fragColor += TEXTURE2D(u_texture, v_blurTexCoordsOffset[8]) * 0.065984;
	fragColor += TEXTURE2D(u_texture, v_blurTexCoordsOffset[9]) * 0.028002;
	fragColor += TEXTURE2D(u_texture, v_blurTexCoordsOffset[10]) * 0.0093;
	// fragColor.rgb /= fragColor.a;

#if __VERSION__ == 100
	gl_FragColor = fragColor;
#endif
}
