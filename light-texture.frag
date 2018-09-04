
#version 330

in vec4 vertex; //FragPos
in vec3 normal;
// in vec2 st;

out vec4 fragColour;

uniform vec3 light_pos;

uniform vec3 light_ambient;     // Light ambient RGBA values
uniform vec3 light_diffuse;     // Light diffuse RGBA values
uniform vec3 light_specular;    // Light specular RGBA values

uniform vec3 mtl_ambient;  // Ambient surface colour
uniform vec3 mtl_diffuse;  // Diffuse surface colour
uniform vec3 mtl_specular; // Specular surface colour

// uniform sampler2D diffMap;

const float shininess = 32;

vec3 BPPoint(in vec4 position, in vec3 norm){

	// vec3 ambient = light_ambient * mtl_ambient;

	// vec3 lightDir = normalize(vec3(light_pos - position));
	// vec3 diffuse = max(dot(lightDir, norm), 0.0) * mtl_diffuse;

	// vec3 viewDir = normalize(view_pos - vec3(position));
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = 0.0;
	// vec3 halfwayDir = normalize(lightDir + viewDir);  
	// spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    // vec3 specular = vec3(0.3) * spec; // assuming bright white light color
    // return ambient + diffuse + specular;

	vec3 s = normalize(light_pos - vec3(position));
    vec3 v = normalize(-position.xyz);
    vec3 r = reflect(-s, norm);

    vec3 ambient = light_ambient * mtl_ambient;

    // The diffuse component
    float sDotN = max(dot(s,norm), 0.0);
    //vec3 diffuse = light_diffuse * vec3(texture(diffMap, st)) * sDotN;
    vec3 diffuse = light_diffuse * mtl_diffuse * sDotN;

    // The specular component
    vec3 spec = vec3(0.0);
    if (sDotN > 0.0)
		spec = light_specular * mtl_specular *
            pow( max(dot(r,v), 0.0), shininess);

    return ambient + diffuse + spec;
}

void main(void){
	fragColour = vec4(BPPoint(vertex, normalize(normal)), 1.0);
}