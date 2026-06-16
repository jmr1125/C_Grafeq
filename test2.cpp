#include "flint/arf.h"
#include "value.hpp"
#include <iostream>
using namespace std;
int main() {
  varible x;
  x.r.push_back(range());
  arf_zero(x.r[0].lo.val);
  arf_one(x.r[0].hi.val);
  cout << mul(x, x) << mul(x, reciprocal(x)) << pow(x, x) << endl;
  arf_div_si(x.r[0].hi.val, x.r[0].hi.val, 2, 128, ARF_RND_NEAR);
  cout << mul(x, x) << mul(x, reciprocal(x)) << pow(x, x);
}
