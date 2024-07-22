#include <iostream>
#include <regex>
#include <stack>
#include <vector>
using namespace std;
enum tok {
  add, // a
  sub, // s
  mul, // m
  div, // d
  pow, // p
  log, // l
  sin, // S
  cos, // C
  tan, // T
  L,
  R,
  X,   // x
  Y,   // y
  val, // v <val>
  seperate
};
struct term {
  tok t;
  double v;
  term(tok t, double v) : t(t), v(v) {}
};
ostream &operator<<(ostream &o, term t) {
  switch (t.t) {
  case tok::X:
    o << "x";
    break;
  case tok::Y:
    o << "y";
    break;
  case tok::val:
    o << "v " << t.v << " ";
    break;
  case tok::L:
    // o << " <L> ";
    break;
  case tok::R:
    // o << " <R> ";
    break;
  case tok::add:
    o << "a";
    break;
  case tok::sub:
    o << "s";
    break;
  case tok::mul:
    o << "m";
    break;
  case tok::div:
    o << "d";
    break;
  case tok::pow:
    o << "p";
    break;
  case tok::log:
    o << "l";
    break;
  case tok::sin:
    o << "S";
    break;
  case tok::cos:
    o << "C";
    break;
  case tok::tan:
    o << "T";
    break;
  case tok::seperate:
    // o << " <seperate> ";
    break;
  }
  o.flush();
  return o;
}

const regex token(
    R"((\-?[.0-9]+)|([xy])|(\+)|(\-)|(\*)|(\/)|(,)|(pow)|(log)|(sin)|(cos)|(tan)|(\()|(\)))");
using expr = vector<term>;
int prec(tok t) {
  switch (t) {
  case tok::pow:
    return 4;
  case tok::log:
    return 4;
  case tok::sin:
    return 4;
  case tok::cos:
    return 4;
  case tok::tan:
    return 4;
  case tok::add:
    return 1;
  case tok::sub:
    return 1;
  case tok::mul:
    return 2;
  case tok::div:
    return 2;
  case tok::L:
    return 0;
  case tok::R:
    return -1;
  case tok::seperate:
    return 1;
  default:
    return 0;
  }
}
// #define debug
int main() {
  string exp;
  getline(cin, exp);
  //   exp="1/(x-1)*x";
  // exp = "x+y";
  exp = "(" + exp + ")";
  expr e;
  {
    for (sregex_iterator it(exp.begin(), exp.end(), token), end; it != end;
         ++it) {
      if (it->str() == "x") {
        e.push_back(term(tok::X, 0));
#ifdef debug
        cout << "var: x" << endl;
#endif
      } else if (it->str() == "y") {
        e.push_back(term(tok::Y, 0));
#ifdef debug
        cout << "var: y" << endl;
#endif
      } else if (it->str() == "+") {
        e.push_back(term(tok::add, 0));
#ifdef debug
        cout << "op: add" << endl;
#endif
      } else if (it->str() == "-") {
        e.push_back(term(tok::sub, 0));
#ifdef debug
        cout << "op: sub" << endl;
#endif
      } else if (it->str() == "*") {
        e.push_back(term(tok::mul, 0));
#ifdef debug
        cout << "op: mul" << endl;
#endif
      } else if (it->str() == "/") {
        e.push_back(term(tok::div, 0));
#ifdef debug
        cout << "op: div" << endl;
#endif
      } else if (it->str() == ",") {
        e.push_back(term(tok::seperate, 0));
#ifdef debug
        cout << "sep" << endl;
#endif
      } else if (it->str() == "pow") {
        e.push_back(term(tok::pow, 0));
#ifdef debug
        cout << "op: pow" << endl;
#endif
      } else if (it->str() == "log") {
        e.push_back(term(tok::log, 0));
#ifdef debug
        cout << "op: log" << endl;
#endif
      } else if (it->str() == "sin") {
        e.push_back(term(tok::sin, 0));
#ifdef debug
        cout << "op: sin" << endl;
#endif
      } else if (it->str() == "cos") {
        e.push_back(term(tok::cos, 0));
#ifdef debug
        cout << "op: cos" << endl;
#endif
      } else if (it->str() == "tan") {
        e.push_back(term(tok::tan, 0));
#ifdef debug
        cout << "op: tan" << endl;
#endif
      } else if (it->str() == "(") {
        e.push_back(term(tok::L, 0));
#ifdef debug
        cout << "L" << endl;
#endif
      } else if (it->str() == ")") {
        e.push_back(term(tok::R, 0));
#ifdef debug
        cout << "R" << endl;
#endif
      } else {
        e.push_back(term(tok::val, stod(it->str())));
#ifdef debug
        cout << "num: " << it->str() << endl;
#endif
      }
    }
  }
  // cout << "======" << endl;
  stack<tok> op;
  auto f_flush = [&](term t) {
    while (!op.empty() && prec(t.t) < prec(op.top()) && op.top() != tok::L) {
      auto o = op.top();
      op.pop();
      cout << term(o, 0);
    }
  };
  for (const auto &t : e) {
    switch (t.t) {
    case tok::X:
    case tok::Y:
    case tok::val:
      cout << t;
      break;
    case tok::L:
      op.push(t.t);
      break;
    case tok::R:
      f_flush(t);
      op.pop();
      break;
    case tok::add:
    case tok::sub:
    case tok::mul:
    case tok::div:
    case tok::pow:
    case tok::log:
    case tok::sin:
    case tok::cos:
    case tok::tan:
      f_flush(t);
      op.push(t.t);
      break;
    case tok::seperate:
      break;
    }
  }
}
