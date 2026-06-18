#include "flint/arb_types.h"
#include "flint/arf.h"
#include "value.hpp"
#include <flint/arb.h>
#include <flint/flint.h>
#include <iostream>
using namespace std;
int main() {
  // arf_t a, b, c;
  // arf_init(a);
  // arf_init(b);
  // arf_init(c);
  // arf_pos_inf(a);
  // // arf_one(b);
  // // arf_neg(b, b);
  // // arf_neg_inf(b);
  // arf_zero(b);
  // arf_mul(c, a, b, 128, ARF_RND_DOWN);
  // arf_print(c);

  // arf_one(b);
  // arf_div(c, b, a, 128, ARF_RND_DOWN);
  // arf_print(c);
  // arf_clear(a);
  // arf_clear(b);
  // arf_clear(c);
  // double l, r;
  // cin >> l >> r;
  // arb_t y1, y2, y3, y4, y5, x;
  // arb_init(x);
  // arb_set_d(x, (l + r) / 2);
  // mag_t rad;
  // mag_init(rad);
  // mag_set_d(rad, (r - l) / 2);
  // arb_add_error_mag(x, rad);
  // arb_init(y1);
  // arb_init(y2);
  // arb_init(y3);
  // arb_init(y4);
  // arb_init(y5);
  // arb_sin(y1, x, 128);
  // arb_tan(y2, x, 128);
  // arb_exp(y3, x, 128);
  // arb_log(y4, x, 128);
  // arb_ui_div(y5, 1, x, 128);
  // auto print_lr = [](const arb_t &x) {
  //   arf_t lo, hi;
  //   arf_init(lo);
  //   arf_init(hi);
  //   arb_get_interval_arf(lo, hi, x, 128);
  //   double dlo = arf_get_d(lo, ARF_RND_FLOOR),
  //          dhi = arf_get_d(hi, ARF_RND_CEIL);
  //   arf_print(lo);
  //   cout << "  ( " << dlo << " ) ~ " << flush;
  //   arf_print(hi);
  //   cout << "  ( " << dhi << " )  ";
  //   if (arb_contains_zero(x)) {
  //     cout << "include 0 ";
  //   }
  //   if (arb_is_finite(x)) {
  //     cout << "is finite";
  //   }
  //   cout << endl;
  // };
  // cout << "sin : " << flush;
  // print_lr(y1);
  // cout << endl << "tan : " << flush;
  // print_lr(y2);
  // cout << endl << "exp : " << flush;
  // print_lr(y3);
  // cout << endl << "log : " << flush;
  // print_lr(y4);
  // cout << endl << "1/x : " << flush;
  // print_lr(y5);
  // arb_clear(x);
  // mag_clear(rad);
  // arb_clear(y1);
  // arb_clear(y2);
  // arb_clear(y3);
  // arb_clear(y4);
  // arb_clear(y5);

  // fval a, b;
  // a.set_ninf();
  // arf_set_d(b.val, 1);
  // fval c;
  // arf_add(c.val, a.val, b.val, 128, ARF_RND_DOWN);
  // arf_print(c.val);

  fval a, b;
  // a.set_ninf();
  //  b.set_pinf();
  // arf_zero(a.val);
  // a.set_ninf();
  // arf_zero(b.val);
  // arf_one(b.val);
  //  b.set_pinf();

  // arb_t x, y;
  // arb_init(x);
  // arb_init(y);
  // // arb_set_interval_arf(x, a.val, b.val, 128);
  // arb_set_str(x, "[1.0 +/- 0.1]", 128);
  // arb_set_str(y, "2.0", 128);
  // // arb_exp(x, x, 128);
  // arb_pow(x, x, y, 128);
  // arb_printd(x, 10);
  // arb_clear(x);
  // arb_clear(y);

  // arf_set_d(a.val, -1);
  // arf_set_d(b.val, 2.0);
  // arb_t aa, bb;
  // arb_init(aa);
  // arb_init(bb);
  // arb_set_arf(aa, a.val);
  // arb_set_arf(bb, b.val);
  // // arb_one(bb);
  // // arb_div_si(bb, bb, 3, 128);
  // arb_pow(aa, aa, bb, 128);
  // arb_print(aa);
  // arb_clear(aa);
  // arb_clear(bb);

  // varible x;
  // x.r.push_back(range());
  // arf_set_d(x.r[0].lo.val, -1);
  // arf_set_d(x.r[0].hi.val, -0);
  // cout << pow(x, x);
  varible val, val1, val2;
  auto f = [&](double xl, double xr, double yl, double yr) {
    varible x, y;
    x.r.push_back(range());
    y.r.push_back(range());
    arf_set_d(x.r[0].lo.val, xl);
    arf_set_d(x.r[0].hi.val, xr);
    arf_set_d(y.r[0].lo.val, yl);
    arf_set_d(y.r[0].hi.val, yr);
    cout << (val = add(mul(x, y), x)) << endl;
  };
  const double xl = -3.0 / 1024, xr = -1.0 / 512, yl = -3.0 / 1024,
               yr = -1.0 / 512;
  const double lx = (xr - xl) / 2, ly = (yr - yl) / 2;
  f(xl, xr, yl, yr);
  val1 = val;
  cout << "===" << endl;
  f(xl, xl + lx, yl, yl + ly);
  val2 = Union(val2, val);
  f(xl, xl + lx, yl + ly, yr);
  val2 = Union(val2, val);
  f(xl + lx, xr, yl, yl + ly);
  val2 = Union(val2, val);
  f(xl + lx, xr, yl + ly, yr);
  val2 = Union(val2, val);
  cout << val2 << endl;
  bool same = false;
  if (val1.r.size() == val2.r.size()) {
    same = true;
    for (int i = 0; i < val1.r.size(); ++i) {
      arf_print(val1.r[i].lo.val);
      if (arf_cmp(val1.r[i].lo.val, val2.r[i].lo.val) != 0) {
        printf("!=");
      } else {
        printf("==");
      }
      arf_print(val2.r[i].lo.val);
      printf("    ");
      arf_print(val1.r[i].hi.val);
      if (arf_cmp(val1.r[i].hi.val, val2.r[i].hi.val) != 0) {
        printf("!=");
      } else {
        printf("==");
      }
      arf_print(val2.r[i].hi.val);
      printf("\n");
      if (arf_cmp(val1.r[i].lo.val, val2.r[i].lo.val) != 0 ||
          arf_cmp(val1.r[i].hi.val, val2.r[i].hi.val) != 0) {
        same = false;
        break;
      }
    }
  }
  cout << same << endl;
}
