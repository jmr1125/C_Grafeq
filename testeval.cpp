#include "eval.h"
#include <iostream>
using namespace std;

int main() {
  // expression expr = tokenize("xysv 1.2 a"); //  x +  y
  // cout << eval(expr, 0, 0) << endl;
  // cout << eval(expr, 0, 1) << endl;
  cout << sin(varible({make_pair(value(-1), value(1))})) << endl;
  cout << sin(varible({make_pair(value(-1), value(3.13 / 2))})) << endl;
  cout << sin(varible({make_pair(value(-1), value(3.15 / 2))})) << endl;
  cout << sin(varible({make_pair(value(-2), value(3.13 / 2))})) << endl;
  cout << sin(varible({make_pair(value(-2), value(3.15 / 2))})) << endl;
  cout << endl;
  cout << cos(varible({make_pair(value(-1), value(1))})) << endl;
  cout << cos(varible({make_pair(value(.01), value(3.13))})) << endl;
  cout << cos(varible({make_pair(value(.01), value(3.15))})) << endl;
  cout << cos(varible({make_pair(value(-.01), value(3.13))})) << endl;
  cout << cos(varible({make_pair(value(-.01), value(3.15))})) << endl;
}
