#include <GL/glew.h>
#include <glfw.h>
#include <stdlib.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct vertex{

	GLfloat x, y, z;

};

using namespace std;

const float PI = 4.0 * atan2(1.0, 1.0), speed = .00005;

GLuint vao[1], vbo[1];
GLchar *vertexsource, *fragmentsource; //receive contents of shader source

GLuint vertexshader, fragmentshader; //reference shaders

GLuint shaderprogram;

void check(char *where){ //check error status

	char * what;
	int err = glGetError(); //0 is no error

	if(!err){
		return;
	}
	if(err == GL_INVALID_ENUM)
		what = "GL_INVALID_ENUM";
	else if(err == GL_INVALID_VALUE)
		what = "GL_INVALID_VALUE";
	else if(err == GL_INVALID_OPERATION)
		what = "GL_INVALID_OPERATION";
	else if(err == GL_INVALID_FRAMEBUFFER_OPERATION)
		what = "GL_INVALID_FRAMEBUFFER_OPERATION";
	else if(err == GL_OUT_OF_MEMORY)
		what = "GL_OUT_OF_MEMORY";
	else
		what = "UNKNOWN_ERROR";
	fprintf(stderr, "Error ($d) %s at %s\n", err, what, where);
	exit(1);
		
}
/*
Based on an algorythm found at http://www.gamedev.net/topic/535593-3d-sphere-drawing-algorithm/
*/
vector<vertex> getSphereCoordinates(float radius, int meridians, int parallels) {
	
	vector<vertex> returnedPositions;
	
	float meridianShift, parallelShift;
	meridianShift = PI/meridians;
	parallelShift = 2*PI/parallels;

	for(float theta = 0; theta < PI; theta += meridianShift) {
		//loop through meridians
		
		for(float phi = 0; phi < 2*PI; phi += parallelShift) {

			
			vertex currentvert, nextVertP, nextVertM, nextVertMP;
//			printf("%i : ", currentvert);
			currentvert.x = cosf(theta) * sinf(phi) * radius;
			nextVertP.x = cosf(theta) * sinf(phi + parallelShift) * radius;
			nextVertM.x = cosf(theta + meridianShift) * sinf(phi) * radius;
			nextVertMP.x = cosf(theta + meridianShift) * sinf(phi + parallelShift) * radius;
	//		printf("%f:", currentvert.x);
			currentvert.y = sinf(theta) * sinf(phi) * radius;
			nextVertP.y = sinf(theta) * sinf(phi + parallelShift) * radius;
			nextVertM.y = sinf(theta + meridianShift) * sinf(phi) * radius;
			nextVertMP.y = sinf(theta + meridianShift) * sinf(phi + parallelShift) * radius;
		//	printf("%f:", currentvert.y);
			currentvert.z = cosf(phi) * radius;
			nextVertP.z = cosf(phi + parallelShift) * radius;
			nextVertM.z = currentvert.z;
			nextVertMP.z = nextVertP.z;
			//printf("%f\n", currentvert.z);
			
			returnedPositions.push_back(currentvert);
			returnedPositions.push_back(nextVertM);
			returnedPositions.push_back(nextVertMP);
			returnedPositions.push_back(currentvert);
			returnedPositions.push_back(nextVertP);
			returnedPositions.push_back(nextVertMP);
			//then through parallels
		}

	}

	return returnedPositions;

}

/*
taken from/based on code from example 2
*/

char* fileToBuf(char* file) { //reads a file into an allocated buffer
	
	FILE *fptr;
	long length;
	char *buf;
	fptr =fopen(file, "rb"); //opens file
	if(!fptr) {
		//return null on fail
		fprintf(stderr, "failed to open %s\n", file);
		return NULL;
	}
	fseek(fptr, 0, SEEK_END); //seek to end
	length = ftell(fptr); //find out how many bytes into the file the end is
	buf = (char*) malloc(length+1); //allocate a sizable buffer
	fseek(fptr, 0, SEEK_SET); //return to start
	fread(buf, length, 1, fptr); //reads the whole file
	fclose(fptr);
	buf[length] = 0; //null termninate
	return buf;
}

void setupShaders(void) {
	char text[1000];
	int length;
	fprintf(stderr, "Set up shaders\n");
	//set up 2 vertex buffer objs
	vertexsource = fileToBuf("gcw.vert");
	fragmentsource = fileToBuf("gcw.frag");
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);
	glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
	glCompileShader(fragmentshader);
	glCompileShader(vertexshader);
	shaderprogram = glCreateProgram();
	glAttachShader(shaderprogram, vertexshader);
	glAttachShader(shaderprogram, fragmentshader);
	glLinkProgram(shaderprogram);
	glGetProgramInfoLog(shaderprogram, 1000, &length, text); //check for errors
	if(length>0)
		fprintf(stderr, "Validate Shader Program\n%s\n", text);
	glUseProgram(shaderprogram);
}



int main(void){
	int running = GL_TRUE;
	float rotation, currentTime, oldTime;
	rotation = 0.f;
	oldTime = glfwGetTime();
	
	//initialise GLFW

	if(!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	//open window

	if(!glfwOpenWindow(600, 600, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glewInit();
	setupShaders();
	vector<vertex> sphereCoords = getSphereCoordinates(1,60,60);

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);	
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, sphereCoords.size() * sizeof(vertex), &(sphereCoords[0]), GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindAttribLocation(shaderprogram, 0, "in_position");
	//main loop
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View       = glm::lookAt(
		glm::vec3(2,2,2), // Camera is at (4,3,3), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);  // Changes for each model !
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
	// Get a handle for our "MVP" uniform.
	// Only at initialisation time.
	GLuint MatrixID = glGetUniformLocation(shaderprogram, "MVP");
 
	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	while( running ) {
		//render here

		currentTime = glfwGetTime();
		rotation += (currentTime - oldTime) * speed;

		if(rotation >= 360.f)
			rotation = rotation -360.f;

		glClearColor(0, 0, 0, 1);
		check("begin render loop");
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(vao[0]);
		check("bind array");
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		check("bind buffer");
		
		glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 View       = glm::lookAt(
			glm::vec3(2,2,2), // Camera is at (4,3,3), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

		View = glm::rotate(View, rotation, glm::vec3(1, 1, 1));

		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model = glm::mat4(1.0f);  // Changes for each model !
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		// Get a handle for our "MVP" uniform.
		// Only at initialisation time.
		GLuint MatrixID = glGetUniformLocation(shaderprogram, "MVP");
 
		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		// For each model you render, since the MVP will be different (at least the M part)
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, sphereCoords.size());

		check("draw");

		glfwSwapBuffers();
		check("swap buffers");
		//check esc key

		running= !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
		glFlush();

	}

	//end app

	glfwTerminate();

	exit(EXIT_SUCCESS);

}
