#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glew.h>
#include <glfw.h>
#include <stdlib.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>
#include <cstdio>
using namespace std;

glm::mat4 View, Projection, MVP;

class drawable{
private:
	GLuint textureID;
	
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
	virtual void animate(GLulong) = 0;
	
	void draw(GLulong time) {

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

textureMaps * textureMappings = (textureMaps*) malloc(14* sizeof(textureMaps));
GLuint defaultShader;
vector<drawable*>  objects;

GLuint ObjectMatrixID;

GLuint lookupTexture(char * name){
	for(int i = 0; i<14; i++){
		if(strcmp(name, textureMappings[i].name)){
			return textureMappings[i].id;
		}
	}
	return -1;
}

GLuint loadTGA_glfw(const char * imagepath){
	glEnable(GL_TEXTURE_2D);
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
 
    // Read the file, call glTexImage2D with the right parameters
    glfwLoadTexture2D(imagepath, 0);
 
    // Nice trilinear filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
 
    // Return the ID of the texture we just created
    return textureID;
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

	textureMappings[11].name = "bulkhead";
	textureMappings[11].id = loadTGA_glfw("MetalBulkheads0048_5_M.tga");

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

	void animate(){}

};
class solarBase : public drawable {

public:
	void setTexture(){
		
		textureID = lookupTexture("concrete-bare");
		
	}
	void animate(){}
};
class observatoryTop : public drawable {
public:
	void setTexture(){
		
		textureID = lookupTexture("window-blocks");
		
	}
	void animate(){}
};

class observatoryBase : public drawable {
public:
	void setTexture(){
		textureID = lookupTexture("scratch-metal");
		
	}
	void animate(){}

};

class landscape : public drawable {

public:
	void setTexture(){
		
		textureID = lookupTexture("sand");
		
	}
	void animate(){}
};

class path : public drawable {

public:
	void setTexture(){
		
		textureID = lookupTexture("odd-glass");
		
	}
	void animate(){}
};

class livingquarters : public drawable {
public:
	void setTexture(){
		
		textureID = lookupTexture("concrete-bare");
		
	}
	void animate(){}
};

class bridge : public drawable {

public:
	void setTexture(){
		

		textureID = lookupTexture("bulkhead");
		
	}
	void animate(){}

};

class smallBuilding : public drawable {


public:
	void setTexture(){

		textureID = lookupTexture("fibre-glass");
		
	}
	void animate(){}
};

class radar : public drawable {

public:
	void setTexture(){

		textureID = lookupTexture("tiles");

	}
	void animate(){}
};

class dishSupport : public drawable {

public:
	void setTexture(){

		textureID = lookupTexture("concrete-bare");

	}
	void animate(){}
};

class dish : public drawable {

public:
	void setTexture(){

		textureID = lookupTexture("scratch-metal");
		
	}
	void animate(){}
};

class aerial : public drawable {

public:
	void setTexture(){

		textureID = lookupTexture("metal-white");
	}
	void animate(){}
};

class shipcone : public drawable {

public:
	void setTexture(){
		textureID = lookupTexture("metal-red");
	}
	void animate(){}
};

class shipfin : public drawable {

public:
	void setTexture(){
		textureID = lookupTexture("metal-red");

	}
	void animate(){}
};

class shipbody : public drawable {

public:
	void setTexture(){
		textureID = lookupTexture("metal-white");
	}
	void animate(){}
};

class shipexhaust : public drawable {
public:
	void setTexture(){
		
		textureID = lookupTexture("metal-floor");
		
	}
	void animate(){}
};

class launchTower : public drawable {

public:
	void setTexture(){
		textureID = lookupTexture("concrete-plate");
	}
	void animate(){}
};

class launchpad : public drawable {

public:
	void setTexture(){
		
		textureID = lookupTexture("tiles");
		
	}
	void animate(){}
};

class skybox : public drawable {

public:
	void setTexture(){

		textureID = lookupTexture("tiles");
	}
	void animate(){}
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

			} else if(strstr(&line[1], "landpad")){

				m = new launchpad();

			} else if(strstr(&line[1], "landscape")){

				m = new landscape();

			} else if(strstr(&line[1], "launchtower")){

				m = new launchTower();

			} else if(strstr(&line[1], "living-quarters")){

				m = new livingquarters();

			} else if(strstr(&line[1], "observatory")){

				m = new observatoryTop();

			} else if(strstr(&line[1], "observatorybase")){

				m = new observatoryBase();

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

			} else if(strstr(&line[1], "walkway")){

				m = new bridge();

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


int main(void){
	int running = GL_TRUE;
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

	init_textures();
	objectInit();

	defaultShader = setupShaders(vertexsource , fragmentsource, vertexShader, fragmentShader, "shaded.vert", "shaded.frag");

	Projection = glm::perspective(45.0f, 1.f, 0.1f, 100.0f);
	View       = glm::lookAt(
		glm::vec3(3,3,3), 
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	glUseProgram(defaultShader);
	ObjectMatrixID = glGetUniformLocation(defaultShader, "MVP");

	glm::vec4 LightV = glm::vec4(0.f, 0.f, .8f, 1.f);
	GLuint LightVID = glGetUniformLocation(defaultShader, "LightV");
	glUniform4fv(LightVID, 1, &LightV[0]);

	glm::vec4 LightC = glm::vec4(0.6f, .2f, .2f, 1.f);
	GLuint LightCID = glGetUniformLocation(defaultShader, "LightC");
	glUniform4fv(LightCID, 1, &LightC[0]);

	glm::vec4 Material = glm::vec4(1.f, 0.f, 0.f, 1.f);
	GLuint MaterialID = glGetUniformLocation(defaultShader, "Material");
	glUniform4fv(MaterialID, 1, &Material[0]);

	while(running){

		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(defaultShader);

		for(unsigned int i = 0; i<objects.size();i++){

			objects[i]->draw(0);

		}

	}

	return 0;
}