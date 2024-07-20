#include <iostream>
#include <regex>
#include <vector>
using namespace std;
enum tok { add, sub, mul, div, pow, log, sin, cos, tan, L, R, X, Y, seperate };
struct term {
  tok t;
  double v;
  term(tok t, double v) : t(t), v(v) {}
};
const regex token(
    R"(([-.0-9]+)|([xy])|(\+)|(\-)|(\*)|(\/)|(,)|(pow)|(log)|(sin)|(cos)|(tan)|(\()|(\)))");
using expr = vector<term>;
int main() {
  string exp;
  getline(cin, exp);
  expr e;
  {
    for (sregex_iterator it(exp.begin(), exp.end(), token), end; it != end;
         ++it) {
      if (it->str() == "x") {
        e.push_back(term(tok::X, 0));
        cout << "var: x" << endl;
      } else if (it->str() == "y") {
	e.push_back(term(tok::Y, 0));
        cout << "var: y" << endl;
      } else if (it->str() == "+") {
	e.push_back(term(tok::add, 0));
        cout << "op: add" << endl;
      } else if (it->str() == "-") {
	e.push_back(term(tok::sub, 0));
        cout << "op: sub" << endl;
      } else if (it->str() == "*") {
	e.push_back(term(tok::mul, 0));
        cout << "op: mul" << endl;
      } else if (it->str() == "/") {
	e.push_back(term(tok::div, 0));
        cout << "op: div" << endl;
      } else if (it->str() == ",") {
	e.push_back(term(seperate, 0));
        cout << "sep" << endl;
      } else if (it->str() == "pow") {
	e.push_back(term(pow, 0));
        cout << "op: pow" << endl;
      } else if (it->str() == "log") {
	e.push_back(term(log, 0));
        cout << "op: log" << endl;
      } else if (it->str() == "sin") {
	e.push_back(term(sin, 0));
        cout << "op: sin" << endl;
      } else if (it->str() == "cos") {
	e.push_back(term(cos, 0));
        cout << "op: cos" << endl;
      } else if (it->str() == "tan") {
	e.push_back(term(tan, 0));
        cout << "op: tan" << endl;
      } else if (it->str() == "(") {
	e.push_back(term(L, 0));
        cout << "L" << endl;
      } else if (it->str() == ")") {
	e.push_back(term(R, 0));
        cout << "R" << endl;
      } else {
	e.push_back(term(X, stod(it->str())));
        cout << "num: " << it->str() << endl;
      }
    }
  }
  for(const auto &t : e){}
}
