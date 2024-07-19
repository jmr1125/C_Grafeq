
// #include <GLFW/glfw3.h>
// #include <iostream>

// static const char *vertex_shader_code = R"(
// #version 110
// attribute vec2 vPos;
// varying vec2 texcoord;
// void main()
// {
//     gl_Position = vec4(vPos, 0.0, 1.0);
//     texcoord = vPos;
// }
// )";

// static const char *fragment_shader_code = R"(
// #version 110
// uniform sampler2D texture;
// uniform vec3 color;
// varying vec2 texcoord;
// void main()
// {
//     gl_FragColor = vec4(color * texture2D(texture, texcoord).rgb, 1.0);
// }
// )";

// const GLfloat vertices[] = {-1, -1, 0, 0, 1, -1, 1, 0, 1, 1, 1, 1, -1, 1, 0, 1};

// void error_callback(int error, const char *description) {
//   std::cerr << "Error: " << description << std::endl;
// }

// void key_callback(GLFWwindow *window, int key, int scancode, int action,
//                   int mods) {
//   if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
//     glfwSetWindowShouldClose(window, GLFW_TRUE);
// }

// int main() {
//   if (!glfwInit()) {
//     std::cerr << "Failed to initialize GLFW" << std::endl;
//     return -1;
//   }

//   glfwSetErrorCallback(error_callback);
//   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
//   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

//   GLFWwindow *window = glfwCreateWindow(512, 512, "grafeq", NULL, NULL);
//   if (!window) {
//     glfwTerminate();
//     return -1;
//   }

//   glfwMakeContextCurrent(window);
//   // gladLoadGL(glfwGetProcAddress);
//   glfwSetKeyCallback(window, key_callback);
//   glfwSwapInterval(1);

//   GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
//   glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
//   glCompileShader(vertex_shader);

//   GLint success;
//   GLchar infoLog[512];
//   glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
//   if (!success) {
//     glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
//     std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
//               << infoLog << std::endl;
//   }

//   GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
//   glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
//   glCompileShader(fragment_shader);
//   glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
//   if (!success) {
//     glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
//     std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
//               << infoLog << std::endl;
//   }

//   GLuint program = glCreateProgram();
//   glAttachShader(program, vertex_shader);
//   glAttachShader(program, fragment_shader);
//   glLinkProgram(program);

//   glGetProgramiv(program, GL_LINK_STATUS, &success);
//   if (!success) {
//     glGetProgramInfoLog(program, 512, NULL, infoLog);
//     std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
//   }

//   GLint vPosLocation = glGetAttribLocation(program, "vPos");
//   GLint colorLocation = glGetUniformLocation(program, "color");
//   GLint textureLocation = glGetUniformLocation(program, "texture");

//   GLuint vertex_buffer;
//   glGenBuffers(1, &vertex_buffer);
//   glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//   GLuint texture;
//   glGenTextures(1, &texture);
//   glBindTexture(GL_TEXTURE_2D, texture);

//   char image[16 * 16];
//   for (int x = 0; x < 16; ++x) {
//     for (int y = 0; y < 16; ++y) {
//       image[x * 16 + y] = rand() % 256;
//     }
//   }

//   glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 16, 16, 0, GL_LUMINANCE,
//                GL_UNSIGNED_BYTE, image);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//   glUseProgram(program);
//   glUniform1i(textureLocation, 0);

//   glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//   glEnableVertexAttribArray(vPosLocation);
//   glVertexAttribPointer(vPosLocation, 2, GL_FLOAT, GL_FALSE,
//                         sizeof(vertices[0]), (void *)0);

//   while (!glfwWindowShouldClose(window)) {
//     glClear(GL_COLOR_BUFFER_BIT);

//     const GLfloat color[] = {1.0f, 1.0f, 1.0f};
//     glUniform3fv(colorLocation, 1, color);
//     glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

//     glfwSwapBuffers(window);
//     glfwPollEvents();
//   }

//   glfwDestroyWindow(window);
//   glfwTerminate();
//   return 0;
// }
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>

static const char* vertex_shader_code = R"(
#version 110
attribute vec2 vPos;
attribute vec2 vTexCoord;
varying vec2 texcoord;
void main()
{
    gl_Position = vec4(vPos, 0.0, 1.0);
    texcoord = vTexCoord;
}
)";

static const char* fragment_shader_code = R"(
#version 110
uniform sampler2D texture;
uniform vec3 color;
varying vec2 texcoord;
void main()
{
    gl_FragColor = vec4(color * texture2D(texture, texcoord).rgb, 1.0);
}
)";

const GLfloat vertices[] = {
    // Positions       // Texture Coords
    -1.0f, -1.0f,      0.0f, 0.0f, // Bottom-left corner
     1.0f, -1.0f,      1.0f, 0.0f, // Bottom-right corner
     1.0f,  1.0f,      1.0f, 1.0f, // Top-right corner
    -1.0f,  1.0f,      0.0f, 1.0f  // Top-left corner
};

void error_callback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(512, 512, "grafeq", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    GLint vPosLocation = glGetAttribLocation(program, "vPos");
    GLint vTexCoordLocation = glGetAttribLocation(program, "vTexCoord");
    GLint colorLocation = glGetUniformLocation(program, "color");
    GLint textureLocation = glGetUniformLocation(program, "texture");

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    char image[16 * 16];
    for (int x = 0; x < 16; ++x)
    {
        for (int y = 0; y < 16; ++y)
        {
            image[x * 16 + y] = rand() % 256;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 16, 16, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glUseProgram(program);
    glUniform1i(textureLocation, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glEnableVertexAttribArray(vPosLocation);
    glVertexAttribPointer(vPosLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(vTexCoordLocation);
    glVertexAttribPointer(vTexCoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        const GLfloat color[] = { 1.0f, 1.0f, 1.0f };
        glUniform3fv(colorLocation, 1, color);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
