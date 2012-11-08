#include <glfw.h>
#include <stdlib.h>

int main(void){
	int running = GL_TRUE;
	int k = 0;

	//initialise GLFW

	if(!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	//open window

	if(!glfwOpenWindow(300, 300, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//main loop

	while( running ) {
		//render here
		if(k <= 70) {
			glClearColor(1., 1., 0., 1.);
		} else {
			glClearColor(0.,1.,1.,1.);
		}

		glClear(GL_COLOR_BUFFER_BIT);
		//swap render buffers
		glfwSwapBuffers();
		k = k+1;
		if(k>140) k=0;
		//check esc key

		running= !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);

	}

	//end app

	glfwTerminate();

	exit(EXIT_SUCCESS);

}