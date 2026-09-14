#include <iostream>
#include <fstream>
#include <future>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "camera.h"


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
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
// vec3 cameraPos   = vec3(0.0f, 0.0f,  3.0f);
// vec3 cameraUp    = vec3(0.0f, 1.0f,  0.0f);
// vec3 direction;
// float camPitch = 0.0f;
// float camYaw = -90.0f;
// Camera camera(cameraPos, cameraUp, camYaw, camPitch);
Camera camera(vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
// float fov = 45.0f;
bool firstMouse = true;
// mat4 view;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// lighting
vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // for macos
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window object
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);  
    glfwSetScrollCallback(window, scroll_callback);
    int monitor_x, monitor_y; 
    glfwGetMonitorPos(glfwGetPrimaryMonitor(), &monitor_x, &monitor_y);
    glfwSetWindowPos(window, monitor_x+100, monitor_y+100);

    // disable mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    if (retina_screen) glViewport(0, 0, 2 * SCR_WIDTH, 2 * SCR_HEIGHT);
    else glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);  

    // Configure the vertex and fragment shaders
    // Shader ourShader("src/shaders/shader.vs", "src/shaders/shader.fs");
    Shader lightingShader("src/shaders/lighting.vs", "src/shaders/lighting.fs");
    Shader lightCubeShader("src/shaders/light_source.vs", "src/shaders/light_source.fs");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    // vec3 cubePositions[] = {
    //     vec3( 0.0f,  0.0f,  0.0f), 
    //     vec3( 2.0f,  5.0f, -15.0f), 
    //     vec3(-1.5f, -2.2f, -2.5f),  
    //     vec3(-3.8f, -2.0f, -12.3f),  
    //     vec3( 2.4f, -0.4f, -3.5f),  
    //     vec3(-1.7f,  3.0f, -7.5f),  
    //     vec3( 1.3f, -2.0f, -2.5f),  
    //     vec3( 1.5f,  2.0f, -2.5f), 
    //     vec3( 1.5f,  0.2f, -1.5f), 
    //     vec3(-1.3f,  1.0f, -1.5f)  
    // };

    // load and create textures
    // unsigned int texture1, texture2;

    // texture 1
    // glGenTextures(1, &texture1);
    // glBindTexture(GL_TEXTURE_2D, texture1); 
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    // stbi_set_flip_vertically_on_load(true); 
    // int width, height, nrChannels;
    // unsigned char *data = stbi_load("src/textures/container.jpg", &width, &height, &nrChannels, 0); 
    // if (data)
    // {
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    // else 
    // {
    //     cout << "Failed to load texture1" << endl;
    // }
    // stbi_image_free(data);

    // texture 2
    // glGenTextures(1, &texture2);
    // glBindTexture(GL_TEXTURE_2D, texture2); 
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    // data = stbi_load("src/textures/awesomeface.png", &width, &height, &nrChannels, 0); 
    // if (data)
    // {
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    // else 
    // {
    //     cout << "Failed to load texture2" << endl;
    // }
    // stbi_image_free(data);

    // set uniforms
    // ourShader.use(); // don't forget to activate the shader before setting uniforms!  
    // glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // set it manually
    // ourShader.setInt("texture2", 1); // or with shader class

    // transformations
    mat4 model;
    mat4 view;
    mat4 projection;


    // Create Buffers
    // unsigned int VBO;
    // glGenBuffers(1, &VBO);
    // unsigned int VAO;
    // glGenVertexArrays(1, &VAO);

    // ..:: Initialization code (done once (unless your object frequently changes)) :: ..
    // 1. bind Vertex Array Object
    // glBindVertexArray(VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    // position attribute
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1); 

    // Create light buffer
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Second, configure the light's VAO (VBO stays the same, the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    // we only need to bind the VBO, the container's VBO's data already contains the data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update the uniform transformations
        // unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
        // unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
        // unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        // glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
        
        
        // Place objects in the scene
        // glBindVertexArray(VAO);
        // for(unsigned int i = 0; i < 10; i++)
        // {
        //     mat4 model = mat4(1.0f);
        //     model = translate(model, cubePositions[i]);
        //     float angle = 20.0f * i; 
        //     model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
        //     // unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        //     unsigned int modelLoc = glGetUniformLocation(lightingShader.ID, "model");
        //     glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        lightingShader.use();
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        lightingShader.setFloat("material.shininess", 32.0f);
        // lightingShader.setVec3("light.ambient",  0.2f, 0.2f, 0.2f);
        // lightingShader.setVec3("light.diffuse",  0.5f, 0.5f, 0.5f); // darken diffuse light a bit
        lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f); 
        vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);
        
        vec3 diffuseColor = lightColor   * vec3(0.5f); 
        vec3 ambientColor = diffuseColor * vec3(0.2f); 
        
        lightingShader.setVec3("light.ambient", ambientColor);
        lightingShader.setVec3("light.diffuse", diffuseColor);

        // camera/view transformation
        view = lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        projection = perspective(radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f); 
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        mat4 model = mat4(1.0f);
        lightingShader.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = mat4(1.0f);
        model = translate(model, lightPos);
        model = scale(model, vec3(0.2f));
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // // 4. draw the object
        // ourShader.use();
        // lightingShader.use();
        // glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
        // glBindTexture(GL_TEXTURE_2D, texture1);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texture2);
        // glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36); 

        // draw the light source
        // ourShader.use();
        // set the model, view and projection matrix uniforms
        // unsigned int modelLoc = glGetUniformLocation(lightCubeShader.ID, "model");
        // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
        // unsigned int viewLoc = glGetUniformLocation(lightCubeShader.ID, "view");
        // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
        // unsigned int projectionLoc = glGetUniformLocation(lightCubeShader.ID, "projection");
        // glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
        // draw the light cube object


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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}