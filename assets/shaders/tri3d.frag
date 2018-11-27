precision highp float;

#if __VERSION__ >= 300
# define IN in
# define OUT out
# define TEXTURE2D texture
#elif __VERSION__ >= 100
# define IN attribute
# define OUT
# define TEXTURE2D texture2D
#endif

IN vec2 v_texCoord;

OUT vec4 fragColor;

uniform sampler2D u_texture;

void main(void) { 
	// u_texture;
	// fragColor = vec4(1.0, 0.5, 0.2, 1.0+v_texCoord.x);
	fragColor = TEXTURE2D(u_texture, v_texCoord);

#if __VERSION__ == 100
	gl_FragColor = fragColor;
#endif
}
