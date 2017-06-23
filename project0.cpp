/*===================================================
// Skeleton Project for CS-150: 3D Computer Graphics
//===================================================*/

#include <glad/glad.h> // must be included first
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

static const int NUM_TRIANGLES = 1;

static const struct
{
    GLfloat x, y, z; // position in R^3
    GLfloat r, g, b; // color
} triangles[3*NUM_TRIANGLES] =
{
    { -0.5f, -0.289, 0.0f, 1.0f, 0.0f, 0.0f },
    {  0.5f, -0.289f, 0.0f, 0.0f, 1.0f, 0.0f },
    {  0.0f,  0.577f, 0.0f, 0.0f, 0.0f, 1.0f }
};

const GLchar* vertexShaderSource = R"glsl(
#version 330
uniform mat4 MVP;
in vec3 vCol;
in vec3 vPos;
out vec3 color;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
    color = vCol;
}
)glsl";

const GLchar* fragmentShaderSource = R"glsl(
#version 330
in vec3 color;
out vec4 fragColor;

void main()
{
    fragColor = vec4(color, 1.0);
}
)glsl";

static void errorCallback(int error, const char* description)
{
    cerr << "GLFW Error: " << description << endl;
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void compileShader(GLuint shader, const char *shaderText)
{
    glShaderSource(shader, 1, &shaderText, NULL);
    glCompileShader(shader);
    GLchar infoLog[8192];
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 8192, NULL, infoLog);
        cerr << "Shader failed to complile." << endl
             << "Error log: " << infoLog << endl;
    }
}

int main(void)
{
    GLFWwindow* window;

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    // Request OpenGL version 3.3.
    // On most linux systems, you can safely comment out the
    // following four hints and you will get the latest version your
    // card supports.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Don't use old OpenGL
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // OSX needs this

    window = glfwCreateWindow(640, 480, "CS 150 Template Project", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, keyCallback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    cout << "GL version: " << glGetString(GL_VERSION) << endl
         << "GL vendor: " << glGetString(GL_VENDOR) << endl
         << "GL renderer: " << glGetString(GL_RENDERER) << endl
         << "GL shading language version: "
         << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    glfwSwapInterval(1); // Framerate matches monitor refresh rate

    GLuint VAO; // vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint vertexBuffer, vertexShader, fragmentShader, program;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

    // Read shaders from files, compile them, and check for errors
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    compileShader(vertexShader, vertexShaderSource);
    compileShader(fragmentShader, fragmentShaderSource);

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        cerr << "ERROR: Shader linking failed." << endl;
        glDeleteProgram(program);
        program = 0u;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint mvpLocation = glGetUniformLocation(program, "MVP");
    GLint vposLocation = glGetAttribLocation(program, "vPos");
    GLint vcolLocation = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vposLocation);
    glVertexAttribPointer(vposLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(triangles[0]), (GLvoid*) 0);
    glEnableVertexAttribArray(vcolLocation);
    glVertexAttribPointer(vcolLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(triangles[0]), (GLvoid*) (sizeof(GLfloat) * 3));

    glEnable(GL_DEPTH_TEST);

    glm::mat4 M, V, P, MVP;
    glm::vec3 eye = glm::vec3(0.0, 0.0, 5.0);
    glm::vec3 center = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
    V = glm::lookAt(eye, center, up);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ratio = width / (float) height;
        P = glm::perspective(0.50f, ratio, 1.0f, 100.0f);
        M = glm::rotate(glm::mat4(1.0f), (float) glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        MVP = P * V * M;

        glUseProgram(program);
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(MVP));
        glDrawArrays(GL_TRIANGLES, 0, 3*NUM_TRIANGLES);

        // check for OpenGL errors
        GLenum error_code;
        while ((error_code = glGetError()) != GL_NO_ERROR)
            cerr << "OpenGL error HEX: " << hex << error_code << endl;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
