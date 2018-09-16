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

uniform float u_alpha;
uniform vec4 u_tint;
uniform sampler2D u_texture;

void main(void) { 
	// fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	// return;

	vec4 tex = TEXTURE2D(u_texture, v_texCoord);

	fragColor = mix(tex, u_tint, u_tint.a);
	fragColor.a = tex.a;

	fragColor.rgb /= fragColor.a;
	fragColor.a -= 1.0-u_alpha;

#if __VERSION__ == 100
	gl_FragColor = fragColor;
#endif
}
