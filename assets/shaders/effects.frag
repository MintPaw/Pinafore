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
uniform bool u_invert;

void main(void) { 
	// fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	// return;

	vec4 tex = TEXTURE2D(u_texture, v_texCoord);

	fragColor = tex;
	fragColor.rgb /= fragColor.a;

	if (u_invert) {
		fragColor.x = 1.0-fragColor.x;
		fragColor.y = 1.0-fragColor.y;
		fragColor.z = 1.0-fragColor.z;
	}

#if __VERSION__ == 100
	gl_FragColor = fragColor;
#endif
}
