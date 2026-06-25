#include "flint/arf.h"
#include "prog.hpp"
#include "value.hpp"
#include <cmath>
#include <fstream>
#include <ios>
#include <iostream>
using namespace std;

int main() {
  prog p;
  {
    // ifstream ifs("23.expr.asm");
    //ifstream ifs("22.asm");
    ifstream ifs("9.expr.asm");
    p.load(ifs);
  }
  varible x, y;
  /*
    [-0.00195312,0]
    [0.96875,0.970703] for 23

    [2,2.00195]
    [-0.00390625,0] for 22
  */
  x.r.push_back(range());
  y.r.push_back(range());
  // arf_set_d(x.r.back().lo.val, -0.00195312);
  // arf_set_d(x.r.back().hi.val, 0);
  // arf_set_d(y.r.back().lo.val, 0.96875);
  // arf_set_d(y.r.back().hi.val, 0.970703);
  // arf_set_d(x.r.back().lo.val, 2);
  // arf_set_d(x.r.back().hi.val, 2.00195);
  // arf_set_d(y.r.back().lo.val, -0.00390625);
  // arf_set_d(y.r.back().hi.val, -0.00390625 / 2);
  arf_set_d(x.r.back().lo.val, 0);
  arf_set_d(x.r.back().hi.val, 0.0390625);
  arf_set_d(y.r.back().lo.val, 1.71875);
  arf_set_d(y.r.back().hi.val, 1.75781);
  auto res = p.eval(x, y);
  cout << res;
  cout << "cont: [" << res.cont.first << res.cont.second << "]" << endl;
}
