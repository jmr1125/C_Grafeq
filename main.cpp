
#include "eval.h"
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
  if (t < 3) {
    // cout << "c" << endl;
    return make_tuple(initI, initJ, delta, t + 1);
  }
  // cout << "d" << endl;
  return make_tuple(0, 0, delta / 2, 0);
}
const int NUM_THREADS = 16;
double gi, gj, gdelta = 0.5;
int gt;
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
  vector<vector<double>> cache(scr_size * 1.5, vector<double>(scr_size * 1.5));
  vector<vector<bool>> cache_b(scr_size * 1.5, vector<bool>(scr_size * 1.5));
  // #define GET_CACHE(x, y, i, j)                                                  \
//   (cache_b[x][y] ? cache[x][y]                                                 \
//                  : (cache[x][y] = eval(expr, i, j, xmin, xmax, ymin, ymax)));  \
//   cache_b[x][y] = true;
  // mutex mtx;
  auto GET_CACHE = [&](int x, int y, double i, double j) {
    // mtx.lock();
    bool b = cache_b[x][y];
    // mtx.unlock();
    if (!b) {
      // mtx.lock();
      double v = eval(expr, i, j, xmin, xmax, ymin, ymax);
      cache[x][y] = v;
      cache_b[x][y] = true;
      // mtx.unlock();
    }
    return cache[x][y];
  };
  bool done = false;
  while (!glfwWindowShouldClose(window)) {
    glBindTexture(GL_TEXTURE_2D, texture);
    if (gdelta >= 1.0 / scr_size) {
      vector<thread *> threads(NUM_THREADS);
      for (int C = 0; C < 1 / gdelta; ++C) {
        threads[C % NUM_THREADS] = new thread([&]() {
          auto i = gi, j = gj, delta = gdelta;
          int x = i * scr_size, y = j * scr_size;
          double v0 = GET_CACHE(x, y, i, j);
          bool is = false;
          if (!isInequality) {
            double v1 = GET_CACHE(x + delta * scr_size, y, i + delta, j);
            double v2 = GET_CACHE(x, y + delta * scr_size, i, j + delta);
            double v3 = GET_CACHE(x + delta * scr_size, y + delta * scr_size,
                                  i + delta, j + delta);
            if ((v0 < 0 || v1 < 0 || v2 < 0 || v3 < 0) &&
                (v0 > 0 || v1 > 0 || v2 > 0 || v3 > 0)) {
              is = true;
              // cout << i << " " << j << "; 1 / " << 1 / delta << endl;
            }
          } else {
            if (v0 > 0) {
              is = true;
              // cout << i << " " << j << "; 1 / " << 1 / delta << " ; v = " <<
              // v0
              //      << endl;
            }
          }
          // image[(x * scr_size + y) * 3 + 0] ^= 255;
          image[(x * scr_size + y) * 3 + 0] = is ? 255 : 0;
          image[(x * scr_size + y) * 3 + 1] = is ? 255 : 0;
          image[(x * scr_size + y) * 3 + 2] = is ? 255 : 0;
        });
        {
          auto [tI, tJ, tD, tT] = get_next(gi, gj, gdelta, gt);
          gi = tI, gj = tJ, gdelta = tD, gt = tT;
        }
        if (C % NUM_THREADS == 0 && C) {
          for (int i = 0; i < NUM_THREADS; ++i) {
            threads[i]->join();
          }
        }
      }

      cout << (log(gdelta) / log(2) + log(scr_size) / log(2)) << " " << gdelta
           << " " << gi << " " << gt << endl;
    } else {
      if (done = false) {
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
