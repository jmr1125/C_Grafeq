#include "value.hpp"
#include "flint/arb.h"
#include "flint/arf.h"
#include <cmath>
#include <functional>
#include <iostream>
#include <utility>
fval::fval() { arf_init(val); }
fval::fval(double x) { arf_set_d(val, x); }
fval::fval(const arf_t &x) { arf_set(val, x); }
fval::~fval() { arf_clear(val); }
varible::varible() { cont = {true, true}; }
varible::varible(range x) {
  cont = {true, true};
  r.push_back(x);
}
void fval::set_pinf() { arf_pos_inf(val); }
void fval::set_ninf() { arf_neg_inf(val); }
void fval::set_nan() { arf_nan(val); }

bool fval::operator<(const fval &r) const { return arf_cmp(val, r.val) < 0; }
bool fval::operator==(const fval &r) const { return arf_cmp(val, r.val) == 0; }
bool fval::operator>(const fval &r) const { return arf_cmp(val, r.val) > 0; }
bool fval::nan() const { return arf_is_nan(val); }
bool fval::pinf() const { return arf_is_pos_inf(val); }
bool fval::ninf() const { return arf_is_neg_inf(val); }

range neg(const range &x) {
  range res;
  arf_neg(res.lo.val, x.hi.val);
  arf_neg(res.hi.val, x.lo.val);
  return std::move(res);
}
range add(const range &x, const range &y) {
  range res;
  arf_add(res.lo.val, x.lo.val, y.lo.val, 128,
          ARF_RND_FLOOR); // can't be -inf and +inf simultaneously
  arf_add(res.hi.val, x.hi.val, y.hi.val, 128, ARF_RND_CEIL);
  return std::move(res);
}
varible reciprocal(const range &x) {
  varible res;
  const int sgn_l = arf_sgn(x.lo.val), sgn_h = arf_sgn(x.hi.val);
  if (sgn_l == 0) {
    range t;
    t.hi.set_pinf();
    arf_si_div(t.lo.val, 1, x.hi.val, 128, ARF_RND_FLOOR);
    res.r.push_back(std::move(t));
    res.cont = {false, true};
    return res;
  } else if (sgn_h == 0) {
    range t;
    t.lo.set_ninf();
    arf_si_div(t.hi.val, 1, x.lo.val, 128, ARF_RND_CEIL);
    res.r.push_back(std::move(t));
    res.cont = {false, true};
    return res;
  } else if (sgn_l < 0 && sgn_h > 0) {
    range t1, t2;
    t1.lo.set_ninf(), t2.hi.set_pinf();
    arf_si_div(t1.hi.val, 1, x.lo.val, 128, ARF_RND_CEIL),
        arf_si_div(t2.lo.val, 1, x.hi.val, 128, ARF_RND_FLOOR);
    res.r.push_back(std::move(t1));
    res.r.push_back(std::move(t2));
    res.cont = {false, true};
    return res;
  }
  arb_t X, one;
  arb_init(X);
  arb_init(one);
  arb_one(one);
  arb_set_interval_arf(X, x.lo.val, x.hi.val, 128);
  arb_div(X, one, X, 128);
  res.r.push_back(range());
  arb_get_interval_arf(res.r[0].lo.val, res.r[0].hi.val, X, 128);
  arb_clear(X);
  arb_clear(one);
  res.cont = {true, true};
  return res;
}
range mul(const range &x, const range &y) {
  fval p[2][4];
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 4; ++j) {
      std::pair<std::reference_wrapper<const fval>,
                std::reference_wrapper<const fval>>
          p1{x.hi, x.lo}, p2{y.hi, y.lo};
      if (j / 2)
        swap(p1.first, p1.second);
      if (j % 2)
        swap(p2.first, p2.second);
      arf_mul(p[i][j].val, p1.first.get().val, p2.first.get().val, 128,
              i ? ARF_RND_CEIL : ARF_RND_FLOOR);
      // arf_printd(p1.first.get().val, 10);
      // printf(" * ");
      // arf_printd(p2.first.get().val, 10);
      // printf(" = ");
      // arf_printd(p[i][j].val, 10);
      // printf("\n");
      if (p[i][j].nan()) {
        if (arf_sgn(p1.second.get().val) == arf_sgn(p2.second.get().val)) {
          arf_pos_inf(p[i][j].val);
        } else {
          arf_neg_inf(p[i][j].val);
        }
      }
    }

  range res;
  arf_set(res.lo.val, p[0][0].val);
  arf_set(res.hi.val, p[1][0].val);
  for (int i = 1; i < 4; ++i) {
    arf_min(res.lo.val, res.lo.val, p[0][i].val);
    arf_max(res.hi.val, res.hi.val, p[1][i].val);
  }
  return res;
}
fval getpi() {
  static fval res;
  static bool init = false;
  if (!init) {
    init = true;
    arb_t pi_b;
    arb_init(pi_b);
    arb_const_pi(pi_b, 128);
    arf_set(res.val, arb_midref(pi_b));
    arb_clear(pi_b);
  }
  return res;
}
// if [x] belongs to a . pi + k . b . pi, where k belongs to Z
// a, b > 0
// bool contains(const range &x, const fval &a, const fval &b) {
//   fval h, l;
//   arf_div(h.val, x.hi.val, getpi().val, 128, ARF_RND_FLOOR);
//   arf_div(l.val, x.lo.val, getpi().val, 128, ARF_RND_CEIL);
//   arf_div(h.val, x.hi.val, b.val, 128, ARF_RND_FLOOR);
//   arf_div(l.val, x.lo.val, b.val, 128, ARF_RND_CEIL);
//   fval c1, c2;
//   arf_div(c1.val, a.val, b.val, 128, ARF_RND_CEIL);
//   arf_div(c2.val, a.val, b.val, 128, ARF_RND_FLOOR);
//   arf_sub(h.val, h.val, c2.val, 128, ARF_RND_CEIL);
//   arf_sub(l.val, l.val, c1.val, 128, ARF_RND_FLOOR);
//   fmpz_t kl, kr;
//   bool res;
//   fmpz_init(kl);
//   fmpz_init(kr);
//   arf_get_fmpz(kl, l.val, ARF_RND_CEIL);
//   arf_get_fmpz(kr, h.val, ARF_RND_FLOOR);
//   res = fmpz_cmp(kl, kr) <= 0;
//   fmpz_clear(kl);
//   fmpz_clear(kr);
//   return res;
// }

range sin(const range &x) {
  // bool to_long = false;
  // {
  //   fval del;
  //   arf_sub(del.val, x.hi.val, x.lo.val, 128, ARF_RND_CEIL);
  //   arf_div_si(del.val, del.val, 2, 128, ARF_RND_CEIL);
  //   to_long = arf_cmp(del.val, getpi().val) >= 0;
  // }
  if (x.hi.pinf() || x.lo.ninf() // || to_long
  ) {
    range res;
    arf_one(res.hi.val);
    arf_neg(res.lo.val, res.hi.val);
    return std::move(res);
  }
  arb_t X;
  range res;
  arb_init(X);
  arb_set_interval_arf(X, x.lo.val, x.hi.val, 128);
  arb_sin(X, X, 128);
  arb_get_interval_arf(res.lo.val, res.hi.val, X, 128);
  arb_clear(X);
  return res;
}
range cos(const range &x) {
  // bool to_long = false;
  // {
  //   fval del;
  //   arf_sub(del.val, x.hi.val, x.lo.val, 128, ARF_RND_CEIL);
  //   arf_div_si(del.val, del.val, 2, 128, ARF_RND_CEIL);
  //   to_long = arf_cmp(del.val, getpi().val) >= 0;
  // }
  if (x.hi.pinf() || x.lo.ninf() // || to_long
  ) {
    range res;
    arf_one(res.hi.val);
    arf_neg(res.lo.val, res.hi.val);
    return std::move(res);
  }
  arb_t X;
  range res;
  arb_init(X);
  arb_set_interval_arf(X, x.lo.val, x.hi.val, 128);
  arb_cos(X, X, 128);
  arb_get_interval_arf(res.lo.val, res.hi.val, X, 128);
  arb_clear(X);
  return res;
}
range exp(const range &x) {
  if (x.lo.ninf() && x.hi.pinf()) {
    range res;
    res.hi.set_pinf();
    return res;
  }
  if (x.lo.ninf()) {
    arb_t r;
    arb_init(r);
    arb_set_arf(r, x.hi.val);
    arb_exp(r, r, 128);
    range res;
    arb_get_ubound_arf(res.hi.val, r, 128);
    arb_clear(r);
    return res;
  }
  if (x.hi.pinf()) {
    arb_t r;
    arb_init(r);
    arb_set_arf(r, x.lo.val);
    arb_exp(r, r, 128);
    range res;
    res.hi.set_pinf();
    arb_get_lbound_arf(res.lo.val, r, 128);
    arb_clear(r);
    return res;
  }
  arb_t X;
  range res;
  arb_init(X);
  arb_set_interval_arf(X, x.lo.val, x.hi.val, 128);
  arb_exp(X, X, 128);
  arb_get_interval_arf(res.lo.val, res.hi.val, X, 128);
  arb_clear(X);
  return res;
}
varible log(const range &x) {
  if (arf_sgn(x.hi.val) <= 0) {
    varible res;
    res.cont = {false, false};
    return res;
  }
  if (arf_sgn(x.lo.val) <= 0) {
    arb_t X;
    range r;
    arb_init(X);
    arb_set_arf(X, x.hi.val);
    arb_log(X, X, 128);
    r.lo.set_ninf();
    arb_get_ubound_arf(r.hi.val, X, 128);
    arb_clear(X);
    varible res;
    res.r.push_back(std::move(r));
    res.cont = {true, true};
    return res;
  }
  varible res;
  res.r.push_back(range());
  arb_t X;
  arb_init(X);
  arb_set_interval_arf(X, x.lo.val, x.hi.val, 128);
  arb_log(X, X, 128);
  // arb_print(X);
  fval l, h;
  arb_get_interval_arf(res.r[0].lo.val, res.r[0].hi.val, X, 128);
  arb_clear(X);
  res.cont = {true, true};
  return res;
}
bool isintersect(const range &a, const range &b) {
  return (arf_cmp(a.lo.val, b.hi.val) <= 0) &&
         (arf_cmp(a.hi.val, b.lo.val) >= 0);
}
void sort(varible &x) {
  sort(x.r.begin(), x.r.end(),
       [](const range &l, const range &r) { return l.lo < r.lo; });
}
void normalize(varible &x) {
  sort(x);
  varible res;
  // std::cout << "x: " << x << std::endl;
  for (const auto &r : x.r) {
    if (res.r.empty()) {
      res.r.push_back(r);
    } else {
      auto &last = res.r.back();
      // std::cout << last << ',' << r << std::endl;
      if (isintersect(last, r)) {
        arf_min(last.lo.val, last.lo.val, r.lo.val);
        arf_max(last.hi.val, last.hi.val, r.hi.val);
      } else {
        res.r.push_back(r);
      }
    }
  }
  std::swap(x, res);
}
decltype(varible::cont) AND(decltype(varible::cont) a,
                            decltype(varible::cont) b) {
  return {a.first && b.first, a.second && b.second};
}
varible Union(const varible &a, const varible &b) {
  varible res;
  for (const auto &r : a.r) {
    res.r.push_back(r);
  }
  for (const auto &r : b.r) {
    res.r.push_back(r);
  }
  normalize(res);
  res.cont = AND(a.cont, b.cont);
  return res;
}
range ppow(const fval &a, const fval &b) {
  arb_t x, y;
  arb_init(x), arb_init(y);
  arb_set_arf(x, a.val), arb_set_arf(y, b.val);
  arb_pow(x, x, y, 128);
  fval l, r;
  arb_get_interval_arf(l.val, r.val, x, 128);
  arb_clear(x);
  arb_clear(y);
  return {.lo = l, .hi = r};
}

varible pow(const range &a, const range &b) {
  if (arf_cmp(b.hi.val, b.lo.val) == 0) {
    if (arf_sgn(a.hi.val) <= 0) {
      varible res;
      res.cont = {false, false};
      return res;
    }
    if (arf_sgn(a.lo.val) < 0) {
      const auto h = ppow(a.hi, b.lo);
      varible res;
      res.r.push_back(range());
      arf_set(res.r[0].hi.val, h.hi.val);
      res.cont = {false, true};
      return res;
    }
    varible res;
    const auto h = ppow(a.hi, b.lo);
    const auto l = ppow(a.lo, b.lo);
    res.r.push_back(range());
    arf_set(res.r[0].hi.val, h.hi.val);
    arf_set(res.r[0].lo.val, l.lo.val);
    res.cont = {true, true};
    return res;
  } else {
    // std::cout << A << ' ' << B << std::endl;
    if (arf_sgn(a.hi.val) <= 0 && arf_sgn(a.lo.val) < 0) {
      auto t = pow(neg(a), b);
      auto t1 = neg(t);
      // std::cout << "t,t1: " << t << ' ' << t1 << std::endl;
      // std::cout << "--- " << t << std::endl;
      auto res = Union(t, t1);
      res.cont = {true, true};
      return res;
    }
    if (arf_sgn(a.lo.val) < 0) {
      range x1, x2;
      arf_set(x1.lo.val, a.lo.val);
      arf_set(x2.hi.val, a.hi.val);
      varible res = Union(pow(x1, b), pow(x2, b));
      res.cont = {false, true};
      return res;
    }
    // std::cout << "--- " << A << ',' << B << std::endl;
    // std::cout << "--- " << log(A) << std::endl;
    // std::cout << "--- " << mul(B, log(A)) << std::endl;
    varible res = exp(mul(varible(a), log(varible(b))));
    res.cont = {true, true};
    return res;
  }
}

varible add(varible a, varible b) {
  varible res;
  for (const auto &x1 : a.r)
    for (const auto &x2 : b.r)
      res.r.push_back(add(x1, x2));
  normalize(res);
  res.cont = AND(a.cont, b.cont);
  return res;
}
varible neg(varible a) {
  varible res;
  for (const auto &x1 : a.r)
    res.r.push_back(neg(x1));
  normalize(res);
  res.cont = a.cont;
  return res;
}
varible mul(varible a, varible b) {
  varible res;
  for (const auto &x1 : a.r)
    for (const auto &x2 : b.r)
      res.r.push_back(mul(x1, x2));
  normalize(res);
  res.cont = AND(a.cont, b.cont);
  return res;
}
varible reciprocal(varible a) {
  varible res;
  for (const auto &x : a.r)
    res = Union(res, reciprocal(x));
  normalize(res);
  res.cont = AND(res.cont, a.cont);
  return res;
}
varible sin(varible a) {
  varible res;
  for (const auto &x1 : a.r)
    res.r.push_back(sin(x1));
  normalize(res);
  res.cont = a.cont;
  return res;
}
varible cos(varible a) {
  varible res;
  for (const auto &x1 : a.r)
    res.r.push_back(cos(x1));
  normalize(res);
  res.cont = a.cont;
  return res;
}
varible tan(varible a) { return mul(sin(a), reciprocal(cos(a))); }
varible log(varible a) {
  varible res;
  for (const auto &x : a.r) {
    res = Union(res, log(x));
  }
  return res;
}
varible exp(varible a) {
  varible res;
  for (const auto &x1 : a.r)
    res.r.push_back(exp(x1));
  normalize(res);
  res.cont = a.cont;
  return res;
}
varible pow(varible a, varible b) {
  varible res;
  for (const auto &x1 : a.r)
    for (const auto &x2 : b.r)
      res = Union(res, pow(x1, x2));
  normalize(res);
  res.cont = AND(res.cont, a.cont);
  res.cont = AND(res.cont, b.cont);
  return res;
}

std::ostream &operator<<(std::ostream &ost, const range &r) {
  ost << "[" << arf_get_str(r.lo.val, 10) << " ~ " << arf_get_str(r.hi.val, 10)
      << "]";
  return ost;
}
std::ostream &operator<<(std::ostream &ost, const varible &r) {
  ost << "{";
  bool f = false;
  for (const auto &r : r.r) {
    if (f) {
      ost << ", ";
    } else
      f = true;
    ost << r;
  }
  ost << "}";
  return ost;
}
