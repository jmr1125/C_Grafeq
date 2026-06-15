#include "flint/arb_types.h"
#include "flint/arf.h"
#include <flint/arb.h>
#include <flint/flint.h>
#include <iostream>
using namespace std;
int main() {
  arf_t a, b, c;
  arf_init(a);
  arf_init(b);
  arf_init(c);
  arf_pos_inf(a);
  // arf_one(b);
  // arf_neg(b, b);
  // arf_neg_inf(b);
  arf_zero(b);
  arf_mul(c, a, b, 128, ARF_RND_DOWN);
  arf_print(c);

  arf_one(b);
  arf_div(c, b, a, 128, ARF_RND_DOWN);
  arf_print(c);
  arf_clear(a);
  arf_clear(b);
  arf_clear(c);
  double l, r;
  cin >> l >> r;
  arb_t y1, y2, y3, y4, y5, x;
  arb_init(x);
  arb_set_d(x, (l + r) / 2);
  mag_t rad;
  mag_init(rad);
  mag_set_d(rad, (r - l) / 2);
  arb_add_error_mag(x, rad);
  arb_init(y1);
  arb_init(y2);
  arb_init(y3);
  arb_init(y4);
  arb_init(y5);
  arb_sin(y1, x, 128);
  arb_tan(y2, x, 128);
  arb_exp(y3, x, 128);
  arb_log(y4, x, 128);
  arb_ui_div(y5, 1, x, 128);
  auto print_lr = [](const arb_t &x) {
    arf_t lo, hi;
    arf_init(lo);
    arf_init(hi);
    arb_get_interval_arf(lo, hi, x, 128);
    double dlo = arf_get_d(lo, ARF_RND_FLOOR),
           dhi = arf_get_d(hi, ARF_RND_CEIL);
    arf_print(lo);
    cout << "  ( " << dlo << " ) ~ " << flush;
    arf_print(hi);
    cout << "  ( " << dhi << " )  ";
    if (arb_contains_zero(x)) {
      cout << "include 0 ";
    }
    if (arb_is_finite(x)) {
      cout << "is finite";
    }
    cout << endl;
  };
  cout << "sin : " << flush;
  print_lr(y1);
  cout << endl << "tan : " << flush;
  print_lr(y2);
  cout << endl << "exp : " << flush;
  print_lr(y3);
  cout << endl << "log : " << flush;
  print_lr(y4);
  cout << endl << "1/x : " << flush;
  print_lr(y5);
  arb_clear(x);
  mag_clear(rad);
  arb_clear(y1);
  arb_clear(y2);
  arb_clear(y3);
  arb_clear(y4);
  arb_clear(y5);
}
