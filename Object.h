#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
// #include "stb_image.h"
#include "tiny_obj_loader.h"

class Object{
public:
	//create an object giving it the file path and shader program to use
	Object(std::string file, unsigned int progID);
	//input file path name
	std::string inputFile;
	//VAO handles for object
	std::vector<unsigned int> vaoHandles;
	//shapes and materials for the object
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
private:
	// void createTextures();
	// void loadRGBTexture(const char *path);
	//create VAOs for the object
	void createVAOs(unsigned int progID);
	//used to get the directory of the material
	std::string getBaseDir(std::string filepath);
	// GLuint TexID[1];
};

#endif
