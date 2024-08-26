
#include "eval.h"
#include <algorithm>
#include <utility>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <thread>
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
tuple<double, double, double> get_next(double i, double j, double delta) {
  if (j + delta < 1) {
    // cout << "a" << endl;
    return make_tuple(i, j + delta, delta);
  }
  if (i + delta < 1) {
    // cout << "b" << endl;
    return make_tuple(i + delta, 0, delta);
  }
  // cout << "d" << endl;
  return make_tuple(0, 0, delta / 2);
}
const int NUM_THREADS = 16;
double gi, gj, gdelta = 0.5;
int scr_size = 512;
bool isInequality = false;
string filename;
double xmin, xmax, ymin, ymax;
int main(int argc, char **argv) {
  for (int i = 0; i < argc; i++) {
    if (string(argv[i]) == "-i") {
      isInequality = true;
    }
    if (string(argv[i]) == "-f" && i < argc - 1) {
      filename = argv[i + 1];
    }
  }
  if (filename == "") {
    cout << "No filename specified" << endl;
    return 1;
  }
  if (isInequality) {
    cout << "inequality" << endl;
  } else {
    cout << "equation" << endl;
  }
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
  {
    if (filename == "-") {
      getline(cin, sexpr);
    } else {
      ifstream f(filename);
      getline(f, sexpr);
    }
  }

  cin >> xmin >> xmax >> ymin >> ymax;
  auto expr = tokenize(sexpr);
  vector<vector<bool>> no_for_sure(scr_size, vector<bool>(scr_size, false));
  auto GET_CACHE = [&](double i, double j) {
    if (no_for_sure[i * scr_size][j * scr_size])
      return false;
    auto d = gdelta;
    varible v =
        eval(expr,
             add(mul(varible(vector({make_pair(value(i), value(i + d))})),
                     value(xmax - xmin)),
                 value(xmin)),
             add(mul(varible(vector({make_pair(value(j), value(j + d))})),
                     value(ymax - ymin)),
                 value(ymin)));
    // cout << "(" << i << ", " << j << ")" << v << endl;
    bool is = false;
    for (auto it = v.ranges.begin(); it != v.ranges.end(); ++it) {
      if (it->first <= 0 && value(0) <= it->second) {
        is = true;
        break;
      }
    }
    if (!is) {
      for (int x = i * scr_size; x < min(1.0, i + d) * scr_size; ++x)
        for (int y = j * scr_size; y < min(1.0, j + d) * scr_size; ++y)
          no_for_sure[x][y] = true;
    }
    return is;
  };
  bool done = false;
  while (!glfwWindowShouldClose(window)) {
    glBindTexture(GL_TEXTURE_2D, texture);
    if (gdelta >= 1.0 / scr_size) {
      for (int C = 0; C < pow(scr_size * gdelta, 1.5); ++C) {
#if 0 
      for (int i = 0; i < scr_size; ++i)
        for (int j = 0; j < scr_size; ++j)
          image[(i * scr_size + j) * 3 + 0] =
              image[(i * scr_size + j) * 3 + 1] =
                  image[(i * scr_size + j) * 3 + 2] =
                      no_for_sure[i][j] ? 255 : 0;
#endif
        if (!no_for_sure[gi * scr_size][gj * scr_size]) {
          auto i = gi, j = gj, delta = gdelta;
          int x = i * scr_size, y = j * scr_size;
          auto is = GET_CACHE(i, j);
#if 0
          // image[(x * scr_size + y) * 3 + 0] ^= 255;
          image[(x * scr_size + y) * 3 + 0] = no_for_sure[x][y] ? 255 : 0;
          image[(x * scr_size + y) * 3 + 1] =
              (no_for_sure[x][y] && is) ? 255 : 0;
          image[(x * scr_size + y) * 3 + 2] = is ? 255 : 0;
#else
          image[(x * scr_size + y) * 3 + 0] =
              image[(x * scr_size + y) * 3 + 1] =
                  image[(x * scr_size + y) * 3 + 2] = is ? 255 : 0;
#endif
        } //  else {
        //   int x = gi * scr_size, y = gj * scr_size;
        //   image[(x * scr_size + y) * 3 + 0] =
        //       image[(x * scr_size + y) * 3 + 1] =
        //           image[(x * scr_size + y) * 3 + 2] = 64;
        // }
        {
          auto [tI, tJ, tD] = get_next(gi, gj, gdelta);
          gi = tI, gj = tJ, gdelta = tD;
        }
      }

      // cout << (log(gdelta) / log(2) + log(scr_size) / log(2)) << " " <<
      // gdelta
      //      << " " << gi << " " << gt << endl;
    } else {
      if (done == false) {
        cout << "done." << endl;
      }
      done = true;
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
