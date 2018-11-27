#if __VERSION__ >= 300
# define IN in
# define OUT out
#elif __VERSION__ >= 100
# define IN attribute
# define OUT varying
#endif

IN vec2 a_position;
OUT vec2 v_texCoord;
OUT vec2 v_blurTexCoordsOffset[11];

uniform mat3 u_matrix;
uniform vec2 u_imageSize;

void main(void) {
	gl_Position = vec4((u_matrix * vec3(a_position, 1)).xy, 0, 1);

	vec2 centerTexCoords = gl_Position.xy;
	centerTexCoords = centerTexCoords * 0.5 + 0.5;
	float pixelSize = 1.0/u_imageSize.x;

	for (int i = -5; i <= 5; i++) {
		v_blurTexCoordsOffset[i+5] = centerTexCoords + vec2(pixelSize * float(i), 0.0);
	}
}

