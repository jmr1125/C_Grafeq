#include <GL/glew.h>    /* include GLEW and new version of GL on Windows */
#include <GLFW/glfw3.h> /* GLFW helper library */
#include <iostream>
#include <tuple>
#include <vector>
using namespace std;

void _print_shader_info_log(GLuint shader_index) {
  int max_length = 2048;
  int actual_length = 0;
  char shader_log[2048];
  glGetShaderInfoLog(shader_index, max_length, &actual_length, shader_log);
  printf("shader info log for GL index %u:\n%s\n", shader_index, shader_log);
}
void _print_programme_info_log(GLuint programme) {
  int max_length = 2048;
  int actual_length = 0;
  char program_log[2048];
  glGetProgramInfoLog(programme, max_length, &actual_length, program_log);
  printf("program info log for GL index %u:\n%s", programme, program_log);
}
GLFWwindow *window = NULL;
const GLubyte *renderer;
const GLubyte *version;
GLuint vao;
GLuint vbo_point, vbo_colours;
int params = -1;
int n;
/* GL shader objects for vertex and fragment shader [components] */
GLuint vert_shader, frag_shader;
/* GL shader programme object [combined, to link] */
GLuint shader_programme;
int scr_size = 512;
int init() {

  /* these are the strings of code for the shaders
  the vertex shader positions each vertex point */
  const char *vertex_shader = "#version 410\n"
                              "layout(location = 0) in vec2 vertex_position;"
                              "layout(location = 1) in vec3 vertex_colour;"
                              "out vec3 colour;"
                              "void main () {"
                              "  colour = vertex_colour;"
                              "  gl_Position = vec4(vertex_position, 0.0, 1.0);"
                              "}";

  /* the fragment shader colours each fragment (pixel-sized area of the
  triangle) */
  const char *fragment_shader = "#version 410\n"
                                "in vec3 colour;"
                                "out vec4 frag_colour;"
                                "void main () {"
                                "  frag_colour = vec4(colour, 1.0);"
                                "}";

  /* start GL context and O/S window using the GLFW helper library */
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }

  /* Version 4.1 Core is a good default that should run on just about
   * everything. Adjust later to suit project requirements. */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(scr_size, scr_size, "c_grafeq", NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

  /* start GLEW extension handler */
  glewExperimental = GL_TRUE;
  glewInit();

  /* get version info */
  renderer = glGetString(GL_RENDERER); /* get renderer string */
  version = glGetString(GL_VERSION);   /* version as a string */
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  /* tell GL to only draw onto a pixel if the shape is closer to the viewer
  than anything already drawn at that pixel */
  glEnable(GL_DEPTH_TEST); /* enable depth-testing */
  /* with LESS depth-testing interprets a smaller depth value as meaning
   * "closer" */
  glDepthFunc(GL_LESS);

  /* a vertex buffer object (VBO) is created here. this stores an array of
  data on the graphics adapter's memory. in our case - the vertex points */
  glGenBuffers(1, &vbo_point);
  glGenBuffers(1, &vbo_colours);

  /* the vertex array object (VAO) is a little descriptor that defines which
  data from vertex buffer objects should be used as input variables to vertex
  shaders. in our case - use our only VBO, and say 'every three floats is a
  variable' */
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  /* "attribute #0 should be enabled when this vao is bound" */
  glEnableVertexAttribArray(0);
  /* this VBO is already bound, but it's a good habit to explicitly specify
  which VBO's data the following vertex attribute pointer refers to */
  glBindBuffer(GL_ARRAY_BUFFER, vbo_point);
  /* "attribute #0 is created from every 3 variables in the above buffer, of
  type float (i.e. make me vec3s)" */
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_colours);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  /* here we copy the shader strings into GL shaders, and compile them. we
  then create an executable shader 'program' and attach both of the compiled
      shaders. we link this, which matches the outputs of the vertex shader to
  the inputs of the fragment shader, etc. and it is then ready to use */
  vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1, &vertex_shader, NULL);
  glCompileShader(vert_shader);
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    fprintf(stderr, "ERROR: GL shader index %i did not compile\n", vert_shader);
    _print_shader_info_log(vert_shader);
    return 1;
  }
  frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, &fragment_shader, NULL);
  glCompileShader(frag_shader);
  params = -1;
  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    fprintf(stderr, "ERROR: GL shader index %i did not compile\n", frag_shader);
    _print_shader_info_log(frag_shader);
    return 1;
  }
  shader_programme = glCreateProgram();
  glAttachShader(shader_programme, frag_shader);
  glAttachShader(shader_programme, vert_shader);

  glBindAttribLocation(shader_programme, 0, "vertex_position");
  glBindAttribLocation(shader_programme, 1, "vertex_colour");
  glLinkProgram(shader_programme);
  params = -1;
  glGetProgramiv(shader_programme, GL_LINK_STATUS, &params);
  if (GL_TRUE != params) {
    fprintf(stderr, "ERROR: could not link shader programme GL index %i\n",
            shader_programme);
    _print_programme_info_log(shader_programme);
    return 1;
  }

  /* this loop clears the drawing surface, then draws the geometry described
      by the VAO onto the drawing surface. we 'poll events' to see if the window
  was closed, etc. finally, we 'swap the buffers' which displays our drawing
      surface onto the view area. we use a double-buffering system which means
      that we have a 'currently displayed' surface, and 'currently being drawn'
      surface. hence the 'swap' idea. in a single-buffering system we would see
      stuff being drawn one-after-the-other */
  int t = 0;
  return 0;
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
void draw_triangle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3,
                   GLfloat y3, GLfloat r, GLfloat g, GLfloat b) {
  GLfloat points[6] = {x1, y1, x2, y2, x3, y3};
  GLfloat colours[9] = {r, g, b, r, g, b, r, g, b};
  glBindBuffer(GL_ARRAY_BUFFER, vbo_point);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), points, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_colours);
  glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colours, GL_STATIC_DRAW);
  glUseProgram(shader_programme);

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}
void draw_square(GLfloat x, GLfloat y, GLfloat a, GLfloat r, GLfloat g,
                 GLfloat b) {
  draw_triangle(x, y, x + a, y, x, y + a, r, g, b);
  draw_triangle(x + a, y, x, y + a, x + a, y + a, r, g, b);
}
int main() {
  double i = 0, j = 0, delta = 0.5;
  vector<vector<tuple<GLfloat, GLfloat, GLfloat>>> screen(
      scr_size, vector<tuple<GLfloat, GLfloat, GLfloat>>(
                    scr_size, make_tuple(0, 0, 0)));
  // for (; delta > 0.1; delta /= 2) {
  //   for (int t = 0; t < 4; t++) {
  //     i = (t < 2) ? 0 : delta / 2;
  //     j = (t % 2) ? 0 : delta / 2;
  //     for (; i < 1; i += delta) {
  //       for (; j < 1; j += delta) {
  //         cout << i << ", " << j << endl;
  //       }
  //     }
  //   }
  // }
  int t = 0;
  // while (delta > 0.25) {
  //   auto T = get_next(i, j, delta, t);
  //   i = get<0>(T);
  //   j = get<1>(T);
  //   delta = get<2>(T);
  //   t = get<3>(T);
  //   cout << i << ", " << j << ", " << delta << ", " << t << endl;
  // }
  init();
  for (int i = 0; i < scr_size; i++)
    for (int j = 0; j < scr_size; j++) {
      screen[i][j] = make_tuple(1, 1, 1);
    }
  double T = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    glfwWaitEventsTimeout(0.1);
    double current_time = glfwGetTime();
    if (current_time - T <= 0.1)
      continue;
    T = current_time;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //    draw
    for (int i = 0; i < scr_size; i++) {
      for (int j = 0; j < scr_size; j++) {
        draw_square(i * 2.0 / scr_size - 1, j * 2.0 / scr_size - 1,
                    1.0 / scr_size, get<0>(screen[i][j]), get<1>(screen[i][j]),
                    get<2>(screen[i][j]));
      }
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
    // cout << i << ", " << j << ", " << delta << ", " << t << endl;
    // {
    //   auto T = get_next(i, j, delta, t);
    //   i = get<0>(T);
    //   j = get<1>(T);
    //   delta = get<2>(T);
    //   t = get<3>(T);
    // }
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
