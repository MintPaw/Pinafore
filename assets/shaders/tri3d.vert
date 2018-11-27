#if __VERSION__ >= 300
# define IN in
# define OUT out
#elif __VERSION__ >= 100
# define IN attribute
# define OUT varying
#endif

IN vec3 a_position;
IN vec2 a_texCoord;

OUT vec2 v_texCoord;

void main() {
	gl_Position = vec4(a_position.xyz, 1.0);

	v_texCoord = a_texCoord;
}
