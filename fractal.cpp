// main.cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>

const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    void main() {
        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    }
)glsl";

// Updated fragment shader source
const char* fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;

    uniform float time;
    uniform vec2 resolution;
    uniform vec2 pan;
    uniform float zoom;

    void main() {
        vec2 pos = (gl_FragCoord.xy - 0.5 * resolution.xy) / (0.5 * zoom * resolution.y) + pan;
        vec2 c = pos;
        vec2 z = vec2(0.0, 0.0);
        
        int max_iter = 300;
        float i;
        for(i = 0.0; i < max_iter; i++) {
            if(length(z) > 4.0) break;
            z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
        }

        float t = sqrt(i / max_iter);
        FragColor = vec4(t, 0.5 * t, 0.5 * (1.0 + sin(3.0 * t + time)), 1.0);
    }
)glsl";

float zoom = 1.0f;
glm::vec2 pan = glm::vec2(0.0f, 0.0f);

int main() {
    // GLFW initialization and window creation
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1200, 1200, "Mandelbrot Set", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLEW initialization
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Shader compilation and linking
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Vertex data setup
    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Main loop
    //float zoom = 1.0f;
    float panX = -0.5f, panY = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Update zoom and pan based on user input
        // For example: Use arrow keys to pan, 'A' and 'D' to zoom in/out

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            zoom *= 1.01f;  // Zoom in
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            zoom /= 1.01f;  // Zoom out
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            pan.x -= 0.01f / zoom;  // Pan left
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            pan.x += 0.01f / zoom;  // Pan right
        }

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "time"), glfwGetTime());
        glUniform2f(glGetUniformLocation(shaderProgram, "resolution"), 800.0f, 600.0f);
        glUniform2f(glGetUniformLocation(shaderProgram, "pan"), panX, panY);
        glUniform1f(glGetUniformLocation(shaderProgram, "zoom"), zoom);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
