
#include "eval.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <mutex>
#include <ostream>
#include <queue>
#include <utility>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
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
int scr_size = 512, scr_size_dep = 9;
double xmin, xmax, ymin, ymax;
enum class point_status { need_to_divide = 0, no, yes };
struct range2 {
  range2() = default;
  range2(double x, double y, int n) {
    r.resize(n);
    for (int i = 0; i < n; ++i) {
      if (y >= 1)
        r[i] += 1;
      if (y >= 1)
        r[i] += 2;
      x *= 2;
      y *= 2;
    }
  }
  tuple<double, double, double> to_xyd() const {

    double d = 1.0;
    double x = 0, y = 0;
    for (const auto &t : r) {
      d /= 2;
      if (t & 1) {
        x += d;
      }
      if (t & 2) {
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
    if (r.size() <= scr_size_dep && rsh.r.size() <= scr_size_dep) {
      if (r.size() > rsh.r.size())
        return true;
      if (r.size() < rsh.r.size())
        return false;
    }
    size_t len = min((size_t)scr_size_dep, r.size());
    auto r_rbegin = std::make_reverse_iterator(r.begin() + len);
    auto r_rend = std::make_reverse_iterator(r.begin());

    auto rsh_rbegin = std::make_reverse_iterator(rsh.r.begin() + len);
    auto rsh_rend = std::make_reverse_iterator(rsh.r.begin());
    if (!equal(r_rbegin, r_rend, rsh_rbegin))
      return std::lexicographical_compare(r_rbegin, r_rend, rsh_rbegin,
                                          rsh_rend);

    if (r.size() > rsh.r.size())
      return true;
    else
      return false;
  }
  vector<char> r;
};
struct expr_drawer {
  expression expr;
  bool isInequality;
  mutex need_to_draw_m, prec_m;
  priority_queue<range2> need_to_draw;
  vector<vector<point_status>> status;
  vector<vector<atomic<int>>> prec;
  vector<vector<atomic<int>>> root_count;
  // vector<tuple<double, double, double>> next_to_draw;
  char c1, c2, c3;
  bool process(const range2 &r // double i, double j, double d
  ) {
    if (!isInequality) {
      range2 r1 = r;
      if (r1.r.size() > scr_size_dep)
        r1.r.resize(scr_size_dep, 0);
      auto [I, J, D] = r1.to_xyd();
      for (int x = I * scr_size; x < (I + D) * scr_size; ++x)
        for (int y = J * scr_size; y < (J + D) * scr_size; ++y) {
          while (true) {
            bool out = false;
            do {
              lock_guard l(prec_m);
              if (prec[x][y] < r1.r.size()) {
                break;
              }
              root_count[x][y]--;
              out = true;
            } while (0);
            if (out) {
              break;
            }
            this_thread::yield();
          }
        }
    }
    auto [i, j, d] = r.to_xyd();
    varible v =
        expr.eval(add(mul(varible(vector({make_pair(value(j), value(j + d))})),
                          value(xmax - xmin)),
                      value(xmin)),
                  add(mul(varible(vector({make_pair(value(i), value(i + d))})),
                          value(ymax - ymin)),
                      value(ymin)));
    // cout << "(" << i << ", " << j << ")" << v << ' ' << d << endl;
#define add_sub()                                                              \
  {                                                                            \
    lock_guard lock(need_to_draw_m);                                           \
    auto r1 = r;                                                               \
    r1.r.push_back(0);                                                         \
    for (int i = 0; i < 4; ++i) {                                              \
      r1.r.back() = i;                                                         \
      need_to_draw.push(r1);                                                   \
    }                                                                          \
  }
    // cout << i << ' ' << j << ' ' << d << endl << v << endl;
    // ;
    point_status is = point_status::no;
    for (auto it = v.ranges.begin(); it != v.ranges.end(); ++it) {
      if (isInequality) {
        if (value(0) <= it->first) {
          is = point_status::yes;
          break;
        }
        if (value(0) <= it->second) {
          is = point_status::need_to_divide;
          if (d > 1.0 / scr_size) {
            add_sub();
          }
          break;
        }
      } else { // equation
        if (it->first <= value(0) && value(0) <= it->second ||
            v.has_undefined) {
          for (int s = 0; s < 4; ++s) {
            range2 r1 = r;
            r1.r.push_back(s);
            if (r1.r.size() > scr_size_dep)
              r1.r.resize(scr_size_dep, 0);
            auto [I, J, D] = r1.to_xyd();
            for (int x = I * scr_size; x < (I + D) * scr_size; ++x)
              for (int y = J * scr_size; y < (J + D) * scr_size; ++y) {
                while (true) {
                  bool out = false;
                  do {
                    lock_guard l(prec_m);
                    if (prec[x][y] < r1.r.size() - 1) {
                      break;
                    }
                    root_count[x][y]++;
                    out = true;
                  } while (0);
                  if (out) {
                    break;
                  }
                  this_thread::yield();
                }
              }
          }
          is = point_status::need_to_divide;
          if (d > 1.0 / scr_size / 64) {
            add_sub();
          }
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
    {
      range2 r1 = r;
      if (r1.r.size() > scr_size_dep)
        r1.r.resize(scr_size_dep, 0);
      auto [I, J, D] = r1.to_xyd();
      for (int x = I * scr_size; x < (I + D) * scr_size; ++x)
        for (int y = J * scr_size; y < (J + D) * scr_size; ++y) {
          while (true) {
            bool out = false;
            do {
              lock_guard l(prec_m);
              if (prec[x][y] < r1.r.size()) {
                break;
              }
              prec[x][y] = r1.r.size() + 1;
              status[x][y] = is;
              out = true;
            } while (0);
            if (out) {
              break;
            }
            this_thread::yield();
          }
        }
    }
    done = false;
    return is != point_status::no;
  };
  string Sexpr;
  expr_drawer(string sexpr, bool ine, bool meticulous = false) {
    lock_guard l(need_to_draw_m);
    expr = tokenize(sexpr);
    status = vector(scr_size + 1,
                    vector(scr_size + 1, point_status::need_to_divide));
    if (!ine) {
      root_count.resize(scr_size + 1);
      for (auto &t : root_count) {
        t = vector<atomic<int>>(scr_size + 1);
        for (auto &u : t) {
          u = 1;
        }
      }
    }
    prec.resize(scr_size + 1);
    for (auto &t : prec) {
      t = vector<atomic<int>>(scr_size + 1);
      for (auto &u : t) {
        if (meticulous) {
          u = scr_size_dep;
        } else {
          u = 0;
        }
      }
    }
    if (!meticulous) {
      need_to_draw.push({0, 0, 0});
    } else {
      for (long long i = 0; i < (1 << (2 * scr_size_dep)); ++i) {
        range2 r;
        r.r.resize(scr_size_dep);
        for (int x = 0; x < scr_size_dep; ++x) {
          r.r[x] = (i >> (x * 2)) & 3;
        }
        need_to_draw.push(r);
        // cout << i << ' ' << (1 << (2 * scr_size_dep)) << endl;
      }
    }
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
        cerr << "Done" << endl;
        return;
      }
      front = need_to_draw.top();
      need_to_draw.pop();
    }
    process(front);
  }
};
bool stop = false;
void set_stop(int x) {
  stop = true;
  cerr << "x: " << x << endl;
} // example expr   xv 30 mSv 0.2 mv 0.4 ays
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
  xmin = ymin = -10;
  xmax = ymax = 10;
  // xmin = ymin = 0; // for six.exp
  // xmax = ymax = 1;
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
  int thread_num = thread::hardware_concurrency();
  int changes = 0;
  int tot = 1024;
  printf("YUV4MPEG2 W%d H%d F25:1 Ip A0:0 C444", scr_size, scr_size);
  putchar(10);
  while (!glfwWindowShouldClose(window)) {
    glBindTexture(GL_TEXTURE_2D, texture);
    if (draws.size()) {
      ++cur;
      cur %= draws.size();
      while (draws.at(cur)->done) {
        ++cur;
        cur %= draws.size();
        if (cur == 0)
          break;
      }
    }
    stop = all_of(draws.begin(), draws.end(),
                  [](shared_ptr<expr_drawer> x) { return x->done; });
    // for (int i = 0; i < scr_size; ++i) {
    //   for (int j = 0; j < scr_size; ++j) {
    //     image[(i * scr_size + j) * 3 + 0] = image[(i * scr_size + j) * 3 + 1]
    //     =
    //         image[(i * scr_size + j) * 3 + 2] = 0;
    //   }
    // }
    for (int i = 0; i < scr_size; ++i) {
      for (int j = 0; j < scr_size; ++j) {
        unsigned char r = 0, g = 0, b = 0;
        for (int c = 0; c < draws.size(); ++c) {
          bool v;
          if (draws.at(c)->isInequality) {
            v = (draws.at(c)->status[i][j] != point_status::no);
          } else {
            v = draws.at(c)->root_count[i][j];
          }
          if (v) {
            r = draws[c]->c1;
            g = draws[c]->c2;
            b = draws[c]->c3;
            break;
          }
        }
        if (image[(i * scr_size + j) * 3 + 0] == r &&
            image[(i * scr_size + j) * 3 + 1] == g &&
            image[(i * scr_size + j) * 3 + 2] == b) {
        } else {
          ++changes;
          image[(i * scr_size + j) * 3 + 0] = r;
          image[(i * scr_size + j) * 3 + 1] = g;
          image[(i * scr_size + j) * 3 + 2] = b;
        }
      }
    }
    bool to_draw_yuv = !stop && changes >= 40;
    if (cur < draws.size()) {
      auto t1 = chrono::system_clock::now();
      vector<thread> ths;
      cerr << cur << " : ";
      {
        int i = 0;
        for (const auto &x : draws[cur]->need_to_draw.top().r) {
          cerr << (int)x;
          if ((++i) == scr_size_dep)
            cerr << " ";
        }
      }
      cerr << "   (" << changes << " px changes)    \r" << flush;
      // int thread_num = // 3;
      //     thread::hardware_concurrency() / 2;
      for (int i = 0; i < thread_num; ++i) {
        ths.push_back(thread([&, i] {
          for (int c = 0; c < tot / thread_num && !stop; ++c) {
            // cout << i << ' ' << c << endl;
            draws.at(cur)->process_one();
          }
        }));
      }
      for (auto &t : ths)
        t.join();
      auto t2 = chrono::system_clock::now();
      if (t2 - t1 < 300ms) {
        tot += 16;
      } else {
        tot -= 16;
      }
    }
    if (draws.size() > cur)
      cerr << draws.at(cur)->need_to_draw.size() << " calc left   ";

    if (to_draw_yuv || stop) {
      if (to_draw_yuv) {
        changes = 0;
      }
#if 0
      printf("FRAME");
      putchar(10);
      for (int c = 0; c < 3; ++c)
        // for (int i = 0; i < scr_size; ++i)
        for (int i = scr_size - 1; i >= 0; --i) {
          // cerr << i << ' ';
          for (int j = 0; j < scr_size; ++j) {
            int r = image[(i * scr_size + j) * 3 + 0],
                g = image[(i * scr_size + j) * 3 + 1],
                b = image[(i * scr_size + j) * 3 + 2];
            int y = 16 + (65.738 * r + 129.057 * g + 25.064 * b) / 256,
                u = 128 + (-37.945 * r - 74.494 * g + 112.439 * b) / 256,
                v = 128 + (112.439 * r - 94.154 * g - 18.285 * b) / 256;
            int yuv[] = {y, u, v};
            putchar(yuv[c]);
          }
        }
#endif
    }
    if (stop) {
      cerr << endl
           << "Equation, Inequality, List, Setcolor, Delete, Quit" << endl;
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
        cerr << " expr > ";
        string sexpr;
        getline(cin, sexpr); // eat the endl
        getline(cin, sexpr);
        if (sexpr[0] == 'F') {
          ifstream e(sexpr.substr(1));
          cerr << "reading " << sexpr.substr(1) << endl;
          sexpr = "";
          do {
            sexpr += e.get();
          } while (e);
          sexpr.erase(sexpr.size() - 1);
        }
        cerr << "color > ";
        int c1, c2, c3;
        cin >> c1 >> c2 >> c3;
        cerr << "start meticulous? >";
        int f;
        cin >> f;
        draws.push_back(make_shared<expr_drawer>(sexpr, isineq, f));
        draws.back()->c1 = c1;
        draws.back()->c2 = c2;
        draws.back()->c3 = c3;
        cerr << "start" << endl << endl;
      } break;
      case 'l':
        for (int i = 0; i < draws.size(); ++i) {
          const auto &x = draws.at(i);
          cerr << i << ": "
               << "expr: " << x->Sexpr << " color: ("
               << ((unsigned int)x->c1 & 0xff) << ", "
               << ((unsigned int)x->c2 & 0xff) << ", "
               << ((unsigned int)x->c3 & 0xff) << ")" << endl;
        }
        break;
      case 's': {
        int i;
        cerr << "  id  >";
        cin >> i;
        if (i >= draws.size()) {
          cerr << i << ">=" << draws.size() << endl;
          break;
        }
        cerr << "color > ";
        int c1, c2, c3;
        cin >> c1 >> c2 >> c3;
        draws.at(i)->c1 = c1;
        draws.at(i)->c2 = c2;
        draws.at(i)->c3 = c3;
      } break;
      case 'd':
        int i;
        cerr << "  id  >";
        cin >> i;
        if (i >= draws.size()) {
          cerr << i << ">=" << draws.size() << endl;
          break;
        }
        draws.erase(draws.begin() + i);
        break;
      case 'T':
        cerr << "thread num: " << thread_num << endl << " -> ";
        cin >> thread_num;
        if (thread_num < 1) {
          cerr << "thread_num<1" << endl;
          thread_num = 1;
        }
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
