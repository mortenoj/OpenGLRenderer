#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "SOIL2/SOIL2.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include "Texture.h"


// Window dimensions
const GLuint WIDTH = 1200, HEIGHT = 800;
int SCREEN_WIDTH, SCREEN_HEIGHT;

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = WIDTH / 2.0f;
bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, -2.0f);

// The MAIN function, from here we start the application and run the game loop
int main() {
    // Init GLFW
    glfwInit();
    
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Game for funs", nullptr, nullptr);
    
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent( window );
    
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
    
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) ) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    
    glEnable(GL_DEPTH_TEST);
    
    // enable alpha support
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Setup and compile our shaders
    Shader shader( "res/shaders/cube.vs", "res/shaders/cube.frag" );
    Shader skyboxShader( "res/shaders/skybox.vs", "res/shaders/skybox.frag" );
    
    GLfloat cubeVertices[] = {
        // Positions          // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    
    GLfloat skyboxVertices[] = {
        // Positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    
    // Setup cube VAO
    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays( 1, &cubeVAO );
    glGenBuffers( 1, &cubeVBO );
    glBindVertexArray( cubeVAO );
    glBindBuffer( GL_ARRAY_BUFFER, cubeVBO );
    
    glBufferData( GL_ARRAY_BUFFER, sizeof( cubeVertices ), &cubeVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), ( GLvoid * ) 0 );
    glEnableVertexAttribArray( 1 );
    
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), ( GLvoid * )( 3 * sizeof( GLfloat ) ) );
    glBindVertexArray(0);
    
    // Setup skybox VAO
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays( 1, &skyboxVAO );
    glGenBuffers( 1, &skyboxVBO );
    glBindVertexArray( skyboxVAO );
    glBindBuffer( GL_ARRAY_BUFFER, skyboxVBO );
    
    glBufferData( GL_ARRAY_BUFFER, sizeof( skyboxVertices ), &skyboxVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), ( GLvoid * ) 0 );
    glBindVertexArray(0);
    
    // Load textures
    GLuint cubeTexture = TextureLoading::LoadTexture( "res/images/container2.png" );
    
    // Cubemap (Skybox)
    vector<const GLchar*> faces;
    faces.push_back( "res/images/skybox/right.tga" );
    faces.push_back( "res/images/skybox/left.tga" );
    faces.push_back( "res/images/skybox/top.tga" );
    faces.push_back( "res/images/skybox/bottom.tga" );
    faces.push_back( "res/images/skybox/back.tga" );
    faces.push_back( "res/images/skybox/front.tga" );
    GLuint cubemapTexture = TextureLoading::LoadCubemap( faces );

    
    // FOV of camera
    glm::mat4 projection(1);
    projection = glm::perspective(camera.getZoom(), (GLfloat) SCREEN_WIDTH / (GLfloat) SCREEN_HEIGHT, 0.1f, 1000.0f);
    
    // Game loop
    while (!glfwWindowShouldClose( window )) {
        GLfloat currentFrame = glfwGetTime(); // for dt
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
//        std::cout << "fps: " << 1/deltaTime << std::endl;
        
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents( );
        
        DoMovement(); // Camera movement
        
        // Render
        // Clear the colorbuffer
        glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        glm::mat4 model(1);
        glm::mat4 view = camera.getViewMatrix();
        
        // Draw our first triangle
        shader.Use();
        
        // Bind Textures using texture units
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, cubeTexture );
        glUniform1i( glGetUniformLocation( shader.Program, "texture1" ), 0 );
        
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation( shader.Program, "model" );
        GLint viewLoc = glGetUniformLocation( shader.Program, "view" );
        GLint projLoc = glGetUniformLocation( shader.Program, "projection" );
        
        // Pass the matrices to the shader
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        glBindVertexArray( cubeVAO );
        for ( GLuint i = 0; i < 16; i++) {
            for ( GLuint j = 0; j < 4; j++) {
                for ( GLuint k = 0; k < 16; k++) {
                    model = glm::mat4(1);
                    model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 1.0f));
                    model = glm::translate(model, glm::vec3(1.0f * i, -1.0f * j, 1.0f * k));
                    glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
                    glDrawArrays( GL_TRIANGLES, 0, 36 );
                }
            }
        }
        glBindVertexArray(0);
        
        
        // Draw skybox as last
        glDepthFunc( GL_LEQUAL );  // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Use( );
        view = glm::mat4( glm::mat3( camera.getViewMatrix( ) ) );    // Remove any translation component of the view matrix
        
        glUniformMatrix4fv( glGetUniformLocation( skyboxShader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( glGetUniformLocation( skyboxShader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
        
        // draw skybox cube
        glBindVertexArray( skyboxVAO );
        glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
        glDepthFunc( GL_LESS ); // Set depth function back to default
        
        // Swap the screen buffers
        glfwSwapBuffers( window );
    }
    
    // Properly de-allocate all resources once they've outlived their purpose
    
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate( );
    
    return EXIT_SUCCESS;
}


void DoMovement() {
    camera.update(deltaTime);
    camera.intersect();
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
        camera.processKeyboard(FORWARD, deltaTime);
    }
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
        camera.processKeyboard(BACKWARD, deltaTime);
    }
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
        camera.processKeyboard(LEFT, deltaTime);
    }
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
        camera.processKeyboard(RIGHT, deltaTime);
    }
    if (keys[GLFW_KEY_SPACE]) {
        camera.processKeyboard(UP, deltaTime);
    }
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos) {
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos; // reversed because yPos start from bottom left
    
    lastX = xPos;
    lastY = yPos;
    
    camera.processMouseMovement(xOffset, yOffset);
}








