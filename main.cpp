#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
#define ACCELERATION 0.1
#include <GL/glew.h>
#include <glfw.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>
#include <cstdio>
using namespace std;

glm::mat4 View, Projection, MVP;
GLuint ObjectMatrixID;
GLuint defaultShader;
GLuint textureIDs[14];
GLuint textureIDIndex = 0;
GLfloat shipStart = 22, shipEnd = 32, shipSpeed = 0.06, shipTimeTaken =0;
glm::vec3 tourpositions[7];
GLfloat tourhorizontals[7];
GLfloat tourverticals[7];

class drawable{
protected:
	GLuint textureID;
	GLfloat timeTaken;
	vector<glm::vec3> vVecs;
	vector<glm::vec2> vtVecs;
	vector<glm::vec3> nVecs;
	glm::mat4 Model;
	GLuint vao[1], vbo[3];
	

public:
	drawable(){}
	void init(vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> uvs) {
		setTexture();
		vVecs = vertices;
		vtVecs = uvs;
		nVecs = normals;


		Model = glm::mat4(1.f);

		glGenBuffers(3, vbo);
		glGenVertexArrays(1, vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vVecs.size() * sizeof(glm::vec3), &vVecs[0], GL_STATIC_DRAW);
		glBindVertexArray(vao[0]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const GLvoid*)0);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, vtVecs.size() * sizeof(glm::vec2), &vtVecs[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, nVecs.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW); 
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const GLvoid*)0);

	}
	virtual void setTexture() = 0;
	virtual void animate(GLfloat time) = 0;
	void resetAnimation(){

		timeTaken = 0;
		
	}
	void draw(GLfloat time) {
		animate(time);
		MVP = Projection * View * Model;
		glUniformMatrix4fv(ObjectMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBindVertexArray(vao[0]);
		glDrawArrays(GL_TRIANGLES, 0, vVecs.size());
	}
};
struct textureMaps{
	char * name;
	GLuint id;
};
vector<drawable*>  objects;
textureMaps * textureMappings = (textureMaps*) malloc(14* sizeof(textureMaps));



GLuint lookupTexture(char * name){
	for(int i = 0; i<14; i++){
		if(strcmp(name, textureMappings[i].name) == 0){
			return textureMappings[i].id;
		}
	}
	return -1;
}

GLuint loadTGA_glfw(const char * imagepath){
	glEnable(GL_TEXTURE_2D);
    // Create one OpenGL texture
    
	glActiveTexture(GL_TEXTURE0);
    // "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureIDs[textureIDIndex]);
 
    // Read the file, call glTexImage2D with the right parameters
    glfwLoadTexture2D(imagepath, 0);
 
    // Nice trilinear filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Return the ID of the texture we just created
	return textureIDs[textureIDIndex++];
}

void init_textures(){

	textureMappings[0].name = "scratch-metal";
	textureMappings[0].id = loadTGA_glfw("MetalBare0145_S.tga");

	textureMappings[1].name = "window-blocks";
	textureMappings[1].id = loadTGA_glfw("WindowsBlocks0052_S.tga");

	textureMappings[2].name = "sand";
	textureMappings[2].id = loadTGA_glfw("SoilSand0068_1_S.tga");

	textureMappings[3].name = "solar-panel";
	textureMappings[3].id = loadTGA_glfw("Various0317_6_S.tga");

	textureMappings[4].name = "metal-floor";
	textureMappings[4].id = loadTGA_glfw("MetalFloorsPainted0044_36_S.tga");

	textureMappings[5].name = "odd-glass";
	textureMappings[5].id = loadTGA_glfw("WindowsOther0067_S.tga");

	textureMappings[6].name = "metal-red";
	textureMappings[6].id = loadTGA_glfw("MetalPainted0121_2_S.tga");

	textureMappings[7].name = "tiles";
	textureMappings[7].id = loadTGA_glfw("TilesPlain0057_7_S.tga");

	textureMappings[8].name = "concrete-bare";
	textureMappings[8].id = loadTGA_glfw("ConcreteBare0314_2_S.tga");

	textureMappings[9].name = "concrete-plate";
	textureMappings[9].id = loadTGA_glfw("ConcretePlates0116_1_S.tga");

	textureMappings[10].name = "concrete-new";
	textureMappings[10].id = loadTGA_glfw("ConcreteNew0011_2_S.tga");

	textureMappings[11].name = "blacksquare";
	textureMappings[11].id = loadTGA_glfw("blackSquare.tga");

	textureMappings[12].name = "fibre-glass";
	textureMappings[12].id = loadTGA_glfw("Fiberglass0006_S.tga");

	textureMappings[13].name = "metal-white";
	textureMappings[13].id = loadTGA_glfw("MetalPainted0132_S.tga");


}

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

class solarPane : public drawable {

public:
	void setTexture(){

		textureID = lookupTexture("solar-panel");

	}

	void animate(GLfloat time){}

};
class solarBase : public drawable {

public:
	void setTexture(){
		
		textureID = lookupTexture("concrete-bare");
		
	}
	void animate(GLfloat time){}
};
class observatoryTop : public drawable {
public:
	void setTexture(){
		
		textureID = lookupTexture("window-blocks");
		
	}
	void animate(GLfloat time){}
};

class observatoryBase : public drawable {
public:
	void setTexture(){
		textureID = lookupTexture("scratch-metal");
		
	}
	void animate(GLfloat time){}

};

class landscape : public drawable {

public:
	void setTexture(){
		
		textureID = lookupTexture("sand");
		
	}
	void animate(GLfloat time){}
};

class path : public drawable {

public:
	void setTexture(){
		
		textureID = lookupTexture("odd-glass");
		
	}
	void animate(GLfloat time){}
};

class livingquarters : public drawable {
public:
	void setTexture(){
		
		textureID = lookupTexture("concrete-bare");
		
	}
	void animate(GLfloat time){}
};

class blackSquare : public drawable {

public:
	void setTexture(){
		

		textureID = lookupTexture("blacksquare");
		
	}
	void animate(GLfloat time){}

};

class smallBuilding : public drawable {


public:
	void setTexture(){

		textureID = lookupTexture("fibre-glass");
		
	}
	void animate(GLfloat time){}
};

class radar : public drawable {

public:
	void setTexture(){

		textureID = lookupTexture("tiles");

	}
	void animate(GLfloat time){}
};

class dishSupport : public drawable {

public:
	void setTexture(){

		textureID = lookupTexture("concrete-bare");

	}
	void animate(GLfloat time){}
};

class dish : public drawable {

public:
	void setTexture(){

		textureID = lookupTexture("scratch-metal");
		
	}
	void animate(GLfloat time){}
};

class aerial : public drawable {

public:
	void setTexture(){

		textureID = lookupTexture("metal-white");
	}
	void animate(GLfloat time){}
};

class shipcone : public drawable {

public:
	void setTexture(){
		textureID = lookupTexture("metal-red");
	}
	void animate(GLfloat time){
	
		if(shipTimeTaken > shipStart && shipTimeTaken < shipEnd){

			Model = glm::translate(Model, glm::vec3(0,shipSpeed*time*(shipTimeTaken - shipStart),0));

		}else {
			Model = glm::mat4(1.f);
		}
	
	}
};

class shipfin : public drawable {

public:
	void setTexture(){
		textureID = lookupTexture("metal-red");

	}
	void animate(GLfloat time){
		if(shipTimeTaken > shipStart && shipTimeTaken < shipEnd){

			Model = glm::translate(Model, glm::vec3(0,shipSpeed*time*(shipTimeTaken - shipStart),0));

		} else {
			Model = glm::mat4(1.f);
		}
	}
};

class shipbody : public drawable {

public:
	void setTexture(){
		textureID = lookupTexture("metal-white");
	}
	void animate(GLfloat time){
		if(shipTimeTaken > shipStart && shipTimeTaken < shipEnd){

			Model = glm::translate(Model, glm::vec3(0,shipSpeed*time*(shipTimeTaken - shipStart),0));

		}else {
			Model = glm::mat4(1.f);
		}
	}
};

class shipexhaust : public drawable {
public:
	void setTexture(){
		
		textureID = lookupTexture("metal-floor");
		
	}
	void animate(GLfloat time){
		if(shipTimeTaken > shipStart && shipTimeTaken < shipEnd){

			Model = glm::translate(Model, glm::vec3(0,shipSpeed*time*(shipTimeTaken - shipStart),0));

		}else {
			Model = glm::mat4(1.f);
		}
	}
};

class launchTower : public drawable {

public:
	void setTexture(){
		textureID = lookupTexture("concrete-plate");
	}
	void animate(GLfloat time){}
};

class launchpadLeft : public drawable {
private:
	GLfloat speed, timeStart, timeEnd, delay;

public:
	void setTexture(){
		
		textureID = lookupTexture("tiles");
		speed = 0.01;
		timeStart = 17;
		timeEnd = 22;
		timeTaken = 0;
		delay = 5;
	}
	void animate(GLfloat time){
		
		timeTaken += time;

		if(timeTaken > timeStart && timeTaken < timeEnd){

			Model = glm::translate(Model, glm::vec3(0,0,speed*time));

		} else if(timeTaken > timeEnd + delay && timeTaken < (2 * timeEnd) + delay - timeStart){
			Model = glm::translate(Model, glm::vec3(0,0,-speed*time));
		}

	}
};

class launchpadRight : public drawable {
private:
	GLfloat speed, timeStart, timeEnd, delay;

public:
	void setTexture(){
		
		textureID = lookupTexture("tiles");
		speed = 0.01;
		timeStart = 17;
		timeEnd = 22;
		timeTaken = 0;
		delay = 5;
	}
	void animate(GLfloat time){
	
		timeTaken += time;

		if(timeTaken > timeStart && timeTaken < timeEnd){

			Model = glm::translate(Model, glm::vec3(0,0,-speed*time));

		} else if(timeTaken > timeEnd + delay && timeTaken < (2 * timeEnd) + delay - timeStart){
			Model = glm::translate(Model, glm::vec3(0,0,speed*time));
		}


	}
};
class skybox : public drawable {

public:
	void setTexture(){

		textureID = lookupTexture("tiles");
	}
	void animate(GLfloat time){}
};

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

void objectInit(){

	char * file = fileToBuf("mars.obj");
	drawable *m = NULL;
	char * line = strtok(file, "\n\r");
	
	vector<glm::vec3> temp_v;
	vector<glm::vec2> temp_vt;
	vector<glm::vec3> temp_vn;

	vector<glm::vec3> * current_v;
	vector<glm::vec2>  * current_vt;
	vector<glm::vec3>  * current_vn;

	


	while(line !=NULL){

		if(line[0] == 'o'){

			if(m != NULL) {

				m->init(*current_v, *current_vn, *current_vt);
				objects.push_back(m);

			}
			current_v = new vector<glm::vec3>();
			current_vn = new vector<glm::vec3>();
			current_vt = new vector<glm::vec2>();

			if(strstr(&line[1], "Dish")){

				m = new dish();

			} else if(strstr(&line[1], "solarPanel")){

				m = new solarBase();

			} else if(strstr(&line[1], "aerial")){

				m = new aerial();

			} else if(strstr(&line[1], "dish-support")){

				m = new dishSupport();

			} else if(strstr(&line[1], "exhaust")){

				m = new shipexhaust();

			} else if(strstr(&line[1], "fin")){

				m = new shipfin();

			} else if(strstr(&line[1], "landpadleft")){

				m = new launchpadLeft();

			} else if(strstr(&line[1], "landpadright")){

				m = new launchpadRight();

			}else if(strstr(&line[1], "landscape")){

				m = new landscape();

			} else if(strstr(&line[1], "launchtower")){

				m = new launchTower();

			} else if(strstr(&line[1], "living-quarters")){

				m = new livingquarters();

			} else if(strstr(&line[1], "observatorybase")){

				m = new observatoryBase();

			} else if(strstr(&line[1], "observatory")){

				m = new observatoryTop();

			} else if(strstr(&line[1], "path")){

				m = new path();

			} else if(strstr(&line[1], "radar")){

				m = new radar();

			} else if(strstr(&line[1], "ship-body")){

				m = new shipbody();

			} else if(strstr(&line[1], "ship-cone")){

				m = new shipcone();

			} else if(strstr(&line[1], "skybox")){

				m = new skybox();

			} else if(strstr(&line[1], "small-building")){

				m = new smallBuilding();

			} else if(strstr(&line[1], "Panel")){

				m = new solarPane();

			} else if(strstr(&line[1], "blacksquare")){

				m = new blackSquare();

			}

		} else if(line[0] == 'v' && line[1] == 't'){

			glm::vec2 vertex;
			sscanf(&line[3], "%f %f", &vertex.x, &vertex.y);
			temp_vt.push_back(vertex);

		} else if(line[0] == 'v' && line[1] == 'n'){

			glm::vec3 vertex;
			sscanf(&line[3], "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			temp_vn.push_back(vertex);

		} else if(line[0] == 'v'){

			glm::vec3 vertex;
			sscanf(&line[2], "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			temp_v.push_back(vertex);

		} else if(line[0] == 'f'){

			unsigned long vertex[3], uv[3], normals[3];
			sscanf(&line[2], "%d/%d/%d %d/%d/%d %d/%d/%d", &vertex[0], &uv[0], &normals[0], &vertex[1], &uv[1], &normals[1], &vertex[2], &uv[2], &normals[2]);

			for(int i = 0; i < 3; i++){

				current_v->push_back(temp_v[vertex[i]-1]);
				current_vt->push_back(temp_vt[uv[i]-1]);
				current_vn->push_back(temp_vn[normals[i]-1]);

			}

		}
		line = strtok(NULL, "\n\r");
	}

}

void gl_init_things(){
	
	GLuint vertexShader = NULL, fragmentShader = NULL;
	GLchar * vertexsource = NULL, * fragmentsource = NULL;
	if(!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	if(!glfwOpenWindow(600, 600, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glewInit();

	glGenTextures(14, textureIDs);
	init_textures();
	objectInit();

	glEnable(GL_DEPTH_TEST);

	defaultShader = setupShaders(vertexsource , fragmentsource, vertexShader, fragmentShader, "shaded.vert", "shaded.frag");

	Projection = glm::perspective(45.0f, 1.f, 0.1f, 100.0f);

	

	glUseProgram(defaultShader);
	ObjectMatrixID = glGetUniformLocation(defaultShader, "MVP");

	glm::vec4 LightV = glm::vec4(0.f, 2.f, -2.f, 1.f);
	GLuint LightVID = glGetUniformLocation(defaultShader, "LightV");
	glUniform4fv(LightVID, 1, &LightV[0]);

	glm::vec4 LightC = glm::vec4(.11f, .11f, .11f, 1.f);
	GLuint LightCID = glGetUniformLocation(defaultShader, "LightC");
	glUniform4fv(LightCID, 1, &LightC[0]);

	glm::vec4 Material = glm::vec4(1.f, 0.f, 0.f, 1.f);
	GLuint MaterialID = glGetUniformLocation(defaultShader, "Material");
	glUniform4fv(MaterialID, 1, &Material[0]);

	tourpositions[0] = glm::vec3(-0.232165, 0.2, 0.493988);
	tourhorizontals[0] = 5.060204;
	tourverticals[0] = 0;

	tourpositions[1] = glm::vec3(-0.400240, 0.2, -0.414293);
	tourhorizontals[1] = 5.803089;
	tourverticals[1] = 0;

	tourpositions[2] = glm::vec3(-0.400240, 0.2, -0.414293);
	tourhorizontals[2] = 1.96857;
	tourverticals[2] = 0;

	tourpositions[3] = glm::vec3(0.093397, 0.051970, -0.444866);
	tourhorizontals[3] = 1.632612;
	tourverticals[3] = 0;

	tourpositions[4] = glm::vec3(0.093397, 0.051970, -0.444866);
	tourhorizontals[4] = 1.01016;
	tourverticals[4] = 0;

	tourpositions[5] = glm::vec3(0.156583, 0.362405, -0.235277);
	tourhorizontals[5] = 1.815373;
	tourverticals[5] = -0.656652;

	tourpositions[6] = glm::vec3(0.156583, 0.362405, -0.235277);
	tourhorizontals[6] = 1.639920;
	tourverticals[6] = 1.1;

}

void tourUpdate(glm::vec3 * position, GLfloat * horizontalAngle, GLfloat * verticalAngle, GLfloat timeDiff){

	GLint positionToUse;
	GLfloat timeLeft;
	bool update = true;
	if(shipTimeTaken < 5){

		positionToUse = 0;
		timeLeft = 5-shipTimeTaken;

	} else if(shipTimeTaken < 10){

		positionToUse = 2;
		timeLeft = 10-shipTimeTaken;

	} else if(shipTimeTaken < 11){

		positionToUse = 3;
		timeLeft = 11-shipTimeTaken;

	} else if(shipTimeTaken < 15){

		positionToUse = 4;
		timeLeft = 15-shipTimeTaken;

	} else  if(shipTimeTaken < 17){
		positionToUse = 5;
		timeLeft = 17-shipTimeTaken;
	} else if(shipTimeTaken > 24 && shipTimeTaken < 29){
		positionToUse = 6;
		timeLeft = 29-shipTimeTaken;
	} else {
		update = false;
	}

	if (update){
		position[0] += (tourpositions[positionToUse] - position[0]) * (timeDiff/timeLeft);
		horizontalAngle[0] += (tourhorizontals[positionToUse] - horizontalAngle[0]) * (timeDiff/timeLeft);
		verticalAngle[0] += (tourverticals[positionToUse] - verticalAngle[0]) * (timeDiff/timeLeft);
	}
}

int main(void){
	int running = GL_TRUE;
	
	gl_init_things();

	GLfloat oldTime = static_cast<float>(glfwGetTime());
	char lastPressedKey = ' ';

	GLfloat speed = 0;
	GLfloat horizontalAngle = 2.447389f;
	GLfloat verticalAngle = 0.0f;
	GLfloat initialFoV = 45.0f;
	glm::vec3 position = glm::vec3( -.232165, .2, .493988 );
	bool tour = false;
	while(running){
		
		GLfloat currentTime = static_cast<float>(glfwGetTime());
		GLfloat timeDiff = currentTime - oldTime;
		oldTime = currentTime;

		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(defaultShader);

		shipTimeTaken += timeDiff;

		glm::vec3 direction(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);

		position += direction * timeDiff * speed;

		

		View = glm::lookAt(
			position,           // Camera is here
			position+direction, // and looks here : at the same position, plus "direction"
			glm::vec3(0,1,0)                  // Head is up (set to 0,-1,0 to look upside-down)
		);

		
		if(shipEnd <= shipTimeTaken){

			for(int i = 0; i< objects.size(); i++){

				objects[i]->resetAnimation();

			}
			shipTimeTaken = 0;
			
			printf("animation reset\n");
		}
		for(unsigned int i = 0; i<objects.size();i++){

			objects[i]->draw(timeDiff);

		}
		glfwSwapBuffers();

		running= !(glfwGetKey(GLFW_KEY_ESC) || glfwGetKey('Q')) && glfwGetWindowParam(GLFW_OPENED);
		if(tour){
			tourUpdate(&position, &horizontalAngle, &verticalAngle, timeDiff);
			
			if(shipTimeTaken >= 30) {
				tour=false;
				printf("tour ended\n");
			}
		}
		else if(glfwGetKey('T')){
			tour = true;
			shipTimeTaken = shipEnd;
			lastPressedKey = 'T';
		}
		else if(glfwGetKey('H') && lastPressedKey!= 'H'){
		
			printf("KEY\t\tFUNCTION\n\n");
			printf("H\t\tHELP\n");
			printf("T\t\tTOUR\n");
			printf("UP\t\tACCELERATE\n");
			printf("DOWN\t\tDECELERATE\n");
			printf("LEFT\t\tROTATE COUNTER CLOCKWISE\n");
			printf("RIGHT\t\tROTATE CLOCKWISE\n");
			printf("PAGEUP\t\tINCREASE ELEVATION\n");
			printf("PAGEDOWN\t\tDECREASE ELEVATION\n");
			printf("P\t\tTAKE TO SCREEN SHOT LOCATION\n");
			printf("=\t\tPRINT CURRENT POSITION AND DIRECTION\n");
			printf("HOME\t\tLOOK UP\n");
			printf("END\t\tLOOK DOWN\n");
			lastPressedKey = 'H';

		}
		else if(glfwGetKey(GLFW_KEY_UP)){
			
			speed+= timeDiff * ACCELERATION;
			lastPressedKey = 'z';
		}
		else if(glfwGetKey(GLFW_KEY_DOWN)){
			
			speed = speed - (timeDiff*ACCELERATION);
			if(speed<0) speed = 0;
			lastPressedKey = 'z';
		}
		else if(glfwGetKey(GLFW_KEY_RIGHT)){
			horizontalAngle = horizontalAngle - 1 * timeDiff;
			lastPressedKey = 'z';
		}
		else if(glfwGetKey(GLFW_KEY_LEFT)){
			horizontalAngle = horizontalAngle + 1 * timeDiff;
			lastPressedKey = 'z';
		}
		else if(glfwGetKey(GLFW_KEY_PAGEUP)){
			position += glm::vec3(0,0.1*timeDiff,0);
			lastPressedKey = 'z';
		}
		else if(glfwGetKey(GLFW_KEY_PAGEDOWN)){
			position += glm::vec3(0,-0.1*timeDiff,0);
			lastPressedKey = 'z';
		}
		else if(glfwGetKey('=') && lastPressedKey != '='){
			printf("Position: %f:%f:%f\n",position.x, position.y, position.z);
			printf("Direction: %f:%f\n", horizontalAngle, verticalAngle);
			lastPressedKey = '=';
		}
		else if(glfwGetKey(GLFW_KEY_HOME)){
			verticalAngle = verticalAngle + 1 * timeDiff;
			lastPressedKey = 'z';
		}
		else if(glfwGetKey(GLFW_KEY_END)){
			verticalAngle = verticalAngle - 1 * timeDiff;
			lastPressedKey = 'z';
		}

		

		glFlush();
	}

	return 0;
}