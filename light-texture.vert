
#version 330

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;
// layout (location = 2) in vec2 a_texcoord;


uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;

out vec4 vertex;
out vec3 normal;
// out vec2 st;

void main(void){
	// st = a_texcoord;
	vertex = view_matrix * model_matrix * vec4(a_vertex, 1.0);
	normal = normalize(normal_matrix * a_normal);
	gl_Position = projection_matrix * vertex;
}
