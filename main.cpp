
#include "eval.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <tuple>
#include <vector>

using namespace std;

static const char *vertex_shader_code = R"(
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

static const char *fragment_shader_code = R"(
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
    -1.0f, -1.0f, 0.0f, 0.0f, // Bottom-left corner
    1.0f,  -1.0f, 1.0f, 0.0f, // Bottom-right corner
    1.0f,  1.0f,  1.0f, 1.0f, // Top-right corner
    -1.0f, 1.0f,  0.0f, 1.0f  // Top-left corner
};

void error_callback(int error, const char *description) {
  std::cerr << "Error: " << description << std::endl;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}
tuple<double, double, double, int> get_next(double i, double j, double delta,
                                            int t) {
  double initI = (t < 2) ? 0 : delta / 2;
  double initJ = (t % 2) ? 0 : delta / 2;
  if (j + delta < 1) {
    // cout << "a" << endl;
    return make_tuple(i, j + delta, delta, t);
  }
  if (i + delta < 1) {
    // cout << "b" << endl;
    return make_tuple(i + delta, initJ, delta, t);
  }
  if (t < 4) {
    // cout << "c" << endl;
    return make_tuple(initI, initJ, delta, t + 1);
  }
  // cout << "d" << endl;
  return make_tuple(0, 0, delta / 2, 0);
}
double i, j, delta = 0.5;
int t;
int scr_size = 512;
int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  glfwSetErrorCallback(error_callback);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  GLFWwindow *window =
      glfwCreateWindow(scr_size, scr_size, "grafeq", NULL, NULL);
  if (!window) {
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
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
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

  vector<unsigned char> image(scr_size * scr_size * 3);

  glUseProgram(program);
  glUniform1i(textureLocation, 0);

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glEnableVertexAttribArray(vPosLocation);
  glVertexAttribPointer(vPosLocation, 2, GL_FLOAT, GL_FALSE,
                        4 * sizeof(GLfloat), (void *)0);

  glEnableVertexAttribArray(vTexCoordLocation);
  glVertexAttribPointer(vTexCoordLocation, 2, GL_FLOAT, GL_FALSE,
                        4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));

  string sexpr;
  getline(cin, sexpr);
  auto expr = tokenize(sexpr);
  vector<vector<optional<double>>> cache(
      scr_size * (1 + 0.5), vector<optional<double>>(scr_size * (1 + 0.5)));
  while (!glfwWindowShouldClose(window)) {
    glBindTexture(GL_TEXTURE_2D, texture);
    for (int C = 0; C < 1 / delta; ++C) {
      if (delta >= 1.0 / scr_size) {
        {
          int x = i * scr_size, y = j * scr_size;
#define GET_CACHE(x, y, i, j)                                                  \
  (cache[x][y].has_value() ? cache[x][y] : (cache[x][y] = eval(expr, i, j)))   \
      .value()
          double v0 = GET_CACHE(x, y, i, j);
          double v1 = GET_CACHE(x + delta * scr_size, y, i + delta, j);
          double v2 = GET_CACHE(x, y + delta * scr_size, i, j + delta);
          double v3 = GET_CACHE(x + delta * scr_size, y + delta * scr_size,
                                i + delta, j + delta);
          bool is = false;
          if ((v0 < 0 || v1 < 0 || v2 < 0 || v3 < 0) &&
              (v0 > 0 || v1 > 0 || v2 > 0 || v3 > 0)) {
            is = true;
            cout << i << " " << j << "; 1 / " << 1 / delta << endl;
          }
          image[(x * scr_size + y) * 3 + 0] ^= 255;
          image[(x * scr_size + y) * 3 + 1] = is ? 255 : 0;
          image[(x * scr_size + y) * 3 + 2] = is ? 255 : 0;
        }
        {
          auto [I, J, D, T] = get_next(i, j, delta, t);
          i = I, j = J, delta = D, t = T;
        }
      }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scr_size, scr_size, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glClear(GL_COLOR_BUFFER_BIT);

    const GLfloat color[] = {1.0f, 1.0f, 1.0f};
    glUniform3fv(colorLocation, 1, color);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
