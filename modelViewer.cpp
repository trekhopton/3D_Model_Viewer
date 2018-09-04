#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "InputState.h"
#include "Viewer.h"
#include "Shader.hpp"
#include "Object.h"

int winX = 500;
int winY = 500;

ObjectViewer *ObjCam;
Viewer *Camera;
glm::vec3 cameraPos(0.0f, 5.0f, -12.0f);
float fov = (float)M_PI/3.0f;

Object *Obj;

glm::vec3 scaleVec;
//modes
int debugMode = 1;
int shaderMode = 1;
int lightMode = 1;
//input data structure
InputState Input;
//shader programs
unsigned int lightTexSh;
unsigned int debugSh;
unsigned int programID;

void setProjection(){
	glm::mat4 projection;

	projection = glm::perspective(fov, (float) winX / winY, 1.0f, 30.0f);

	// Load it to the shader program
	int projHandleD = glGetUniformLocation(debugSh, "projection_matrix");
	int projHandleL = glGetUniformLocation(lightTexSh, "projection_matrix");
	if (projHandleD == -1 || projHandleL == -1) {
		fprintf(stderr, "Error updating proj matrix\n");
		exit(1);
	}
	glUseProgram(debugSh);
	glUniformMatrix4fv(projHandleD, 1, false, glm::value_ptr(projection));
	glUseProgram(lightTexSh);
	glUniformMatrix4fv(projHandleL, 1, false, glm::value_ptr(projection));
}
// setup the shaders, called at the start, and called every time the lightMode changes
int setupShader(unsigned int id){
	glUseProgram(id);

	//Uniform lighting variables
	int lightambientHandle = glGetUniformLocation(id, "light_ambient");
	int lightdiffuseHandle = glGetUniformLocation(id, "light_diffuse");
	int lightspecularHandle = glGetUniformLocation(id, "light_specular");
	if (lightambientHandle == -1 || lightdiffuseHandle == -1 || lightspecularHandle == -1) {
		fprintf(stderr, "Error: can't find light uniform variables\n");
		return 1;
	}

	float lightambient[3] = { 0.1f, 0.1f, 0.1f };
	float lightdiffuse[3];
	float lightspecular[3] = { 1.0f, 1.0f, 1.0f };
	// change the colour of the light depending on the mode
	if(lightMode == 1){
		lightdiffuse[0] = 0.5;
		lightdiffuse[1] = 0.5;
		lightdiffuse[2] = 1.0;
	} else if (lightMode == 2){
		lightdiffuse[0] = 1.0;
		lightdiffuse[1] = 1.0;
		lightdiffuse[2] = 1.0;
	} else {
		lightdiffuse[0] = 1.0;
		lightdiffuse[1] = 1.0;
		lightdiffuse[2] = 0.0;
	}

	glUniform3fv(lightambientHandle, 1, lightambient);
	glUniform3fv(lightdiffuseHandle, 1, lightdiffuse);
	glUniform3fv(lightspecularHandle, 1, lightspecular);

	return 0;	// return success
}
// calculates a scale vector, using a bounding sphere, to scale the objects to the correct size in the view
void scaleToFit(){

	float minx;
	float miny;
	float minz;

	float maxx;
	float maxy;
	float maxz;

		//find the shapes minimum and maximum xyz
	for(int i = 0; i < Obj->shapes.size(); i++){

		for(int j = 0; j < Obj->shapes[i].mesh.positions.size(); j++){

			if(i == 0 && j == 0){
				minx = Obj->shapes[i].mesh.positions[j];
				maxx = Obj->shapes[i].mesh.positions[j];
			}
			if(i == 0 && j == 1){
				miny = Obj->shapes[i].mesh.positions[j];
				maxy = Obj->shapes[i].mesh.positions[j];
			}
			if(i == 0 && j == 2){
				minz = Obj->shapes[i].mesh.positions[j];
				maxz = Obj->shapes[i].mesh.positions[j];
			}

			if(Obj->shapes[i].mesh.positions[j] > maxx){
				maxx = Obj->shapes[i].mesh.positions[j];
			}
			if(Obj->shapes[i].mesh.positions[j] < minx){
				minx = Obj->shapes[i].mesh.positions[j];
			}
			j++;
			if(Obj->shapes[i].mesh.positions[j] > maxy){
				maxy = Obj->shapes[i].mesh.positions[j];
			}
			if(Obj->shapes[i].mesh.positions[j] < miny){
				miny = Obj->shapes[i].mesh.positions[j];
			}
			j++;
			if(Obj->shapes[i].mesh.positions[j] > maxz){
				maxz = Obj->shapes[i].mesh.positions[j];
			}
			if(Obj->shapes[i].mesh.positions[j] < minz){
				minz = Obj->shapes[i].mesh.positions[j];
			}
		}
	}
	//calculate the bounding sphere and set the scale vector accordingly
	glm::vec3 centerPoint = glm::vec3((maxx+minx)/2.0, (maxy+miny)/2.0, (maxz+minz)/2.0);
	glm::vec3 maxPoint = glm::vec3(maxx, maxy, maxz);
	float radius = glm::distance(maxPoint, centerPoint);
	
	float maxRad = glm::distance(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f)) * sin(fov/2.0);
	float scale = (maxRad / radius) / 2.0;
	std::cout << scale << std::endl;
	scaleVec = glm::vec3(scale, scale, scale);
}

void render(double deltaT){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//update camera based on input
	Camera->update(Input);
	glm::mat4 viewMatrix;
	viewMatrix = Camera->getViewMtx();

	int viewHandle = glGetUniformLocation(programID, "view_matrix");
	if (viewHandle == -1) {
		std::cout << "Uniform: view is not an active uniform label\n";
	}
	glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMatrix));

	glUseProgram(programID);

	int modelHandle = glGetUniformLocation(programID, "model_matrix");
	int normHandle = glGetUniformLocation(programID, "normal_matrix");
	if (modelHandle == -1 || normHandle == -1){
		fprintf(stderr, "Error: can't find matrix uniforms\n");
		exit(1);
	}

	int mtlambientHandle;
	int mtldiffuseHandle;
	int mtlspecularHandle;
	int lightPosHandle;
	int viewPosHandle;
	// int diffHandle;

	if(shaderMode == 1){
		mtldiffuseHandle = glGetUniformLocation(programID, "mtl_diffuse");
		if (mtldiffuseHandle == -1){
			fprintf(stderr, "Error: can't find material uniforms\n");
			exit(1);
		}

		int modeHandle = glGetUniformLocation(programID, "mode");
		glUniform1i(modeHandle, debugMode);

	} else {

		// diffHandle = glGetUniformLocation(programID, "diffMap");
		// if (diffHandle == -1) {
		// 	fprintf(stderr, "Error: can't find texture maps\n");
		// 	exit(1);
		// }
		mtlambientHandle = glGetUniformLocation(programID, "mtl_ambient");
		mtldiffuseHandle = glGetUniformLocation(programID, "mtl_diffuse");
		mtlspecularHandle = glGetUniformLocation(programID, "mtl_specular");
		
		if (mtlambientHandle == -1 || mtldiffuseHandle == -1 || mtlspecularHandle == -1){
			fprintf(stderr, "Error: can't find material uniform variables\n");
			exit(1);
		}

		lightPosHandle = glGetUniformLocation(programID, "light_pos");

		if (lightPosHandle == -1){
			fprintf(stderr, "Error: can't find lighting uniforms\n");
			exit(1);
		}
		// set light position based on light mode
		float lightPos[3];
		if(lightMode == 1){
			lightPos[0] = 0.0;
			lightPos[1] = 2.0;
			lightPos[2] = 0.0;
		} else if(lightMode == 2){
			lightPos[0] = 0.0;
			lightPos[1] = 0.0;
			lightPos[2] = -2.0;
		} else {
			lightPos[0] = 0.0;
			lightPos[1] = 2.0;
			lightPos[2] = 0.0;
		}
		
		glUniform3fv(lightPosHandle, 1, lightPos);
	}

	for(int i = 0; i < Obj->shapes.size(); i++){

		if(shaderMode == 1){
			glUniform3fv(mtlambientHandle, 1, Obj->materials[Obj->shapes[i].mesh.material_ids[0]].ambient);
			glUniform3fv(mtldiffuseHandle, 1, Obj->materials[Obj->shapes[i].mesh.material_ids[0]].diffuse);
			glUniform3fv(mtlspecularHandle, 1, Obj->materials[Obj->shapes[i].mesh.material_ids[0]].specular);
			// glUniform1i(diffHandle, 0);

		} else {
			glUniform3fv(mtldiffuseHandle, 1, Obj->materials[Obj->shapes[i].mesh.material_ids[0]].diffuse);
		}

		glBindVertexArray(Obj->vaoHandles[i]);

		glm::mat4 modelMatrix;
		glm::mat3 normMatrix;

		modelMatrix = glm::scale(modelMatrix, scaleVec);
		glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMatrix));
		normMatrix = glm::mat3(modelMatrix);
		glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
		glDrawElements(GL_TRIANGLES, Obj->shapes[i].mesh.indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		glFlush();
	}
}

//Callbacks.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (action == GLFW_PRESS){
		switch(key){
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GL_TRUE);
				break;
			case GLFW_KEY_D: //change debug mode
				if(shaderMode == 1){
					if (debugMode == 1){
						debugMode = 2;
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					}
					else if (debugMode == 2){
						debugMode = 3;
					}
					else {
						debugMode = 1;
						if(shaderMode == 1){
							glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						}
					}
				}
				break;
			case GLFW_KEY_L: // change light mode
				if(shaderMode == 2){
					if (lightMode == 1){
						lightMode = 2;
						setupShader(programID);
					}
					else if (lightMode == 2){
						lightMode = 3;
						setupShader(programID);
					}
					else {
						lightMode = 1;
						setupShader(programID);
					}
				}
				break;
			case GLFW_KEY_S: // change shader to use
				if (shaderMode == 1) {
					shaderMode = 2;
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					programID = lightTexSh;
					if (setupShader(programID) != 0) {
						fprintf(stderr, "Can't initialise shaders!\n");
						exit(1);
					}	
				}
				else {
					shaderMode = 1;
					if (debugMode == 1){
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					}
					programID = debugSh;
				}
				break;
			default:
				break;
		}
	}
}

// Called when the window is resized.
void reshape_callback( GLFWwindow *window, int x, int y ){
	winX = x;
	winY = y;
	setProjection();
	glViewport( 0, 0, x, y );
}

void mouse_pos_callback(GLFWwindow* window, double x, double y){
	// Use a global data structure to store mouse info
	// We can then use it however we want
	Input.update((float)x, (float)y);
	float FOVmin = ((float)M_PI)/20.0;
	float FOVmax = ((float)M_PI)/2;
	if(Input.rMousePressed){
		fov = fov + Input.deltaY * 0.005;
		setProjection();
		if(fov < FOVmin){
			fov = FOVmin;
		} else if (fov > FOVmax){
			fov = FOVmax;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		Input.rMousePressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE){
		Input.rMousePressed = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		Input.lMousePressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
		Input.lMousePressed = false;
	}
}

//Error callback for GLFW. Simply prints error message to stderr.
static void error_callback(int error, const char* description){
	fputs(description, stderr);
}

int main (int argc, char **argv){
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		exit(1);
	}

	// Specify that we want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window and OpenGL context
	window = glfwCreateWindow(winX, winY, "Model Viewer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

	// Set OpenGL state we need for this application.
	glClearColor(0.5F, 0.5F, 0.5F, 0.0F);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(programID);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	debugSh = LoadShaders("debug.vert", "debug.frag");
	lightTexSh = LoadShaders("light-texture.vert", "light-texture.frag");
	programID = debugSh;

	if (debugSh == 0 || lightTexSh == 0) {
		fprintf(stderr, "Can't compile shaders!\n");
		exit(1);
	}

	setProjection();

	ObjCam = new ObjectViewer(cameraPos);
	Camera = ObjCam;

	Obj = new Object(argv[1], programID);
	scaleToFit();

	// Define callback functions and start main loop
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, reshape_callback);

	// GLFW records the time since the program started (in seconds)
    double start = glfwGetTime();
    double now;

	while (!glfwWindowShouldClose(window)){
		now = glfwGetTime();

        render(now - start);
		start = now;
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(0);

	delete ObjCam;
	delete Obj;

	return 0;
}
