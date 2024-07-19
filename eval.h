#include <vector>
using std::vector;
enum OP {
  ADD,     // a
  SUB,     // s
  MUL,     // m
  DIV,     // d
  POW,     // p
  LOG,     // l
  SIN,     // S
  COS,     // C
  TAN,     // T
  VAR1,    // x
  VAR2,    // y
  CONSTANT // v
};
struct term {
  OP op;
  double constant;
};
using expression = vector<term>;
expression tokenize(const std::string &expr);
double eval(const expression &expr, double var1, double var2);
