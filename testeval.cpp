#include "eval.h"
#include <iostream>
using namespace std;

int main() {
  expression expr = tokenize("xysv 1.2 a"); //  x +  y
  cout << eval(expr, 0, 0) << endl;
  cout << eval(expr, 0, 1) << endl;
}
