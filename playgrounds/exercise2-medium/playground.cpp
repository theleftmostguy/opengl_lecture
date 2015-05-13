// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

std::string getFileContents(const char * fname) {
	std::string ret;
	std::ifstream stream(fname, std::ios::in);
	if(stream.is_open()){
		std::string Line = "";
		while(getline(stream, Line))
			ret += "\n" + Line;
		stream.close();
	} else {
		fprintf(stderr, "Impossible to open %s. Are you in the right directory ?\n", fname);
		getchar();
		ret = "ERROR READING FILE";
	}
	return ret;
}

class ShaderProgram {
private:
	GLuint programID;
	void compileShader(GLuint shaderID, std::string src) {
		GLint Result = GL_FALSE;
		int InfoLogLength;
		char const * srcptr = src.c_str();
		glShaderSource(shaderID, 1, &srcptr , NULL);
		glCompileShader(shaderID);
		// Check Vertex Shader
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			std::vector<char> errormsg(InfoLogLength+1);
			glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &errormsg[0]);
			printf("%s\n", &errormsg[0]);
		}
	}
	GLuint LoadShader(GLenum shaderType, const char * filename) {
		GLuint shaderId = glCreateShader(shaderType);
		std::string code = getFileContents(filename);
		compileShader(shaderId, code);
		return shaderId;
	}
public:
	ShaderProgram() {
		programID = glCreateProgram();
	}
	void addShader(GLenum shaderType, const char * filename) {
		std::cout << "Loading shader from " << filename << std::endl;
		GLuint newShader = LoadShader(shaderType, filename);
		glAttachShader(programID, newShader);
		glDeleteShader(newShader);
	}
	GLuint finalize() {
		glLinkProgram(programID);
		GLint Result = GL_FALSE;
		int InfoLogLength;
		glGetProgramiv(programID, GL_LINK_STATUS, &Result);
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			std::vector<char> ProgramErrorMessage(InfoLogLength+1);
			glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			fprintf(stderr,"%s\n", &ProgramErrorMessage[0]);
		}
		return programID;
	}
};

// An array of 3 vectors which represents 3 vertices
static const int NUM_CIRCLE_VERTICES = 50;
static GLfloat g_vertex_buffer_data[3*NUM_CIRCLE_VERTICES];

int main( void )
{
	for (int i=0; i<NUM_CIRCLE_VERTICES; i++) {
		float f = i * 2*M_PI / NUM_CIRCLE_VERTICES;
		g_vertex_buffer_data[3*i] = cos(f)+2.0f;
		g_vertex_buffer_data[3*i+1] = sin(f);
		g_vertex_buffer_data[3*i+2] = 0.0f;
	}

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 01", NULL, NULL);
	if( window == NULL ){
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			fprintf(stderr, "-1 OpenGL Error: %d\n", error);
		}
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
	    fprintf(stderr, "-1 OpenGL Error: %d\n", error);
	}

	glewExperimental = GL_TRUE;
	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}


	error = glGetError();
	if (error != GL_NO_ERROR)
	{
	    fprintf(stderr, "0 OpenGL Error: %d\n", error);
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	
	
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	//glEnable (GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	 
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	 
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	 
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// load shaders
	ShaderProgram shaderProgram;
	shaderProgram.addShader(GL_VERTEX_SHADER, "vertex");
	shaderProgram.addShader(GL_FRAGMENT_SHADER, "fragment");
	shaderProgram.addShader(GL_GEOMETRY_SHADER, "geometry");
	GLuint shader = shaderProgram.finalize();
	
	// matrices
	glm::mat4 projection = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 100.0f);
	glm::mat4 view       = glm::lookAt(
		glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 MVP0 = projection * view;
	
	glm::vec4 x_vec = MVP0 * glm::vec4(1.0f,0.0f,0.0f,1.0f);
	glm::vec4 z_vec = MVP0 * glm::vec4(0.0f,0.0f,1.0f,1.0f);

	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	float offset = 0.0f;
	do{
		offset += 1.f;
		// clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // actually use our shaders
		glUseProgram(shader);
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f),offset,glm::vec3(0,1,0));
        glm::mat4 MVP = MVP0 * rotation;
		// set offset in shader
		GLint offsetloc = glGetUniformLocation(shader,"offset");
		glUniform1f(offsetloc,offset);
		GLuint MVPloc = glGetUniformLocation(shader,"MVP");
		GLint segcntloc = glGetUniformLocation(shader,"segcount");
		int MAXSEGMENTS = 80;
		int segments = ((int) offset) % (2*MAXSEGMENTS);
		if (segments > MAXSEGMENTS)
			segments = 2*MAXSEGMENTS-segments;
		glUniform1i(segcntloc,segments);
		glUniformMatrix4fv(MVPloc, 1, GL_FALSE, &MVP[0][0]);
		// some gl magic; look it up.
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glDrawArrays(GL_LINE_LOOP, 0, NUM_CIRCLE_VERTICES);
		glDisableVertexAttribArray(0);
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			fprintf(stderr, "0 OpenGL Error: %d\n", error);
		}
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

