
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
enum class point_status { need_to_divide = 0, no, yes };
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
  vector<vector<point_status>> status(
      scr_size + 1, vector(scr_size + 1, point_status::need_to_divide));
  auto get_color =
      [](point_status is,
         bool undefined) -> tuple<unsigned char, unsigned char, unsigned char> {
    unsigned char color0 = 0, color1 = 0, color2 = 0;
    if (is == point_status::no) {
      color0 = color1 = color2 = 0;
    }
    if (is == point_status::yes && !undefined) {
      color0 = color1 = color2 = 255;
    }
    if (is == point_status::need_to_divide && undefined) {
      // color0 = 255; // r
      color0 = 64;
      color1 = color2 = 0;
    }
    if (is == point_status::need_to_divide && !undefined) {
      color1 = 255; // g
      color0 = color2 = 0;
    }
    if (is != point_status::no) {
      color2 = 128;
    }
    return make_tuple(color0, color1, color2);
  };
  vector<tuple<double, double, double>> next_to_draw;
  next_to_draw.reserve(4);
  auto process = [&](double i, double j, double d) {
    if (status[i * scr_size][j * scr_size] == point_status::no)
      return false;
    if (status[i * scr_size][j * scr_size] == point_status::yes)
      return true;
    varible v =
        eval(expr,
             add(mul(varible(vector({make_pair(value(j), value(j + d))})),
                     value(xmax - xmin)),
                 value(xmin)),
             add(mul(varible(vector({make_pair(value(i), value(i + d))})),
                     value(ymax - ymin)),
                 value(ymin)));
    // cout << "(" << i << ", " << j << ")" << v << endl;
    point_status is = point_status::no;
    for (auto it = v.ranges.begin(); it != v.ranges.end(); ++it) {
      if (isInequality) {
        if (value(0) <= it->first) {
          is = point_status::yes;
          break;
        }
        if (value(0) <= it->second) {
          is = point_status::need_to_divide;
#define add_sub()                                                              \
  next_to_draw.push_back({i, j, d / 2});                                       \
  if (i + d <= 1)                                                              \
    next_to_draw.push_back({i + d / 2, j, d / 2});                             \
  if (j + d <= 1)                                                              \
    next_to_draw.push_back({i, j + d / 2, d / 2});                             \
  if (i + d <= 1 && j + d <= 1)                                                \
    next_to_draw.push_back({i + d / 2, j + d / 2, d / 2});
          add_sub();
          break;
        }
      } else {
        if (it->first <= 0 && value(0) <= it->second) {
          is = point_status::need_to_divide;
          add_sub();
          break;
        } else {
          is = point_status::no;
        }
      }
    }
    // cout << i << ' ' << j << ' ' << d << " " << v << endl;
    cout << i * (ymax - ymin) + ymin << ' ' << j * (xmax - xmin) + xmin << ' '
         << d * (ymax - ymin) << " " << d * (xmax - xmin) << " " << v << endl;
    for (int x = i * scr_size; x < min(1.0, i + d) * scr_size; ++x)
      for (int y = j * scr_size; y < min(1.0, j + d) * scr_size; ++y) {
        auto [color0, color1, color2] = get_color(is, v.has_undefined);
        image[(x * scr_size + y) * 3 + 0] = color0;
        image[(x * scr_size + y) * 3 + 1] = color1;
        image[(x * scr_size + y) * 3 + 2] = color2;
        status[x][y] = is;
      }
    return is != point_status::no;
  };
  bool done = false;
  bool stop = false;
  double delta = 0.5;
  vector<tuple<double, double, double>> need_to_draw;
  need_to_draw.push_back({0, 0, 1});
  auto it = 0;
  while (!glfwWindowShouldClose(window)) {
    glBindTexture(GL_TEXTURE_2D, texture);
    bool done = false;
    if (!stop) {
      for (int C = 0;
           C <
#if 1
           min(max(need_to_draw.size(), (size_t)1), (size_t)10000) // 1
#else
           1 // 0
#endif
           ;
           ++C) {
        if (it == need_to_draw.size()) {
          done = true;
          it = 0;
          break;
        }

        if (status[get<0>(need_to_draw[it]) * scr_size]
                  [get<1>(need_to_draw[it]) * scr_size] ==
            point_status::need_to_divide) {
          auto is = apply(process, need_to_draw[it]);
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
        if (delta < 0.5 / scr_size) {
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
        need_to_draw = next_to_draw;
        next_to_draw.clear();
        next_to_draw.reserve(need_to_draw.size());
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
