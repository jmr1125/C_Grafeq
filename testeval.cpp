#include "eval.h"
#include <cmath>
#include <ios>
#include <iostream>
using namespace std;

int main() {
  // expression expr = tokenize("xysv 1.2 a"); //  x +  y
  // cout << eval(expr, 0, 0) << endl;
  // cout << eval(expr, 0, 1) << endl;
  // cout << sin(varible({make_pair(value(-1), value(1))})) << endl;
  // cout << sin(varible({make_pair(value(-1), value(3.13 / 2))})) << endl;
  // cout << sin(varible({make_pair(value(-1), value(3.15 / 2))})) << endl;
  // cout << sin(varible({make_pair(value(-2), value(3.13 / 2))})) << endl;
  // cout << sin(varible({make_pair(value(-2), value(3.15 / 2))})) << endl;
  // cout << endl;
  // cout << cos(varible({make_pair(value(-1), value(1))})) << endl;
  // cout << cos(varible({make_pair(value(.01), value(3.13))})) << endl;
  // cout << cos(varible({make_pair(value(.01), value(3.15))})) << endl;
  // cout << cos(varible({make_pair(value(-.01), value(3.13))})) << endl;
  // cout << cos(varible({make_pair(value(-.01), value(3.15))})) << endl;
  // cout << endl << boolalpha;
  // cout << (value(1) < 2) << endl;
  // cout << (value(1) <= 2) << endl;
  // cout << (value(1) < value(0, value::inf)) << endl;
  // cout << (value(0, value::ninf) < value(0)) << endl;
  // cout << (value(0, value::ninf) < value(0, value::inf)) << endl;
  // cout << endl;
  // cout << (value(3) < 2) << endl;
  // cout << (value(2) <= 2) << endl;
  // cout << (value(0, value::inf) < value(0, value::inf)) << endl;
  // cout << (value(0, value::ninf) < value(0, value::ninf)) << endl;
  // cout << (value(0, value::ninf) > value(0, value::inf)) << endl;
  // cout << endl;
  // cout << (value(3) == 2) << endl;
  // cout << (value(2) == 2) << endl;
  // cout << (value(0, value::inf) == value(0, value::inf)) << endl;
  // cout << (value(0, value::ninf) == value(0, value::ninf)) << endl;
  // cout << (value(0, value::ninf) == value(0, value::inf)) << endl;
  // cout << endl;
  // cout << varible(value(1)) << endl;
  // cout << varible(value(1), value(2)) << endl;
  // cout << one_div(varible(value(1), value(2))) << endl;
  // cout << one_div(varible(value(-2), value(-1))) << endl;
  // cout << one_div(varible(value(-2), value(1))) << endl;
  // cout << one_div(varible(value(-2), value(0, value::inf))) << endl;
  // cout << one_div(varible(value(2), value(0, value::inf))) << endl;
  // cout << one_div(varible(value(0, value::ninf), value(-2))) << endl;
  // cout << one_div(varible(value(0, value::ninf), value(2))) << endl;
  // cout << one_div(varible(value(0, value::ninf), value(0, value::inf))) <<
  // endl; cout << endl; cout << one_div(one_div(varible(value(1), value(2))))
  // << endl; cout << one_div(one_div(varible(value(-1), value(2)))) << endl;
  // cout << one_div(one_div(varible(value(-2), value(-1)))) << endl;
  // cout << one_div(one_div(varible(value(-2), value(0)))) << endl;
  // cout << one_div(one_div(varible(value(0), value(2)))) << endl;
  // cout << endl;
  // cout << mul(varible(1, 2), varible(3, 4)) << endl;
  // cout << mul(varible(1, 2), varible(-3, 4)) << endl;
  // cout << mul(varible(1, 2), varible(-4, -3)) << endl;
  // cout << endl;
  // cout << log(varible(1, 3)) << endl;
  // cout << log(varible(1, value(0, value::inf))) << endl;
  // cout << log(varible(0.5, value(0, value::inf))) << endl;
  // cout << log(varible(0, value(3))) << endl;
  // cout << log(varible(-1, value(3))) << endl;
  // cout << log(varible(value(0, value::ninf), value(3))) << endl;
  // cout << log(varible(0, value(0, value::inf))) << endl;
  // cout << log(varible(-1, value(0, value::inf))) << endl;
  // cout << log(varible(value(0, value::ninf), value(0, value::inf))) << endl;
  // cout << endl;
  // cout << sin(varible(M_PI * 4, M_PI * 5)) << endl;
  // cout << div(sin(varible(1.3125, 1.3125 + 0.0625)),
  //             cos(varible(varible(1.3125, 1.3125 + 0.0625))));
  // cout << div(sin(varible(3.11 / 2, 3.13 / 2)),
  //             cos(varible(3.11 / 2, 3.13 / 2)))
  //      << endl;
  // cout << div(sin(varible(3.13 / 2, 3.15 / 2)),
  //             cos(varible(3.13 / 2, 3.15 / 2)))
  //      << endl;
  // cout << one_div(cos(varible(3.13 / 2, 3.15 / 2))) << endl;
  // cout << one_div(cos(varible(3.15 / 2, 3.17 / 2))) << endl;
  // cout << one_div(sin(varible(-5 * M_PI + 0.1, -4 * M_PI - 0.1))) << endl;
  cout << tan(varible(M_PI / 2 - 0.1, M_PI / 2 + 0.1)) << endl;
  cout << tan(varible(-M_PI / 2 - 0.1, -M_PI / 2 + 0.1)) << endl;
}
