precision mediump float;

#if __VERSION__ >= 300
in vec2 v_texCoord;
out vec4 fragColor;
#define TEXTURE2D texture
#elif __VERSION__ >= 100
varying vec2 v_texCoord;
vec4 fragColor;
#define TEXTURE2D texture2D
#endif

uniform float u_alpha;
uniform vec4 u_tint;
uniform sampler2D u_texture;

void main(void) { 
	// fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	// return;

	vec4 dataFrag = TEXTURE2D(u_texture, v_texCoord/4.0);

	fragColor.r = dataFrag.r;
	fragColor.b = dataFrag.r;
	fragColor.g = dataFrag.r;
	fragColor.g = dataFrag.r;
	fragColor = (u_tint - fragColor)*u_tint.a + fragColor;
	fragColor.rgb /= fragColor.a;
	fragColor.a -= 1.0-u_alpha;

#if __VERSION__ == 100
	gl_FragColor = fragColor;
#endif
}
