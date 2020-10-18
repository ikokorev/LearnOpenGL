#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void OnFramebufferSizeChanged(GLFWwindow* Window, int NewWindowWidth, int NewWindowHeight);
void ProcessInput(GLFWwindow* Window);

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
    const float windowWidth = 800.f;
    const float windowHeight = 600.f;
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChanged);

	// GLAD: loads all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

	    glClear(GL_COLOR_BUFFER_BIT);
	    
	    glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void OnFramebufferSizeChanged(GLFWwindow* Window, int NewWindowWidth, int NewWindowHeight)
{
    glViewport(0, 0, NewWindowWidth, NewWindowHeight);
}

void ProcessInput(GLFWwindow* Window)
{
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Window, true);
    }
}