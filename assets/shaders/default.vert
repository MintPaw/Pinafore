#if __VERSION__ >= 300
# define IN in
# define OUT out
#elif __VERSION__ >= 100
# define IN attribute
# define OUT varying
#endif

IN vec2 a_position;
IN vec2 a_texCoord;
OUT vec2 v_texCoord;

uniform mat3 u_matrix;
uniform mat3 u_uv;

void main(void) {
	v_texCoord = vec4((u_uv * vec3(a_texCoord, 1)).xy, 0, 1).xy;
	gl_Position = vec4((u_matrix * vec3(a_position, 1)).xy, 0, 1);
}

