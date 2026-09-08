#include <iostream>
#include <fstream>
#include <future>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

// fcts declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int createShaderProgram();
string read(const char* filePath);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// global variables
vec3 cameraPos   = vec3(0.0f, 0.0f,  3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp    = vec3(0.0f, 1.0f,  0.0f);
vec3 direction;
float camPitch = 0.0f;
float camYaw = -90.0f;
float lastX = 400, lastY = 300;
float fov = 45.0f;
bool firstMouse = true;
mat4 view;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // for macos
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create window object
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Define Viewport
    bool retina_screen = false;
    // Check if the os is macos
    #ifdef __APPLE__
        retina_screen = true;
    #endif
    if (retina_screen) glViewport(0, 0, 1600, 1200);
    else glViewport(0, 0, 800, 600);

    // Resize callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Configure the vertex and fragment shaders
    Shader ourShader("src/shaders/shader.vs", "src/shaders/shader.fs");

    float vertices[] = {
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

    vec3 cubePositions[] = {
        vec3( 0.0f,  0.0f,  0.0f), 
        vec3( 2.0f,  5.0f, -15.0f), 
        vec3(-1.5f, -2.2f, -2.5f),  
        vec3(-3.8f, -2.0f, -12.3f),  
        vec3( 2.4f, -0.4f, -3.5f),  
        vec3(-1.7f,  3.0f, -7.5f),  
        vec3( 1.3f, -2.0f, -2.5f),  
        vec3( 1.5f,  2.0f, -2.5f), 
        vec3( 1.5f,  0.2f, -1.5f), 
        vec3(-1.3f,  1.0f, -1.5f)  
    };

    // load and create textures
    unsigned int texture1, texture2;

    // texture 1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    stbi_set_flip_vertically_on_load(true); 
    int width, height, nrChannels;
    unsigned char *data = stbi_load("src/textures/container.jpg", &width, &height, &nrChannels, 0); 
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else 
    {
        cout << "Failed to load texture1" << endl;
    }
    stbi_image_free(data);

    // texture 2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    data = stbi_load("src/textures/awesomeface.png", &width, &height, &nrChannels, 0); 
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else 
    {
        cout << "Failed to load texture2" << endl;
    }
    stbi_image_free(data);

    // set uniforms
    ourShader.use(); // don't forget to activate the shader before setting uniforms!  
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // set it manually
    ourShader.setInt("texture2", 1); // or with shader class

    // transformations
    mat4 model = mat4(1.0f);
    // model = rotate(model, radians(-55.0f), vec3(1.0f, 0.0f, 0.0f));
    // mat4 view = mat4(1.0f);
    // view = translate(view, vec3(0.0f, 0.0f, -3.0f));
    mat4 projection;
    projection = perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    // vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
    // vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
    // vec3 cameraDirection = normalize(cameraPos - cameraTarget);
    // vec3 up = vec3(0.0f, 1.0f, 0.0f); 
    // vec3 cameraRight = normalize(cross(up, cameraDirection));
    // vec3 cameraUp = cross(cameraDirection, cameraRight);
    // view = lookAt(vec3(0.0f, 0.0f, 3.0f), 
  	// 	   vec3(0.0f, 0.0f, 0.0f), 
  	// 	   vec3(0.0f, 1.0f, 0.0f));
    view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    direction.x = cos(radians(camYaw)) * cos(radians(camPitch));
    direction.y = sin(radians(camPitch));
    direction.z = sin(radians(camYaw)) * cos(radians(camPitch));


    // Create Buffers
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    // unsigned int EBO;
    // glGenBuffers(1, &EBO);

    // ..:: Initialization code (done once (unless your object frequently changes)) :: ..
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
    // 3. then set our vertex attributes pointers
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    // glEnableVertexAttribArray(1);  
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); 

    glEnable(GL_DEPTH_TEST);  

    // register mouse callback fct
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disable mouse cursor
    glfwSetCursorPosCallback(window, mouse_callback);  
    glfwSetScrollCallback(window, scroll_callback); 

    // Render loop
    while(!glfwWindowShouldClose(window))
    {
        // inputs
        processInput(window);

        // delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  

        // rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
        unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
        unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
        
        glBindVertexArray(VAO);
        for(unsigned int i = 0; i < 10; i++)
        {
            mat4 model = mat4(1.0f);
            model = translate(model, cubePositions[i]);
            float angle = 20.0f * i; 
            model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
            unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // const float radius = 10.0f;
        // float camX = sin(glfwGetTime()) * radius;
        // float camZ = cos(glfwGetTime()) * radius;
        // // mat4 view;
        // view = lookAt(vec3(camX, 0.0, camZ), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
        view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = perspective(radians(fov), 800.0f / 600.0f, 0.1f, 100.0f); 


        // // 4. draw the object
        ourShader.use();
        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 
        glDrawArrays(GL_TRIANGLES, 0, 36); 
        // glBindVertexArray(0); // no need to unbind it every time

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// fcts definitions

void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    camYaw   += xoffset;
    camPitch += yoffset; 
    if(camPitch > 89.0f)
        camPitch =  89.0f;
    if(camPitch < -89.0f)
        camPitch = -89.0f;
    direction.x = cos(radians(camYaw)) * cos(radians(camPitch));
    direction.y = sin(radians(camPitch));
    direction.z = sin(radians(camYaw)) * cos(radians(camPitch));
    cameraFront = normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f; 
}