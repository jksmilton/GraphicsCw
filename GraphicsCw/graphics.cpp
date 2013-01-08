#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glew.h>
#include <glfw.h>
#include <stdlib.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

const float PI = static_cast<float>( 4.0 * atan2(1.0, 1.0) ), speed = 60;
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
struct vertex{

	GLfloat x, y, z;

};
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
/*
Based on an algorythm found at http://www.gamedev.net/topic/535593-3d-sphere-drawing-algorithm/
*/

/*
taken from/based on code from example 2
*/
GLuint setupShaders(GLchar * vertexsource, GLchar * fragmentsource, GLuint vertexshader, GLuint fragmentshader, char * vertFile, char * fragFile) {
	char text[1000];
	GLuint shaderprogram;
	int length;
	fprintf(stderr, "Set up shaders\n");
	//set up 2 vertex buffer objs
	vertexsource = fileToBuf(vertFile);
	fragmentsource = fileToBuf(fragFile);
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);
	glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
	glCompileShader(vertexshader);
	glCompileShader(fragmentshader);
	
	shaderprogram = glCreateProgram();
	
	glAttachShader(shaderprogram, vertexshader);
	glAttachShader(shaderprogram, fragmentshader);
	glLinkProgram(shaderprogram);
	glGetProgramInfoLog(shaderprogram, 1000, &length, text); //check for errors
	if(length>0)
		fprintf(stderr, "Validate Shader Program\n%s\n", text);
	
	return shaderprogram;

}

class SphereRotate {

private:
	GLuint vao[1], vbo[1];
	GLchar *vertexsource, *fragmentsource; //receive contents of shader source
	GLuint vertexshader, fragmentshader; //reference shaders
	GLuint shaderprogram;
	vector<vertex> sphereCoordinates;
	GLuint MatrixID, TranslationID;
	float oldTime;
	glm::mat4 MVP, Model, Projection, View, Translation;

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

public:

	bool init(){

		shaderprogram = setupShaders(vertexsource, fragmentsource, vertexshader, fragmentshader, "gcw.vert", "gcw.frag");
		sphereCoordinates = getSphereCoordinates(1, 20, 20);
		
		glGenVertexArrays(1, vao);
		glBindVertexArray(vao[0]);
		glGenBuffers(1, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);	
		glEnableVertexAttribArray(0);
		glBufferData(GL_ARRAY_BUFFER, sphereCoordinates.size() * sizeof(vertex), &(sphereCoordinates[0]), GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindAttribLocation(shaderprogram, 0, "in_position");
		//main loop
		Projection = glm::perspective(45.0f, 1.f, 0.1f, 100.0f);
		View       = glm::lookAt(
			glm::vec3(2,2,2), // Camera is at (4,3,3), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
		Translation = glm::mat4(1.f);
		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);  // Changes for each model !
		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		// Get a handle for our "MVP" uniform.
		// Only at initialisation time.
		MatrixID = glGetUniformLocation(shaderprogram, "MVP");
		TranslationID = glGetUniformLocation(shaderprogram, "translation");
	


		return true;
	}

	void render(){
		glUseProgram(shaderprogram);
		float rotation, currentTime;
		glm::mat4 translation;			
		currentTime = static_cast<float>(glfwGetTime());
		rotation = (currentTime - oldTime) * speed;
		oldTime = currentTime;
		
		//printf("rotation: %f\n", rotation);

		glBindVertexArray(vao[0]);
		//check("bind array");
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//check("bind buffer");
		
		//View = glm::rotate(View, rotation, glm::vec3(1, 1, 1));
		Translation = glm::rotate(Translation, rotation, glm::vec3(1,1,0));
		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);  // Changes for each model !
		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		// Get a handle for our "MVP" uniform.

 
		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		// For each model you render, since the MVP will be different (at least the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(TranslationID, 1, GL_FALSE, &Translation[0][0]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, sphereCoordinates.size());


	}
};

class coneRotate {

	GLuint vao[1], vbo[1];
	GLchar *vertexsource, *fragmentsource; //receive contents of shader source
	GLuint vertexshader, fragmentshader; //reference shaders
	GLuint shaderprogram, TranslationID;
	vector<vertex> coneCoordinates;
	GLuint MatrixID;
	float oldTime;
	glm::mat4 MVP, Model, Projection, View, Translate;
	vector<vertex> getConeCoordinates(float height, float radius, int points){

		float edgeShift = 2 * PI / points;
		vector<vertex> returnedPoints;
		
		vertex origin, top;

		origin.x = 0;
		origin.y = 0;
		origin.z = 0;

		top.x = 0;
		top.y = 0;
		top.z = height;

		for(float theta = 0; theta < 2 * PI; theta+= edgeShift){

			vertex current, next;

			current.x = radius * sinf(theta);
			next.x = radius * sinf(theta + edgeShift);
			current.y = radius * cosf(theta);
			next.y = radius * cosf(theta + edgeShift);
			current.z = 0;
			next.z = 0;

			returnedPoints.push_back(current);
			returnedPoints.push_back(next);
			returnedPoints.push_back(top);
			returnedPoints.push_back(current);
			returnedPoints.push_back(next);
			returnedPoints.push_back(origin);

		}

		return returnedPoints;

	}

	public:

	bool init(){

		shaderprogram = setupShaders(vertexsource, fragmentsource, vertexshader, fragmentshader, "gcw.vert", "gcw.frag");
		coneCoordinates = getConeCoordinates(1.5, 1, 20);
		
		glGenVertexArrays(1, vao);
		glBindVertexArray(vao[0]);
		glGenBuffers(1, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);	
		glEnableVertexAttribArray(0);
		glBufferData(GL_ARRAY_BUFFER, coneCoordinates.size() * sizeof(vertex), &(coneCoordinates[0]), GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindAttribLocation(shaderprogram, 0, "in_position");
		//main loop
		Projection = glm::perspective(45.0f, 1.f, 0.1f, 100.0f);
		View       = glm::lookAt(
			glm::vec3(4,4,3), // 
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
		Translate = glm::mat4(1.f);
		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);  // Changes for each model !
		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		// Get a handle for our "MVP" uniform.
		// Only at initialisation time.
		MatrixID = glGetUniformLocation(shaderprogram, "MVP");
		TranslationID = glGetUniformLocation(shaderprogram, "translation");

		return true;
	}

	void render(){
		glUseProgram(shaderprogram);
		float rotation, currentTime;
						
		currentTime = static_cast<float>(glfwGetTime());
		rotation = (currentTime - oldTime) * speed;
		oldTime = currentTime;
		
		//printf("rotation: %f\n", rotation);

		glBindVertexArray(vao[0]);
		//check("bind array");
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//check("bind buffer");
		
		View = glm::rotate(View, rotation, glm::vec3(1, 1, 1));

		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);  // Changes for each model !
		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		// Get a handle for our "MVP" uniform.
 
		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		// For each model you render, since the MVP will be different (at least the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(TranslationID,1 , GL_FALSE, &Translate[0][0]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, coneCoordinates.size());


	}
};

class SphereNormals { 

	GLuint vao[2], vbo[2];
	GLchar *vertexsource, *fragmentsource; //receive contents of shader source
	GLuint vertexshader, fragmentshader; //reference shaders
	GLuint shaderprogram, TranslationID;
	vector<vertex> sphereCoordinates;
	vector<vertex> sphereNormals;
	GLuint MatrixID;
	float oldTime;
	glm::mat4 MVP, Model, Projection, View, Translate;
	vector<vertex> getSphereCoordinates(float radius, int meridians, int parallels) {
	
		vector<vertex> returnedPositions;
	
		float meridianShift, parallelShift;
		meridianShift = PI/meridians;
		parallelShift = 2*PI/parallels;

		for(float theta = 0; theta < PI; theta += meridianShift) {
			//loop through meridians
		
			for(float phi = 0; phi < 2*PI; phi += parallelShift) {

			
				vertex currentvert, nextVertP, nextVertM, nextVertMP, normalVert;
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

				normalVert.x = static_cast<GLfloat>(currentvert.x * 1.1);
				normalVert.y = static_cast<GLfloat>(currentvert.y * 1.1);
				normalVert.z = static_cast<GLfloat>(currentvert.z * 1.1);

				sphereNormals.push_back(currentvert);
				sphereNormals.push_back(normalVert);

				//printf("current vert: %f,%f,%f; normal vert: %f,%f,%f\n", currentvert.x,currentvert.y,currentvert.z, normalVert.x,normalVert.y,normalVert.z);

			}

	}
	
	return returnedPositions;

}
	public:

	bool init(){

		shaderprogram = setupShaders(vertexsource, fragmentsource, vertexshader, fragmentshader, "gcw.vert", "gcw.frag");
		sphereCoordinates = getSphereCoordinates(1, 20, 20);
		
		glGenVertexArrays(2, vao);
		glBindVertexArray(vao[0]);
		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

		glEnableVertexAttribArray(0);

		glBufferData(GL_ARRAY_BUFFER, sphereCoordinates.size() * sizeof(vertex), &(sphereCoordinates[0]), GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindVertexArray(vao[1]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glEnableVertexAttribArray(0);
		glBufferData(GL_ARRAY_BUFFER, sphereNormals.size() * sizeof(vertex), &(sphereNormals[0]), GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindAttribLocation(shaderprogram, 0, "in_position");
		
		Translate = glm::mat4(1.f);

		Projection = glm::perspective(45.0f, 1.f, 0.1f, 100.0f);
		View       = glm::lookAt(
			glm::vec3(2,2,2), // Camera is at (4,3,3), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);  // Changes for each model !
		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		// Get a handle for our "MVP" uniform.
		// Only at initialisation time.
		MatrixID = glGetUniformLocation(shaderprogram, "MVP");
		TranslationID = glGetUniformLocation(shaderprogram, "translation");
		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		// For each model you render, since the MVP will be different (at least the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);


		return true;
	}

	void render(){
		glUseProgram(shaderprogram);
		float rotation, currentTime;
						
		currentTime = static_cast<float>(glfwGetTime());
		rotation = (currentTime - oldTime) * speed;
		oldTime = currentTime;
		
		//printf("rotation: %f\n", rotation);

		
		
		View = glm::rotate(View, rotation, glm::vec3(1, 1, 1));

		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);  // Changes for each model !
		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		// Get a handle for our "MVP" uniform.
		// Only at initialisation time.
		
 
		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		// For each model you render, since the MVP will be different (at least the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glBindVertexArray(vao[0]);
		//check("bind array");
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//check("bind buffer");

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, sphereCoordinates.size());
		glUniformMatrix4fv(TranslationID,1 , GL_FALSE, &Translate[0][0]);
		glBindVertexArray(vao[1]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

		glDrawArrays(GL_LINES, 0, sphereNormals.size());

	}

};

class Paths { 

	GLuint vao[4], vbo[4];
	GLchar *vertexsource, *fragmentsource; //receive contents of shader source
	GLuint vertexshader, fragmentshader; //reference shaders
	GLuint shaderprogram, TranslationID;
	vector<vertex> coordinates[4];
	int direction[4];
	bool first;
	GLuint MatrixID;
	float oldTime;
	glm::mat4 MVP, Model, Projection, View, Translate[4];
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
	vector<vertex> getConeCoordinates(float height, float radius, int points){

		float edgeShift = 2 * PI / points;
		vector<vertex> returnedPoints;
		
		vertex origin, top;

		origin.x = 0;
		origin.y = 0;
		origin.z = 0;

		top.x = 0;
		top.y = 0;
		top.z = height;

		for(float theta = 0; theta < 2 * PI; theta+= edgeShift){

			vertex current, next;

			current.x = radius * sinf(theta);
			next.x = radius * sinf(theta + edgeShift);
			current.y = radius * cosf(theta);
			next.y = radius * cosf(theta + edgeShift);
			current.z = 0;
			next.z = 0;

			returnedPoints.push_back(current);
			returnedPoints.push_back(next);
			returnedPoints.push_back(top);
			returnedPoints.push_back(current);
			returnedPoints.push_back(next);
			returnedPoints.push_back(origin);

		}

		return returnedPoints;

	}
	
	void move(float timeDifference, int index){
		float change = timeDifference * speed / 100.f;
		glm::vec4 workingWith = Translate[index][3];
		
		if(direction[index] == 1){

			workingWith[1] += change;
			workingWith[0] = -4.f;

			if(workingWith[1] >= 4.f)
				direction[index] = 2;

		} else if(direction[index] == 2){

			workingWith[0] += change;
			workingWith[1] = 4.f;

			if(workingWith[0] >= 4.f)
				direction[index] = 3;

		} else if(direction[index] == 3){

			workingWith[1] =  workingWith[1] - change;
			workingWith[0] = 4.f;

			if(workingWith[1] <= -4.f)
				direction[index] = 4;

		} else if(direction[index] == 4){

			workingWith[0] = workingWith[0] - change;
			workingWith[1] = -4.f;

			if(workingWith[0] <= -4.f)
				direction[index] = 1;
		}

		Translate[index] = glm::mat4(
			glm::vec4(1, 0, 0, 0),
			glm::vec4(0, 1, 0, 0),
			glm::vec4(0, 0, 1, 0),
			workingWith
			);
		
	}
public:

	bool init(){
		first = true;
		shaderprogram = setupShaders(vertexsource, fragmentsource, vertexshader, fragmentshader, "gcw.vert", "gcw.frag");
		coordinates[0] = getSphereCoordinates(1, 20, 20);
		coordinates[3] = getSphereCoordinates(1.5, 30, 30);
		coordinates[2] = getConeCoordinates(1.5, 1, 20);
		coordinates[1] = getConeCoordinates(2, 0.5, 15);
		oldTime = static_cast<float>(glfwGetTime());
		glGenVertexArrays(4, vao);
		glGenBuffers(4, vbo);
				
		for(int i = 0; i < 4; i++){
			glBindVertexArray(vao[i]);
			glBindBuffer(GL_ARRAY_BUFFER,vbo[i]);

			glEnableVertexAttribArray(0);

			glBufferData(GL_ARRAY_BUFFER, coordinates[i].size() * sizeof(vertex), &(coordinates[i][0]), GL_STATIC_DRAW);
			glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		}
		

		glBindAttribLocation(shaderprogram, 0, "in_position");
		
		for(int i = 0; i< 4;i++)
			Translate[i] = glm::mat4(1.f);

		Translate[0] = glm::mat4(
			glm::vec4(1, 0, 0, 0),
			glm::vec4(0, 1, 0, 0),
			glm::vec4(0, 0, 1, 0),
			glm::vec4(4, 4, 0, 1)
			);
		direction[0] = 3;
		Translate[1] = glm::mat4(
			glm::vec4(1, 0, 0, 0),
			glm::vec4(0, 1, 0, 0),
			glm::vec4(0, 0, 1, 0),
			glm::vec4(-4, 4, 0, 1)
			);
		direction[1] = 2;
		Translate[2] = glm::mat4(
			glm::vec4(1, 0, 0, 0),
			glm::vec4(0, 1, 0, 0),
			glm::vec4(0, 0, 1, 0),
			glm::vec4(4, -4, 0, 1)
			);
		direction[2] = 4;
		Translate[3] = glm::mat4(
			glm::vec4(1, 0, 0, 0),
			glm::vec4(0, 1, 0, 0),
			glm::vec4(0, 0, 1, 0),
			glm::vec4(-4, -4, 0, 1)
			);
		direction[3] = 1;
		Projection = glm::perspective(45.0f, 1.f, 0.1f, 100.0f);
		View       = glm::lookAt(
			glm::vec3(10,10,10), // Camera is at (4,3,3), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
		
		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);  // Changes for each model !
		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		// Get a handle for our "MVP" uniform.
		// Only at initialisation time.
		MatrixID = glGetUniformLocation(shaderprogram, "MVP");
		TranslationID = glGetUniformLocation(shaderprogram, "translation");
		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		// For each model you render, since the MVP will be different (at least the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);


		return true;
	}

	void render(){
		glUseProgram(shaderprogram);
		float timeChange, currentTime;
						
		currentTime = static_cast<float>(glfwGetTime());
		timeChange = (currentTime - oldTime);
		oldTime = currentTime;
		
		//View = glm::rotate(View, rotation, glm::vec3(1, 1, 1));

		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);  // Changes for each model !
		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		// Get a handle for our "MVP" uniform.
		
		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		// For each model you render, since the MVP will be different (at least the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glDrawArrays(GL_TRIANGLES, 0, sphereCoordinates.size());
		//glUniformMatrix4fv(TranslationID,1 , GL_FALSE, &Translate[0][0]);
		for(int i = 0; i< 4; i++){
			move(timeChange, i);
			glBindVertexArray(vao[i]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
			glUniformMatrix4fv(TranslationID, 1, GL_FALSE, &Translate[i][0][0]);

			glDrawArrays(GL_TRIANGLES, 0,coordinates[i].size());

		}
	//	glDrawArrays(GL_LINES, 0, sphereNormals.size());

	}

};

class SphereShaded {

	GLuint vao[2], vbo[2];
	GLchar *vertexsource, *fragmentsource; //receive contents of shader source
	GLuint vertexshader, fragmentshader; //reference shaders
	GLuint shaderprogram;
	vector<vertex> sphereCoordinates;
	vector<vertex> normals;
	GLuint MatrixID, TranslationID, LightV_ID, LightC_ID, MaterialID;
	float oldTime;
	glm::mat4 MVP, Model, Projection, View, Translation;
	glm::vec4 LightV, LightC, Material;
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
				normals.push_back(currentvert);

			}

	}

	return returnedPositions;

}

public:

	bool init(){

		shaderprogram = setupShaders(vertexsource, fragmentsource, vertexshader, fragmentshader, "shaded.vert", "shaded.frag");
		sphereCoordinates = getSphereCoordinates(1, 15, 15);

		
		
		glGenVertexArrays(1, vao);
		glBindVertexArray(vao[0]);

		glGenBuffers(1, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);	

		glEnableVertexAttribArray(0);

		glBufferData(GL_ARRAY_BUFFER, sphereCoordinates.size() * sizeof(vertex), &(sphereCoordinates[0]), GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindVertexArray(vao[1]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

		glEnableVertexAttribArray(1);

		glBufferData(GL_ARRAY_BUFFER, sphereCoordinates.size() * sizeof(vertex), &(sphereCoordinates[0]), GL_STATIC_DRAW);
		
		glVertexAttribPointer((GLuint) 1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindAttribLocation(shaderprogram, 0, "in_position");
		glBindAttribLocation(shaderprogram, 1, "in_normal");
		//main loop
		Projection = glm::perspective(45.0f, 1.f, 0.1f, 100.0f);
		View       = glm::lookAt(
			glm::vec3(2,2,2), // Camera is at (4,3,3), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
		Translation = glm::mat4(1.f);
		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);  // Changes for each model !
		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		// Get a handle for our "MVP" uniform.
		// Only at initialisation time.
		
		LightV = glm::vec4(1.f, 1.f, 1.f, 1.f);
		LightV_ID = glGetUniformLocation(shaderprogram, "LightV");
		glUniformMatrix4fv(LightV_ID, 1, GL_FALSE, &LightV[0]);

		LightC = glm::vec4(0.4f, 0.4f, 0.4f, 1.f);
		LightC_ID = glGetUniformLocation(shaderprogram, "LightC");
		glUniformMatrix4fv(LightC_ID, 1, GL_FALSE, &LightC[0]);
		
		Material = glm::vec4(.0f, .2f, .2f, 1.f);
		MaterialID = glGetUniformLocation(shaderprogram, "Material");
		glUniformMatrix4fv(MaterialID, 1, GL_FALSE, &Material[0]);
		
		/*
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		
		GLfloat light_ambient[] = {.2,.2,.2,1.};
		GLfloat light_diffuse[] = {.2,.2,.2,1.};
		GLfloat light_specular[] = {.2,.2,.2,1.};
		GLfloat light_position[] = {2., 2., 2., 0.};


		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
*/
		MatrixID = glGetUniformLocation(shaderprogram, "MVP");
		TranslationID = glGetUniformLocation(shaderprogram, "translation");
	

		return true;
	}

	void render(){
		glEnable(GL_DEPTH_TEST);
		glUseProgram(shaderprogram);
		float rotation, currentTime;			
		currentTime = static_cast<float>(glfwGetTime());
		rotation = (currentTime - oldTime) * speed;
		oldTime = currentTime;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//printf("rotation: %f\n", rotation);

		glBindVertexArray(vao[0]);
		//check("bind array");
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//check("bind buffer");
		
		View = glm::rotate(View, rotation, glm::vec3(1, 1, 1));
		Translation = glm::rotate(Translation, rotation, glm::vec3(1,1,0));
		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);  // Changes for each model !
		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		// Get a handle for our "MVP" uniform.

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		// For each model you render, since the MVP will be different (at least the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(TranslationID, 1, GL_FALSE, &Translation[0][0]);
		glUniform4fv(LightC_ID, 1, &LightC[0]);
		glUniform4fv(LightV_ID, 1, &LightV[0]);
		glUniform4fv(MaterialID, 1, &Material[0]);
		
		glDrawArrays(GL_TRIANGLES, 0, sphereCoordinates.size());
		/*
		printf("lightc: %i, lightv: %i, material: %i, matrixID: %i\n", LightC_ID, LightV_ID, MaterialID, MatrixID);
		printf("lightc: %f, %f, %f, %f\n", LightC[0], LightC[1], LightC[2], LightC[3]);
		printf("lightv: %f, %f, %f, %f\n", LightV[0], LightV[1], LightV[2], LightV[3]);
		printf("material: %f, %f, %f, %f\n", Material[0], Material[1], Material[2], Material[3]);
		*/
		
	}
};



int main(void){
	int running = GL_TRUE;
	char currentKey = 'A';
	SphereRotate sphereA;
	coneRotate coneB;
	SphereNormals normalsC;
	Paths pathsE;
	SphereShaded shadedD;
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
	
	if(!(sphereA.init() && coneB.init() && normalsC.init() && pathsE.init() && shadedD.init()))
		exit(EXIT_FAILURE);

	while( running ) {
		//render here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);

		if(currentKey == 'A'){
			sphereA.render();
		} else if(currentKey =='B'){
			coneB.render();
		} else if(currentKey == 'C'){
			normalsC.render();
		} else if(currentKey == 'D'){
			shadedD.render();
		} else if(currentKey == 'E'){
			pathsE.render();
		}
		//check("draw");
		glfwSwapBuffers();
		//check("swap buffers");
		//check esc key

		running= !(glfwGetKey(GLFW_KEY_ESC) || glfwGetKey('Q')) && glfwGetWindowParam(GLFW_OPENED);
		
		if(glfwGetKey('A'))
			currentKey = 'A';
		else if(glfwGetKey('B'))
			currentKey='B';
		else if(glfwGetKey('C'))
			currentKey='C';
		else if(glfwGetKey('D'))
			currentKey='D';
		else if(glfwGetKey('E'))
			currentKey = 'E';
		glFlush();
				
	}

	//end app

	glfwTerminate();

	exit(EXIT_SUCCESS);

}
