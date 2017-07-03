/*===================================================
// Skeleton Project for CS-150: 3D Computer Graphics
//===================================================*/

#include <glad/glad.h> // must be included first
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

struct Vertex {
    vec3 position;
};

const int NUM_OCTANT_LEVELS = 3;
const int POW_2_NOL = (1 << NUM_OCTANT_LEVELS); // 2^NUM_OCTANT_LEVELS
array<Vertex, (POW_2_NOL+1)*(POW_2_NOL+2)/2> octant; // populated in init_octant()
const int NUM_OCTANT_IDX = POW_2_NOL*POW_2_NOL*3;
array<GLuint, NUM_OCTANT_IDX> octant_idx; // populated in init_octant()

// Global Parameters
GLfloat xAngle = 0.0f;
GLfloat yAngle = 0.0f;
double xCursor;
double yCursor;
float mouseSpeed = 0.01f;

const GLchar* vertexShaderSource = R"glsl(
#version 330
uniform mat4 MVP;
in vec3 posn_obj;

void main()
{
    gl_Position = MVP * vec4(posn_obj, 1.0);
}
)glsl";

const GLchar* fragmentShaderSource = R"glsl(
#version 330
uniform vec3 uColor;
out vec4 fragColor;

void main()
{
    fragColor = vec4(uColor, 1.0);
}
)glsl";

void errorCallback(int error, const char* description)
{
    cerr << "GLFW Error: " << description << endl;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

float zoomAngle = 0.50f;
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    static const float zoomEpsilon = 0.02f;
    zoomAngle += yoffset*zoomEpsilon;
}

void buttonCallback(GLFWwindow* window, int button, int action, int mods)
{ // see glfw/examples/wave.c
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;
    if (action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(window, &xCursor, &yCursor);
    }
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void cursorCallback(GLFWwindow* window, double x, double y)
{ // see glfw/examples/wave.c
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
    {
        xAngle += (GLfloat) (x - xCursor) * mouseSpeed;
        yAngle += (GLfloat) (y - yCursor) * mouseSpeed;
        xCursor = x;
        yCursor = y;
    }
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

void init_octant()
{
    int i = 0;
    int j = 0;
    GLuint ll = 0;
    octant[i].position = vec3(1.0f, 0.0f, 0.0f);
    float d = 1.0f/(POW_2_NOL);
    for (int r=0; r<POW_2_NOL; r++) {
        for (int s=1; s<POW_2_NOL-r+1; s++) {
            i++;
            octant[i].position = octant[i-1].position + vec3(-d, d, 0.0f);
            octant_idx[j++] = ll + s - 1;
            octant_idx[j++] = ll + s;
            octant_idx[j++] = ll + s + POW_2_NOL - r;
        }
        for (int t=1; t<POW_2_NOL-r; t++) {
            octant_idx[j++] = ll + t;
            octant_idx[j++] = ll + t + POW_2_NOL + 1 - r;
            octant_idx[j++] = ll + t + POW_2_NOL - r;
        }
        // move to first entry of next row
        ll += POW_2_NOL + 1 - r;
        i++;
        octant[i].position = octant[i-POW_2_NOL-1+r].position + vec3(-d, 0, d);
    }
    for (auto &v : octant) {
        v.position = normalize(v.position);
    }
}

int main(void)
{
    GLFWwindow* window;

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    // Request OpenGL version 3.3.
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
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, buttonCallback);
    glfwSetCursorPosCallback(window, cursorCallback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    cout << "GL version: " << glGetString(GL_VERSION) << endl
         << "GL vendor: " << glGetString(GL_VENDOR) << endl
         << "GL renderer: " << glGetString(GL_RENDERER) << endl
         << "GL shading language version: "
         << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    glfwSwapInterval(1); // Framerate matches monitor refresh rate

    // Compile shaders and check for errors
    GLuint vertexShader, fragmentShader, program;
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

    GLint l_MVP = glGetUniformLocation(program, "MVP");
    GLint l_uColor = glGetUniformLocation(program, "uColor");
    GLint l_posn_obj = glGetAttribLocation(program, "posn_obj");

    init_octant();
    // Send data to OpenGL context
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, octant.size() * sizeof(Vertex),
                 octant.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(l_posn_obj);
    glVertexAttribPointer(l_posn_obj, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (GLvoid*) 0);
    glGenBuffers(1, &EBO); // element buffer (indices)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, octant_idx.size()*sizeof(GLuint),
                 octant_idx.data(), GL_STATIC_DRAW);

    glUseProgram(program);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode
    mat4 M, V, P, MVP;
    vec3 eye = vec3(0.0, 7.0, 15.0);
    vec3 center = vec3(0.0, 0.0, 0.0);
    vec3 up = vec3(0.0, 1.0, 0.0);
    V = lookAt(eye, center, up);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ratio = width / (float) height;
        P = perspective(zoomAngle, ratio, 1.0f, 100.0f);
        M = rotate(mat4(1.0), xAngle, vec3(0.0f, 1.0f, 0.0f));
        M = rotate(M, yAngle, vec3(1.0f, 0.0f, 0.0f));
        MVP = P * V * M;

        glUniformMatrix4fv(l_MVP, 1, GL_FALSE, value_ptr(MVP));
        glUniform3f(l_uColor, 0.0f, 0.7f, 0.0f); // dark green
        glDrawElements(GL_TRIANGLES, octant_idx.size(), GL_UNSIGNED_INT, 0);

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
