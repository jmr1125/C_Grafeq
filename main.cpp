#include "flint/arf.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <mutex>
#include <ostream>
#include <queue>
#include <utility>
#define GL_SILENCE_DEPRECATION
#include "prog.hpp"
#include "value.hpp"
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
} // example expr   xv 30 mSv 0.2 mv 0.4 ays
enum state { yes, maybe, no };
void plot(double xmin, double xmax, double ymin, double ymax, int scr_size,
          vector<state> &image, string fname) {
  prog p;
  {
    ifstream ifs(fname);
    p.load(ifs);
  }
  for_each(image.begin(), image.end(), [](state &t) { t = maybe; });
  int k = log2(scr_size);
  vector<pair<range, range>> U, U1;
  {
    fval xl, xh, yl, yh;
    arf_set_d(xl.val, xmin);
    arf_set_d(xh.val, xmax);
    arf_set_d(yl.val, ymin);
    arf_set_d(yh.val, ymax);
    U1.push_back(pair<range, range>());
    arf_set(U1[0].first.lo.val, xl.val);
    arf_set(U1[0].first.hi.val, xh.val);
    arf_set(U1[0].second.lo.val, yl.val);
    arf_set(U1[0].second.hi.val, yh.val);
  }
  auto func = [&](const varible &x, const varible &y) {
    // // auto val = add(mul(x, y), x);
    // //  auto val = add(sin(mul(x, y)), sin(x));
    // //  auto val = add(pow(x, x), neg(y));
    // varible two;
    // two.r.push_back(range());
    // arf_set_d(two.r[0].lo.val, 2);
    // arf_set_d(two.r[0].hi.val, 2);
    // varible half;
    // half.r.push_back(range());
    // arf_set_d(half.r[0].lo.val, .5);
    // arf_set_d(half.r[0].hi.val, .5);
    // //  auto val = add(neg(y), pow(x, x));
    // // auto val = add(neg(y), reciprocal(add(x, two)));
    // // auto val = add(neg(y), add(log(cos(x)), pow(mul(x, x), half)));
    // auto val = add(neg(exp(add(sin(x), cos(y)))), sin(exp(add(x, y))));
    // //  auto val = add(x, y);
    // //  auto val = add(x, log(y));
    // //  auto val = add(sin(x), sin(y));
    // // auto val = add(y, pow(mul(x, x), half));
    auto val = p.eval(x, y);
    // cout << x << ' ' << y << " ==> " << val << endl;
    return val;
  };
  while (k >= 0 && !U1.empty()) {
    swap(U, U1);
    U1.clear();
    for (const auto &u : U) {
      int xl = (arf_get_d(u.first.lo.val, ARF_RND_NEAR) - xmin) /
               (xmax - xmin) * scr_size;
      int xh = (arf_get_d(u.first.hi.val, ARF_RND_NEAR) - xmin) /
               (xmax - xmin) * scr_size;
      int yl = (arf_get_d(u.second.lo.val, ARF_RND_NEAR) - ymin) /
               (ymax - ymin) * scr_size;
      int yh = (arf_get_d(u.second.hi.val, ARF_RND_NEAR) - ymin) /
               (ymax - ymin) * scr_size;
      varible x(u.first), y(u.second);
      auto val = func(x, y);
      bool f = true;
      for (const auto &r : val.r) {
        if (arf_sgn(r.hi.val) >= 0 && arf_sgn(r.lo.val) <= 0) {
          f = false;
        }
      }
      // cout << xl << ' ' << xh << ' ' << yl << ' ' << yh << endl;
      // cout << x << ' ' << y << " => " << val << " " << f << endl;
      if (f) {
        for (int i = yl; i < yh; ++i)
          for (int j = xl; j < xh; ++j) {
            image[i * scr_size + j] = no;
          }
      } else {
        fval xm, ym;
        arf_add(xm.val, u.first.lo.val, u.first.hi.val, 128, ARF_RND_NEAR);
        arf_div_ui(xm.val, xm.val, 2, 128, ARF_RND_NEAR);
        arf_add(ym.val, u.second.lo.val, u.second.hi.val, 128, ARF_RND_NEAR);
        arf_div_ui(ym.val, ym.val, 2, 128, ARF_RND_NEAR);
        for (int i = 0; i < 4; ++i) {
          U1.push_back(pair<range, range>());
          if (i / 2) {
            arf_set(U1.back().first.lo.val, u.first.lo.val);
            arf_set(U1.back().first.hi.val, xm.val);
          } else {
            arf_set(U1.back().first.lo.val, xm.val);
            arf_set(U1.back().first.hi.val, u.first.hi.val);
          }
          if (i % 2) {
            arf_set(U1.back().second.lo.val, u.second.lo.val);
            arf_set(U1.back().second.hi.val, ym.val);
          } else {
            arf_set(U1.back().second.lo.val, ym.val);
            arf_set(U1.back().second.hi.val, u.second.hi.val);
          }
        }
      }
    }
    k--;
  }
  cout << "px done" << endl;
  vector<int> root_cnt(scr_size * scr_size, 0);
  for (const auto &u : U1) {
    const double xx0 = arf_get_d(u.first.lo.val, ARF_RND_FLOOR),
                 yy0 = arf_get_d(u.second.lo.val, ARF_RND_FLOOR);
    const int x0 = (xx0 - xmin) / (xmax - xmin) * scr_size;
    const int y0 = (yy0 - ymin) / (ymax - ymin) * scr_size;
    root_cnt[y0 * scr_size + x0]++;
  }
  while (!U1.empty()) {
    swap(U, U1);
    U1.clear();
    for (const auto &u : U) {
      int x0 = (arf_get_d(u.first.lo.val, ARF_RND_FLOOR) - xmin) /
               (xmax - xmin) * scr_size;
      int y0 = (arf_get_d(u.second.lo.val, ARF_RND_FLOOR) - ymin) /
               (ymax - ymin) * scr_size;
      if (root_cnt[y0 * scr_size + x0] < 0)
        continue;
      varible x(u.first), y(u.second);
      auto val = func(x, y);
      bool f = true;
      for (const auto &r : val.r) {
        if (arf_sgn(r.hi.val) >= 0 && arf_sgn(r.lo.val) <= 0) {
          f = false;
        }
      }
      // cout << xl << ' ' << xh << ' ' << yl << ' ' << yh << endl;
      // cout << x << ' ' << y << " => " << val << " " << f << endl;
      root_cnt[y0 * scr_size + x0]--;
      if (f) {
        if (root_cnt[y0 * scr_size + x0] == 0 &&
            image[y0 * scr_size + x0] == maybe) {
          image[y0 * scr_size + x0] = no;
        }
      } else {
        varible llx, hhx, lly, hhy;
        llx.r.push_back(range());
        hhx.r.push_back(range());
        lly.r.push_back(range());
        hhy.r.push_back(range());
        llx.r[0].lo = llx.r[0].hi = u.first.lo;
        hhx.r[0].lo = hhx.r[0].hi = u.first.hi;
        lly.r[0].lo = lly.r[0].hi = u.second.lo;
        hhy.r[0].lo = hhy.r[0].hi = u.second.hi;
        vector<pair<bool, bool>> sgns;
        for (int i = 0; i < 4; ++i) {
          auto val = func((i / 2) ? llx : hhx, (i % 2) ? lly : hhy);
          bool m = false, p = false;
          for (const auto &u : val.r) {
            if (arf_sgn(u.hi.val) <= 0)
              m = true;
            if (arf_sgn(u.hi.val) >= 0)
              p = true;
          }
          if (m || p)
            sgns.push_back({m, p});
        }
        bool t = false;
        for (int i = 0; i < sgns.size(); ++i) {
          for (int j = 0; j < i; ++j) {
            if (sgns[i].first && sgns[j].second ||
                sgns[i].second && sgns[j].first) {
              t = true;
              break;
            }
          }
          if (t)
            break;
        }
        // t = arf_sgn(ll.r[0].hi.val) * arf_sgn(hh.r[0].hi.val) <= 0;
        if (val.cont == make_pair(true, true) && t) {
          image[y0 * scr_size + x0] = yes;
          root_cnt[y0 * scr_size + x0] = -1;
        } else {
          root_cnt[y0 * scr_size + x0] += 4;
          fval xm, ym;
          arf_add(xm.val, u.first.lo.val, u.first.hi.val, 128, ARF_RND_NEAR);
          arf_div_ui(xm.val, xm.val, 2, 128, ARF_RND_NEAR);
          arf_add(ym.val, u.second.lo.val, u.second.hi.val, 128, ARF_RND_NEAR);
          arf_div_ui(ym.val, ym.val, 2, 128, ARF_RND_NEAR);
          for (int i = 0; i < 4; ++i) {
            U1.push_back(pair<range, range>());
            if (i / 2) {
              arf_set(U1.back().first.lo.val, u.first.lo.val);
              arf_set(U1.back().first.hi.val, xm.val);
            } else {
              arf_set(U1.back().first.lo.val, xm.val);
              arf_set(U1.back().first.hi.val, u.first.hi.val);
            }
            if (i % 2) {
              arf_set(U1.back().second.lo.val, u.second.lo.val);
              arf_set(U1.back().second.hi.val, ym.val);
            } else {
              arf_set(U1.back().second.lo.val, ym.val);
              arf_set(U1.back().second.hi.val, u.second.hi.val);
            }
          }
        }
      }
    }
    k--;
  }
  cout << "subpx done" << endl;
}
struct plotter {
  enum pxstate { yes, maybe, no };
  vector<pxstate> image;
  vector<int> root_cnt;
  int scr_size;
  double xmin, xmax, ymin, ymax;
  vector<pair<range, range>> U, U1;
  int k;
  enum plotstate { px, subpx, done } state;
  decltype(U)::iterator iter;
  prog p;
  plotter(int scr_size, double xmin, double xmax, double ymin, double ymax,
          string file)
      : scr_size(scr_size), xmin(xmin), xmax(xmax), ymin(ymin), ymax(ymax) {
    image.resize(scr_size * scr_size, maybe);
    root_cnt.resize(scr_size * scr_size, 0);
    k = log2(scr_size);
    fval xl, xh, yl, yh;
    arf_set_d(xl.val, xmin);
    arf_set_d(xh.val, xmax);
    arf_set_d(yl.val, ymin);
    arf_set_d(yh.val, ymax);
    U1.push_back(pair<range, range>());
    arf_set(U1[0].first.lo.val, xl.val);
    arf_set(U1[0].first.hi.val, xh.val);
    arf_set(U1[0].second.lo.val, yl.val);
    arf_set(U1[0].second.hi.val, yh.val);
    iter = U.begin();
    state = px;
    ifstream ifs(file);
    p.load(ifs);
  }
  void step() {
    if (state == done)
      return;
    if (state == px) {
      if (k < 0) {
        for (const auto &u : U1) {
          const double xx0 = arf_get_d(u.first.lo.val, ARF_RND_FLOOR),
                       yy0 = arf_get_d(u.second.lo.val, ARF_RND_FLOOR);
          const int x0 = (xx0 - xmin) / (xmax - xmin) * scr_size;
          const int y0 = (yy0 - ymin) / (ymax - ymin) * scr_size;
          root_cnt[y0 * scr_size + x0]++;
        }
        state = subpx;
        iter = U.begin();
        return;
      }
      if (iter == U.begin()) {
        if (U1.empty()) {
          state = done;
          return;
        }
        swap(U, U1);
        U1.clear();
        iter = U.begin();
      }
      if (iter == U.end()) {
        iter = U.begin();
        k--;
        return;
      }
      const auto &u = *iter;
      int xl = (arf_get_d(u.first.lo.val, ARF_RND_NEAR) - xmin) /
               (xmax - xmin) * scr_size;
      int xh = (arf_get_d(u.first.hi.val, ARF_RND_NEAR) - xmin) /
               (xmax - xmin) * scr_size;
      int yl = (arf_get_d(u.second.lo.val, ARF_RND_NEAR) - ymin) /
               (ymax - ymin) * scr_size;
      int yh = (arf_get_d(u.second.hi.val, ARF_RND_NEAR) - ymin) /
               (ymax - ymin) * scr_size;
      varible x(u.first), y(u.second);
      auto val = p.eval(x, y);
      bool f = true;
      for (const auto &r : val.r) {
        if (arf_sgn(r.hi.val) >= 0 && arf_sgn(r.lo.val) <= 0) {
          f = false;
        }
      }
      // cout << xl << ' ' << xh << ' ' << yl << ' ' << yh << endl;
      // cout << x << ' ' << y << " => " << val << " " << f << endl;
      if (f) {
        for (int i = yl; i < yh; ++i)
          for (int j = xl; j < xh; ++j) {
            image[i * scr_size + j] = no;
          }
      } else {
        fval xm, ym;
        arf_add(xm.val, u.first.lo.val, u.first.hi.val, 128, ARF_RND_NEAR);
        arf_div_ui(xm.val, xm.val, 2, 128, ARF_RND_NEAR);
        arf_add(ym.val, u.second.lo.val, u.second.hi.val, 128, ARF_RND_NEAR);
        arf_div_ui(ym.val, ym.val, 2, 128, ARF_RND_NEAR);
        for (int i = 0; i < 4; ++i) {
          U1.push_back(pair<range, range>());
          if (i / 2) {
            arf_set(U1.back().first.lo.val, u.first.lo.val);
            arf_set(U1.back().first.hi.val, xm.val);
          } else {
            arf_set(U1.back().first.lo.val, xm.val);
            arf_set(U1.back().first.hi.val, u.first.hi.val);
          }
          if (i % 2) {
            arf_set(U1.back().second.lo.val, u.second.lo.val);
            arf_set(U1.back().second.hi.val, ym.val);
          } else {
            arf_set(U1.back().second.lo.val, ym.val);
            arf_set(U1.back().second.hi.val, u.second.hi.val);
          }
        }
      }
      iter++;
    }
    if (state == subpx) {
      if (iter == U.begin()) {
        if (U1.empty()) {
          state = done;
          return;
        }
        swap(U, U1);
        U1.clear();
        iter = U.begin();
      }
      if (iter == U.end()) {
        iter = U.begin();
        k--;
        return;
      }
      const auto &u = *iter;
      // cout << state << ' ' << k << ' ' << iter - U.begin() << ' ' << U.size()
      //      << " " << U1.size() << endl;
      int x0 = (arf_get_d(u.first.lo.val, ARF_RND_FLOOR) - xmin) /
               (xmax - xmin) * scr_size;
      int y0 = (arf_get_d(u.second.lo.val, ARF_RND_FLOOR) - ymin) /
               (ymax - ymin) * scr_size;
      if (root_cnt[y0 * scr_size + x0] < 0) {
        iter++;
        return;
      }
      varible x(u.first), y(u.second);
      auto val = p.eval(x, y);
      bool f = true;
      for (const auto &r : val.r) {
        if (arf_sgn(r.hi.val) >= 0 && arf_sgn(r.lo.val) <= 0) {
          f = false;
        }
      }
      // cout << xl << ' ' << xh << ' ' << yl << ' ' << yh << endl;
      // cout << x << ' ' << y << " => " << val << " " << f << endl;
      root_cnt[y0 * scr_size + x0]--;
      if (f) {
        if (root_cnt[y0 * scr_size + x0] == 0 &&
            image[y0 * scr_size + x0] == maybe) {
          image[y0 * scr_size + x0] = no;
        }
      } else {
        varible llx, hhx, lly, hhy;
        llx.r.push_back(range());
        hhx.r.push_back(range());
        lly.r.push_back(range());
        hhy.r.push_back(range());
        llx.r[0].lo = llx.r[0].hi = u.first.lo;
        hhx.r[0].lo = hhx.r[0].hi = u.first.hi;
        lly.r[0].lo = lly.r[0].hi = u.second.lo;
        hhy.r[0].lo = hhy.r[0].hi = u.second.hi;
        vector<pair<bool, bool>> sgns;
        for (int i = 0; i < 4; ++i) {
          auto val = p.eval((i / 2) ? llx : hhx, (i % 2) ? lly : hhy);
          bool m = false, p = false;
          for (const auto &u : val.r) {
            if (arf_sgn(u.hi.val) <= 0)
              m = true;
            if (arf_sgn(u.hi.val) >= 0)
              p = true;
          }
          if (m || p)
            sgns.push_back({m, p});
        }
        bool t = false;
        for (int i = 0; i < sgns.size(); ++i) {
          for (int j = 0; j < i; ++j) {
            if (sgns[i].first && sgns[j].second ||
                sgns[i].second && sgns[j].first) {
              t = true;
              break;
            }
          }
          if (t)
            break;
        }
        // t = arf_sgn(ll.r[0].hi.val) * arf_sgn(hh.r[0].hi.val) <= 0;
        if (val.cont == make_pair(true, true) && t) {
          image[y0 * scr_size + x0] = yes;
          root_cnt[y0 * scr_size + x0] = -1;
        } else {
          root_cnt[y0 * scr_size + x0] += 4;
          fval xm, ym;
          arf_add(xm.val, u.first.lo.val, u.first.hi.val, 128, ARF_RND_NEAR);
          arf_div_ui(xm.val, xm.val, 2, 128, ARF_RND_NEAR);
          arf_add(ym.val, u.second.lo.val, u.second.hi.val, 128, ARF_RND_NEAR);
          arf_div_ui(ym.val, ym.val, 2, 128, ARF_RND_NEAR);
          for (int i = 0; i < 4; ++i) {
            U1.push_back(pair<range, range>());
            if (i / 2) {
              arf_set(U1.back().first.lo.val, u.first.lo.val);
              arf_set(U1.back().first.hi.val, xm.val);
            } else {
              arf_set(U1.back().first.lo.val, xm.val);
              arf_set(U1.back().first.hi.val, u.first.hi.val);
            }
            if (i % 2) {
              arf_set(U1.back().second.lo.val, u.second.lo.val);
              arf_set(U1.back().second.hi.val, ym.val);
            } else {
              arf_set(U1.back().second.lo.val, ym.val);
              arf_set(U1.back().second.hi.val, u.second.hi.val);
            }
          }
        }
      }
      iter++;
    }
  }
};
int main(int argc, char **argv) {
  int scr_size = 512;
  // cin >> scr_size;

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
  bool quit = false;
  vector<plotter> plot_pool;
  thread calc_th([&]() { // plot(-10, 10, -10, 10, scr_size, img, "test_prog");
    plotter plot(scr_size, -10, 10, -10, 10, "test_prog");
    int i = 0;
    while (!quit) {
      auto t0 = chrono::steady_clock::now();
      for (; !quit;) {
        for (int i = 0; i < 10; ++i)
          plot.step();
        if (chrono::steady_clock::now() - t0 >= 10ms) {
          i = (i + 1) % plot_pool.size();
        }
      }
    }
  });
  while (!glfwWindowShouldClose(window)) {
    glBindTexture(GL_TEXTURE_2D, texture);

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
  quit = true;
  calc_th.join();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
