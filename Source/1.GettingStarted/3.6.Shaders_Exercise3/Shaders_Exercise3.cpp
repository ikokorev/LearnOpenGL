#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include <experimental/filesystem>


#include "LearnOpenGL/ShaderProgram.h"

void OnFramebufferSizeChanged(GLFWwindow* Window, int NewWindowWidth, int NewWindowHeight);
void ProcessInput(GLFWwindow* Window);

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

    ShaderProgram program("Source\\1.GettingStarted\\3.6.Shaders_Exercise3\\3.6.Shader.vs", "Source\\1.GettingStarted\\3.6.Shaders_Exercise3\\3.6.Shader.fs");
    program.UseProgram();

    // Vertices data (positions are in NDC)
    float vertices[] =
    {
        // positions                 // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,    // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f // top
    };

    // Create and bind vertex array object, to store all vertex attributes related calls with it
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind vertex array object, to store all vertex attributes related calls with it
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 

    // Allocates memory on GPU and stores data withing the initialized memory in the currently bound VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // Specifies how OpenGL should interpret the vertex buffer data whenever a drawing call is made. Specification is stored in currently bound VAO. Binding for position attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
    // Enables first vertex attribute (attributes are disabled by default)
    glEnableVertexAttribArray(0);
    
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind current VBO, cause its' no longer needed (the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    //glBindVertexArray(0);

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        // --- Input ---
        ProcessInput(window);

        // --- Render ---
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind VAO every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // optional: de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

void OnFramebufferSizeChanged(GLFWwindow* Window, int NewWindowWidth, int NewWindowHeight)
{
    // Resize viewport accordingly
    glViewport(0, 0, NewWindowWidth, NewWindowHeight);
}

void ProcessInput(GLFWwindow* Window)
{
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Window, true);
    }
}