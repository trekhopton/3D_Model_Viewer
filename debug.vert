
#version 330

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;

out vec3 normal;

void main(void){
	vec4 vertex = view_matrix * model_matrix * vec4(a_vertex, 1.0);
	normal = normalize(normal_matrix * a_normal);
	gl_Position = projection_matrix * vertex;
}
