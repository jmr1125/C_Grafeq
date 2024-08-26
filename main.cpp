
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
  // xmin = ymin = -10;
  // xmax = ymax = 10;
  auto expr = tokenize(sexpr);
  vector<vector<char>> no_for_sure(scr_size, vector<char>(scr_size, false));
  auto GET_CACHE = [&](double i, double j, double d) {
    if (no_for_sure[i * scr_size][j * scr_size])
      return false;
    varible v =
        eval(expr,
             add(mul(varible(vector({make_pair(value(j), value(j + d))})),
                     value(xmax - xmin)),
                 value(xmin)),
             add(mul(varible(vector({make_pair(value(i), value(i + d))})),
                     value(ymax - ymin)),
                 value(ymin)));
    // cout << "(" << i << ", " << j << ")" << v << endl;
    bool is = false;
    for (auto it = v.ranges.begin(); it != v.ranges.end(); ++it) {
      if (isInequality) {
        if (value(0) <= it->second) {
          is = true;
          break;
        }
      } else {
        if (it->first <= 0 && value(0) <= it->second) {
          is = true;
          break;
        }
      }
    }
    if (!is) {
      cout << i << ' ' << j << ' ' << d << " " << v << endl;
      cout << i * (ymax - ymin) + ymin << ' ' << j * (xmax - xmin) + xmin << ' '
           << d * (ymax - ymin) << " " << d * (xmax - xmin) << " " << v << endl;
      for (int x = i * scr_size; x < min(1.0, i + d) * scr_size; ++x)
        for (int y = j * scr_size; y < min(1.0, j + d) * scr_size; ++y) {
          image[(x * scr_size + y) * 3 + 0] =
              image[(x * scr_size + y) * 3 + 1] =
                  image[(x * scr_size + y) * 3 + 2] = 255;
          no_for_sure[x][y] = true;
        }
    }
    return is;
  };
  bool done = false;
  bool stop = false;
  double delta = 0.5;
  vector<tuple<double, double, double>> need_to_draw;
  auto it = 0;
  while (!glfwWindowShouldClose(window)) {
    glBindTexture(GL_TEXTURE_2D, texture);
    bool done = false;
    if (!stop) {
      for (int C = 0; C < 1 // pow(scr_size, 1.5)
           ;
           ++C) {
        if (it == need_to_draw.size()) {
          done = true;
          it = 0;
          break;
        }

        if (!no_for_sure[get<0>(need_to_draw[it]) * scr_size]
                        [get<1>(need_to_draw[it]) * scr_size]) {
          auto is = apply(GET_CACHE, need_to_draw[it]);
        }

        ++it;
      }
      cout << it << "/" << need_to_draw.size() << endl;
      if (done) {
        // for (int i = 0; i < scr_size; ++i) {
        //   for (int j = 0; j < scr_size; ++j) {
        //     if (!no_for_sure[i][j])
        //       image[(i * scr_size + j) * 3 + 0] =
        //           image[(i * scr_size + j) * 3 + 1] =
        //               image[(i * scr_size + j) * 3 + 2] = 0;
        //   }
        // }
        delta /= 2;
        if (delta < 1.0 / scr_size) {
          stop = true;
          for (int i = 0; i < scr_size; ++i) {
            // x= v*(max-min)+min
            auto x0 = -xmin / (xmax - xmin);
            auto y0 = -ymin / (ymax - ymin);
            if (0 <= x0 && x0 <= 1) {
              image[(i * scr_size + int(x0 * scr_size)) * 3 + 0] = 128;
            }
            if (0 <= y0 && y0 <= 1) {
              image[(int(y0 * scr_size) * scr_size + i) * 3 + 0] = 128;
            }
          }
          cout << "done." << endl;
        }
        cout << delta << endl;
        vector<tuple<double, double, double>> t;
        t.clear();
        t.reserve(1.0 / delta / delta);
        for (double i = 0; i < 1; i += delta) {
          for (double j = 0; j < 1; j += delta) {
            if (!no_for_sure[i * scr_size][j * scr_size]) {
              t.push_back({i, j, delta});
            }
          }
        }
        need_to_draw = t;
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
