#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "LearnOpenGL/ShaderProgram.h"
#include "Images/stb_image.h"

void OnFramebufferSizeChanged(GLFWwindow* Window, int NewWindowWidth, int NewWindowHeight);
void ProcessInput(GLFWwindow* Window, float& OutBlendingScale);

// Settings
const int WINDOW_WIDTH = 800.f;
const int WINDOW_HEIGHT = 600.f;

int main()
{
    // GLWF initialization
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Initialization adjunct for MacOS 
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLWF window creation
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Init context & bind window framebuffer size change callback
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChanged);

    // GLAD: loads all OpenGL function pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    ShaderProgram program("Source\\1.GettingStarted\\4.6.Textures_Exercise4\\4.6.Shader.vs", "Source\\1.GettingStarted\\4.6.Textures_Exercise4\\4.6.Shader.fs");
    program.UseProgram();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertexData[] = {
        // positions        // colors           // texture coords
        0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    
    unsigned int vertexIndices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // Create and bind vertex array object, to store all vertex attributes related calls with it
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind vertex array object, to store all vertex attributes related calls with it
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 

    // Allocates memory on GPU and stores data withing the initialized memory in the currently bound VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // Allocates memory and stores data withing the initialized memory in the currently bound EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW); 

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load and create textures
    // -------------------------
    unsigned int textures[2];
    glGenTextures(2, textures);

    glActiveTexture(GL_TEXTURE0); // it's active texture unit by default by i'll bind it explicitly here, to illustrate the concept
    glBindTexture(GL_TEXTURE_2D, textures[0]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load("Resources\\Textures\\container.jpg", &width, &height, &nrChannels, 0);
    if (data != nullptr)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true); // flip image on load  
    data = stbi_load("Resources\\Textures\\awesomeface.png", &width, &height, &nrChannels, 0);
    if (data != nullptr)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    program.SetProgramUniform("texture2", 1); // Bind second texture uniform sample with second texture unit

    float initialBlendingScale = 0.2;
    
    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        // --- Input ---
        ProcessInput(window, initialBlendingScale);
        program.SetProgramUniform("blendingScale", initialBlendingScale);

        // --- Render ---
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // optional: de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

void OnFramebufferSizeChanged(GLFWwindow* Window, int NewWindowWidth, int NewWindowHeight)
{
    // Resize viewport accordingly
    glViewport(0, 0, NewWindowWidth, NewWindowHeight);
}

void ProcessInput(GLFWwindow* Window, float& OutBlendingScale)
{
    const float blendingScaleChangeStep = 0.01;
    
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Window, true);
    }
    else if (glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        OutBlendingScale -= blendingScaleChangeStep;
        if (OutBlendingScale < 0.f)
        {
            OutBlendingScale = 0.f;
        }
    }
    else if(glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        OutBlendingScale += blendingScaleChangeStep;
        if (OutBlendingScale > 1.0f)
        {
            OutBlendingScale = 1.0f;
        }
    }
}