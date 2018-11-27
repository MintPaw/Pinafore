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

void main(void) {
	v_texCoord.x = a_texCoord.x;
	v_texCoord.y = 1.0 - a_texCoord.y;
	gl_Position = vec4((u_matrix * vec3(a_position, 1)).xy, 0, 1);
}

