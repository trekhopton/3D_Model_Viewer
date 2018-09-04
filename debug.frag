
#version 330

in vec3 normal;

uniform vec3 mtl_diffuse;

out vec4 fragColour;

uniform int mode;

void main(void){
	//change the ouput colour for different debug modes
	if(mode == 1){
		fragColour = vec4(0.9, 0.9, 0.3, 1.0);
	} else if(mode == 2) {
		fragColour = vec4(normal, 1.0);
	} else {
		fragColour = vec4(mtl_diffuse, 1.0);
	}
}
