#include <GL/glew.h>
#include <glfw.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

GLuint vao[1], vbo[2]; //create handles for vertex array obj and 2 vertex buffer objects
const GLfloat diamond[4][2] = {
	{0., 0.5},
	{0.5, 0.0},
	{0.0, -0.5},
	{-0.5, 0.0}
};
const GLfloat colours[4][3] = {
	{1., 0., 0.},
	{0., 1.0, 0.},
	{0.0, 0., 1.},
	{1., 1.0, 1.}
};

GLchar *vertexsource, *fragmentsource; //receive contents of shader source

GLuint vertexshader, fragmentshader; //reference shaders

GLuint shaderprogram; //handle to shader program

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

void setupShaders(void) {
	char text[1000];
	int length;
	fprintf(stderr, "Set up shaders\n");
	//set up 2 vertex buffer objs
	vertexsource = fileToBuf("example2.vert");
	fragmentsource = fileToBuf("example2.frag");
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

void SetupGeometry(void){
    /*
    Bind our first VBO as being the active buffer and storing vertex attributes (coordinates)
    Bind our second VBO as being the active buffer and storing vertex attributes (colors)
    Copy the color data from colors to our buffer
    Copy the vertex data from diamond to our buffer
    8 * sizeof(GLfloat) is the size of the diamond array, since it contains 8 GLfloat values
    12 * sizeof(GLfloat) is the size of the colors array, since it contains 12 GLfloat values
    Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex
    Enable attribute index 0 as being used
    Enable attribute index 1 as being used
    Specify that our color data is going into attribute index 1, and contains three floats per vertex
    Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color
    */
    fprintf(stderr, "Set up vertices\n");
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[1]);
    glGenBuffers(2, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), diamond, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glEnableVertexAttribArray(1);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colours, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindAttribLocation(shaderprogram, 0, "in_Position");
    glBindAttribLocation(shaderprogram, 1, "in_Color");
    }  /* finished */


void Render(void) {
    /* Make our background black */
    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    check("Test point");
    glFlush();
    }


int main(void){
	int running = GL_TRUE;
	int k = 0;

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
	SetupGeometry();

	//main loop

	while( running ) {
		//render here
		
		Render();
		glfwSwapBuffers();

		//check esc key

		running= !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);

	}

	//end app

	glfwTerminate();

	exit(EXIT_SUCCESS);

}