
#include "eval.h"
#include <algorithm>
#include <bitset>
#include <list>
#include <mutex>
#include <queue>
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
double xmin, xmax, ymin, ymax;
enum class point_status { need_to_divide = 0, no, yes };
struct range2 {
  range2() = default;
  range2(double x, double y, int n) {
    r.resize(n);
    for (int i = 0; i < n; ++i) {
      r[i][0] = (x >= 1);
      r[i][1] = (y >= 1);
      x *= 2;
      y *= 2;
    }
  }
  tuple<double, double, double> to_xyd() const {

    double d = 1.0;
    double x = 0, y = 0;
    for (const auto &t : r) {
      d /= 2;
      if (t[0]) {
        x += d;
      }
      if (t[1]) {
        y += d;
      }
    }
    return {x, y, d};
  }
  pair<pair<double, double>, pair<double, double>> to_ranges() const {
    auto [x, y, d] = to_xyd();
    return make_pair(make_pair(x - d, x + d), make_pair(y - d, y + d));
  }
  bool operator<(const range2 &rsh) const {
    auto [x1, y1, d1] = to_xyd();
    auto [x2, y2, d2] = rsh.to_xyd();
    return make_tuple(d1, x1, y1) < make_tuple(d2, x2, y2);
    // return lexicographical_compare(r.begin(), r.end(), rsh.r.begin(),
    //                                rsh.r.end(),
    //                                [](const bitset<2> &a, const bitset<2> &b)
    //                                {
    //                                  return a.to_ulong() < b.to_ulong();
    //                                });
  }
  vector<bitset<2>> r;
};
struct expr_drawer {
  expression expr;
  bool isInequality;
  mutex need_to_draw_m;
  priority_queue<range2> need_to_draw;
  vector<vector<point_status>> status =
      vector(scr_size + 1, vector(scr_size + 1, point_status::need_to_divide));
  // vector<tuple<double, double, double>> next_to_draw;
  char c1, c2, c3;
  bool process(const range2 &r // double i, double j, double d
  ) {
    auto [i, j, d] = r.to_xyd();
    // if (status[i * scr_size][j * scr_size] == point_status::no)
    //   return false;
    // if (status[i * scr_size][j * scr_size] == point_status::yes)
    //   return true;
    varible v =
        eval(expr,
             add(mul(varible(vector({make_pair(value(j), value(j + d))})),
                     value(xmax - xmin)),
                 value(xmin)),
             add(mul(varible(vector({make_pair(value(i), value(i + d))})),
                     value(ymax - ymin)),
                 value(ymin)));
    // cout << "(" << i << ", " << j << ")" << v << ' ' << d << endl;
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
  {                                                                            \
    lock_guard lock(need_to_draw_m);                                           \
    auto r1 = r;                                                               \
    r1.r.push_back(0);                                                         \
    if (d > 1.0 / scr_size) {                                                  \
      r1.r.back() = 0;                                                         \
      need_to_draw.push(r1);                                                   \
      if (i + d <= 1) {                                                        \
        r1.r.back() = 1;                                                       \
        need_to_draw.push(r1);                                                 \
      }                                                                        \
      if (j + d <= 1) {                                                        \
        r1.r.back() = 2;                                                       \
        need_to_draw.push(r1);                                                 \
      }                                                                        \
      if (i + d <= 1 && j + d <= 1) {                                          \
        r1.r.back() = 3;                                                       \
        need_to_draw.push(r1);                                                 \
      }                                                                        \
    }                                                                          \
  }
          add_sub();
          break;
        }
      } else {
        if (it->first <= value(0) && value(0) <= it->second) {
          is = point_status::need_to_divide;
          add_sub();
          break;
        } else {
          is = point_status::no;
        }
      }
    }
    /*
    cout << i << ' ' << j << ' ' << d << " " << v << endl;
    cout << i * (ymax - ymin) + ymin << ' ' << j * (xmax - xmin) + xmin << ' '
         << d * (ymax - ymin) << " " << d * (xmax - xmin) << " " << v << endl;
    */
    for (int x = i * scr_size; x < min(1.0, i + d) * scr_size; ++x)
      for (int y = j * scr_size; y < min(1.0, j + d) * scr_size; ++y) {
        // auto [color0, color1, color2] = get_color(is, v.has_undefined);
        // image[(x * scr_size + y) * 3 + 0] = color0;
        // image[(x * scr_size + y) * 3 + 1] = color1;
        // image[(x * scr_size + y) * 3 + 2] = color2;
        // cout << "(" << x << "," << y << ")" << (is == point_status::yes)
        //      << (is == point_status::no);
        status[x][y] = is;
      }
    done = false;
    return is != point_status::no;
  };
  string Sexpr;
  expr_drawer(string sexpr, bool ine) {
    lock_guard l(need_to_draw_m);
    expr = tokenize(sexpr);
    need_to_draw.push({0, 0, 0});
    isInequality = ine;
    Sexpr = sexpr;
  }
  // int id = 0;
  bool done = false;
  void process_one() {
    if (done) {
      return;
    }
    decltype(need_to_draw)::value_type front;
    {
      lock_guard l(need_to_draw_m);
      if (need_to_draw.size() == 0) {
        done = true;
        cout << "Done" << endl;
        return;
      }
      // if (id == need_to_draw.size()) {
      //   id = 0;
      //   need_to_draw = next_to_draw;
      // next_to_draw.clear();
      //   if (need_to_draw.size() == 0) {
      //     done = true;
      //     cout << "Done" << endl;
      //   }
      //   return;
      // }
      front = need_to_draw.top();
      need_to_draw.pop();
    }
    // if (status[get<0>(front) * scr_size][get<1>(front) * scr_size] ==
    //     point_status::need_to_divide)
    process(front);
    // process(get<0>(front), get<1>(front), get<2>(front));
    //  ++id;
  }
};
bool stop = false;
void set_stop(int x) {
  stop = true;
  cout << "x: " << x << endl;
}
int main(int argc, char **argv) {
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

  // cin >> xmin >> xmax >> ymin >> ymax;
  // xmin = ymin = -10;
  // xmax = ymax = 10;
  xmin = ymin = 0; // for six.exp
  xmax = ymax = 1;
  // xmin = -8.61425; for xlcp.exp
  // xmax = -6.857;
  // ymin = -7.02332031125;
  // ymax = 6.0648203125;
  signal(SIGINT, set_stop);
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
      color0 = color1 = color2 = 64;
    }
    if (is == point_status::need_to_divide && !undefined) {
      color1 = 255; // g
      color0 = color2 = 64;
    }
    if (is != point_status::no) {
      // color2 = 128;
      color1 = 255; // g
      color0 = color2 = 0;
    }
    return make_tuple(color0, color1, color2);
  };
  bool done = false;
  auto it = 0;
  vector<shared_ptr<expr_drawer>> draws;
  int cur = 0;
  while (!glfwWindowShouldClose(window)) {
    glBindTexture(GL_TEXTURE_2D, texture);
    if (draws.size())
      while (draws.at(cur)->done) {
        ++cur;
        cur %= draws.size();
        if (cur == 0)
          break;
      }
    if (cur == 0 &&
        ((draws.size() == 1 && draws.at(0)->done) || draws.size() != 1))
      stop = true;
    if (cur < draws.size()) {
      vector<thread> ths;
      cout << cur << " : ";
      for (const auto &x : draws[cur]->need_to_draw.top().r) {
        cout << x.to_ullong();
      }
      cout << endl;
      for (int i = 0; i < 8; ++i) {
        ths.push_back(thread([&, i] {
          for (int c = 0; c < 64 && !stop; ++c) {
            // cout << i << ' ' << c << endl;
            draws.at(cur)->process_one();
          }
        }));
      }
      for (auto &t : ths)
        t.join();
    }
    if (draws.size() > cur)
      cout << draws.at(cur)->need_to_draw.size() << endl;
    for (int i = 0; i < scr_size; ++i) {
      for (int j = 0; j < scr_size; ++j) {
        image[(i * scr_size + j) * 3 + 0] = image[(i * scr_size + j) * 3 + 1] =
            image[(i * scr_size + j) * 3 + 2] = 0;
      }
    }
    for (int c = 0; c < draws.size(); ++c)
      for (int i = 0; i < scr_size; ++i) {
        for (int j = 0; j < scr_size; ++j) {
          bool v = (draws.at(c)->status[i][j] != point_status::no);
          if (v) {
            image[(i * scr_size + j) * 3 + 0] = draws[c]->c1;
            image[(i * scr_size + j) * 3 + 1] = draws[c]->c2;
            image[(i * scr_size + j) * 3 + 2] = draws[c]->c3;
          }
          // bool u = (draws[c].status[i][j] == point_status::need_to_divide);
          // if (u) {
          //   image[(i * scr_size + j) * 3 + 0] = 64;
          //   image[(i * scr_size + j) * 3 + 1] = 64;
          //   image[(i * scr_size + j) * 3 + 2] = 64;
          // }
        }
      }

    if (stop) {
      cout << "Equation, Inequality, List, Setcolor, Delete, Quit" << endl;
      cin.clear();
      char c;
      cin >> c;
      if (c == 'q')
        break;
      bool isineq = false;
      switch (c) {
      case 'i':
        isineq = true;
      case 'e': {
        cout << " expr > ";
        string sexpr;
        getline(cin, sexpr); // eat the endl
        getline(cin, sexpr);
        if (sexpr[0] == 'F') {
          ifstream e(sexpr.substr(1));
          cout << "reading " << sexpr.substr(1) << endl;
          sexpr = "";
          do {
            sexpr += e.get();
          } while (e);
          sexpr.erase(sexpr.size() - 1);
        }
        cout << "color > ";
        int c1, c2, c3;
        cin >> c1 >> c2 >> c3;
        draws.push_back(make_shared<expr_drawer>(sexpr, isineq));
        draws.back()->c1 = c1;
        draws.back()->c2 = c2;
        draws.back()->c3 = c3;
      } break;
      case 'l':
        for (int i = 0; i < draws.size(); ++i) {
          const auto &x = draws.at(i);
          cout << i << ": "
               << "expr: " << x->Sexpr << " color: ("
               << ((unsigned int)x->c1 & 0xff) << ", "
               << ((unsigned int)x->c2 & 0xff) << ", "
               << ((unsigned int)x->c3 & 0xff) << ")" << endl;
        }
        break;
      case 's': {
        int i;
        cout << "  id  >";
        cin >> i;
        if (i >= draws.size()) {
          cout << i << ">=" << draws.size() << endl;
          break;
        }
        cout << "color > ";
        int c1, c2, c3;
        cin >> c1 >> c2 >> c3;
        draws.at(i)->c1 = c1;
        draws.at(i)->c2 = c2;
        draws.at(i)->c3 = c3;
      } break;
      case 'd':
        int i;
        cout << "  id  >";
        cin >> i;
        if (i >= draws.size()) {
          cout << i << ">=" << draws.size() << endl;
          break;
        }
        draws.erase(draws.begin() + i);
        break;
      }
      stop = false;
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
