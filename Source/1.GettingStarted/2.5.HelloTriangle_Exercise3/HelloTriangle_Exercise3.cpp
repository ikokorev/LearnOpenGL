#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void OnFramebufferSizeChanged(GLFWwindow* Window, int NewWindowWidth, int NewWindowHeight);
void ProcessInput(GLFWwindow* Window);

/* Configures provided VAO & VBO associated with it, to draw a triangle with 3 vertices in future, considering current shaders attributes */
void ConfigureTriangleDrawContext(GLuint& VAO, GLuint& AssociatedVBO, GLfloat (&VerticesData)[9]);

/*
 * Creates and links shader program with given, already compiled vertex shader (simply uses it's ID) and fragment shader source code to use:
 * firstly, compile it, and than use it during shader program linkage (using given vertex and compiled fragment shaders).
 */
GLuint CreateShaderProgram(const GLuint& vertexShaderID, const char*& FragmentShaderSourceCode);

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
const char* fragmentShaderSourceOrange = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

// Just use "yellow" as any fragment color
const char* fragmentShaderSourceYellow = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
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

    // Create and compile vertex shader
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Verify compiled shader for errors
    GLint bOperationScceeded = 0;
    constexpr int compileInfoLogSize = 512;
    char infoLog[compileInfoLogSize];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &bOperationScceeded);
    if (!bOperationScceeded)
    {
        glGetShaderInfoLog(vertexShader, compileInfoLogSize, nullptr, infoLog);
        std::cout << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    const GLuint firstShaderProgram = CreateShaderProgram(vertexShader, fragmentShaderSourceOrange);
    const GLuint secondShaderProgram = CreateShaderProgram(vertexShader, fragmentShaderSourceYellow);
    glDeleteShader(vertexShader);

    // Vertices data in NDC (Normalized Device Coordinates)
    float firstTriangleVertices[] =
    {
        -1.f, -0.5f, 0.0f,
        0.0f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };

    float secondTriangleVertices[] =
    {
        0.f, -0.5f, 0.0f,
        1.0f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f
    };

    // Create and bind vertex array object, to store all vertex attributes related calls with it
    GLuint VAOs[2], VBOs[2];
    glGenVertexArrays(2, VAOs); // we can also generate multiple VAOs or buffers at the same time
    glGenBuffers(2, VBOs);

    ConfigureTriangleDrawContext(VAOs[0], VBOs[0], firstTriangleVertices);
    ConfigureTriangleDrawContext(VAOs[1], VBOs[1], secondTriangleVertices);

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        // --- Input ---
        ProcessInput(window);

        // --- Render ---
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw first triangle
        glUseProgram(firstShaderProgram);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Draw second triangle
        glUseProgram(secondShaderProgram);
        glBindVertexArray(VAOs[1]); 
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind VAO every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // optional: de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteProgram(firstShaderProgram);

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

void ConfigureTriangleDrawContext(GLuint& VAO, GLuint& AssociatedVBO, GLfloat (&VerticesData)[9])
{
    // Bind vertex array object, to store all vertex attributes related calls with it
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, AssociatedVBO); 

    // Allocates memory on GPU and stores data withing the initialized memory in the currently bound VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(VerticesData), &VerticesData, GL_STATIC_DRAW);

    // Specifies how OpenGL should interpret the vertex buffer data whenever a drawing call is made. Specification is stored in currently bound VAO.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    // Enables first vertex attribute (attributes are disabled by default)
    glEnableVertexAttribArray(0);

    // Unbind current VBO, cause its' no longer needed (the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

GLuint CreateShaderProgram(const GLuint& vertexShaderID, const char*& FragmentShaderSourceCode)
{
    // Create and compile fragment shader from given source code
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FragmentShaderSourceCode, nullptr);
    glCompileShader(fragmentShader);

    GLint bOperationScceeded = 0;
    constexpr int compileInfoLogSize = 512;
    char infoLog[compileInfoLogSize];

    // Verify compiled shader for errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &bOperationScceeded);
    if (!bOperationScceeded)
    {
        std::fill(std::begin(infoLog), std::end(infoLog), '\0');
        glGetShaderInfoLog(fragmentShader, compileInfoLogSize, nullptr, infoLog);
        std::cout << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // Link shader program using given vertex and compile fragment shaders
    const GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShaderID);
    glAttachShader(shaderProgram, fragmentShader);
    glDeleteShader(fragmentShader); // we can safely delete compiled shader after attachment to a shader program
    glLinkProgram(shaderProgram);

    // Verify shader program linkage
    bOperationScceeded = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &bOperationScceeded);
    if (!bOperationScceeded)
    {
        std::fill(std::begin(infoLog), std::end(infoLog), '\0');
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "Shaders linkage failed:\n" << infoLog << std::endl;
    }

    return shaderProgram;
}