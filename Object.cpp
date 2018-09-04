#define TINYOBJLOADER_IMPLEMENTATION

#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3
#define VALS_PER_TEX 2

#include "Object.h"

std::string Object::getBaseDir(std::string filepath) {
  if (filepath.find_last_of("/\\") != std::string::npos)
	return filepath.substr(0, filepath.find_last_of("/\\")+1);
  return "";
}

Object::Object(std::string file, unsigned int progID){
	inputFile = file;
	std::string basedir = getBaseDir(inputFile);
	std::string err;
	bool ret = tinyobj::LoadObj(shapes, materials, err, inputFile.c_str(), basedir.c_str());

	if (!err.empty()) { // `err` may contain warning message.
	  std::cerr << err << std::endl;
	}

	if (!ret) {
	  exit(1);
	}

	// createTextures();
	createVAOs(progID);
}

void Object::createVAOs(unsigned int progID){

	glUseProgram(progID);

	for(int i = 0; i < shapes.size(); i++){

		unsigned int vaoh;
		vaoHandles.push_back(vaoh);

		glGenVertexArrays(1, &vaoHandles[i]);
		glBindVertexArray(vaoHandles[i]);

		unsigned int buffer[4];
		glGenBuffers(4, buffer);

		// Set vertex position attribute
		glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes[i].mesh.positions.size(), 
		&shapes[i].mesh.positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

		// Normal attributes
		glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes[i].mesh.normals.size(), 
		&shapes[i].mesh.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

		// // Tex coords
		// glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
		// glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 1, //1 is texture count per shape
		// &shapes[i].mesh.texcoords[0], GL_STATIC_DRAW);
		// glEnableVertexAttribArray(2);
		// glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

		// Vertex indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 
		shapes[i].mesh.indices.size(), &shapes[i].mesh.indices[0], GL_STATIC_DRAW);
	}
}

// void Object::loadRGBTexture(const char *path){
//     int x, y, n;

//     // Load from file. Force RGB image pixel format
//     unsigned char *data = stbi_load(path, &x, &y, &n, 3);

//     // Copy to GPU as data for the currently active texture.
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
//     stbi_image_free(data);
//     glGenerateMipmap(GL_TEXTURE_2D);
// }

// void Object::createTextures(){

// 		glGenTextures(1, TexID);

// 		for(int i = 0; i < shapes.size(); i++){
					
// 			glActiveTexture(GL_TEXTURE0);
// 			glBindTexture(GL_TEXTURE_2D, TexID[0]);
// 			loadRGBTexture(materials[shapes[i].mesh.material_ids[0]].diffuse_texname.c_str());
// 		}
// }