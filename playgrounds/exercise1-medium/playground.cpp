// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// An array of 3 vectors which represents 3 vertices
static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

static const GLfloat cube_vertex_data[] = {
	-0.5f,-0.5f, 0.5f,
	 0.5f,-0.5f, 0.5f,
	 0.5f,-0.5f,-0.5f,
	-0.5f,-0.5f,-0.5f,
	-0.5f, 0.5f, 0.5f,
	 0.5f, 0.5f, 0.5f,
	 0.5f, 0.5f,-0.5f,
	-0.5f, 0.5f,-0.5f,
};

static const GLubyte cube_indices[] = {
//	0,1,3,2,6,1,5,0,4,3,7,6,4,5, // facing inside
	0,3,1,2,6,3,7,0,4,1,5,6,4,7, // facing outside
};

GLuint MyLoadShaders(const char * vertex_file_path,const char * fragment_file_path) { 
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
 
    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
 
    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
 
    GLint Result = GL_FALSE;
    int InfoLogLength;
 
    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
 
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
 
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
 
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
 
    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
 
    return ProgramID;
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
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

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	
	//glEnable (GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	 
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	 
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	 
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertex_data), cube_vertex_data, GL_STATIC_DRAW);

	// load shaders
	GLuint shader = MyLoadShaders("cube_vertex","cube_fragment");

	// matrices
	glm::mat4 projection = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 100.0f);
	glm::mat4 view       = glm::lookAt(
		glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 MVP0 = projection * view;

	int i=3;
	float j=45.0f;
	bool canInc = true;
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	do{
		// clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
        // actually use our shaders
        glUseProgram(shader);
        // rotate the cube by angle j
		j = j+1.0f;
		if (j>360.0f)
			j -= 360.0f;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f),j,glm::vec3(0,1,0));
        glm::mat4 MVP = MVP0 * rotation;
       	GLuint MVP_handle = glGetUniformLocation(shader,"MVP");
       	// copy the matrix to the gpu
       	glUniformMatrix4fv(MVP_handle, 1, GL_FALSE, &MVP[0][0]);
		// Draw the triangle !
//		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLE_STRIP, i, GL_UNSIGNED_BYTE, cube_indices);
		glDisableVertexAttribArray(0);

		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			fprintf(stderr, "0 OpenGL Error: %d\n", error);
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			if (canInc) {
				canInc = false;
				i++;
				if (i>14)
					i=3;
				printf("%d\n",i);
			}
		} else {
			canInc = true;
		}
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

