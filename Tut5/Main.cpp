#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "math_3d.h"
#include "LoaderShader.h"

using namespace std;

//function prototypes
void renderScene();
void createVertexBuffer();
void initialiseGlutCallback();

GLuint VBO;
GLuint programId;
GLuint matrixID;
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;
glm::mat4 MVP;

int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 300);
	glutInitWindowSize(400, 300);
	glutCreateWindow("Tutorial 05 - Adding Transformation to Triangle");

	initialiseGlutCallback();

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s\n", glewGetErrorString(res));

		return 1;
	}

	//need to be after glewInit(), otherwise got error
	programId = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	//get a handle for our "MVP" uniform
	matrixID = glGetUniformLocation(programId, "MVP");

	//projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	//or, for an ortho camera :
	/*projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f); // In world coordinates*/

	//camera matrix
	view = glm::lookAt(
		glm::vec3(0, 0, 5), //camera is at (0, 0, 5), in World Space
		glm::vec3(0, 0, 0), //and looks at the origin
		glm::vec3(0, 1, 0)  //head is up (set to 0,-1,0 to look upside-down)
		);
	//model matrix : an identity matrix (model will be at the origin)
	model = glm::mat4(1.0f);
	
	//scale the model by 2.0 for each axis
	//model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

	//rotate 90 degrees in counter-clockwise direction about z-axis
	//rotation in clockwise direction will require negative angles
	//model = glm::rotate(model, 90.0f, glm::vec3(0, 0, 1));

	//translate the model 2 units to the right along x-axis
	model = glm::translate(model, glm::vec3(2, 0, 0));
	
	//our ModelViewProjection : multiplication of our 3 matrices
	//remember, matrix multiplication is the other way around
	MVP = projection * view * model;

	//blue background
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

	createVertexBuffer();

	glutMainLoop();

	return 0;
}

void renderScene() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//use the shader
	glUseProgram(programId);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//draw triangle
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}

void createVertexBuffer() {
	//setting vertices of triangle
	Vector3f vertices[3];
	vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
	vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
	vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void initialiseGlutCallback() {

	glutDisplayFunc(renderScene);
}