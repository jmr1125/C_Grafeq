// #include <GL/glew.h>    /* include GLEW and new version of GL on Windows */
// #include <GLFW/glfw3.h> /* GLFW helper library */
// #include <iostream>
// #include <tuple>
// #include <vector>
// using namespace std;
// static const char *vertex_shader_code = R"(
// #version 410
// attribute vec2 vPosition;
// varying vec2 texcoord;
// void main(){
// gl_Position = vec4(vPosition,0.0,1.0);
// texcoord = vPosition;
// }
// )";
// static const char *fragment_shader_code = R"(
// #version 410
// uniform sampler2D texture;
// uniform vec3 color;
// varying vec2 texcoord;
// void main(){
// 	gl_FragColor = vec4(color*texture2D(texture,texcoord).rgb, 1.0);
// }
// )";
// const GLfloat vertices[] = {-1, -1, 1, -1, 1, 1, -1, 1};
// tuple<double, double, double, int> get_next(double i, double j, double delta,
//                                             int t) {
//   double initI = (t < 2) ? 0 : delta / 2;
//   double initJ = (t % 2) ? 0 : delta / 2;
//   if (j + delta < 1) {
//     // cout << "a" << endl;
//     return make_tuple(i, j + delta, delta, t);
//   }
//   if (i + delta < 1) {
//     // cout << "b" << endl;
//     return make_tuple(i + delta, initJ, delta, t);
//   }
//   if (t < 4) {
//     // cout << "c" << endl;
//     return make_tuple(initI, initJ, delta, t + 1);
//   }
//   // cout << "d" << endl;
//   return make_tuple(0, 0, delta / 2, 0);
// }
// void error_callback(int err, const char *desc) {
//   cerr << "err: " << err << endl << "desc: " << desc << endl;
// }
// int main() {
//   GLFWwindow *window;
//   GLuint texture, program, vertex_buffer;
//   GLint vPosLocation, vColorLocation, textureLocation;
//   glfwSetErrorCallback(error_callback);
//   if (!glfwInit()) {
//     cout << "failed to initialize GLFW" << endl;
//     return -1;
//   }
//   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
//   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//   window = glfwCreateWindow(512, 512, "grafeq", NULL, NULL);
//   if (!window) {
//     glfwTerminate();
//     return -1;
//   }
//   glfwMakeContextCurrent(window);
//   glewExperimental = GL_TRUE;
//   glewInit();
//   cout << "GL_VERSION: " << glGetString(GL_VERSION) << endl;
//   cout << "GLSL_VERSION: " << glGetString(GL_SHADING_LANGUAGE_VERSION) <<
//   endl; GLuint vertex_shader, fragment_shader; glGenTextures(1, &texture);
//   glBindTexture(GL_TEXTURE_2D, texture);
//   char image[16 * 16];
//   for (int x = 0; x < 16; ++x) {
//     for (int y = 0; y < 16; ++y) {
//       image[x * 16 + y] = 1;
//     }
//   }
//   glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 16, 16, 0, GL_LUMINANCE,
//                GL_UNSIGNED_BYTE, image);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//   vertex_shader = glCreateShader(GL_VERTEX_SHADER);
//   glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
//   glCompileShader(vertex_shader);

//   fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
//   glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
//   glCompileShader(fragment_shader);

//   program = glCreateProgram();
//   glAttachShader(program, vertex_shader);
//   glAttachShader(program, fragment_shader);
//   glLinkProgram(program);

//   vColorLocation = glGetUniformLocation(program, "vColor");
//   vPosLocation = glGetAttribLocation(program, "vPosition");
//   textureLocation = glGetUniformLocation(program, "texture");
//   glGenBuffers(1, &vertex_buffer);
//   glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW);

//   glUseProgram(program);
//   glUniform1i(textureLocation, 0);

//   glEnable(GL_TEXTURE_2D);
//   glBindTexture(GL_TEXTURE_2D, texture);

//   glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//   glEnableVertexAttribArray(vPosLocation);
//   glVertexAttribPointer(vPosLocation, 2, GL_FLOAT, GL_FALSE,
//                         sizeof(GLfloat) * 2, 0);
//   while (!glfwWindowShouldClose(window)) {
//     const GLfloat color[] = {1.0f, 1.0f, 1.0f};
//     glUniform3fv(vColorLocation, 1, color);
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

static const char *vertex_shader_code = R"(
#version 110
attribute vec2 vPos;
varying vec2 texcoord;
void main()
{
    gl_Position = vec4(vPos, 0.0, 1.0);
    texcoord = vPos;
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

const GLfloat vertices[] = {0, 0, 1, 0, 1, 1, 0, 1};

void error_callback(int error, const char *description) {
  std::cerr << "Error: " << description << std::endl;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  glfwSetErrorCallback(error_callback);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  GLFWwindow *window = glfwCreateWindow(512, 512, "grafeq", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  // gladLoadGL(glfwGetProcAddress);
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
  for (int x = 0; x < 16; ++x) {
    for (int y = 0; y < 16; ++y) {
      image[x * 16 + y] = rand() % 256;
    }
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 16, 16, 0, GL_LUMINANCE,
               GL_UNSIGNED_BYTE, image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glUseProgram(program);
  glUniform1i(textureLocation, 0);

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glEnableVertexAttribArray(vPosLocation);
  glVertexAttribPointer(vPosLocation, 2, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void *)0);

  while (!glfwWindowShouldClose(window)) {
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
