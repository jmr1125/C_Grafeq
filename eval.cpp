#include "eval.h"
#include <cmath>
#include <sstream>
#include <stack>
using namespace std;
expression tokenize(const string &expr) {
  stringstream ss(expr);
  expression e;
  char c;
  while (ss >> c) {
    if (c == 'v') {
      double v;
      ss >> v;
      e.push_back({CONSTANT, v});
    } else if (c == 'a') {
      e.push_back({ADD});
    } else if (c == 's') {
      e.push_back({SUB});
    } else if (c == 'm') {
      e.push_back({MUL});
    } else if (c == 'd') {
      e.push_back({DIV});
    } else if (c == 'p') {
      e.push_back({POW});
    } else if (c == 'l') {
      e.push_back({LOG});
    } else if (c == 'S') {
      e.push_back({SIN});
    } else if (c == 'C') {
      e.push_back({COS});
    } else if (c == 'T') {
      e.push_back({TAN});
    } else if (c == 'x') {
      e.push_back({VAR1});
    } else if (c == 'y') {
      e.push_back({VAR2});
    }
  }
  return e;
}
double eval(const expression &e, double x, double y) {
  stack<double> num;
  double a, b;
  for (auto &i : e) {
    switch (i.op) {
    case ADD:
      b = num.top();
      num.pop();
      a = num.top();
      num.pop();
      num.push(a + b);
      break;
    case SUB:
      b = num.top();
      num.pop();
      a = num.top();
      num.pop();
      num.push(a - b);
      break;
    case MUL:
      b = num.top();
      num.pop();
      a = num.top();
      num.pop();
      num.push(a * b);
      break;
    case DIV:
      b = num.top();
      num.pop();
      a = num.top();
      num.pop();
      num.push(a / b);
      break;
    case POW:
      b = num.top();
      num.pop();
      a = num.top();
      num.pop();
      num.push(pow(a, b));
      break;
    case LOG:
      b = num.top();
      num.pop();
      a = num.top();
      num.pop();
      num.push(log(a) / log(b));
      break;
    case SIN:
      a = num.top();
      num.pop();
      num.push(sin(a));
      break;
    case COS:
      a = num.top();
      num.pop();
      num.push(cos(a));
      break;
    case TAN:
      a = num.top();
      num.pop();
      num.push(tan(a));
      break;
    case VAR1:
      num.push(x);
      break;
    case VAR2:
      num.push(y);
      break;
    case CONSTANT:
      num.push(i.constant);
      break;
    }
  }
  return num.top();
}
