#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void OnFramebufferSizeChanged(GLFWwindow* Window, int NewWindowWidth, int NewWindowHeight);
void ProcessInput(GLFWwindow* Window);

// Settings
const int WINDOW_WIDTH = 800.f;
const int WINDOW_HEIGHT = 600.f;

// Simply forwards position data to the shader's output
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// Just use "orange" as any fragment color
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

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

    // Compile and then verify vertex shader
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLint bOperationScceeded = 0;
    constexpr int compileInfoLogSize = 512;
    char infoLog[compileInfoLogSize];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &bOperationScceeded);
    if (!bOperationScceeded)
    {
        glGetShaderInfoLog(vertexShader, compileInfoLogSize, nullptr, infoLog);
        std::cout << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // Compile and then verify fragment shader
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    bOperationScceeded = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &bOperationScceeded);
    if (!bOperationScceeded)
    {
        std::fill(std::begin(infoLog), std::end(infoLog), '\0');
        glGetShaderInfoLog(fragmentShader, compileInfoLogSize, nullptr, infoLog);
        std::cout << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // Configure shader program and delete shaders
    const GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glDeleteShader(vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glDeleteShader(fragmentShader);
    glLinkProgram(shaderProgram);

    // Verify shader program
    bOperationScceeded = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &bOperationScceeded);
    if (!bOperationScceeded)
    {
        std::fill(std::begin(infoLog), std::end(infoLog), '\0');
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "Shaders linkage failed:\n" << infoLog << std::endl;
    }

    // Vertices data in NDC (Normalized Device Coordinates)
    float vertices[] =
    {
        0.5f, 0.5f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f // top left 
    };

    unsigned int vertexIndices[] =
    {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    // Create and bind vertex array object, to store all vertex attributes related calls with it
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create and bind EBO
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // Allocates memory and stores data withing the initialized memory in the currently bound EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW); 
    
    // Create and bind VBO
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Allocates memory and stores data withing the initialized memory in the currently bound VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Specifies how OpenGL should interpret the vertex buffer data whenever a drawing call is made. Specification is stored in currently bound VAO.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    // Enables first vertex attribute (attributes are disabled by default)
    glEnableVertexAttribArray(0);

    // Unbind current VBO, cause its' no longer needed (the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    /* Sets polygon rasterization mode, to draw in wireframe polygons. */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        // --- Input ---
        ProcessInput(window);

        // --- Render ---
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // optional: de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

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
